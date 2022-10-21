#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <stack>

#include <glm/glm.hpp>

#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Light.hpp"
#include "graphics/Model.hpp"

#include "graphics/models/Box.hpp"
#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"
// #include "graphics/models/Gun.hpp"
#include "graphics/models/Sphere.hpp"

#include "physics/Environment.hpp"

#include "io/Keyboard.hpp"
#include "io/Mouse.hpp"
#include "io/Joystick.hpp"
#include "io/Camera.hpp"

#include "algorithms/States.hpp"

#include "Scene.hpp"

Scene scene;

void processInput(double dataTime);

Camera cam;

glm::mat4 transform = glm::mat4(1.0f);

Joystick mainJ(0);

int activeCam = 0;

float limitFPS = 1.f/60.f;

int currentFPS = 0;
float timer = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool spotLightOn = true;

Sphere sphere(10);

int main(){
    std::cout << "Hello world!\n";

    scene = Scene(3, 3, "OpenGL Tutorial", 800, 600);
    if(!scene.init()){
        std::cout << "Could not open window" << std::endl;
        glfwTerminate();
        return -1;
    }

    scene.cameras.push_back(&cam);
    scene.activeCamera = 0;
    // Limits FPS
    glfwSwapInterval(1);
    // #####################
    //     S H A D E R S
    // #####################

    Shader lampShader("../shaders/instanced/instanced.vs", "../shaders/lamp.fs");
    Shader shader("../shaders/instanced/instanced.vs", "../shaders/object.fs");
    Shader troglodyteShader("../shaders/instanced/instanced.vs", "../shaders/object.fs");
    Shader boxShader("../shaders/instanced/box.vs", "../shaders/instanced/box.fs");
    
    // ###################
    //     M O D E L S
    // ###################
    // Creates a model as a defined mesh or else

    Model troglodyte("troglodyte", BoundTypes::AABB, 1, CONST_INSTANCES);
    troglodyte.loadModel("../assets/models/troglodyte.gltf");

    Lamp lamp(4);
    // Let that scene will reqister and use that in his own scene 
    scene.registerModel(&lamp);
    scene.registerModel(&troglodyte);

    scene.registerModel(&sphere);

    // Load all model data
    scene.loadModels();
    
    
    DirectLight directLight = {glm::vec3(-0.2f, -1.0f, -0.3f), 
                            glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
                            glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), 
                            glm::vec4(0.75f, 0.75f, 0.75f, 1.0f) }; 
    scene.dirLight = &directLight; 
 
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f);
    float k0 = 1.0f;
    float k1 = 0.09;
    float k2 = 0.032f;

    PointLight pointLights[4];

    for (unsigned int i = 0; i < 4; ++i){
        pointLights[i] = {
            pointLightPositions[i],
            k0, k1, k2,
            ambient, diffuse, specular
        };
        scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]);
        scene.pointLights.push_back(&pointLights[i]);
        States::activate(&scene.activePointLights, i);
    }
    
    scene.generateInstance(troglodyte.id, glm::vec3(1.0f), 30.0f, glm::vec3(0.0f, 0.0f, -6.0f));
    // Cube cube(Material::emerald, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.75f));
    // cube.init();


    SpotLight spotLight = {
        // cameras[activeCam].cameraPos, cameras[activeCam].cameraFront,
        cam.cameraPos, cam.cameraFront,
        glm::cos(glm::radians(12.5f)),
        glm::cos(glm::radians(20.f)),
        1.0f, 0.07f, 0.032f,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
    };
    scene.spotLights.push_back(&spotLight);
    scene.activeSpotLights = 1; //0b00000001

    // Instantiate instances
    scene.initInstances();

    mainJ.update();
    if(mainJ.isPresent()){
        std::cout << mainJ.getName() << " is present." << std::endl;
    } else {
        std::cout << "Not present." << std::endl;
    }

    // Main engine
    while (!scene.shouldClose()) {
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
		
        // process input
		processInput(deltaTime);
        
		
        // clear render
		scene.update();

        // Remove launch object if too far
        for (int i = 0; i < sphere.currentNoInstances; ++i){
            if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) {
                scene.markForDeletion(sphere.instances[i]->instanceId);
            }
        }

        // Render launch objects
        if (sphere.currentNoInstances > 0) {
            scene.renderShader(shader);
            scene.renderInstances(sphere.id, shader, deltaTime);
        }


        // Render lamps
        scene.renderShader(lampShader);
        scene.renderInstances(lamp.id, lampShader, deltaTime);

        // Render troglodyte
        scene.renderShader(troglodyteShader);
        scene.renderInstances(troglodyte.id, troglodyteShader, deltaTime);
        
        // send new frame to window
        scene.clearDeadInstances();
		scene.newFrame();
        // }

        timer += deltaTime;
        if(timer >= 1.0f){
            // timer -= (int)timer;
            timer = 0.0f;
            std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
            currentFPS = 0;
        }

    }
    // Cleanup objects
    scene.cleanup();
    return EXIT_SUCCESS;
}

void launchItem(float dt){
    RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(1.0f), 1.0f, cam.cameraPos);
    if (rb){
        // Instance generated
        rb->transferEnergy(100.0f, cam.cameraFront);
        rb->applyAcceleration(Environment::gravitationalAcceleration);
    }
}

void processInput(double deltaTime) {
    scene.processInput(deltaTime);

    // Update flash light
    if(States::isIndexActive(&scene.activeSpotLights, 0)) {
        scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
        scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
    }

    if (Keyboard::key(GLFW_KEY_ESCAPE) || mainJ.buttonState(GLFW_JOYSTICK_BTN_RIGHT)){
        scene.setShouldClose(true);
    }

    if(Keyboard::keyWentDown(GLFW_KEY_F)){
        // spotLightOn = !spotLightOn;
        States::toggleIndex(&scene.activeSpotLights, 0);
    }

    if(Keyboard::keyWentDown(GLFW_KEY_TAB)){
        activeCam += (activeCam == 0) ? 1 : -1;
    }

    for(int i = 0; i < 4; ++i){
        if(Keyboard::keyWentDown(GLFW_KEY_1 + i)) {
            States::toggleIndex(&scene.activePointLights, i);
        }
    }

    if (Keyboard::keyWentDown(GLFW_KEY_E)){
    // if (Keyboard::key(GLFW_MOUSE_BUTTON_LEFT)){
        // sphere.rb.applyImpulse(Camera::defaultCamera.cameraFront, 500.0f, deltaTime);
        launchItem(deltaTime);
    }

    // Joystick inputs
    mainJ.update();
}
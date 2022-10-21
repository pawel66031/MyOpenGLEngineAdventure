#include "Scene.hpp"

unsigned int Scene::scrWidth = 0;
unsigned int Scene::scrHeight = 0;

std::string Scene::generateId() {
    for (int i = currentId.length() - 1; i >= 0; --i){
        if((int)currentId[i] != (int)'z') {
            currentId[i] = (char)(((int)currentId[i]) + 1);
            break;
        }
        else {
            currentId[i] = 'a';
        }
    }
    return currentId;
}

/*  Callbacks */
// Window resize
void Scene::framebufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    Scene::scrWidth = width;
    Scene::scrHeight = height;
}

/*
    constructor
*/
Scene::Scene() 
    : currentId("aaaaaaa"){}
Scene::Scene(int glfwVersionMajor, int glfwVersionMinor,
    const char* title, unsigned int scrWidth, unsigned int scrHeight)
    : glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor),
    title(title),
    activeCamera(-1),
    activePointLights(0), activeSpotLights(0),
        currentId("aaaaaaa") {

        Scene::scrWidth = scrWidth;
        Scene::scrHeight = scrHeight;

        setWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
    }

/*
    initialization
*/
bool Scene::init(){
    if(!glfwInit())
        return false;

    // Set version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // initialize window
    window = glfwCreateWindow(scrWidth, scrHeight, title, NULL, NULL);
    if (window == NULL){
        // not created
        return false;
    }
    glfwMakeContextCurrent(window);

    // Set GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    // Setup screen
    glViewport(0, 0, scrWidth, scrHeight);

    /*
        Callbacks
    */
    // Frame size
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Key pressed
    glfwSetKeyCallback(window, Keyboard::keyCallback);
    // Cursor moved
    glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
    // Mouse button pressed
    glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
    // Mouse scroll
    glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

    /*
        Set rendering parameters
    */
    glEnable(GL_DEPTH_TEST); // Doesn't show vertices not visible to camera (back of object)
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor

    return true;
}

/*
    main loop methods
*/
// process input
void Scene::processInput(float deltaTime){
    if (activeCamera != -1 && activeCamera < cameras.size()){
        // Active camera exist

        // Set camera direction
        // if (!(Mouse::getDX() == 0 && Mouse::getDY() == 0)){

        double dx = Mouse::getDX(), dy = Mouse::getDY();
        if (dx != 0 || dy != 0){
            cameras[activeCamera]->updateCameraDirection(dx, dy);
        }
        // }

        // Set camera zoom
        double scrollDy = Mouse::getScrollDY();
        if (scrollDy != 0){
            cameras[activeCamera]->updateCameraZoom(scrollDy);
        }
    
        // Set camera position
        if(Keyboard::key(GLFW_KEY_W)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::FORWARD, deltaTime);
        }
        if(Keyboard::key(GLFW_KEY_S)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::BACKWARD, deltaTime);
        }
        if(Keyboard::key(GLFW_KEY_A)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::LEFT, deltaTime);
        }
        if(Keyboard::key(GLFW_KEY_D)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::RIGHT, deltaTime);
        }
        if(Keyboard::key(GLFW_KEY_SPACE)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::UP, deltaTime);
        }
        if(Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
            cameras[activeCamera]->updateCameraPos(CameraDirection::DOWN, deltaTime);
        }

        // Set matrices
        view = cameras[activeCamera]->getViewMatrix();
        projection = glm::perspective(
            glm::radians(cameras[activeCamera]->getZoom()), // FOV
            (float)scrWidth / (float)scrHeight,             // Aspect ratio
            0.1f, 100.0f                                     // Near and far bound
        );

        // Set position at end
        cameraPos = cameras[activeCamera]->cameraPos;
    }
}


// Update screen before each frame
void Scene::update(){
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Update screen after frame
void Scene::newFrame(){
    // Send new frame to window
    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Set uniform shader variables (lighting, etc.)
void Scene::renderShader(Shader shader, bool applyLighting){
    // Activate shader
    shader.activate();

    // Set camera values
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.set3Float("viewPos", cameraPos);

    // Lighting
    if (applyLighting) {
        //  Point lights
        unsigned int noLights = pointLights.size();
        unsigned int noActiveLights = 0;
        for(unsigned int i = 0; i < noLights; ++i){
            if (States::isActive(&activePointLights, i)) {
                // i'th light is active
                pointLights[i]->render(shader, noActiveLights);
                ++noActiveLights;
            }
        }
        shader.setInt("noPointLights", noActiveLights);

        // Spot lights
        noLights = spotLights.size();
        noActiveLights = 0;
        for(unsigned int i = 0; i < noLights; ++i){
            if(States::isActive(&activeSpotLights, i)){
                // i'th spot light active
                spotLights[i]->render(shader, noActiveLights);
                ++noActiveLights;
            }
        }
        shader.setInt("noSpotLights", noActiveLights);

        // Directional light
        dirLight->render(shader);
    }
}

void Scene::renderInstances(std::string modelId, Shader shader, float dt) {
    models[modelId]->render(shader, dt, this);
}

/*
    cleanup method
*/
void Scene::cleanup(){
    models.traverse([](Model* model) -> void {
        model->cleanup();
    });
    
    glfwTerminate();
}

/*
accessors
*/
bool Scene::shouldClose(){
    return glfwWindowShouldClose(window);
}

Camera* Scene::getActiveCamera(){
    return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

/*
    modifiers
*/
void Scene::setShouldClose(bool shouldClose){
    glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::setWindowColor(float r, float g, float b, float a){
    bg[0] = r;
    bg[1] = g;
    bg[2] = b;
    bg[3] = a;
}

/*
    Model/instance methods
*/
void Scene::registerModel(Model* model){
    models.insert(model->id, model);
}

RigidBody* Scene::generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos){
    RigidBody* rb = models[modelId]->generateInstance(size, mass, pos);
    if(rb){
        // Successfully generated
        std::string id = generateId();
        rb->instanceId = id;
        instances.insert(id, rb);
        return rb;
    }
    return nullptr;
}

void Scene::initInstances(){
    models.traverse([](Model* model)-> void {
        model->initInstances();
    });
}

void Scene::loadModels(){
    models.traverse([](Model* model)-> void {
        model->init();
    });
}

void Scene::removeInstance(std::string instanceId){
    /*
        Remove all locations
        - Scene::instances
        - Model::instances
    */

   std::string targetModel = instances[instanceId]->modelId;

   models[targetModel]->removeInstance(instanceId);

   instances[instanceId] = nullptr;

   instances.erase(instanceId);
}

void Scene::markForDeletion(std::string instanceId){
    States::activate(&instances[instanceId]->state, INSTANCE_DEAD);
    instancesToDelete.push_back(instances[instanceId]);
}

void Scene::clearDeadInstances(){
    for (RigidBody* rb : instancesToDelete) {
        removeInstance(rb->instanceId);
    }
    instancesToDelete.clear();
}
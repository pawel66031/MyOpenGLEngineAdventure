#ifndef SCENE_HPP
#define SCENE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include "graphics/Light.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Model.hpp"

#include "io/Camera.hpp"
#include "io/Keyboard.hpp"
#include "io/Mouse.hpp"

#include "algorithms/States.hpp"
#include "algorithms/Trie.hpp"

class Model;

class Scene{
public:
    trie::Trie<Model*> models;
    trie::Trie<RigidBody*> instances;

    std::vector<RigidBody*> instancesToDelete;

    // ###  Callbacks ###
    // Window resize
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    /*
        constructor
    */
   Scene();
   Scene(int glfwVersionMajor, int glfwVersionMinor,
        const char* title, unsigned int scrWidth, unsigned int scrHeight);

    /*
        initialization
    */
   bool init();

    /*
        main loop methods
    */
    // process input
    void processInput(float dt);

    // Update screen before each frame
    void update();

    // Update screen after frame
    void newFrame();

    // Set uniform shader variables (lighting, etc.)
    void renderShader(Shader shader, bool applyLighting = true);

    void renderInstances(std::string modelId, Shader shader, float dt);

    /*
        cleanup method
    */
   void cleanup();

   /*
    accessors
   */
    bool shouldClose();

    Camera* getActiveCamera();

    /*
        modifiers
    */
    void setShouldClose(bool shouldClose);

    void setWindowColor(float r, float g, float b, float a);

    /*
        Model/instance methods
    */
   void registerModel(Model* model);

   RigidBody* generateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos);

   void initInstances();

   void loadModels();

   void removeInstance(std::string instanceId);

   void markForDeletion(std::string instanceId);

   void clearDeadInstances();

   std::string currentId;
   std::string generateId();

    /*
        Lights
    */
   // List of point lights
   std::vector<PointLight*> pointLights;
   unsigned int activePointLights = 0;

   // List of spot lights
   std::vector<SpotLight*> spotLights;
   unsigned int activeSpotLights = 0;
   // Direction light
   DirectLight* dirLight;
   bool dirLightActive;

    /*
        Camera
    */
    std::vector<Camera*> cameras;
    unsigned int activeCamera;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 cameraPos;

protected:
    // Window object
    GLFWwindow* window;

    // window vals
    const char* title;
    static unsigned int scrWidth;
    static unsigned int scrHeight;

    float bg[4]; // Background color

    // GLFW version
    int glfwVersionMajor;
    int glfwVersionMinor;
};

#endif //SCENE_HPP
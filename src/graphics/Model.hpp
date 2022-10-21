#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Mesh.hpp"

#include "models/Box.hpp"

#include "../physics/RigidBody.hpp"
#include "../algorithms/Bounds.hpp"

#include "../Scene.hpp"

// Model switches
#define DYNAMIC             (unsigned int)1
#define CONST_INSTANCES     (unsigned int)2
#define NO_TEX              (unsigned int)4

class Scene; // Forward declaration

class Model {
public:
    std::string id;

    // glm::vec3 pos;
    RigidBody rb;
    glm::vec3 size;

    BoundTypes boundType;

    std::vector<Mesh> meshes;
    std::vector<BoundingRegion> boundingRegions;

    std::vector<RigidBody*> instances;

    unsigned int maxNoInstances;
    unsigned int currentNoInstances;

    unsigned int switches;

    Model(std::string id, BoundTypes boundType, unsigned int maxNoInstances, unsigned int flags = 0);

    // Initialize method
    virtual void init();

    RigidBody* generateInstance(glm::vec3 size, float mass, glm::vec3 pos);

    void initInstances();

    void loadModel(std::string path);

    virtual void render(Shader shader, float dt, Scene *scene, bool setModel = true);

    void cleanup();

    void removeInstance(unsigned int idx);

    void removeInstance(std::string instanceId);

    unsigned int getIdx(std::string id);

protected:
    bool noTex; // No texture

    std::string directory;

    std::vector<Texture> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

    // VBOs for positions and sizes
    BufferObject posVBO;
    BufferObject sizeVBO;
};

#endif //MODEL_H
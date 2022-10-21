#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"
#include "glMemory.hpp"

#include "models/Box.hpp"

#include "../algorithms/Bounds.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static std::vector<Vertex> genList(float* vertices, int noVertices);
};

class Mesh {
public:
    BoundingRegion br;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ArrayObject VAO;

    std::vector<Texture> textures;
    aiColor4D diffuse;
    aiColor4D specular;

    // default constructor 
    Mesh();
 
    // initialize as textured object
    Mesh(BoundingRegion br, std::vector<Texture> textures = {});
 
    // initialize as material object
    Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);
 
    // load vertex and index data
    void loadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void render(Shader shader, unsigned int noInstances);

    void cleanup();

private:
    unsigned int VBO, EBO;

    bool noTex;

    void setup();
};

#endif //MESH_H
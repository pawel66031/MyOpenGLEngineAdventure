#include "Mesh.hpp"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices){
    
    std::vector<Vertex> ret(noVertices);

    int stride = sizeof(Vertex) / sizeof(float);

    for(int i = 0; i < noVertices; ++i){
        int index = 0;
        ret[i].pos = glm::vec3(
            vertices[i * stride + index++],
            vertices[i * stride + index++],
            vertices[i * stride + index++]
        );

        ret[i].normal = glm::vec3(
            vertices[i * stride + index++],
            vertices[i * stride + index++],
            vertices[i * stride + index++]
        );

        ret[i].texCoord = glm::vec2(
            vertices[i * stride + index++],
            vertices[i * stride + index++]
        );
    }

    return ret;
}

// default constructor
Mesh::Mesh() {}
 
// initialize as textured object
Mesh::Mesh(BoundingRegion br, std::vector<Texture> textures)
    : br(br), textures(textures), noTex(false) {}
 
// initialize as material object
Mesh::Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec)
    : br(br), diffuse(diff), specular(spec), noTex(true) {}
 
// load vertex and index data
void Mesh::loadData(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices) {
    this->vertices = _vertices;
    this->indices = _indices;
 
    // bind VAO
    VAO.generate();
    VAO.bind();
 
    // generate/set EBO
    VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
    VAO["EBO"].generate();
    VAO["EBO"].bind();
    VAO["EBO"].setData<GLuint>(this->indices.size(), &this->indices[0], GL_STATIC_DRAW);
 
    // load data into vertex buffers
    VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
    VAO["VBO"].generate();
    VAO["VBO"].bind();
    VAO["VBO"].setData<Vertex>(this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);
 
    // set the vertex attribute pointers
    VAO["VBO"].bind();
    // vertex Positions
    VAO["VBO"].setAttrPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
    // normal ray
    VAO["VBO"].setAttrPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
    // vertex texture coords
    VAO["VBO"].setAttrPointer<GLfloat>(2, 3, GL_FLOAT, 8, 6);
 
    VAO["VBO"].clear();
 
    ArrayObject::clear();
}

void Mesh::render(Shader shader, unsigned int noInstances){
    if (noTex) {
        // Materials
        shader.set4Float("material.diffuse", diffuse);
        shader.set4Float("material.specular", specular);
        shader.setInt("noTex", 1);
    }
    else {
        // Textures
        unsigned int diffuseIdx = 0;
        unsigned int specularIdx = 0;

        for(unsigned int i = 0; i < textures.size(); ++i){
            // Activate texture
            glActiveTexture(GL_TEXTURE0 + i);

            // Retrieve texture info
            std::string name;
            switch(textures[i].type){
                case aiTextureType_DIFFUSE:
                    name = "diffuse" + std::to_string(diffuseIdx++);
                    break;
                case aiTextureType_SPECULAR:
                    name = "specular" + std::to_string(specularIdx++);
                    break;
            }

            // Set the shader value
            shader.setInt(name, i);
            // Bind texture
            textures[i].bind();
        }
    }

    VAO.bind();
    VAO.draw(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, noInstances);
    ArrayObject::clear();

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup(){
    // Bind VAO
    VAO.generate();
    VAO.bind();

    // Generate/set EBO
    VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
    VAO["EBO"].generate();
    VAO["EBO"].bind();
    VAO["EBO"].setData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);

    // Generate/set VBO
    VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
    VAO["VBO"].generate();
    VAO["VBO"].bind();
    VAO["VBO"].setData<Vertex>(vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Set vertex attrib pointers
    // Vertex positions
    VAO["VBO"].setAttrPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
    // Normal array
    VAO["VBO"].setAttrPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
    // Texture coordinates
    VAO["VBO"].setAttrPointer<GLfloat>(2, 2, GL_FLOAT, 8, 6);

    VAO["VBO"].clear();

    ArrayObject::clear();
}

void Mesh::cleanup(){
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    VAO.cleanup();
}
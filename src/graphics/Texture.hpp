#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

class Texture {
public:
    Texture();
    Texture(std::string dir, std::string path, aiTextureType type);

    void generate();
    void load(bool flip = true);

    void bind();

    unsigned int id;
    aiTextureType type;
    std::string dir;
    std::string path;
};

#endif //TEXTURE_H
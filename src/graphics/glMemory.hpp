#ifndef GLMEMORY_HPP
#define GLMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

/*
    Class for buffer objects
    -VBOs, EBOs, etc
*/
class BufferObject {
public:
    // Value/location
    GLuint val;
    // Type of buffer (GL_ARRAY_BUFFER || GL_ELEMENT_ARRAY_BUFFER, etc)
    GLenum type;

    BufferObject() {}

    BufferObject(GLenum type) 
        : type(type) {}

    // Generate object
    void generate() {
        glGenBuffers(1, &val);
    }

    // Bind object
    void bind(){
        glBindBuffer(type, val);
    }

    // Set data (glBufferData)
    template<typename T>
    void setData(GLuint noElements, T* data, GLenum usage) {
        glBufferData(type, noElements * sizeof(T), data, usage);
    }

    // Update data (glBufferSubData)
    template<typename T>
    void updateData(GLintptr offset, GLuint noElements, T* data) {
        glBufferSubData(type, offset, noElements * sizeof(T), data);
    }

    // Set attribute pointers
    template<typename T>
    void setAttrPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0) {
        glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
        glEnableVertexAttribArray(idx);
        if(divisor > 0){
            // Reset _idx_ attribute every _divisor iteration (instancing)
            glVertexAttribDivisor(idx, divisor);
        }
    }

    // Clear buffer objects (bind 0)
    void clear(){
        glBindBuffer(type, 0);
    }

    // Cleanup
    void cleanup(){
        glDeleteBuffers(1, &val);
    }
};

/*
    Structure class for array objects
    - VAO
*/
class ArrayObject {
public:
    // Value/location
    GLuint val;

    // Map of names to buffers
    std::map<const char*, BufferObject> buffers;

    // Get buffer (override [])
    BufferObject& operator[](const char* key) {
        return buffers[key];
    }

    // Generate object
    void generate(){
        glGenVertexArrays(1, &val);
    }

    // Bind
    void bind() {
        glBindVertexArray(val);
    }

    // Draw
    void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instanceCount = 1){
        glDrawElementsInstanced(mode, count, type, (void*)indices, instanceCount);
    }

    // Cleanup
    void cleanup(){
        glDeleteVertexArrays(1, &val);
        for (auto& pair : buffers) {
            pair.second.cleanup();
        }
    }

    // Clear array object (bind 0)
    static void clear(){
        glBindVertexArray(0);
    }
};

#endif //GLMEMORY_HPP
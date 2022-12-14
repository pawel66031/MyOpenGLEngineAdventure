#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aOffset;
layout (location = 4) in vec3 aSize;

//out vec3 ourColor;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model; //set in code
uniform mat4 view; //set in code
uniform mat4 projection; //set in code

void main(){
    // vec3 pos = vec3(aPos.x * aSize.x, aPos.y * aSize.y, aPos.z * aSize.z);
    vec3 pos = aPos * aSize + aOffset;

    // FragPos = vec3(model * vec4(pos + aOffset, 1.0));
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoord = aTexCoord;
}
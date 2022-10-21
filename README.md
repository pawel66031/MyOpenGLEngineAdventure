# Game engine powered with OpenGL
This repository shows my process of learning OpenGL. Right now is a very basic game that lets you to move freely camera, toggle lights and shoot spheres. In order to make program work, it's recommended to add classes each to implement behaviour, like mouse/keyboard events, collecting mesh datas, compiling shaders etc.\
The reason of creating game engine in OpenGL is a passion of games and to create first big own project that will be compatible with most operation systems like Windows, Linux or MacOS. For that moment, it's possible to run on Windows (tested with MSYS2) and Linux (X11).

## Libraries
Some libraries needs to be included in order to run engine:
* GLFW (Graphics Library Framework) - easily creates window and react on keyboard keys and mouse movement
* GLM (OpenGL Mathematics) - GLSL 
* STB - loads textures for 3D model
# cmake_minimum_required(VERSION 3.0.0)

# get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
# project(${PROJECT_NAME})

add_subdirectory(io)
add_subdirectory(physics)

set(THIS_PROJECT_SRC_DIRECTORIES
    Shader.cpp
)

# add_executable(${PROJECT_NAME}
#                 main.cpp
#                 ${THIS_PROJECT_SRC_DIRECTORIES}
# )

add_library(shaders
        graphics/Light.cpp
        graphics/Light.hpp
        graphics/Material.hpp
        graphics/Material.cpp
        graphics/Mesh.hpp
        graphics/Mesh.cpp
        graphics/Model.cpp
        graphics/Model.hpp
        graphics/Shader.cpp
        graphics/Shader.hpp
        graphics/Texture.hpp
        graphics/Texture.cpp
        # graphics/models/Box.hpp
        graphics/models/Cube.hpp
        
        Scene.hpp
        Scene.cpp
        )
add_library(user_algorithms
    algorithms/Bounds.cpp
    algorithms/Bounds.hpp
    algorithms/List.hpp
    algorithms/Octree.cpp
    algorithms/Octree.hpp
    algorithms/States.hpp
    algorithms/Trie.hpp
)

# foreach(item IN LISTS shaders)
#     message(STATUS ${item})    
# endforeach()

# target_include_directories(algorithms PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/algorithms")
target_include_directories(shaders PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(user_algorithms PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
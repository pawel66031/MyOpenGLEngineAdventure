cmake_minimum_required(VERSION 3.0.0)

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(MyOpenGLAdventure)

set (PROJECT_MAJOR 0)
set (PROJECT_MINOR 0)
set (PROJECT_PATCH 1)
set (PROJECT_VERSION "${PROJECT_MAJOR}.${PROJECT_MINOR}.${PROJECT_PATCH}")

set(CMAKE_CXX_STANDARD 17)

# include_directories(include)
# include(CTest)
# enable_testing()


# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

if(OPENGL_FOUND)
message(STATUS "opengl found")
else(OPENGL_FOUND)
message(STATUS "opengl not found")
endif()

# target_link_libraries(MyOpenGLAdventure ${OPENGL_gl_LIBRARY})


add_subdirectory(src)

# # Third party libraries
# add_library(stb
#             include/stb/stb_image.h)

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
  set(CMAKE_REQUIRED_INCLUDES /usr/local/include)
endif()
      

# include_directories(${MyOpenGLAdventure_SOURCE_DIR}/include
#                     /usr/local/include/stb/stb_image.h
#                     /usr/local/include
# )

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/glad.c
    )

# target_include_directories(${PROJECT_NAME} PUBLIC 
#             "usr/include"
#             "include"
# )

# target_link_libraries(MyOpenGLAdventure ${GLFW_LIBRARY})
set(USER_LINK_LIBRARIES 
      shaders
      user_algorithms
      physics
      assimp
      io
)

if (UNIX)
target_link_libraries(${PROJECT_NAME}
                    glfw
                    GL
                    # glad
                    ${USER_LINK_LIBRARIES}
                    )
elseif(WIN32)
target_link_libraries(${PROJECT_NAME}
                    glfw3
                    opengl32
                    ${USER_LINK_LIBRARIES}
                    )        
endif()

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

add_library(physics
            RigidBody.cpp
            Environment.cpp
            )

target_include_directories(physics PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/physics")
add_library(io
            Camera.cpp
            Camera.hpp
            Keyboard.cpp
            Keyboard.hpp
            Mouse.cpp
            Mouse.hpp
            Joystick.cpp
            Joystick.hpp
            )

target_include_directories(io PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/io")
cmake_minimum_required(VERSION 3.16)
project(SurvivalHorror VERSION 1.0 LANGUAGES CXX)

# Enforce modern C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find OS-native OpenGL and GLU libraries (automatically handles opengl32/glu32 on Win, GL/GLU on Linux)
find_package(OpenGL REQUIRED)

# Find GLFW3 library
find_package(glfw3 CONFIG REQUIRED)

# Define all source files
# Define all source files
set(SOURCES
    src/main.cpp
    src/Engine.cpp
    src/Camera.cpp
    src/Scene.cpp
    src/TextureManager.cpp
    src/Debris.cpp
)

# Create the executable
add_executable(horror_engine ${SOURCES})

# Include directory so files inside src/ can include "Utils.h" cleanly
target_include_directories(horror_engine PRIVATE src)

# Link all required graphics libraries to our executable
target_link_libraries(horror_engine PRIVATE
    OpenGL::GL
    OpenGL::GLU
    glfw
)
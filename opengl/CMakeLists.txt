cmake_minimum_required(VERSION 3.16)
project(opengl)

set(CMAKE_CXX_STANDARD 14)

add_executable(opengl main.cpp Shader.h camera.h Mesh.h Model.h)

target_link_libraries(opengl glfw GL glut SOIL GLEW assimp ${GL_LIBRARY} m)
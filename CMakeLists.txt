cmake_minimum_required(VERSION 2.8)
project(OpenCV_camera_in_OpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(lib
    src/opengl_helper.cpp
    src/opengl_helper.hpp
    src/left09.cpp
    src/left09.hpp
)

add_executable(main src/main.cpp)
target_link_libraries(main lib GL glfw GLEW)

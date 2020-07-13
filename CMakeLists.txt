cmake_minimum_required(VERSION 2.8)
project (OpenCV_camera_in_OpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(main src/main.cpp src/opengl_helper.cpp src/opengl_helper.hpp)
target_link_libraries(main GL glfw GLEW)

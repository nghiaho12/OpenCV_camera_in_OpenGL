#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <sstream>
#include <vector>

#include "opengl_helper.hpp"
#include "left09.hpp"

// Texture shader
// Grayscale image only
const std::string VERTEX_SHADER = R"###(
#version 330 core
layout(location = 0) in vec4 vertexPosTexCoord;

uniform mat4 mvp;
out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(vertexPosTexCoord.x, vertexPosTexCoord.y, 0.0, 1.0);
    texCoord = vertexPosTexCoord.zw;
}
)###";

const std::string FRAGMENT_SHADER = R"###(
#version 330 core

in vec2 texCoord;
uniform sampler2D ourTexture;
out vec4 color;

void main()
{
    float r = texture(ourTexture, texCoord).r;
    color = vec4(r, r, r, 1.0);
    //color = vec4(1,1,1,1);
}
)###";

static void error_callback(int error, const char* description)
{
    std::cerr << "glfw error: " << description << "\n";
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "glfwInit failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(left09_width(), left09_height(), "OpenCV camera in OpenGL example", NULL, NULL);

    if (!window) {
        std::cerr << "glfwCreateWindow failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();

    if (GLEW_OK != err) {
        std::cerr << "glewInit error: " << glewGetErrorString(err) << "\n";
        return -1;
    }

    std::cout << "Using GLEW " <<glewGetString(GLEW_VERSION) << "\n";

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLuint vertex_buffer;
    GLuint vertex_array;
    GLuint texture_buffer;

    GL_CHECK(glGenVertexArrays(1, &vertex_array));
    GL_CHECK(glBindVertexArray(vertex_array));

    // vertices and texcoord for the texture quad
    float w = left09_width();
    float h = left09_height();
    const float vertices[] = {
        0.0, 0.0,   0.0, 0.0,
        0.0, h,     0.0, 1.0,
        w, 0.0,     1.0, 0.0,
        w, h,       1.0, 1.0};

    GL_CHECK(glGenBuffers(1, &vertex_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr));

    // load the texture
    GL_CHECK(glGenTextures(1, &texture_buffer));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture_buffer));
    GL_CHECK(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED, // grayscale image loaded into red channel
        left09_width(),
        left09_height(),
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        left09_data()));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLuint program = loadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    GLint mvp_location = glGetUniformLocation(program, "mvp");

    while (!glfwWindowShouldClose(window)) {
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        GL_CHECK(glViewport(0, 0, width, height));
        GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

        // NOTE: MUST cast as float else this will be treated as int and will silently do something unexpected!
        // Also we flip the y-axis so (0,0) is at the top left corner of the viewport
        glm::mat4 mvp = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

        GL_CHECK(glUseProgram(program));
        GL_CHECK(glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp)));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture_buffer));
        GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

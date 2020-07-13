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

// Texture shader
const std::string VERTEX_SHADER = R"###(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 mvp;
out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1);
    texCoord = vertexTexCoord;
}
)###";

const std::string FRAGMENT_SHADER = R"###(
#version 330 core

in vec2 texCoord;
uniform sampler2D ourTexture;
out vec4 color;

void main()
{
    //color = texture(ourTexture, texCoord);
    color = vec4(1,1,1,1);
}
)###";

static void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << "\n";
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

    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenCV camera in OpenGL example", NULL, NULL);

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

    // NOTE: OpenGL error checks have been omitted for brevity
    const float vertices[] = {
        0,0,0,
        0,100,0,
        100,0,0,
        100,100,0};

    GLuint vertex_buffer;
    GLuint vertex_array;

    GL_CHECK(glGenVertexArrays(1, &vertex_array));
    GL_CHECK(glBindVertexArray(vertex_array));

    GL_CHECK(glGenBuffers(1, &vertex_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

    GLuint program = loadShaders(VERTEX_SHADER, FRAGMENT_SHADER);

    GLint mvp_location = glGetUniformLocation(program, "mvp");

    while (!glfwWindowShouldClose(window)) {
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        GL_CHECK(glViewport(0, 0, width, height));
        GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

        // NOTE: MUST cast as float else this will silently treat it as int not do what you want!
        glm::mat4 mvp = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

        GL_CHECK(glUseProgram(program));
        GL_CHECK(glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp)));

        GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

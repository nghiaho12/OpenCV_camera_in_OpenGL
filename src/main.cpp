#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <vector>

#include "opengl_helper.hpp"
#include "shader.hpp"
#include "left09.hpp"

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
    // From OpenCV camera calibration for opencv/samples/data/left*.jpg

    // These units are in meters
    constexpr float square_size = 0.02;
    constexpr float board_width = 8 * square_size;
    constexpr float board_height = 5 * square_size;
    constexpr float board_depth = 0.07;

    // NOTE: We assume undistortion is already applied to the image eg. cv::undistort
    // Hence we don't use any distortion value in this example.
    constexpr float fx = 5.3646257838368388e+02;
    constexpr float fy = 5.3641495077527384e+02;
    constexpr float cx = 3.4236864003069093e+02;
    constexpr float cy = 2.3554895272852343e+02;

    // NOTE: glm is column first then row
    glm::mat4 camera_matrix(1.0);

    // typical 3x3 camera matrix but as 4x4 instead
    camera_matrix[0][0] = fx;
    camera_matrix[1][1] = fy;
    camera_matrix[2][0] = cx;
    camera_matrix[2][1] = cy;

    // extrinsics for opencv/samples/data/left09.jpg
    glm::mat4 board_pose(1.0);

    // rotation
    board_pose[0][0] = 0.9032419130191708;
    board_pose[0][1] = 0.08505603062460287;
    board_pose[0][2] = 0.4206180193713239;

    board_pose[1][0] = -0.1694525821415028;
    board_pose[1][1] = 0.9712014559118498;
    board_pose[1][2] = 0.1674919533597971;

    board_pose[2][0] = -0.3942586320818429;
    board_pose[2][1] = -0.2225605618457364;
    board_pose[2][2] = 0.8916428249808688;

    // translation
    board_pose[3][0] = -5.3109423342047581e-02;
    board_pose[3][1] = -6.4807198552484332e-02;
    board_pose[3][2] = 2.2278644271121698e-01;

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

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint texture_buffer;
    GLuint cuboid_vertex_buffer;
    GLuint cuboid_color_buffer;
    GLuint cuboid_index_buffer;

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

    // cuboid model
    w = board_width;
    h = board_height;

    const float cuboid_vertices[] = {
        0, 0, 0,
        w, 0, 0,
        w, h, 0,
        0, h, 0,
        0, 0, -board_depth,
        w, 0, -board_depth,
        w, h, -board_depth,
        0, h, -board_depth};

    const float cuboid_color[] = {
        1.0, 0.0, 0.0, 0.5,
        1.0, 0.0, 0.0, 0.5,
        1.0, 0.0, 0.0, 0.5,
        1.0, 0.0, 0.0, 0.5,
        0.0, 1.0, 0.0, 0.5,
        0.0, 1.0, 0.0, 0.5,
        0.0, 1.0, 0.0, 0.5,
        0.0, 1.0, 0.0, 0.5};

    const uint32_t cuboid_triangle_indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 4, 5,
        1, 2, 6,
        6, 5, 1,
        0, 4, 7,
        0, 3, 7,
        2, 3, 7,
        2, 6, 7};

    GL_CHECK(glGenBuffers(1, &cuboid_vertex_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cuboid_vertex_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cuboid_vertices), cuboid_vertices, GL_STATIC_DRAW));

    GL_CHECK(glGenBuffers(1, &cuboid_color_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cuboid_color_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cuboid_color), cuboid_color, GL_STATIC_DRAW));

    GL_CHECK(glGenBuffers(1, &cuboid_index_buffer));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboid_index_buffer));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cuboid_triangle_indices), cuboid_triangle_indices, GL_STATIC_DRAW));

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

    GLuint vertex_shader = loadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    GLuint texture_shader = loadShaders(TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER);

    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glEnableVertexAttribArray(1));

    while (!glfwWindowShouldClose(window)) {
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        GL_CHECK(glViewport(0, 0, width, height));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

        // OpenGL convention, -z is into the screen
        constexpr float zNear = 0.0;
        constexpr float zFar = -10;

        // Flip the y-axis so (0,0) is at the top left corner of the viewport
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, zNear, zFar);

        // Draw the texture
        GL_CHECK(glUseProgram(texture_shader));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(texture_shader, "mvp"), 1, GL_FALSE, glm::value_ptr(projection)));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture_buffer));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
        GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr));
        GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        // Draw the cuboid
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        GL_CHECK(glUseProgram(vertex_shader));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(vertex_shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(vertex_shader, "camera"), 1, GL_FALSE, glm::value_ptr(camera_matrix)));
        GL_CHECK(glUniformMatrix4fv(glGetUniformLocation(vertex_shader, "model"), 1, GL_FALSE, glm::value_ptr(board_pose)));

        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cuboid_vertex_buffer));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cuboid_color_buffer));
        GL_CHECK(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr));

        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboid_index_buffer));
        GL_CHECK(glDrawElements(GL_TRIANGLES, sizeof(cuboid_triangle_indices)/sizeof(cuboid_triangle_indices[0]),  GL_UNSIGNED_INT , 0));

        GL_CHECK(glDisable(GL_DEPTH_TEST));
        GL_CHECK(glDisable(GL_BLEND));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

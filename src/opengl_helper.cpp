#include <GL/glew.h>
#include <sstream>
#include <vector>

#include "opengl_helper.hpp"

void checkOpenGLError(const char* stmt, const char* fname, int line)
{
   GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::stringstream ss;

        ss << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt;
        throw std::runtime_error(ss.str());
    }
}

static void checkShader(GLuint shader_id)
{
    GLint result = GL_FALSE;
    int info_log_length;

    GL_CHECK(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result));
    GL_CHECK(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length));

    if (info_log_length > 0) {
        std::vector<char> message(info_log_length+1);
        GL_CHECK(glGetShaderInfoLog(shader_id, info_log_length, NULL, message.data()));
        throw std::runtime_error(message.data());
    }
}

// https://github.com/opengl-tutorials
GLuint loadShaders(const std::string &vertex_shader_code, const std::string &fragment_shader_code)
{
 // Create the shaders
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile Vertex Shader
    char const * vertex_source_pointer = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
    glCompileShader(vertex_shader_id);
    checkShader(vertex_shader_id);

    // Compile Fragment Shader
    char const * fragment_source_pointer = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
    glCompileShader(fragment_shader_id);
    checkShader(fragment_shader_id);

    // Link the program
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    GLint result = GL_FALSE;
    int info_log_length;

    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0) {
        std::vector<char> program_error_message(info_log_length+1);
        glGetProgramInfoLog(program_id, info_log_length, NULL, program_error_message.data());
        throw std::runtime_error(program_error_message.data());
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

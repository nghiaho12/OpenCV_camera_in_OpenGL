#pragma once

void checkOpenGLError(const char* stmt, const char* fname, int line);

#ifdef GL_CHECK
#error "GL_CHECK already defined somewhere else!"
#endif

#define GL_CHECK(stmt) do { \
        stmt; \
        checkOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

GLuint loadShaders(const std::string &vertex_shader_code, const std::string &fragment_shader_code);

#pragma once

#include <string>

// Very basic vertex shader
static const std::string VERTEX_SHADER = R"###(
#version 330 core
layout(location = 0) in vec3 vertexPosition;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1);
}
)###";

// Fixed color fragment shader
static const std::string FRAGMENT_SHADER = R"###(
#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}
)###";

// Texture shader, grayscale image only
const std::string TEXTURE_VERTEX_SHADER = R"###(
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

const std::string TEXTURE_FRAGMENT_SHADER = R"###(
#version 330 core

in vec2 texCoord;
uniform sampler2D ourTexture;
out vec4 color;

void main()
{
    float r = texture(ourTexture, texCoord).r;
    color = vec4(r, r, r, 1.0);
}
)###";
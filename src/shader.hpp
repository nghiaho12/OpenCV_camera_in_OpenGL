#pragma once

#include <string>

static const std::string VERTEX_SHADER = R"###(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 projection;
uniform mat4 camera;
uniform mat4 model;
out vec4 color;

void main()
{
    // project to 2d
    vec4 v = camera * model * vec4(vertexPosition, 1);

    // NOTE: v.z is left untounched to maintain depth information!
    v.xy /= v.z;

    // To NDC
    gl_Position = projection * v;

    color = vertexColor;
}
)###";

static const std::string FRAGMENT_SHADER = R"###(
#version 330 core

in vec4 color;
out vec4 out_color;

void main()
{
    out_color = color;
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
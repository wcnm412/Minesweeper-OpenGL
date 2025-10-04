#version 460 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColour;

out vec3 fragmentColour;

uniform mat4 MVP;

void main() {
    // gl_Position.xyz = vertexPosition_modelspace;
    // gl_Position.w = 1.0;

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    fragmentColour = vertexColour;
}
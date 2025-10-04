#version 460 core
in vec3 fragmentColour;

out vec3 colour;

void main() {
    colour = fragmentColour;
}
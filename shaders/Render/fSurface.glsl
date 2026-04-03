#version 460 core

uniform vec3 inColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(inColor, 1.0);
}
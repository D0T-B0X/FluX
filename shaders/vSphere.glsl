#version 460 core

layout (location = 0) in vec3   aPos;
layout (location = 1) in vec3   fv3WorldPosition;
layout (location = 2) in vec3   fv3Color;

out vec3  inColor;
out vec3  normalPos;
out vec3  FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform float fRadius;

void main() {
    // ----------- Fragment Shader Data ---------- 

    inColor = fv3Color;

    // ----------- Vertex Shader Code ------------
    // calculate and pass normal to fragement shader
    normalPos = normalize(aPos);

    // transform to world space
    vec3 modelPos = normalPos * fRadius + fv3WorldPosition;
    FragPos = modelPos;

    // apply view and projection transformation (eventually)
    vec4 finalPos = projection * view * vec4(FragPos, 1.0);

    // pass final position data to the pipeline
    gl_Position = finalPos;
}
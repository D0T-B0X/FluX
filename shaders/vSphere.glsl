#version 460 core

layout (location = 0) in vec3   aPos;
layout (location = 1) in vec3   fv3WorldPosition;
layout (location = 2) in vec3   fv3Color;
layout (location = 3) in float  fRadius;
layout (location = 4) in float  fEmitter;

out vec3  inColor;
out float emitter;

// uniform mat4 view;
// uniform mat4 projection;

void main() {
    // ----------- Fragment Shader Data ---------- 

    inColor = fv3Color;
    emitter = fEmitter;

    // ----------- Vertex Shader Code ------------

    // transform to world space
    vec3 modelPos = normalize(aPos) * fRadius + fv3WorldPosition;

    // apply view and projection transformation (eventually)
    vec4 finalPos = vec4(modelPos, 1.0);

    // pass final position data to the pipeline
    gl_Position = finalPos;
}
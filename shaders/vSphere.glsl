#version 460 core

layout (location = 0) in vec3   aPos;
layout (location = 1) in vec4   position_mass;
layout (location = 2) in vec4   color_padding;
layout (location = 3) in vec4   velocity_padding;

out vec3  inColor;
out vec3  normalPos;
out vec3  FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform float fRadius;

void main() {
    // ----------- Fragment Shader Data ---------- 

    inColor = color_padding.xyz;

    // ----------- Vertex Shader Code ------------
    // calculate and pass normal to fragement shader
    normalPos = normalize(aPos);

    vec3 fv3WorldPosition = position_mass.xyz;

    // transform to world space
    vec3 modelPos = normalPos * fRadius + fv3WorldPosition;
    FragPos = modelPos;

    // apply view and projection transformation
    vec4 finalPos = projection * view * vec4(FragPos, 1.0);

    // pass final position data to the pipeline
    gl_Position = finalPos;
}
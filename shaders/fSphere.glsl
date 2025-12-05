#version 460 core
 
out vec4 FragColor;

in vec3  inColor;
in float emitter;

void main() {
    // set emission status
    bool bEmitter = (emitter > 0.5);

    // set output color
    FragColor = vec4(inColor, 1.0);
}
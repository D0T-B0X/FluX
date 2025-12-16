#version 460 core
 
out vec4 FragColor;

in vec3  inColor;
in vec3  normalPos;
in vec3  FragPos;

uniform vec3 lightSourcePosition;
uniform vec3 lightColor;

void main() {
    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightSourcePosition - FragPos);

    float diff = max(dot(normalPos, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 finalColor = (diffuse + ambient) * inColor;    

    // set output color
    FragColor = vec4(finalColor, 1.0);
}
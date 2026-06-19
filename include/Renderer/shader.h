#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

enum ShaderMesh {
    SPHERE_SHADER,
    SURFACE_SHADER
};

class Shader {
public:
    Shader();

    // Render specific shader functions
    void load(ShaderMesh type, const char* vertexPath, const char* fragmentPath);
    void use(ShaderMesh type);
    void setBool(ShaderMesh type, const char* name, int val);
    void setInt(ShaderMesh type, const char* name, int val);
    void setFloat(ShaderMesh type, const char* name, float val);
    void setVec3(ShaderMesh type, const char* name, glm::vec3 vec);
    void setMat4(ShaderMesh type, const char* name, glm::mat4 mat);

    // Physics specfic shader functions
    // (yes just normal shader functions without a type, cant be helped) 
    void load(const char* computePath);
    void use();
    void setBool(const char* name, int val);
    void setInt(const char* name, int val);
    void setFloat(const char* name, float val);
    void setVec3(const char* name, glm::vec3 vec);
    void setMat4(const char* name, glm::mat4 mat);

private:

    uint SphereID;
    uint SurfaceID; 
    uint PhysicsID;

    void checkCompileErrors(uint shader, const char* type, ShaderMesh mType); // Render
    void checkCompileErrors(uint shader, const char* type);                 // Physics
};

#endif

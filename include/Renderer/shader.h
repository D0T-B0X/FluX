#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

enum MeshType {
    SPHERE,
    SURFACE
};

class Shader {
public:
    Shader();

    // Render specific shader functions
    void load(MeshType type, const char* vertexPath, const char* fragmentPath);
    void use(MeshType type);
    void setBool(MeshType type, const char* name, int val);
    void setInt(MeshType type, const char* name, int val);
    void setFloat(MeshType type, const char* name, float val);
    void setVec3(MeshType type, const char* name, glm::vec3 vec);
    void setMat4(MeshType type, const char* name, glm::mat4 mat);

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

    void checkCompileErrors(uint shader, const char* type, MeshType mType); // Render
    void checkCompileErrors(uint shader, const char* type);                 // Physics
};

#endif

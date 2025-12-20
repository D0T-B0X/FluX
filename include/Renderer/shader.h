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

    void load(MeshType type, const char* vertexPath, const char* fragmentPath);
    void use(MeshType type);
    void setBool(MeshType type, const char* name, int val);
    void setInt(MeshType type, const char* name, int val);
    void setFloat(MeshType type, const char* name, float val);
    void setVec3(MeshType type, const char* name, glm::vec3 vec);
    void setMat4(MeshType type, const char* name, glm::mat4 mat);

private:

    uint SphereID;
    uint SurfaceID; 
    uint PhysicsID;

    void checkCompileErrors(uint shader, const char* type, MeshType mType);
};

#endif

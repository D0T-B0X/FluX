#include "Renderer/shader.h"

Shader::Shader()
    :
    SphereID(0),
    SurfaceID(0)
    { }

void Shader::load(MeshType type, const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vertexStream;
        std::stringstream fragmentStream;
        
        vertexStream << vShaderFile.rdbuf();
        fragmentStream << fShaderFile.rdbuf();

        vertexCode = vertexStream.str();
        fragmentCode = fragmentStream.str();
    } catch (std::ifstream::failure e) {
        std::string debugTypeName = type == SPHERE ? "SPHERE" : "SURFACE";
        std::cout << "ERROR::SHADER_FILE::NOT_SUCCESSFUlLY_READ for type: " << debugTypeName << std::endl;
    }

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();

    uint vertex;
    uint fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX", type);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", type);

    // Assign the program ID of the object type being rendered
    uint& ID = type == SPHERE ? SphereID : SurfaceID;

    // Create program and attach the shaders to it
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    // Link the program to the GPU
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM", type);

    // Delete shaders after compilation is complete
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use(MeshType type) {
    uint& ID = type == SPHERE ? SphereID : SurfaceID;
    glUseProgram(ID);
}

void Shader::setBool(MeshType type, const char* name, int val) {
    uint& ID = type == SURFACE ? SurfaceID : SphereID;
    glProgramUniform1i(ID, glGetUniformLocation(ID, name), val);
}

void Shader::setInt(MeshType type, const char* name, int val) {
    uint& ID = type == SURFACE ? SurfaceID : SphereID;
    glProgramUniform1i(ID, glGetUniformLocation(ID, name), val);
}

void Shader::setFloat(MeshType type, const char* name, float val) {
    uint& ID = type == SURFACE ? SurfaceID : SphereID;
    glProgramUniform1f(ID, glGetUniformLocation(ID, name), val); 
}

void Shader::setVec3(MeshType type, const char* name, glm::vec3 vec) {
    uint& ID = type == SURFACE ? SurfaceID : SphereID;
    glProgramUniform3fv(ID, glGetUniformLocation(ID, name), 1, glm::value_ptr(vec));
}

void Shader::setMat4(MeshType type, const char* name, glm::mat4 mat) {
    uint& ID = type == SURFACE ? SurfaceID : SphereID;
    glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::checkCompileErrors(uint shader, const char* type, MeshType mType) {
    int success;
    char infoLog[1024];
    std::string debugTypeName = mType == SPHERE ? "SPHERE" : "SURFACE";

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_ERROR of type: " << type 
                      << " for " << debugTypeName
                      << "\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM::LINK_ERROR of type: " << type 
                      << " for " << debugTypeName
                      << "\n" << infoLog << std::endl;
        }      
    }
}
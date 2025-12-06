#ifndef RENDERER_H
#define RENDERER_H

// OpenGL and GLFW helper libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Renderer specific libraries
#include "config.h"
#include "shader.h"
#include "camera.h"
#include "settings.h"

// Import scene data
#include "scene.h"

class Renderer {
public:
    Renderer(Scene& activeScene);

    void          renderFrame();
    void          setSphereSubdivisions(uint subdivs);
    void          drawSpheres();
    void          drawSurfaces();
    bool          shouldEnd();
    void          cleanup();

private:
    // ------------ Helper libaries --------------
    GLFWwindow   *window             = nullptr;
    Shader        shader;
    Camera        camera;
    Scene&        renderScene;
    Sphere3D      globalSphere;
    
    // --------- Mesh specific variables ---------
    uint          sphereIndexCount   =     0;
    uint          uVAO               =     0;
    uint          uVBO               =     0;
    uint          uInstancedVBO      =     0;
    uint          uEBO               =     0;

    // -------------- I/O functions -------------- 
    void          createWindow();
    void          loadGLAD();
    void          processKeyboardInput();
    void          processMouseInput(double xpos, double ypos);
    void          uploadSphereMesh();
    static void   cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif

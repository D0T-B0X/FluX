#ifndef RENDERER_H
#define RENDERER_H

// Renderer specific libraries
#include "config.h"
#include "shader.h"
#include "camera.h"

// Import scene data
#include "scene.h"

class Renderer {
public:
    Renderer(Scene& activeScene);

    void          renderFrame();
    void          uploadSphereMesh();
    void          setSphereSubdivisions(uint subdivs);
    void          setUniforms();
    void          drawSpheres();
    void          drawSurfaces();
    bool          shouldEnd();
    void          cleanup();

private:
    // ------------ Helper libaries --------------
    GLFWwindow   *window;
    Shader        shader;
    Camera        camera;
    Scene&        renderScene;
    
    // --------- Mesh specific variables ---------
    bool          uploadRadiusUniform;
    uint          sphereIndexCount;
    uint          uVAO;
    uint          uVBO;              
    uint          uInstancedVBO;      
    uint          uEBO;  
    
    // -------------- I/O functions -------------- 
    void          createWindow();
    void          loadGLAD();
    void          processKeyboardInput();
    void          processMouseInput();
    static void   cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif

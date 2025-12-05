#include "Renderer/renderer.h"

Renderer::Renderer(Scene& activeScene) : renderScene(activeScene) {

    // Initialize OpenGL with version 4.6
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a new window and load GLAD
    createWindow();
    loadGLAD();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Enable depth test to get 3D output
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowUserPointer(window, this);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    // Load shaders for surface and sphere objects
    shader.load(SPHERE, SPHERE_VSHADER_PATH, SPHERE_FSHADER_PATH);
    shader.load(SURFACE, SURFACE_VSHADER_PATH, SURFACE_FSHADER_PATH);

    uploadSphereMesh();
}

void Renderer::renderFrame() {
    processKeyboardInput();

    // Clear frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawSpheres();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::setSphereSubdivisions(uint subdivs) {
    globalSphere.setSubdivision(subdivs);

    glBindVertexArray(uVAO);

    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        globalSphere.getVerticesSize(), 
        globalSphere.getVertices(), 
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        globalSphere.getIndicesSize(),
        globalSphere.getIndices(),
        GL_STATIC_DRAW
    );

    this->sphereIndexCount = globalSphere.getIndexCount();
    glBindVertexArray(0);
}

void Renderer::drawSpheres() {
    if (renderScene.hasNoSpheres()) { return; }

    shader.use(SPHERE);
    glBindVertexArray(uVAO);

    glBindBuffer(GL_ARRAY_BUFFER, uInstancedVBO);

    // Orphan memory
    glBufferData(
        GL_ARRAY_BUFFER, 
        renderScene.getSpheresDataSize(), 
        NULL, 
        GL_STREAM_DRAW
    );

    // Fill orphan memory with struct data
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        renderScene.getSpheresDataSize(),
        renderScene.getSpheresData()
    );

    glDrawElementsInstanced(
        GL_TRIANGLES, 
        this->sphereIndexCount, 
        GL_UNSIGNED_INT, 
        NULL, 
        renderScene.getSpheresSize()
    );

    glBindVertexArray(0);
}

bool Renderer::shouldEnd() {
    return glfwWindowShouldClose(window);
}

void Renderer::cleanup() {
    glfwTerminate();
}

void Renderer::createWindow() {
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, NAME, NULL, NULL);
    if (!window) {
        printf("Failed to create a window\n");
        exit(-1);
    }
    glfwMakeContextCurrent(window);
}

void Renderer::loadGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load glad" << std::endl;
        exit(-1); 
    }
}

void Renderer::processKeyboardInput() {
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Renderer::processMouseInput(double xpos, double ypos) {

}

void Renderer::uploadSphereMesh() {

    if (!this->uVAO) {
        glGenVertexArrays(1, &uVAO);
        glGenBuffers(1, &uVBO);
        glGenBuffers(1, &uInstancedVBO);
        glGenBuffers(1, &uEBO);
    }

    glBindVertexArray(uVAO);

    // Mesh geometry
    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        globalSphere.getVerticesSize(),
        globalSphere.getVertices(),
        GL_STATIC_DRAW
    );

    // Mesh position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Instanced data attributes
    glBindBuffer(GL_ARRAY_BUFFER, uInstancedVBO);

    // Location 1: vec3 position (offset 0)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, fv3Position));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // One per instance

    // Location 2: vec3 color (offset 12)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, fv3Color));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Location 3: float radius (offset 24)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, fRadius));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Location 4: float emitter (offset 28)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, fEmitter));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        globalSphere.getIndicesSize(),
        globalSphere.getIndices(),
        GL_STATIC_DRAW
    );

    this->sphereIndexCount = globalSphere.getIndexCount();

    glBindVertexArray(0);
}

void Renderer::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) { renderer->processMouseInput(xpos, ypos); }
}
#include "Renderer/renderer.h"

Renderer::Renderer(Scene& activeScene) 
    :
    window(nullptr), 
    renderScene(activeScene),
    uploadRadiusUniform(false),
    sphereIndexCount(0),
    uVAO(0),
    uVBO(0),
    uInstancedVBO(0),
    uEBO(0)
{

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
    setUniforms();
}

void Renderer::renderFrame() {
    // Scene timing update
    renderScene.currTime = glfwGetTime();
    renderScene.dt = renderScene.currTime - renderScene.lastTime;
    renderScene.lastTime = renderScene.currTime;

    if (renderScene.getGlobalSphere().isMeshDrity()) {
        shader.setFloat(SPHERE, "fRadius", renderScene.getGlobalSphere().getRadius());
        renderScene.getGlobalSphere().setMeshDirtyStatus();
    }

    shader.setMat4(SPHERE, "view", camera.generateViewMatrix());
    shader.setMat4(SPHERE, "projection", camera.generateProjectionMatrix());

    processKeyboardInput();

    // Clear frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawSpheres();
    drawSurfaces();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::setSphereSubdivisions(uint subdivs) {
    renderScene.getGlobalSphere().setSubdivision(subdivs);

    glBindVertexArray(uVAO);

    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        renderScene.getGlobalSphere().getVerticesSize(), 
        renderScene.getGlobalSphere().getVertices(), 
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        renderScene.getGlobalSphere().getIndicesSize(),
        renderScene.getGlobalSphere().getIndices(),
        GL_STATIC_DRAW
    );

    this->sphereIndexCount = renderScene.getGlobalSphere().getIndexCount();
    glBindVertexArray(0);
}

void Renderer::setUniforms() {
    // Set Sphere Uniforms
    shader.use(SPHERE);

    shader.setFloat(SPHERE, "fRadius", renderScene.getGlobalSphere().getRadius());
    // Test light source
    shader.setVec3(SPHERE, "lightSourcePosition", glm::vec3(2.0f, 2.0f, 3.0f));
    shader.setVec3(SPHERE, "lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Set Surface Uniforms
    shader.use(SURFACE); // TODO => Make separate function for surfaces
}

void Renderer::drawSpheres() {
    if (renderScene.hasNoSpheres()) { return; }

    shader.use(SPHERE);
    static int count = 0;

    if (renderScene.getGlobalSphere().isMeshDrity()) {
        shader.setFloat(SPHERE, "fRadius", renderScene.getGlobalSphere().getRadius());
        renderScene.getGlobalSphere().setMeshDirtyStatus();
    }

    glBindVertexArray(uVAO);

    glDrawElementsInstanced(
        GL_TRIANGLES, 
        this->sphereIndexCount, 
        GL_UNSIGNED_INT, 
        NULL, 
        renderScene.getSpheresSize()
    );

    glBindVertexArray(0);
}

void Renderer::drawSurfaces() {
    shader.use(SURFACE);
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
    float dt = renderScene.dt;

    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::FORWARD, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::BACKWARD, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::LEFT, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::RIGHT, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::UP, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.processKeyboardInput(Direction::DOWN, dt);
    }
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Renderer::uploadSphereMesh() {

    if (!this->uVAO) {
        glGenVertexArrays(1, &uVAO);
        glGenBuffers(1, &uVBO);
        glGenBuffers(1, &uEBO);
    }

    glBindVertexArray(uVAO);

    // Mesh geometry
    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    glBufferData(
        GL_ARRAY_BUFFER, 
        renderScene.getGlobalSphere().getVerticesSize(),
        renderScene.getGlobalSphere().getVertices(),
        GL_STATIC_DRAW
    );

    // Mesh position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Instanced data attributes
    glBindBuffer(GL_ARRAY_BUFFER, renderScene.particleSSBO);

    // Location 1: xyz position, w mass (offset 0)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, position_mass));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // One per instance

    // Location 2: xyz color, w radius (offset 16)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, color_padding));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Location 3: xyz velocity, w padding (offset 32)
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SphereInstanceData), (void*)offsetof(SphereInstanceData, velocity_padding));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        renderScene.getGlobalSphere().getIndicesSize(),
        renderScene.getGlobalSphere().getIndices(),
        GL_STATIC_DRAW
    );

    this->sphereIndexCount = renderScene.getGlobalSphere().getIndexCount();

    glBindVertexArray(0);
}

void Renderer::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    Renderer* self = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (!self) return;
    self->camera.mousePosition.x = xpos;
    self->camera.mousePosition.y = ypos;

    self->camera.processMouseInput();
}
#include "scene.h"

Scene::Scene()
    :
    dt(0.0f),
    currTime(0.0f),
    lastTime(0.0f),
    position_massSSBO(0),
    velocity_densitySSBO(0),
    force_pressureSSBO(0),
    color_paddingSSBO(0),
    particleCount(0)
    { }

Sphere3D& Scene::getGlobalSphere() {
    return globalSphere;
}

void Scene::addSurface(SurfaceInstanceData surface) {
    Surfaces.push_back(surface);
}

SurfaceInstanceData Scene::createSurface(sNormal normal, uint density, float distance) {
    return surfaceMesh.generateSurface(normal, density, distance);
}

bool Scene::hasNoSpheres() {
    return particleCount == 0;
}

Particles& Scene::getSpheres() {
    return particles;
}

unsigned int Scene::getParticleCount() {
    return particleCount;
}
unsigned int Scene::getPropertyDataSize() {
    return particleCount * sizeof(glm::vec4);
}

const void* Scene::getPositionMassData() {
    return particles.position_mass.data();
}

const void* Scene::getVelocityDensityData() {
    return particles.velocity_density.data();
}

const void* Scene::getForcePressureData() {
    return particles.force_pressure.data();
}

const void* Scene::getColorPaddingData() {
    return particles.color_padding.data();
}

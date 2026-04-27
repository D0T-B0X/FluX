#include "scene.h"

Scene::Scene()
    :
    dt(0.0f),
    currTime(0.0f),
    lastTime(0.0f),
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

int Scene::getParticleCount() {
    return particleCount;
}

void
Scene::incrementParticleCount() {
    ++particleCount;
}

unsigned int Scene::getPropertyDataSize() {
    return particleCount * sizeof(glm::vec4);
}

unsigned int Scene::getParticleCountSize() {
    return particleCount * sizeof(unsigned int);
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

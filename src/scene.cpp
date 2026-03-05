#include "scene.h"

Scene::Scene()
    :
    dt(0.0f),
    currTime(0.0f),
    lastTime(0.0f),
    particleSSBO(0)
    { }

Sphere3D& Scene::getGlobalSphere() {
    return globalSphere;
}

void Scene::addSphere(Particle sphere) {
    Spheres.push_back(sphere);
}

void Scene::addSurface(SurfaceInstanceData surface) {
    Surfaces.push_back(surface);
}

SurfaceInstanceData Scene::createSurface(sNormal normal, uint density, float distance) {
    return surfaceMesh.generateSurface(normal, density, distance);
}

bool Scene::hasNoSpheres() {
    return Spheres.empty();
}

std::vector<Particle>& Scene::getSpheres() {
    return Spheres;
}

unsigned int Scene::getSpheresDataSize() {
    return Spheres.size() * sizeof(Particle);
}

unsigned int Scene::getSpheresSize() {
    return Spheres.size();
}

const void* Scene::getSpheresData() {
    return Spheres.data();
}


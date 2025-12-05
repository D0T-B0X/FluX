#ifndef SCENE_H
#define SCENE_H

#include "Object/body.h"

struct Scene {
public:

    // -------- Sphere control functions ---------
    void addSphere(SphereInstanceData sphere) {
        Spheres.push_back(sphere);
    }

    bool hasNoSpheres() {
        return Spheres.empty();
    }

    unsigned int getSpheresDataSize() {
        return Spheres.size() * sizeof(SphereInstanceData);
    }

    unsigned int getSpheresSize() {
        return Spheres.size();
    }

    const void* getSpheresData() {
        return Spheres.data();
    }

private:

    // Holds all sphere data in the simulation
    std::vector <SphereInstanceData> Spheres;
};

#endif

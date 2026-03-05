#ifndef BODY_H
#define BODY_H

#include "Mesh/Sphere3D.h"
#include "Mesh/Surface3D.h"

struct Particle {
    glm::vec4 position_mass;
    glm::vec4 velocity_density;
    glm::vec4 force_pressure;
    glm::vec4 color_padding;
};

#endif
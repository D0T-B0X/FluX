#ifndef BODY_H
#define BODY_H

#include "Mesh/Sphere3D.h"
#include "Mesh/Surface3D.h"

struct SphereInstanceData {
    glm::vec4 position_mass;
    glm::vec4 color_padding;
    glm::vec4 velocity_padding;
};

#endif
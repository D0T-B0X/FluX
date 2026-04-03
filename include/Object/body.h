#ifndef BODY_H
#define BODY_H

#include "Mesh/Sphere3D.h"
#include "Mesh/Surface3D.h"

struct Particles {
    std::vector<glm::vec4> position_mass;
    std::vector<glm::vec4> velocity_density;
    std::vector<glm::vec4> force_pressure;
    std::vector<glm::vec4> color_padding;
};

#endif
#ifndef SURFACE_3D_H
#define SURFACE_3D_H

#include <vector>
#include <glm/glm.hpp>

inline constexpr float EPSILON_C = 1e-3;

typedef unsigned int uint;

struct SurfaceInstanceData {
    friend class Surface3D;

public:
    void setScale(float scale) {
        if (scale < EPSILON_C) { scale = EPSILON_C; }
        fScale = scale;
    }

    void setPosition(glm::vec3 position) {
        fv3Position = position;
    }

private:
    std::vector<float>   fvVertices;
    std::vector<uint>    uvIndices;
    glm::vec3            fv3Position;
    float                fScale;
};

enum sNormal {
    X_NORMAL,
    Y_NORMAL,
    Z_NORMAL
};

class Surface3D {
public:
    Surface3D();

    SurfaceInstanceData generateSurface(sNormal normal, uint density, float distance);

private:
    std::vector<uint> calculateIndices();
};

#endif
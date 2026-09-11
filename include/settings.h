#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @file settings.h
 * @author DotBox
 * @brief This file is meant to hold all simulation constants
 * @version 0.1
 * @date 2025-11-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "glm/common.hpp"

// -------- OpenGL Window Settings ----------
inline const char*              NAME                     =    "FluX";

inline const unsigned int       SCR_WIDTH                =    1920u;
inline const unsigned int       SCR_HEIGHT               =    1080u;

// -------- Dimensions Constraints ----------
inline constexpr int            GRID_SIDE                =    20;
inline constexpr float          MIN_BOUND                =   -0.2f;
inline constexpr float          MAX_BOUND                =    0.2f;
inline constexpr float          FLOOR_BOUNDARY           =    MIN_BOUND - 0.3f;
inline constexpr float          SPHERE_RADIUS            =    0.008f;
inline constexpr unsigned int   SPHERE_SUBDIVISIONS      =    6u;

// -------- Camera Settings -----------------
inline constexpr glm::vec3      CAMERA_POSITION          =    glm::vec3(0.0f, MAX_BOUND - 0.2, MAX_BOUND + 0.3);
inline constexpr float          FOV                      =    90.0f;
inline constexpr float          MOUSE_SENSITIVITY        =    0.5f;   
inline constexpr float          MOVEMENT_SPEED           =    1.0f;

// -------- SPH constants (SI: kg, m, s) ----
inline constexpr float          SPEED_OF_SOUND           =    10.0f;                                                           // c = 60 m/s
inline constexpr float          RESTING_DENSITY          =    1000.0f;                                                         // kg/m³ (water)
inline constexpr int            GAMMA                    =    7;  
inline constexpr float          K                        =    (RESTING_DENSITY * (SPEED_OF_SOUND*SPEED_OF_SOUND) / GAMMA);     // Pa  (B = ρ₀c²/γ)

// -------- TIME-STEPS ----------------------
inline constexpr float          PHYSICS_DT               =    0.002f;
inline constexpr float          RENDER_DT                =    0.01667f;

// -------- Physical Constants --------------
inline constexpr glm::vec3      GRAV_CONSTANT            =    glm::vec3(0.0f, -9.81f, 0.0f);
inline constexpr float          DAMPING_COEFF            =    0.7f;
inline constexpr float          X_CONTAINER_MAX          =    0.4f;
inline constexpr float          X_CONTAINER_MIN          =   -0.4f;
inline constexpr float          Z_CONTAINER_MAX          =    0.3f;
inline constexpr float          Z_CONTAINER_MIN          =   -0.3f;

// -------- Fluid Properties ----------------
inline constexpr float          VISCOSITY                =    0.05f;        

// -------- Compute Properties --------------

// **** !! W A R N I N G !! ****
// **** !! W A R N I N G !! ****
// DO NOT MODIFY UNLESS YOU KNOW WHAT YOU ARE DOING
inline constexpr int            THREADS_PER_GROUP        =    256;
inline constexpr int            PARTICLES_PER_WORKGROUP  =    THREADS_PER_GROUP;

#endif
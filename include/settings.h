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
inline constexpr int            GRID_SIDE                =    16;
inline constexpr float          MIN_BOUND                =   -2.0f;
inline constexpr float          MAX_BOUND                =    2.0f;
inline constexpr float          FLOOR_BOUNDARY           =    MIN_BOUND - 0.5f;
inline constexpr float          SPHERE_RADIUS            =    0.08f;
inline constexpr unsigned int   SPHERE_SUBDIVISIONS      =    6u;

// -------- Camera Settings -----------------
inline constexpr glm::vec3      CAMERA_POSITION          =    glm::vec3(0.0f, MAX_BOUND - 2.0, MAX_BOUND + 3.0);
inline constexpr float          FOV                      =    90.0f;
inline constexpr float          MOUSE_SENSITIVITY        =    0.5f;   
inline constexpr float          MOVEMENT_SPEED           =    1.0f;

// -------- SPH constants (SI: kg, m, s) ----
inline constexpr float          K                        =    14300.0f;     // Pa  (B = ρ₀c²/γ, c ≈ 60 m/s)
inline constexpr float          RESTING_DENSITY          =    1000.0f;      // kg/m³ (water)
inline constexpr int            GAMMA                    =    7;  

// -------- TIME-STEPS ----------------------
inline constexpr float          PHYSICS_DT               =    0.0020f;
inline constexpr float          RENDER_DT                =    0.0166f;

// -------- Physical Constants --------------
inline constexpr glm::vec3      GRAV_CONSTANT            =    glm::vec3(0.0f, -9.81f, 0.0f);
inline constexpr float          DAMPING_COEFF            =    0.7f;

// -------- Fluid Properties ----------------
inline constexpr float          VISCOSITY                =    0.001f;        

// -------- Compute Properties --------------

// **** !! W A R N I N G !! ****
// **** !! W A R N I N G !! ****
// DO NOT MODIFY UNLESS YOU KNOW WHAT YOU ARE DOING
inline constexpr int            THREADS_PER_GROUP        =    256;
inline constexpr int            PARTICLES_PER_WORKGROUP  =    THREADS_PER_GROUP * 2;

#endif
#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @file settings.h
 * @author DotBox
 * @brief This file is meant to hold all simulator specific constants
 * @version 0.1
 * @date 2025-11-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "glm/common.hpp"

// -------- OpenGL Window Settings ---------
inline const char*         NAME                 =       "FluX";

inline const unsigned int  SCR_WIDTH            =       1920;
inline const unsigned int  SCR_HEIGHT           =       1200;

// -------- Camera Settings -----------------
inline constexpr glm::vec3 CAMERA_POSITION      =       glm::vec3(0.0f, 0.0f, 2.0f);
inline constexpr float     FOV                  =       90.0f;
inline constexpr float     MOUSE_SENSITIVITY    =       0.5f;   
inline constexpr float     MOVEMENT_SPEED       =       0.5f;

// -------- SPH constants (SI: kg, m, s) ----
inline constexpr float     K                    =       500000.0f;    // Pa  (B = ρ₀c²/γ, c ≈ 60 m/s)
inline constexpr float     RESTING_DENSITY      =       1000.0f;      // kg/m³ (water)
inline constexpr int       GAMMA                =       7;  

// -------- TIME-STEPS ----------------------
inline constexpr float     PHYSICS_DT           =       0.0020f;
inline constexpr float     RENDER_DT            =       0.0166f;

#endif
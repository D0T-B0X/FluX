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

// -------- OpenGL Window Settings ---------
inline const char*         NAME                 =       "FluX";

inline const unsigned int  SCR_WIDTH            =       1920;
inline const unsigned int  SCR_HEIGHT           =       1200;

// -------- Camera Settings -----------------
inline constexpr float     FOV                  =       90.0f;
inline constexpr float     MOUSE_SENSITIVITY    =       0.5f;   
inline constexpr float     MOVEMENT_SPEED       =       10.0f;

// -------- SPH constants -------------------
inline constexpr float     SMOOTHING_RADIUS     =       10.0f;
inline constexpr float     K                    =       3.5f;     // Stiffness coefficient
inline constexpr float     RESTING_DENSITY      =       1.0f;
inline constexpr int       GAMMA                =       7;  

#endif
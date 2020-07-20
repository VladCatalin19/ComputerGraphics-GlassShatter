#pragma once

#include <Core/Engine.h>

// Camera
#define CAMERA_INIT_POS     glm::vec3(-0.38f, 0.18f, -5.0f)
#define CAMERA_INIT_ROT     glm::quat(glm::vec3(RADIANS(-10), RADIANS(195), 0.0f))

// Ball
#define BALL_RADIUS         0.05f
#define BALL_INIT_SPEED     25.0f

// Glass pane
#define GLASS_INIT_POS      glm::vec3(0.0f, 0.0f, 0.0f)
#define GLASS_LENGTH        3
#define GLASS_BREADTH       0.05f
#define GLASS_HEIGHT        2

// Physics
#define GRAVIT_ACCEL        glm::vec3(0.0f, -9.8f, 0.0f)

// Skybox
#define SKYBOX_SCALE        100

// Cube map
#define CUBEMAP_INIT_POS    glm::vec3(0.0f, 0.0f, 0.0f)
#define CUBEMAP_FACE_DIM    1024

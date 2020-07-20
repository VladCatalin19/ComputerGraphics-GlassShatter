# pragma once

#include <Core/Engine.h>

class Ball {
public:
    Ball();
    Ball(glm::vec3 position, float radius, glm::vec3 speed);
    ~Ball();

private:
    glm::vec3 position;
    glm::vec3 initialPosition;
    float radius;
    glm::vec3 speed;

public:
    glm::vec3 GetPosition();
    void SetPosition(glm::vec3 position);
    glm::vec3 GetInitialPosition();
    float GetRadius();
    glm::vec3 GetSpeed();
    void SetSpeed(glm::vec3 speed);
};
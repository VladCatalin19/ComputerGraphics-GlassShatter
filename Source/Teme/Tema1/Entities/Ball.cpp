#include "Ball.h"
#include <iostream>
Ball::Ball() {}

Ball::Ball(glm::vec3 position, float radius, glm::vec3 speed)
{
    this->position = position;
    this->initialPosition = position;
    this->radius = radius;
    this->speed = speed;
}

Ball::~Ball() {}

glm::vec3 Ball::GetPosition()
{
    return this->position;
}
void Ball::SetPosition(glm::vec3 position)
{
    this->position = position;
}
glm::vec3 Ball::GetInitialPosition()
{
    return this->initialPosition;
}
float Ball::GetRadius()
{
    return this->radius;
}
glm::vec3 Ball::GetSpeed()
{
    return this->speed;
}
void Ball::SetSpeed(glm::vec3 speed)
{
    this->speed = speed;
}
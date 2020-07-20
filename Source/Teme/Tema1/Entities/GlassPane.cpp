#include "GlassPane.h"

GlassPane::GlassPane() {}

GlassPane::GlassPane(glm::vec3 position, float length, float breadth, float height)
{
    this->position = position;
    this->lenght = length;
    this->breadth = breadth;
    this->height = height;
    this->isBroken = false;
}

GlassPane::~GlassPane() {}

glm::vec3 GlassPane::GetPosition()
{
    return this->position;
}
float GlassPane::GetLength()
{
    return this->lenght;
}
float GlassPane::GetBreadth()
{
    return this->breadth;
}
float GlassPane::GetHeight()
{
    return this->height;
}
bool GlassPane::IsBroken()
{
    return this->isBroken;
}
void GlassPane::SetBroken(bool isBroken)
{
    this->isBroken = isBroken;
}

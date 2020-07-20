#pragma once

#include <Core/Engine.h>

class GlassPane {
public:
    GlassPane();
    GlassPane(glm::vec3 position, float length, float breadth, float height);
    ~GlassPane();

private:
    glm::vec3 position;
    float lenght;
    float breadth;
    float height;
    bool isBroken;

public:
    glm::vec3 GetPosition();
    float GetLength();
    float GetBreadth();
    float GetHeight();
    bool IsBroken();
    void SetBroken(bool isBroken);
};
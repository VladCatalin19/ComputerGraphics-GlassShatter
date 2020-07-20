#pragma once

#include <Component/SimpleScene.h>
#include <Core/Engine.h>
#include <unordered_set>
#include <vector>
#include "Entities/Ball.h"
#include "Entities/GlassPane.h"
#include "CubeMap.h"
using namespace std;
using namespace glm;

class Tema1 : public SimpleScene {

public:
    Tema1();
    ~Tema1();

    struct Particle {
        vec4 position;
        vec4 speed;
        vec4 rotationAngle;
        vec4 initilalAngle;
        vec4 pointsNum;
        vec4 pointsPos[16];
    };
    
    struct SpotLight {
        vec3 position;
        float angle;
        vec3 direction;
    };

    struct Lights {
        float materialKd;
        float materialKs;
        int materialShininess;
        vec3 lightColor;

        int numLights;
        SpotLight *spotLights;
    };

    void Init() override;

private:
    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
    void OnWindowResize(int width, int height) override;

    bool LineBoxIntersect(vec3 l1, vec3 l2, vec3 b1, vec3 b2, vec3 &hit);
    void GenerateFragments(vec2 hitPoint);
    void GenerateParticles();

    void Reset();

    void DrawScene(bool updatePhysics);
    void DrawBall();
    void DrawGlassPane();
    void DrawGlassFragments(bool updatePhysics);
    void DrawSkybox();
    void DrawOrnaments();

    EngineComponents::ProjectionInfo projectionInfoMain;
    EngineComponents::ProjectionInfo projectionInfoFragments;

    Ball *ball;
    GlassPane *glassPane;

    vector<vector<vec2>> fragmentPoints;

    ParticleEffect<Particle> *particleEffect;

    unsigned int skyboxTextureID;
    CubeMap* glassPaneCubeMap;

    float logicalTime;
    float deltaTimeScaled;
    float timeScale;
    float timeWhenBroken;

    enum ShaderMode {cubemap, normal, wireframe};
    ShaderMode shaderMode;

    Lights *lights;
    vector<string> ornamentNames;
};

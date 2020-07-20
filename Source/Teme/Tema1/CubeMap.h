#pragma once

#include <Core/Engine.h>

class CubeMap
{
public:
    CubeMap();
    ~CubeMap();
    CubeMap(glm::vec3 position, glm::vec3 cameraDist);
    enum CubeFaces { posx, negx, posy, negy, posz, negz };

    void Generate(int cubeSize);
    void Bind();
    void BindDefault();
    void Clear();
    static unsigned int LoadCubeMapWithImages(
        const std::string &posx, const std::string &posy, const std::string &posz,
        const std::string &negx, const std::string &negy, const std::string &negz);

    unsigned int GetTextureID();
    glm::vec3 GetPosition();
    void SetPosition(glm::vec3);
    glm::vec3* GetCameraRelativePositions();
    glm::quat* GetCameraRotations();

private:
    unsigned int textureID;
    unsigned int fbo;
    unsigned int depthBuffer;
    unsigned int faceSize;
    glm::vec3 position;
    glm::vec3 cameraRelativePositions[6];
    glm::quat cameraRotations[6];
};
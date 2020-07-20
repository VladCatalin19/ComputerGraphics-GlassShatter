#include "CubeMap.h"
#include <stdio.h>
#include <stb/stb_image.h>
using namespace glm;

CubeMap::CubeMap()
{
    depthBuffer = textureID = fbo = 0;
}

CubeMap::~CubeMap()
{
    if (depthBuffer != 0) glDeleteRenderbuffers(1, &depthBuffer);
    if (fbo != 0) glDeleteFramebuffers(1, &fbo);
}

CubeMap::CubeMap(glm::vec3 position, vec3 cameraDist)
{
    this->position = position;
    cameraRelativePositions[posx] = vec3(cameraDist.x, 0.0f, 0.0f);
    cameraRelativePositions[negx] = -vec3(cameraDist.x, 0.0f, 0.0f);
    cameraRelativePositions[posy] = vec3(0.0f, cameraDist.y, 0.0f);
    cameraRelativePositions[negy] = -vec3(0.0f, cameraDist.y, 0.0f);
    cameraRelativePositions[posz] = vec3(0.0f, 0.0f, cameraDist.z);
    cameraRelativePositions[negz] = -vec3(0.0f, 0.0f, cameraDist.z);

    cameraRotations[posx] = quat(vec3(0.0f,          RADIANS(90),  RADIANS(180)));
    cameraRotations[negx] = quat(vec3(0.0f,         -RADIANS(90),  RADIANS(180)));
    cameraRotations[posy] = quat(vec3(-RADIANS(90),  RADIANS(180), RADIANS(180)));
    cameraRotations[negy] = quat(vec3( RADIANS(90),  RADIANS(180), RADIANS(180)));
    cameraRotations[posz] = quat(vec3(0.0f,          RADIANS(180), RADIANS(180)));
    cameraRotations[negz] = quat(vec3(0.0f,          0.0f,         RADIANS(180)));
}

void CubeMap::Generate(int faceSize)
{
    this->faceSize = faceSize;

    // Generate framebuffer object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    CheckOpenGLError();

    // Generate depth buffer object
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, faceSize, faceSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthBuffer);
    CheckOpenGLError();

    // Generate cube map texture object
    glGenTextures(1, &textureID);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    CheckOpenGLError();

    // Generate texture for cube's faces
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, faceSize,
                     faceSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    CheckOpenGLError();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CheckOpenGLError();

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CheckOpenGLError();
}

void CubeMap::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    CheckOpenGLError();
    glViewport(0, 0, faceSize, faceSize);
}

void CubeMap::BindDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckOpenGLError();
}

void CubeMap::Clear()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


unsigned int CubeMap::LoadCubeMapWithImages(
    const std::string &posx, const std::string &posy, const std::string &posz,
    const std::string &negx, const std::string &negy, const std::string &negz)
{
    int width, height, chn;

    // Alpha is not needed for a skybox
    unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 3);
    unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 3);
    unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 3);
    unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 3);
    unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 3);
    unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 3);

    // Create OpenGL texture
    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    // Bind the texture
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    float maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load texture information for each face
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_posx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_posy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_posz);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_negx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_negy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data_negz);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // free memory
    SAFE_FREE(data_posx);
    SAFE_FREE(data_posy);
    SAFE_FREE(data_posz);
    SAFE_FREE(data_negx);
    SAFE_FREE(data_negy);
    SAFE_FREE(data_negz);

    return textureID;
}


unsigned int CubeMap::GetTextureID()
{
    return textureID;
}

glm::vec3 CubeMap::GetPosition()
{
    return position;
}
void CubeMap::SetPosition(glm::vec3 position)
{
    this->position = position;
}
glm::vec3* CubeMap::GetCameraRelativePositions()
{
    return cameraRelativePositions;
}
glm::quat* CubeMap::GetCameraRotations()
{
    return cameraRotations;
}

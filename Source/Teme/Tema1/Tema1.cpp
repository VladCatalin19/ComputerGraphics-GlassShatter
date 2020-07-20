#include "Tema1.h"

#include "Constants.h"

#include <iostream>
#include <Core/Engine.h>
#include <vector>

#define JC_VORONOI_IMPLEMENTATION
#include "Voronoi/jc_voronoi.h"

#define JC_VORONOI_CLIP_IMPLEMENTATION
#include "Voronoi/jc_voronoi_clip.h"

using namespace std;
using namespace glm;

Tema1::Tema1()
{
    ball = nullptr;
    glassPane = nullptr;
    particleEffect = nullptr;
    glassPaneCubeMap = nullptr;
    lights = nullptr;
}
Tema1::~Tema1()
{
    delete ball;
    delete glassPane;
    delete particleEffect;
    delete glassPaneCubeMap;
    if (lights) delete[] lights->spotLights;
    delete lights;
}

void Tema1::Init()
{
    ornamentNames = {
        "bed",
        "ceilings",
        "desk",
        "door",
        "floor",
        "lights",
        "walls",
        "wardrobe_big",
        "wardrobe_medium",
        "wardrobe_small",
        "window_frame",
        "laptop",
        "poster1",
        "poster2",
        "poster3"
    };
    /*************************************************************************/
    /*                              Lights                                   */
    /*************************************************************************/
    lights = new Lights;
    lights->materialKd = 0.5f;
    lights->materialKs = 0.5f;
    lights->materialShininess = 10;
    lights->lightColor = vec3(0.75f, 0.77f, 1.0f);
    lights->numLights = 7;
    float coneAngle = 85.0f;
    vec3 coneDirection = vec3(0.0f, -1.0f, 0.0f);
    vec3 conePositions[] = {
        vec3(-0.835, 1.434, -1.681),
        vec3( 1.982, 1.434, -1.681),
        vec3( 1.982, 1.434, -3.570),
        vec3(-0.835, 1.434, -3.570),
        vec3(-0.835, 1.434, -5.458),
        vec3( 1.982, 1.434, -5.458),
        vec3( 4.801, 1.434, -5.458)
    };

    lights->spotLights = new SpotLight[lights->numLights];
    for (int i = 0; i < lights->numLights; ++i) {
        lights->spotLights[i].angle = coneAngle;
        lights->spotLights[i].direction = coneDirection;
        lights->spotLights[i].position = conePositions[i];
    }

    /*************************************************************************/
    /*                              Camera                                   */
    /*************************************************************************/
    EngineComponents::Camera *camera = GetSceneCamera();
    camera->SetPositionAndRotation(CAMERA_INIT_POS, CAMERA_INIT_ROT);
    camera->Update();
    projectionInfoMain = camera->GetProjectionInfo();

    projectionInfoFragments.FoVy = 90.0f;
    projectionInfoFragments.zNear = projectionInfoMain.zNear;
    projectionInfoFragments.zFar = projectionInfoMain.zFar;
    projectionInfoFragments.aspectRatio = 1.0f;
    projectionInfoFragments.width = CUBEMAP_FACE_DIM;
    projectionInfoFragments.height = CUBEMAP_FACE_DIM;
    projectionInfoFragments.isPerspective = true;

    /*************************************************************************/
    /*                              Meshes                                   */
    /*************************************************************************/
    Mesh *mesh = new Mesh("window");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "window.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("ball");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props", "ball.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("cube");
    mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    for (string s : ornamentNames) {
        mesh = new Mesh(s);
        mesh->LoadMesh(RESOURCE_PATH::MODELS + "Props/bedroom", s + ".obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    /*************************************************************************/
    /*                              Shaders                                  */
    /*************************************************************************/
    Shader *shader = new Shader("CubeMap");
    std::string shaderPath = "Source/Teme/Tema1/Shaders/";
    shader->AddShader(shaderPath + "CubeMap.VS.glsl", GL_VERTEX_SHADER);
    shader->AddShader(shaderPath + "CubeMap.FS.glsl", GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("Glass");
    shader->AddShader(shaderPath + "Glass.VS.glsl", GL_VERTEX_SHADER);
    shader->AddShader(shaderPath + "Particle.FS.glsl", GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("Particle");
    shader->AddShader(shaderPath + "Particle.VS.glsl", GL_VERTEX_SHADER);
    shader->AddShader(shaderPath + "Particle.GS.glsl", GL_GEOMETRY_SHADER);
    shader->AddShader(shaderPath + "Particle.FS.glsl", GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("Ornaments");
    shader->AddShader(shaderPath + "Ornaments.VS.glsl", GL_VERTEX_SHADER);
    shader->AddShader(shaderPath + "Ornaments.FS.glsl", GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    /*************************************************************************/
    /*                             Textures                                  */
    /*************************************************************************/
    string texturePath = RESOURCE_PATH::TEXTURES + "Skybox/";
    skyboxTextureID = CubeMap::LoadCubeMapWithImages(
        texturePath + "posx.png",
        texturePath + "posy.png",
        texturePath + "posz.png",
        texturePath + "negx.png",
        texturePath + "negy.png",
        texturePath + "negz.png"
    );    

    ball = nullptr;
    glassPane = new GlassPane(GLASS_INIT_POS, GLASS_LENGTH, GLASS_BREADTH, GLASS_HEIGHT);
    logicalTime = 0.0f;
    timeScale = 1.0f;
    timeWhenBroken = 0.0f;

    particleEffect = nullptr;

    shaderMode = cubemap;

    glassPaneCubeMap = new CubeMap(CUBEMAP_INIT_POS,
                                vec3(GLASS_LENGTH / 2.0f,
                                     GLASS_HEIGHT / 2.0f,
                                     GLASS_BREADTH / 2.5f));
    glassPaneCubeMap->Generate(CUBEMAP_FACE_DIM);
}

void Tema1::FrameStart()
{
    ClearScreen();
}

inline float square(float x) { return x * x; }
inline float angle(vec3 a, vec3 b) { return acos(dot(normalize(a), normalize(b))); }

void Tema1::Update(float deltaTimeSeconds)
{
    if (ball != nullptr) {
        deltaTimeScaled = timeScale * deltaTimeSeconds;
        logicalTime += deltaTimeScaled;

        vec3 l1 = ball->GetPosition();
        vec3 l2 = l1 + ball->GetSpeed() * deltaTimeScaled;
        vec3 dp = 0.5f * vec3(glassPane->GetLength(), glassPane->GetHeight(), glassPane->GetBreadth());
        vec3 b1 = glassPane->GetPosition() - dp;
        vec3 b2 = glassPane->GetPosition() + dp;
        vec3 hit;

        if (ball != nullptr && !glassPane->IsBroken() && LineBoxIntersect(l1, l2, b1, b2, hit)) {
            glassPane->SetBroken(true);
            timeWhenBroken = logicalTime;
            vec2 hitPoint = vec2(hit.x, hit.y);

            GenerateFragments(hitPoint);
            GenerateParticles();

            // Decrease ball's speed due to impact
            ball->SetSpeed(ball->GetSpeed() * 0.7f);
        }

        // Update ball stats
        vec3 position = ball->GetPosition();
        vec3 speed = ball->GetSpeed();
        position += speed * deltaTimeScaled;
        speed += GRAVIT_ACCEL * deltaTimeScaled;
        ball->SetPosition(position);
        ball->SetSpeed(speed);

        // Move cubemap
        if (glassPane->IsBroken() && logicalTime >= timeWhenBroken) {
            vec3 pos = speed * deltaTimeScaled;
            pos.x /= 5.0f;
            pos.z /= 5.0f;
            glassPaneCubeMap->SetPosition(glassPaneCubeMap->GetPosition() + pos);
        }
    }

    // Save camera stats
    EngineComponents::Camera *camera = GetSceneCamera();
    glm::vec3 camPosition = camera->transform->GetWorldPosition();
    glm::quat camRotation = camera->transform->GetWorldRotation();

    // Draw scene in cubemap
    glassPaneCubeMap->Bind();
    glassPaneCubeMap->Clear();
    for (int i = 0; i < 6; ++i) {
        // Render each face of the cubemap
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               glassPaneCubeMap->GetTextureID(), 0);
        glassPaneCubeMap->Clear();

        camera->SetPosition(glassPaneCubeMap->GetCameraRelativePositions()[i] +
                            glassPaneCubeMap->GetPosition());
        camera->SetRotation(glassPaneCubeMap->GetCameraRotations()[i]);
        camera->SetProjection(projectionInfoFragments);
        DrawScene(false);
    }

    // Restore scene properties
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

    // Restore comera stats
    camera->SetPosition(camPosition);
    camera->SetRotation(camRotation);
    camera->SetProjection(projectionInfoMain);

    DrawScene(true);
}

void Tema1::GenerateFragments(vec2 hitPoint)
{
    int innerPoints = 5 + rand() % 3;
    int centerPoints = 10 + rand() % 5;
    int outerPoints = 5 + rand() % 2;
    int count = innerPoints + centerPoints + outerPoints;

    jcv_point* points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)count);
    if (!points) {
        return;
    }

    float dist, angle, jitter;
    // Generate the random sites
    // Small inner circle: 5 - 7 points
    for (int i = 0; i < innerPoints; ++i)
    {
        dist = (5 + rand() % 6) / 100.0f;
        angle = 2.0f * (float)AI_MATH_PI * i / innerPoints;
        points[i].x = (float)hitPoint.x + dist * cos(angle);
        points[i].y = (float)hitPoint.y + dist * sin(angle);
    }

    // Medium center circle: 10 - 14 points
    for (int i = innerPoints; i < innerPoints + centerPoints; ++i)
    {
        dist = (25 + rand() % 15) / 100.0f;
        angle = 2.0f * (float)AI_MATH_PI * i / centerPoints;
        points[i].x = (float)hitPoint.x + dist * cos(angle);
        points[i].y = (float)hitPoint.y + dist * sin(angle);
    }

    // Large outer circle: 5 - 6 points
    for (int i = innerPoints + centerPoints; i < count; ++i)
    {
        dist = (80 + rand() % 10) / 100.0f;
        jitter = (rand() % 200 - 100.0f) / 300.0f;
        angle = 2.0f * (float)AI_MATH_PI * i / outerPoints + jitter;
        points[i].x = (float)hitPoint.x + dist * cos(angle);
        points[i].y = (float)hitPoint.y + dist * sin(angle);
    }

    // Clipping rectangle. All points outside this box will be discarded
    jcv_rect bounding_box = {
        { glassPane->GetPosition().x - glassPane->GetLength() / 2.0f,
          glassPane->GetPosition().y - glassPane->GetHeight() / 2.0f
        },
        { glassPane->GetPosition().x + glassPane->GetLength() / 2.0f,
          glassPane->GetPosition().y + glassPane->GetHeight() / 2.0f
        }
    };

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(count, points, &bounding_box, NULL, &diagram);

    fragmentPoints = vector<vector<vec2>>(diagram.numsites);
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    // After generating the diagram, get all points from each polygon
    for (int i = 0; i < diagram.numsites; ++i)
    {
        jcv_graphedge* graph_edge = sites[i].edges;
        while (graph_edge) {
            fragmentPoints[i].push_back(vec2((float)(graph_edge->pos[0].x),
                                             (float)(graph_edge->pos[0].y)));
            graph_edge = graph_edge->next;
        }
    }

    jcv_diagram_free(&diagram);
    free(points);
}

void Tema1::GenerateParticles()
{
    particleEffect = new ParticleEffect<Particle>();
    particleEffect->Generate((unsigned int)fragmentPoints.size(), true);

    SSBO<Particle> *particleSSBO = particleEffect->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    for (unsigned int i = 0; i < fragmentPoints.size(); ++i) {
        vec4 pos = vec4(0);
        float glassPaneZ = glassPane->GetPosition().z + glassPane->GetBreadth() / 2.0f;

        // Find center of gravity
        vec3 center = vec3(0);
        for (int j = 0; j < fragmentPoints[i].size(); ++j) {
            vec3 worldPos = vec3(fragmentPoints[i][j].x, fragmentPoints[i][j].y, glassPaneZ);
            center += worldPos;
        }
        center /= (float)fragmentPoints[i].size();

        // Translate each point so that the center is the origin of the polygon
        mat4 trans = translate(mat4(1), -center);
        for (int j = 0; j < fragmentPoints[i].size(); ++j) {
            vec3 worldPos = vec3(fragmentPoints[i][j].x, fragmentPoints[i][j].y, glassPaneZ);
            vec4 transPos = trans * vec4(worldPos, 1.0f);
            data[i].pointsPos[j] = transPos;
        }

        data[i].pointsNum = vec4((float)fragmentPoints[i].size());
        data[i].position = vec4(center, 0.0f);

        // Calculate rotation based on impact point
        float attenuation = 1.0f / (20.0f * length(center - ball->GetPosition()));
        data[i].speed = vec4(ball->GetSpeed() * attenuation, 0.0f);

        vec3 firstVec = ball->GetPosition() - center;
        vec3 secondVec = ball->GetPosition() + ball->GetSpeed() - center;
        float angleX = sign(firstVec.y * firstVec.z)
                     * angle(vec3(0, firstVec.y,  firstVec.z),
                             vec3(0, secondVec.y, secondVec.z));
        float angleY = sign(firstVec.x * firstVec.z)
                     * angle(vec3(firstVec.x,  0, firstVec.z),
                             vec3(secondVec.x, 0, secondVec.z));
        float angleZ = sign(firstVec.x * firstVec.y)
                     * angle(vec3(firstVec.x,  firstVec.y,  0),
                             vec3(secondVec.x, secondVec.y, 0));

        data[i].rotationAngle = vec4(0.0f);
        data[i].initilalAngle = attenuation * vec4(angleX, angleY, angleZ, 0.0f);
    }
    particleSSBO->SetBufferData(data);
}

bool GetIntersection(float fDst1, float fDst2, vec3 p1, vec3 p2, vec3 &hit)
{
    if (fDst1 * fDst2 >= 0.0f) return false;
    if (fDst1 == fDst2) return false;
    hit = p1 + (p2 - p1) * (-fDst1 / (fDst2 - fDst1));
    return true;
}

bool InBox(vec3 hit, vec3 b1, vec3 b2, const int axis)
{
    if (axis == 1 && hit.z > b1.z && hit.z < b2.z && hit.y > b1.y && hit.y < b2.y) return true;
    if (axis == 2 && hit.z > b1.z && hit.z < b2.z && hit.x > b1.x && hit.x < b2.x) return true;
    if (axis == 3 && hit.x > b1.x && hit.x < b2.x && hit.y > b1.y && hit.y < b2.y) return true;
    return false;
}

// Returns true if line (l1, l2) intersects with axis aligned box (b1, b2)
// Returns intersection point in hit
bool Tema1::LineBoxIntersect(vec3 l1, vec3 l2, vec3 b1, vec3 b2, vec3 &hit)
{
    if (l2.x < b1.x && l1.x < b1.x) return false;
    if (l2.x > b2.x && l1.x > b2.x) return false;
    if (l2.y < b1.y && l1.y < b1.y) return false;
    if (l2.y > b2.y && l1.y > b2.y) return false;
    if (l2.z < b1.z && l1.z < b1.z) return false;
    if (l2.z > b2.z && l1.z > b2.z) return false;
    if (l1.x > b1.x && l1.x < b2.x &&
        l1.y > b1.y && l1.y < b2.y &&
        l1.z > b1.z && l1.z < b2.z) {
        hit = l1;
        return true;
    }
    if ((GetIntersection(l1.x - b1.x, l2.x - b1.x, l1, l2, hit) && InBox(hit, b1, b2, 1))
        || (GetIntersection(l1.y - b1.y, l2.y - b1.y, l1, l2, hit) && InBox(hit, b1, b2, 2))
        || (GetIntersection(l1.z - b1.z, l2.z - b1.z, l1, l2, hit) && InBox(hit, b1, b2, 3))
        || (GetIntersection(l1.x - b2.x, l2.x - b2.x, l1, l2, hit) && InBox(hit, b1, b2, 1))
        || (GetIntersection(l1.y - b2.y, l2.y - b2.y, l1, l2, hit) && InBox(hit, b1, b2, 2))
        || (GetIntersection(l1.z - b2.z, l2.z - b2.z, l1, l2, hit) && InBox(hit, b1, b2, 3)))
        return true;

    return false;
}

void Tema1::Reset()
{
    delete ball;
    ball = nullptr;

    fragmentPoints.clear();
    delete particleEffect;

    particleEffect = nullptr;

    logicalTime = 0.0f;
    timeScale = 1.0f;
    timeWhenBroken = 0.0f;
    shaderMode = cubemap;

    glassPaneCubeMap->SetPosition(GLASS_INIT_POS);
    glassPane->SetBroken(false);
}

void Tema1::DrawScene(bool updatePhysics)
{
    if (ball != nullptr) {
        DrawBall();
    }
    if (shaderMode == wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glassPane->IsBroken() && logicalTime >= timeWhenBroken) {
        DrawGlassFragments(updatePhysics);
    }
    else {
        DrawGlassPane();
    }
    if (shaderMode == wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    DrawOrnaments();
    DrawSkybox();
}

void Tema1::DrawBall()
{
    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, ball->GetPosition());
    modelMatrix = scale(modelMatrix, vec3(ball->GetRadius()));
    RenderMesh(meshes["ball"], shaders["Simple"], modelMatrix);
}

void Tema1::DrawGlassPane()
{
    Shader *shader = shaders["Glass"];
    shader->Use();

    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, glassPane->GetPosition());

    EngineComponents::Camera *camera = GetSceneCamera();

    // Vertex shader uniforms
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetProjectionMatrix()));

    // Fragment shader uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glassPaneCubeMap->GetTextureID());
    glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);

    vec3 cameraPosition = camera->transform->GetWorldPosition();
    glUniform3f(shader->GetUniformLocation("camera_position"),
                cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniform1i(shader->GetUniformLocation("mode"), shaderMode);
    meshes["window"]->Render();
}

void Tema1::DrawGlassFragments(bool updatePhysics)
{
    Shader *shader = shaders["Particle"];
    shader->Use();

    mat4 modelMatrix = mat4(1);
    modelMatrix = translate(modelMatrix, glassPane->GetPosition());
    EngineComponents::Camera *camera = GetSceneCamera();

    // Fragment shader uniforms
    glUniform1f(shader->GetUniformLocation("delta_time_seconds"), deltaTimeScaled);
    glUniform3f(shader->GetUniformLocation("gravitational_acceleration"),
                GRAVIT_ACCEL.x, GRAVIT_ACCEL.y, GRAVIT_ACCEL.z);
    // Tell the shader if the position and rotation of the particles should be
    // changed. When drawing into cubemap, it is important not to update those
    glUniform1i(shader->GetUniformLocation("update_physics"), updatePhysics);

    // Geometry shader uniforms
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetProjectionMatrix()));
    glUniform1i(shader->GetUniformLocation("fragment_num"),
                (int)fragmentPoints.size());
    glUniform1f(shader->GetUniformLocation("thickness"),
                glassPane->GetBreadth());

    // Fragment shader uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glassPaneCubeMap->GetTextureID());
    glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);

    vec3 cameraPosition = camera->transform->GetWorldPosition();
    glUniform3f(shader->GetUniformLocation("camera_position"),
                cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniform1i(shader->GetUniformLocation("mode"),
                shaderMode);


    particleEffect->Render(camera, shader);
}

void Tema1::DrawSkybox()
{
    vec3 pos = GetSceneCamera()->transform->GetWorldPosition();
    Shader *shader = shaders["CubeMap"];
    shader->Use();

    // Move skybox to camera position
    EngineComponents::Camera *camera = GetSceneCamera();
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(SKYBOX_SCALE));

    // Vertex shader uniforms
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(camera->GetProjectionMatrix()));

    // Fragment shader uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glUniform1i(shader->GetUniformLocation("texture_cubemap"), 0);

    meshes["cube"]->Render();
}

void Tema1::DrawOrnaments()
{
    Shader *shader = shaders["Ornaments"];
    shader->Use();
    EngineComponents::Camera *camera = GetSceneCamera();

    // Fragment shader uniforms
    vec3 cameraPosition = camera->transform->GetWorldPosition();
    glUniform3f(shader->GetUniformLocation("camera_position"),
                cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniform1f(shader->GetUniformLocation("material_kd"), lights->materialKd);
    glUniform1f(shader->GetUniformLocation("material_ks"), lights->materialKs);
    glUniform1i(shader->GetUniformLocation("material_shininess"), lights->materialShininess);
    glUniform3f(shader->GetUniformLocation("light_color"),
                lights->lightColor.x, lights->lightColor.y, lights->lightColor.z);

    glUniform1i(shader->GetUniformLocation("num_lights"), lights->numLights);
    for (int i = 0; i < lights->numLights; ++i) {
        string elem_name = "spot_lights[" + to_string(i) + "]";
        string position_name = elem_name + ".position";
        string angle_name = elem_name + ".cone_angle";
        string direction_name = elem_name + ".cone_direction";

        glUniform3f(shader->GetUniformLocation(position_name.c_str()),
                    lights->spotLights[i].position.x,
                    lights->spotLights[i].position.y,
                    lights->spotLights[i].position.z);
        glUniform1f(shader->GetUniformLocation(angle_name.c_str()),
                    lights->spotLights[i].angle);
        glUniform3f(shader->GetUniformLocation(direction_name.c_str()),
                    lights->spotLights[i].direction.x,
                    lights->spotLights[i].direction.y,
                    lights->spotLights[i].direction.z);
    }
    for (string s : ornamentNames) {
        RenderMesh(meshes[s], shader, mat4(1));
    }
}

void Tema1::FrameEnd()
{
    //DrawCoordinatSystem();
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{

}

void Tema1::OnKeyPress(int key, int mods)
{
    const float deltaScale = 0.1f;
    switch (key) {
    case GLFW_KEY_EQUAL:
        timeScale += deltaScale;
        printf("Time scale: %.2f\n", timeScale);
        break;

    case GLFW_KEY_MINUS:
        timeScale -= deltaScale;
        printf("Time scale: %.2f\n", timeScale);
        break;

    case GLFW_KEY_LEFT_BRACKET:
        timeScale = 0.0f;
        printf("Time scale: %.2f\n", timeScale);
        break;

    case GLFW_KEY_RIGHT_BRACKET:
        timeScale = 1.0f;
        printf("Time scale: %.2f\n", timeScale);
        break;

    case GLFW_KEY_1:
        shaderMode = cubemap;
        break;

    case GLFW_KEY_2:
        shaderMode = normal;
        break;

    case GLFW_KEY_3:
        shaderMode = wireframe;
        break;

    case GLFW_KEY_R:
        Reset();
        printf("Simulation Reset\n");
        break;
    }
}

void Tema1::OnKeyRelease(int key, int mods)
{

}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Shoot a ball from camera's perspective when left mouse button is pressed
    // and no other ball has been shot
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (ball == nullptr) {
            EngineComponents::Camera *camera = GetSceneCamera();
            vec3 direction = normalize(-camera->transform->GetLocalOZVector());
            ball = new Ball(camera->transform->GetWorldPosition(),
                            BALL_RADIUS, direction * BALL_INIT_SPEED);
        }
    }
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{

}

void Tema1::OnWindowResize(int width, int height)
{

}

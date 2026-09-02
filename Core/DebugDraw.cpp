#include "DebugDraw.h"
#include <Core/CameraComponent.h>
#include <Core/IRenderProxyManager.h>
#include <Core/Map.h>
DebugDraw::DebugDraw() {}

DebugDraw::~DebugDraw() {}

void DebugDraw::DrawLine(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                         const CoreMath::Vector4 &color)
{

    if (map == nullptr)
        return;

    uint32_t renderID = map->GetRenderID();

    DrawLine(renderID, start, end, color);
}

void DebugDraw::DrawLine(uint32_t renderID, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                         const CoreMath::Vector4 &color)
{

    Core::IRenderProxyManager *renderProxyManager = Core::IRenderProxyManager::GetInstance();
    if (renderProxyManager)
    {

        renderProxyManager->DrawLine(renderID, start, end, color);
    }
}

void DebugDraw::DrawLine(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &dir, float length,
                         const CoreMath::Vector4 &color)
{

    CoreMath::Vector3 dirNor = dir.GetNormalize();

    DrawLine(map, start, start + dir * length, color);
}

void DebugDraw::DrawCircle(Map *map, const CoreMath::Vector3 &center, const CoreMath::Vector3 &right,
                           const CoreMath::Vector3 &up, float radius, const CoreMath::Vector4 &color)
{

    if (map == nullptr)
        return;

    uint32_t renderID = map->GetRenderID();

    int segmentNum = 36;

    float segmentStepRadian = CoreMath::PI * 2.0f / segmentNum;

    std::vector<CoreMath::Vector3> vertexList;
    vertexList.push_back(center + right * radius); //(0도 정점)

    for (int i = 1; i <= segmentNum; ++i)
    {
        float radian = segmentStepRadian * i;
        CoreMath::Vector3 vertex;

        vertex = center + radius * cos(radian) * right + radius * sin(radian) * up;
        vertexList.push_back(vertex);
    }

    for (int i = 0; i < vertexList.size() - 1; ++i)
    {
        DrawLine(renderID, vertexList[i], vertexList[i + 1], color);
    }
}

void DebugDraw::DrawArrow(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                          const CoreMath::Vector4 &color)
{

    if (map == nullptr)
        return;

    DrawLine(map, start, end, color);

    CoreMath::Vector3 dir = end - start;
    float length = dir.Length();
    dir.Normalize();
    float headerLength = length * 0.2f;

    CoreMath::Vector3 circleCenter = end - dir * headerLength;
    CoreMath::Vector3 right, up;
    BuildOrthogonalBaiscVector(dir, right, up);

    float radius = 3.0f;
    DrawCircle(map, circleCenter, right, up, radius);

    // 원뿔 모서리
    float stepNum = 8;
    float stepRadian = CoreMath::PI * 2.0f / stepNum;

    for (int i = 0; i < stepNum; ++i)
    {
        float radian = i * stepRadian;

        CoreMath::Vector3 vertex;
        vertex = circleCenter + (right * cos(radian) + up * sin(radian)) * radius;
        DrawLine(map, vertex, end, color);
    }
}

void DebugDraw::DrawSphere(Map *map, const CoreMath::Vector3 &center, float radius, const CoreMath::Vector4 &color)
{

    if (map == nullptr)
        return;

    const CoreMath::Vector3 right = {1, 0, 0};
    const CoreMath::Vector3 up = {0, 1, 0};
    const CoreMath::Vector3 forward = {0, 0, 1};

    // 정면을 향하는 원
    DrawCircle(map, center, right, up, radius, color);

    // 위를 향하는 눕여진 원
    DrawCircle(map, center, right, forward, radius, color);

    // 세워진 원
    DrawCircle(map, center, forward, up, radius, color);
}

void DebugDraw::DrawBillboardCircle(Map *map, const CoreMath::Vector3 &eyePos, const CoreMath::Vector3 &center,
                                    float radius, const CoreMath::Vector4 &color)
{

    if (map == nullptr)
        return;

    CoreMath::Vector3 look = center - eyePos;
    look.Normalize();

    CoreMath::Vector3 right;
    CoreMath::Vector3 up;
    BuildOrthogonalBaiscVector(look, right, up);
    // 정면을 향하는 원
    DrawCircle(map, center, right, up, radius, color);
}

void DebugDraw::BuildOrthogonalBaiscVector(const CoreMath::Vector3 &dir, CoreMath::Vector3 &oRight,
                                           CoreMath::Vector3 &oUp)
{

    CoreMath::Vector3 worldUp = {0, 1, 0};
    if (std::abs(CoreMath::Dot(dir, worldUp)) > 0.98f)
    {
        worldUp = {0, 0, 1};
    }

    oRight = worldUp.Cross(dir).GetNormalize();
    oUp = dir.Cross(oRight).GetNormalize();
}
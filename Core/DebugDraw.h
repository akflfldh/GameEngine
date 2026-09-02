#pragma once

#include <Core/CoreDllExport.h>
#include <CoreMath/CoreMath.h>

class Map;

class CORE_API_LIB DebugDraw
{
  public:
    ~DebugDraw();

    static void DrawLine(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                         const CoreMath::Vector4 &color = {1, 0, 0, 1});

    static void DrawLine(uint32_t renderID, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                         const CoreMath::Vector4 &color);

    static void DrawLine(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &dir, float length,
                         const CoreMath::Vector4 &color = {1, 0, 0, 1});

    static void DrawCircle(Map *map, const CoreMath::Vector3 &center, const CoreMath::Vector3 &right,
                           const CoreMath::Vector3 &up, float radius, const CoreMath::Vector4 &color = {1, 0, 0, 1});

    static void DrawArrow(Map *map, const CoreMath::Vector3 &start, const CoreMath::Vector3 &end,
                          const CoreMath::Vector4 &color = {1, 0, 0, 1});

    static void DrawSphere(Map *map, const CoreMath::Vector3 &center, float radius,
                           const CoreMath::Vector4 &color = {1, 0, 0, 1});

    // 항상 카메라에 정면을 바라보는 원
    static void DrawBillboardCircle(Map *map, const CoreMath::Vector3 &eyePos, const CoreMath::Vector3 &center,
                                    float radius, const CoreMath::Vector4 &color = {1, 0, 0, 1});

  private:
    DebugDraw();

    static void BuildOrthogonalBaiscVector(const CoreMath::Vector3 &dir, CoreMath::Vector3 &oRight,
                                           CoreMath::Vector3 &oUp);
};

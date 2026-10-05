#include "EditorDebugDraw.h"
#include <Core/CameraComponent.h>
#include <Core/BoxColliderComponent.h>
#include <Core/DebugDraw.h>
#include <Core/LightComponent.h>
#include <Core/Map.h>
#include <cmath>
EditorDebugDraw::EditorDebugDraw() {}

EditorDebugDraw::~EditorDebugDraw() {}

void EditorDebugDraw::DrawBoxColliderVisual(Map *editorMap, const BoxColliderComponent *collider)
{
    if (!editorMap || !collider || collider->GetDeadState())
        return;

    const auto &center = collider->GetBoxCenter();
    const auto &halfExtent = collider->GetBoxHalfExtent();
    const auto &worldTransform = collider->GetTransformWorld();
    CoreMath::Vector3 corners[8];

    // 각 비트가 X/Y/Z 축의 +/-를 나타낸다. 중심과 반크기는 모두 컴포넌트 로컬 값이다.
    // SceneComponent는 월드 TRS를 합성하므로 여기서 TransformPoint를 한 번만 적용한다.
    // 월드 반크기를 미리 곱하면 물리 bridge와 달리 스케일이 중복 적용된다.
    for (int i = 0; i < 8; ++i)
    {
        const CoreMath::Vector3 localCorner = {
            center.X + ((i & 1) ? halfExtent.X : -halfExtent.X),
            center.Y + ((i & 2) ? halfExtent.Y : -halfExtent.Y),
            center.Z + ((i & 4) ? halfExtent.Z : -halfExtent.Z)};
        corners[i] = worldTransform.TransformPoint(localCorner);
        // 편집 중 비정상 Transform이 들어와도 NaN/Inf 선을 렌더 스냅샷에 넣지 않는다.
        if (!std::isfinite(corners[i].X) || !std::isfinite(corners[i].Y) || !std::isfinite(corners[i].Z))
            return;
    }

    const CoreMath::Vector4 color = {0.2f, 1.0f, 0.2f, 1.0f};
    // 한 축의 비트만 다른 꼭짓점을 연결하되 낮은 인덱스에서만 제출해 12개 모서리를 한 번씩 그린다.
    // DebugDraw 명령은 프레임마다 정리되므로 persistent 렌더 프록시 없이 매 Update에 재제출한다.
    //000, 001 ,010 011 , 100, 101 , 110 , 111  
    for (int i = 0; i < 8; ++i)
    {
        for (int axisBit = 1; axisBit <= 4; axisBit <<= 1)
        {
            
            if ((i & axisBit) == 0) //중복방지 
                DebugDraw::DrawLine(editorMap, corners[i], corners[i | axisBit], color);
        }
    }
}

void EditorDebugDraw::DrawLightVisual(Map *editorMap, LightComponent *lightComponent, CameraComponent *editorCameraCom)
{
    if (editorMap == nullptr || lightComponent == nullptr || editorCameraCom == nullptr)
        return;

    CoreMath::Vector3 lightPositionWorld = lightComponent->GetPositionWorld();

    switch (lightComponent->GetLightType())
    {
    case Core::ELightType::eDirectional:
    {
        CoreMath::Vector3 look = lightComponent->GetForwardWorld();

        DebugDraw::DrawArrow(editorMap, lightPositionWorld, lightPositionWorld + look * 10.0f);
    }
    break;
    case Core::ELightType::ePoint:
    {
        float radius = lightComponent->GetFalloffEnd();

        DebugDraw::DrawBillboardCircle(editorMap, editorCameraCom->GetPositionWorld(), lightPositionWorld, radius);
        DebugDraw::DrawSphere(editorMap, lightPositionWorld, radius);
    }
    break;

    case Core::ELightType::eSpot:

        break;
    }
}

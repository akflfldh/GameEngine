#include "EditorDebugDraw.h"
#include <Core/CameraComponent.h>
#include <Core/DebugDraw.h>
#include <Core/LightComponent.h>
#include <Core/Map.h>
EditorDebugDraw::EditorDebugDraw() {}

EditorDebugDraw::~EditorDebugDraw() {}

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

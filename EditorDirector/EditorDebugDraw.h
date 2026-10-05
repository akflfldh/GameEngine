#pragma once

class LightComponent;
class Map;
class CameraComponent;
class BoxColliderComponent;

class EditorDebugDraw
{
  public:
    ~EditorDebugDraw();

    static void DrawLightVisual(Map *editorMap, LightComponent *lightComponent, CameraComponent *editorCameraCom);
    static void DrawBoxColliderVisual(Map *editorMap, const BoxColliderComponent *collider);

  private:
    EditorDebugDraw();
};

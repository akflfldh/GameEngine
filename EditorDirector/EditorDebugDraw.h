#pragma once

class LightComponent;
class Map;
class CameraComponent;

class EditorDebugDraw
{
  public:
    ~EditorDebugDraw();

    static void DrawLightVisual(Map *editorMap, LightComponent *lightComponent, CameraComponent *editorCameraCom);

  private:
    EditorDebugDraw();
};

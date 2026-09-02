#pragma once

#include <memory>
#include <vector>

class Map;
class CameraComponent;
class IEditorVisualizer;

class EditorVisualizerManager
{
  public:
    EditorVisualizerManager();
    ~EditorVisualizerManager();

    void Initialize(Map *editorMap);
    void BindMap(Map *sourceMap);
    void UnBindMap();
    void Update(CameraComponent *editorCamera);
    //    void SetVisible(bool visible);
  private:
    std::vector<std::unique_ptr<IEditorVisualizer>> mVisualizerList;
};

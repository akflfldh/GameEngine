#pragma once

#include <memory>
#include <vector>

class Map;
class CameraComponent;
class IEditorVisualizer;

// Visualizer 인스턴스만 소유하며, 원본 맵과 시각화 객체를 담는 보조 맵은 소유하지 않는다.
// 워크스페이스의 바인딩 정책에 따라 실제 조명과 편집용 조명 시각화를 구분한다.
class EditorVisualizerManager
{
  public:
    EditorVisualizerManager();
    ~EditorVisualizerManager();

    void Initialize(Map *editorMap);
    void BindMap(Map *sourceMap, bool bUseLightVisualizer = true);
    void UnBindMap();
    void Update(CameraComponent *editorCamera);
    //    void SetVisible(bool visible);
  private:
    std::vector<std::unique_ptr<IEditorVisualizer>> mVisualizerList;
};

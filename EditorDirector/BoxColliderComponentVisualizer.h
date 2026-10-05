#pragma once

#include <EditorDirector/IEditorVisualizer.h>

/// 편집 맵의 BoxColliderComponent 설정을 읽어 박스 테두리만 그리는 Visualizer다.
/// 별도 Entity, 메시, 물리 바디를 생성하거나 소유하지 않는다.
/// 대상 컴포넌트는 매 Update에서 맵/오브젝트의 현재 목록을 조회하므로
/// 프레임 사이에 raw pointer 목록이나 추가·제거 콜백을 유지하지 않는다.
/// 맵 수명과 Update 호출은 기존 EditorMode/EditorVisualizerManager 경로가 관리한다.
class BoxColliderComponentVisualizer : public IEditorVisualizer
{
  public:
    void Update(CameraComponent *editorCamera) override;
    void UnBindMap() override;
};

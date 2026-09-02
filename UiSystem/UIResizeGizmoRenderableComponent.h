#pragma once

#include "UiSystem/UIRenderableComponent.h"

#include "UIResizeGizmoRenderableComponent.generated.h"
#include <glm/glm.hpp>

namespace UI
{

// ui요소에 4개의 모서리를 렌더하는기능이 제공된다 (가장 overlay가 높다.(resize용이기에))
class UISYSTEM_API REFLECT_CLASS(EngineClass) UIResizeGizmoRenderableComponent : public UIRenderableComponent
{
    GENERATED_BODY(UIResizeGizmoRenderableComponent)

  public:
    UIResizeGizmoRenderableComponent();
    virtual ~UIResizeGizmoRenderableComponent();

    virtual void Update(float deltaTime) override;

    void SetColor(const glm::vec4 &color);
    const glm::vec4 &GetColor() const;

    void SetThickness(float thickness);
    float GetThickness() const;

    // 버텍스버퍼의 크기는 16이상
    inline virtual size_t GetVertexNum() const override;
    virtual uint32_t GetVertices(UIVertex *oUIVertices) const override;

    inline virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;

    virtual int IsPointInside(float x, float y) const override;

    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;
#pragma region Input
    //    virtual void HandleInput(const Quad::InputData &inputData, bool &bConsume) override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    // virtual void OnMouseClick(const Quad::InputData &inputData, bool &bConsume) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
    //    virtual void OnKeyDown(const Quad::InputData &inputData, bool &bConsume) override;

    virtual bool IsHovered() const override;
    virtual void OnChangeHoverPart(int before, int after) override;
#pragma endregion
  private:
    // 주어진 좌표 ,x,y를 포함하는 활성화할 border 평가
    int EvaluateActiveBorder(float x, float y) const;

  private:
    // 테투리의 두께
    float mThickness;
    glm::vec4 mColor;
    glm::vec4 mActiveColor; // 활성화된 모서리의 색
    int mActiveBorder;      // 활성화(클릭,드래그)된 모서리넘버 //  시계방향으로 0 1 2 3     //-1은 없다.

    bool bHover;
    bool bPress;
};

} // namespace UI
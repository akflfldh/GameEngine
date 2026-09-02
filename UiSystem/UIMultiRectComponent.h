#pragma once

#include <UiSystem/UIRenderableComponent.h>
#include <vector>

#include "UIMultiRectComponent.generated.h"
namespace UI
{

struct RenderRect
{
    glm::vec2 mTopLeftPos;
    glm::vec2 mBottomRightPos;

    // size
};

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIMultiRectComponent : public UI::UIRenderableComponent
{
    GENERATED_BODY(UIMultiRectComponent)
  public:
    UIMultiRectComponent();
    virtual ~UIMultiRectComponent();

    virtual void Update(float deltaTime) override;

    virtual size_t GetVertexNum() const override;
    virtual uint32_t GetVertices(UIVertex *oUIVertices) const override; // nullptr 전달시 개수 리턴

    virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;

    // 렌더링할 사각형 영역 리스트
    void SetRects(const std::vector<RenderRect> &rects);

    void SetColor(float r, float g, float b, float a);
    void SetColor(const UI::UIColor &color);

    void Reset();

  protected:
    virtual void OnBegin() override;

  private:
    void UpdateVertex() const;
    void UpdateColor() const;

  private:
    std::vector<UI::RenderRect> mRectList;
    mutable bool mDirty;

    mutable std::vector<UI::UIVertex> mVertices;
    mutable std::vector<uint32_t> mIndices;

    UI::UIColor mColor;
    mutable bool mColorDirty;
};
} // namespace UI

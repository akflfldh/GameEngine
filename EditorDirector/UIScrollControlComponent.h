#pragma once

#include <UiSystem/IUIComponent.h>

#include "UIScrollControlComponent.generated.h"

enum class EScrollDirection : uint8_t
{
    eVertical = 0,
    eHorizontal
};

class REFLECT_CLASS(EngineClass) UIScrollControlComponent : public UI::IUIComponent
{
    GENERATED_BODY(UIScrollControlComponent)
  public:
    UIScrollControlComponent();
    virtual ~UIScrollControlComponent();
    virtual void Update(float deltaTime) override;

    virtual int IsPointInside(float x, float y) const override;

#pragma region Input
    virtual void HandleInput(const Quad::RawInputData &inputData, bool &bConsume) override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    virtual void OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
    virtual void OnKeyDown(const Quad::EKeyCode &key, bool &bConsume) override;

    // 기본 false 리턴
    virtual bool IsHovered() const override;

    virtual void OnChangeHoverPart(int before, int after) override;

#pragma endregion

    void SetScrollDirection(EScrollDirection dir);
    EScrollDirection GetScrollDirection() const;

    void SetDestElement(UI::UIElement *dest);

    void UpdateDestPanelPos();

  private:
    bool mHover;
    bool mPress;
    float mHoverColorIntensity;
    float mClickOffset;

    EScrollDirection mScrollDirection;

    glm::vec3 mOriginalColor;

    // scroll에의해서 이동시킬대상
    UI::UIElement *mDestElement;
};

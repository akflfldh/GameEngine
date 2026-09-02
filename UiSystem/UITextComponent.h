#pragma once

#include <CoreAsset/AssetPtr.h>
#include <UiSystem/UIRenderableComponent.h>
#include <string>
#include <vector>

#include <UITextComponent.generated.h>
#include <glm/detail/type_float.hpp>

namespace UI
{
struct TextSelectionRect
{
    glm::vec2 mPos;  // 선택 영역의 시작 좌표 (Top-Left)
    glm::vec2 mSize; // 선택 영역의 크기 (Width, Height)
};

// 텍스트의 출력, 즉 보여지는것만 담당하는 컴포넌트
class UISYSTEM_API REFLECT_CLASS(EngineClass) UITextComponent : public UIRenderableComponent
{

    GENERATED_BODY(UITextComponent)

  public:
    UITextComponent();
    virtual ~UITextComponent();

    void SetText(const std::string &text);
    const std::string &GetText() const;

    void SetFontSize(float fontSize);
    float GetFontSize() const;

    virtual size_t GetVertexNum() const override;
    virtual uint32_t GetVertices(UIVertex *oUIVertices) const override;

    virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;

    EUITextClipingMode GetClipingMode() const;
    void SetClipingMode(EUITextClipingMode mode);

    void SetOverflowMode(EUITextOverflowMode mode);
    EUITextOverflowMode GetOverflowMode() const;

#pragma region Padding
    void SetPadding(float left, float right, float top, float bottom);
    void SetPaddingLeft(float size);
    void SetPaddingRight(float size);
    void SetPaddingTop(float size);
    void SetPaddingBottom(float size);

    float GetPaddingLeft() const;
    float GetPaddingRight() const;
    float GetPaddingTop() const;
    float GetPaddingBottom() const;

    /* UIElement영역엥서  실제 패딩을 고려한 텍스트 영역  (UIElement에 상대적 로컬)*/
    float GetActualLeft() const;
    float GetActualRight() const;
    float GetActualTop() const;
    float GetActualBottom() const;

#pragma endregion

    float GetLineHeight() const;

    void SetColor(float r, float g, float b);
    void SetColor(const UIColor &color);

    glm::vec2 GetCursorPos(int index) const;
    int GetCursorIndexFromPos(const glm::vec2 &localPos);

    std::vector<TextSelectionRect> GetSelectionRects(int startCursorIndex, int endCursorIndex);

    void SetScrollOffsetX(float offset);
    float GetScrollOffsetX() const;

  protected:
    virtual void OnBegin() override;
    void MarkDirty();

  private:
    void UpdateIfNeed() const;

    void UpdateTextVertices() const;
    void UpdateColor() const;

    // 맨처음 시작위치(UIElement 영역에 상대적 로컬)
    float GetTextStartX() const;
    float GetTextStartY() const;

  private:
    std::string mText;
    uint32_t mSize;
    float mFontSize;

    mutable std::vector<UI::UIVertex> mVertexVec;
    mutable std::vector<uint32_t> mIndexVec;

    EUITextAlignment mAlignmentMode;

    EUITextOverflowMode mOverflowMode;
    EUITextClipingMode mClipingMode;

    CoreAsset::AssetPtr mFont;

#pragma region Padding

    float mPaddingLeft;
    float mPaddingRight;
    float mPaddingTop;
    float mPaddingBottom;
#pragma endregion

    mutable glm::vec2 mCachedOwnerElementSize;

    mutable bool mDirty = false;
    mutable bool mColorDirty = false;

#pragma region Scrolling

    float mScrollOffsetX = 0.0f;
#pragma endregion

    UIColor mColor;
};

} // namespace UI
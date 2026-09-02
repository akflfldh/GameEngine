#include "UITextComponent.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Font.h>
#include <CoreAsset/UIMaterialManager.h>
#include <UiSystem/UIElement.h>
#include <Utility/Utility.h>

UI::UITextComponent::UITextComponent()
    : mFont(2), mFontSize(21.0f), mPaddingLeft(0), mPaddingRight(0), mPaddingTop(0), mPaddingBottom(0),
      mAlignmentMode(EUITextAlignment::eLeft), mOverflowMode(EUITextOverflowMode::eOverflow)
{
    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();

    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    if (font)
    {
        mMeshComponent.mUIMaterial =
            uiMaterialManager->GetOrCreateDefaultFontMaterial(font->GetGlyphAltas().GetAssetID());
    }
}

UI::UITextComponent::~UITextComponent() {}

void UI::UITextComponent::SetText(const std::string &text)
{

    if (mText == text)
        return;

    mText = text;
    MarkDirty();
}

const std::string &UI::UITextComponent::GetText() const
{
    return mText;
    // TODO: 여기에 return 문을 삽입합니다.
}

void UI::UITextComponent::SetFontSize(float fontSize)
{

    mFontSize = fontSize;
    //    UpdateTextVertices();
    MarkDirty();
}

float UI::UITextComponent::GetFontSize() const
{
    return mFontSize;
}

size_t UI::UITextComponent::GetVertexNum() const
{
    UpdateIfNeed();
    return mVertexVec.size();
}

uint32_t UI::UITextComponent::GetVertices(UIVertex *oUIVertices) const
{
    UpdateIfNeed();

    glm::vec2 originalPos = GetOwnerUIElement()->mTransform.GetWorldPosition();

    if (oUIVertices == nullptr)
    {
        return static_cast<uint32_t>(mVertexVec.size());
    }

    for (size_t i = 0; i < mVertexVec.size(); ++i)
    {
        oUIVertices[i] = mVertexVec[i];
        oUIVertices[i].mPos += originalPos;
    }
    return static_cast<uint32_t>(mVertexVec.size());
}

size_t UI::UITextComponent::GetIndexNum() const
{

    UpdateIfNeed();

    return mIndexVec.size();
}

void UI::UITextComponent::GetIndices(uint32_t *oIndices) const
{
    UpdateIfNeed();

    for (size_t i = 0; i < mIndexVec.size(); ++i)
    {
        oIndices[i] = mIndexVec[i];
    }
}

UI::EUITextClipingMode UI::UITextComponent::GetClipingMode() const
{
    return mClipingMode;
}

void UI::UITextComponent::SetClipingMode(EUITextClipingMode mode)
{

    mClipingMode = mode;

    if (mClipingMode == EUITextClipingMode::eScissor)
    {
        GetOwnerUIElement()->SetUseScissorRect(true);
    }
    else
    {
        GetOwnerUIElement()->SetUseScissorRect(false);
    }
}

void UI::UITextComponent::SetOverflowMode(EUITextOverflowMode mode)
{
    if (mOverflowMode != mode)
    {
        mOverflowMode = mode;
        UpdateTextVertices();
    }
}

UI::EUITextOverflowMode UI::UITextComponent::GetOverflowMode() const
{
    return mOverflowMode;
}

void UI::UITextComponent::SetPadding(float left, float right, float top, float bottom)
{

    mPaddingLeft = left;
    mPaddingRight = right;
    mPaddingTop = top;
    mPaddingBottom = bottom;

    UpdateTextVertices();
}

void UI::UITextComponent::SetPaddingLeft(float size)
{
    mPaddingLeft = size;
    UpdateTextVertices();
}

void UI::UITextComponent::SetPaddingRight(float size)
{

    mPaddingRight = size;
    UpdateTextVertices();
}

void UI::UITextComponent::SetPaddingTop(float size)
{

    mPaddingTop = size;
    UpdateTextVertices();
}

void UI::UITextComponent::SetPaddingBottom(float size)
{

    mPaddingBottom = size;
    UpdateTextVertices();
}

float UI::UITextComponent::GetPaddingLeft() const
{
    return mPaddingLeft;
}

float UI::UITextComponent::GetPaddingRight() const
{
    return mPaddingRight;
}

float UI::UITextComponent::GetPaddingTop() const
{
    return mPaddingTop;
}

float UI::UITextComponent::GetPaddingBottom() const
{
    return mPaddingBottom;
}

float UI::UITextComponent::GetTextStartX() const
{

    switch (mAlignmentMode)
    {

    case EUITextAlignment::eLeft:

        return GetActualLeft() - mScrollOffsetX;

    case EUITextAlignment::eCenter:

        return GetActualLeft();

    case EUITextAlignment::eRight:

        return GetActualLeft();
    }
}

float UI::UITextComponent::GetTextStartY() const
{
    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    float startY = GetActualTop() + font->GetFontMatrix().mAscender * mFontSize;
    return startY;
}

float UI::UITextComponent::GetLineHeight() const
{
    if (mFont.Get())
    {
        CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());
        return font->GetFontMatrix().mLineHeight * mFontSize;
    }

    return 0.0f;
}

void UI::UITextComponent::SetColor(float r, float g, float b)
{
    SetColor({r, g, b});
}

void UI::UITextComponent::SetColor(const UIColor &color)
{
    mColor = color;

    uint32_t packedColor = UIColorUtility::PackColor(mColor.mR, mColor.mG, mColor.mB, 1.0f);
    mColorDirty = true;
}

glm::vec2 UI::UITextComponent::GetCursorPos(int index) const
{

    if (mDirty)
        UpdateTextVertices();
    std::vector<uint32_t> unicodePointVec = CoreUtility::Utility::GetUnicodeFromUTF8(mText);
    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    float cursorStartX = GetTextStartX();
    float cursorStartY = GetTextStartY();

    float cursorX = cursorStartX;
    float cursorY = cursorStartY;

    const CoreAsset::FontAltas &fontAltas = font->GetFontAltas();
    float pxRange = fontAltas.mPxRange * mFontSize;
    float glyphAtlasSize = fontAltas.mSize;

    const CoreAsset::FontMatrix &fontMatrix = font->GetFontMatrix();
    const float lineHeight = fontMatrix.mLineHeight * mFontSize;

    for (int i = 0; i < index; ++i)
    {
        const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(unicodePointVec[i]);

        // TODO 멀티라인일때만
        if (unicodePointVec[i] == '\n')
        {
            // if (mOverflowMode == MultiLine)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;

                continue;
            }
        }
        else if (unicodePointVec[i] == '\t')
        {

            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * 4 * mFontSize;
            continue;
        }
        else if (unicodePointVec[i] == ' ')
        {
            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * mFontSize;
            continue;
        }

        if (mOverflowMode == EUITextOverflowMode::eOverflow)
        {
        }
        else if (mOverflowMode == EUITextOverflowMode::eWordWrap)
        {
            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 이문자는다음줄에서 시작해야한다.
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;
            }
        }
        else if (mOverflowMode == EUITextOverflowMode::eEllipsis)
        {

            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 더이상 출력 X
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                break;
            }
        }

        // index 글자 뒤에 커서위치

        cursorX += fontGlyph->mAdvance * mFontSize;
    }

    return {cursorX, cursorY - (font->GetFontMatrix().mAscender * mFontSize)};
}

int UI::UITextComponent::GetCursorIndexFromPos(const glm::vec2 &localPos)
{
    if (mDirty)
        UpdateTextVertices();
    std::vector<uint32_t> unicodePointVec = CoreUtility::Utility::GetUnicodeFromUTF8(mText);
    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    float cursorStartX = GetTextStartX();
    float cursorStartY = GetTextStartY();

    float cursorX = cursorStartX;
    float cursorY = cursorStartY;

    const CoreAsset::FontAltas &fontAltas = font->GetFontAltas();
    float pxRange = fontAltas.mPxRange * mFontSize;
    float glyphAtlasSize = fontAltas.mSize;

    const CoreAsset::FontMatrix &fontMatrix = font->GetFontMatrix();
    const float lineHeight = fontMatrix.mLineHeight * mFontSize;

    float decender = -fontMatrix.mDescender * mFontSize;

    for (int i = 0; i < unicodePointVec.size(); ++i)
    {
        const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(unicodePointVec[i]);

        // TODO 멀티라인일때만
        if (unicodePointVec[i] == '\n')
        {
            // if (mOverflowMode == MultiLine)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;

                continue;
            }
        }
        else if (unicodePointVec[i] == '\t')
        {

            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * 4 * mFontSize;
            continue;
        }
        else if (unicodePointVec[i] == ' ')
        {
            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * mFontSize;
            continue;
        }

        if (mOverflowMode == EUITextOverflowMode::eOverflow)
        {
        }
        else if (mOverflowMode == EUITextOverflowMode::eWordWrap)
        {
            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 이문자는다음줄에서 시작해야한다.
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;
            }
        }
        else if (mOverflowMode == EUITextOverflowMode::eEllipsis)
        {

            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 더이상 출력 X
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                break;
            }
        }

        // index 글자 뒤에 커서위치

        cursorX += fontGlyph->mAdvance * mFontSize;

        if ((cursorX >= localPos.x) && ((cursorY + decender) >= localPos.y))
        {
            return i;
        }
    }

    return unicodePointVec.size();
}

std::vector<UI::TextSelectionRect> UI::UITextComponent::GetSelectionRects(int startCursorIndex, int endCursorIndex)
{
    if (mDirty)
        UpdateTextVertices();

    std::vector<uint32_t> unicodePointVec = CoreUtility::Utility::GetUnicodeFromUTF8(mText);
    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    float cursorStartX = GetTextStartX();
    float cursorStartY = GetTextStartY();

    float cursorX = cursorStartX;
    float cursorY = cursorStartY;

    const CoreAsset::FontAltas &fontAltas = font->GetFontAltas();
    float pxRange = fontAltas.mPxRange * mFontSize;
    float glyphAtlasSize = fontAltas.mSize;

    const CoreAsset::FontMatrix &fontMatrix = font->GetFontMatrix();
    const float lineHeight = fontMatrix.mLineHeight * mFontSize;

    float decender = -fontMatrix.mDescender * mFontSize;

    // cursor index 값은 문자의 index를 나타내는것으로 취급가능
    // ex ) cursor index가 0인 경우 cursor가 0번문자 앞에 존재.
    // 따라서 영역에 0번문자가 포함됨
    std::vector<UI::TextSelectionRect> selectionRects;

    float selectionLineStartX = 0.0f;
    TextSelectionRect selectionRect;

    // 마지막 selection rect처리여부를 검사하기위한 현재까지처리해서 rect를 구축한 마지막  cursorIndex
    bool isBuildingRect = false;
    for (int i = 0; i < endCursorIndex; ++i)
    {
        const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(unicodePointVec[i]);

        if (startCursorIndex == i)
        {
            selectionRect.mPos.x = cursorX;
            selectionRect.mPos.y = cursorY - fontMatrix.mAscender * mFontSize;
            selectionRect.mSize.y = lineHeight;
            isBuildingRect = true;
        }

        // TODO 멀티라인일때만
        if (unicodePointVec[i] == '\n')
        {
            // if (mOverflowMode == MultiLine)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;

                continue;
            }
        }
        else if (unicodePointVec[i] == '\t')
        {

            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * 4 * mFontSize;
            continue;
        }
        else if (unicodePointVec[i] == ' ')
        {
            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * mFontSize;
            continue;
        }

        if (mOverflowMode == EUITextOverflowMode::eOverflow)
        {
        }
        else if (mOverflowMode == EUITextOverflowMode::eWordWrap)
        {
            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 이문자는다음줄에서 시작해야한다.
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {

                // 하나의 selection rect 완성
                if (isBuildingRect)
                {
                    selectionRect.mSize.x = cursorX - selectionRect.mPos.x;
                    selectionRects.push_back(selectionRect);

                    isBuildingRect = false;
                }
                cursorX = cursorStartX;
                cursorY += lineHeight;

                // 새로운 selection rect 시작
                if (i >= startCursorIndex)
                {
                    selectionRect.mPos.x = cursorX;
                    selectionRect.mPos.y = cursorY - fontMatrix.mAscender * mFontSize;
                    isBuildingRect = true;
                }
            }
        }
        else if (mOverflowMode == EUITextOverflowMode::eEllipsis)
        {

            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 더이상 출력 X
            if ((GetActualRight()) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                break;
            }
        }

        cursorX += fontGlyph->mAdvance * mFontSize;
    }

    if (isBuildingRect)
    {
        selectionRect.mSize.x = cursorX - selectionRect.mPos.x;
        selectionRects.push_back(selectionRect);
    }

    return selectionRects;
}

void UI::UITextComponent::SetScrollOffsetX(float offset)
{

    if (offset != mScrollOffsetX)
    {
        mScrollOffsetX = offset;
        MarkDirty();
    }
}

float UI::UITextComponent::GetScrollOffsetX() const
{
    return mScrollOffsetX;
}

void UI::UITextComponent::OnBegin()
{

    UpdateTextVertices();
}

void UI::UITextComponent::MarkDirty()
{
    mDirty = true;
}

void UI::UITextComponent::UpdateIfNeed() const
{

    if (mCachedOwnerElementSize != GetOwnerUIElement()->mTransform.GetSize() || mDirty)
    {
        UpdateTextVertices();
    }

    if (mColorDirty)
    {
        UpdateColor();
    }
}

void UI::UITextComponent::UpdateTextVertices() const
{

    UI::UIElement *ownerElement = GetOwnerUIElement();

    glm::vec2 originPos = {0, 0};
    mCachedOwnerElementSize = ownerElement->mTransform.GetSize();

    std::vector<uint32_t> unicodePointVec = CoreUtility::Utility::GetUnicodeFromUTF8(mText);
    CoreAsset::Font *font = static_cast<CoreAsset::Font *>(mFont.Get());

    const CoreAsset::FontAltas &fontAltas = font->GetFontAltas();
    float pxRange = fontAltas.mPxRange * mFontSize;
    float glyphAtlasSize = fontAltas.mSize;

    const CoreAsset::FontMatrix &fontMatrix = font->GetFontMatrix();
    const float lineHeight = fontMatrix.mLineHeight * mFontSize;

    std::vector<UI::UIVertex> vertexVec;
    std::vector<uint32_t> indexVec;

    float cursorStartX = GetTextStartX();
    float cursorStartY = GetTextStartY();

    float cursorX = cursorStartX;
    float cursorY = cursorStartY;

    uint32_t indexOffset = 0;

    uint32_t packedColor = UIColorUtility::PackColor(mColor.mR, mColor.mG, mColor.mB, 1.0f);

    for (int i = 0; i < unicodePointVec.size(); ++i)
    {

        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;
        const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(unicodePointVec[i]);

        // TODO 멀티라인일때만
        if (unicodePointVec[i] == '\n')
        {
            // if (mOverflowMode == MultiLine)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;

                continue;
            }
        }
        else if (unicodePointVec[i] == '\t')
        {

            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * 4 * mFontSize;
            continue;
        }
        else if (unicodePointVec[i] == ' ')
        {
            const CoreAsset::FontGlyph *fontGlyph = font->GetGlyph(' ');
            cursorX += fontGlyph->mAdvance * mFontSize;
            continue;
        }

        if (mOverflowMode == EUITextOverflowMode::eOverflow)
        {
        }
        else if (mOverflowMode == EUITextOverflowMode::eWordWrap)
        {
            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 이문자는다음줄에서 시작해야한다.
            if ((GetActualRight() + originPos.x) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                cursorX = cursorStartX;
                cursorY += lineHeight;
            }
        }
        else if (mOverflowMode == EUITextOverflowMode::eEllipsis)
        {

            // 만약 이 문자를 배치하고나서 다음커서의 위치가 영역을 벗어난다면 더이상 출력 X
            if ((GetActualRight() + originPos.x) <= cursorX + fontGlyph->mAdvance * mFontSize)
            {
                break;
            }
        }

        left = cursorX + fontGlyph->mPlaneBounds.mLeft * mFontSize;
        right = cursorX + fontGlyph->mPlaneBounds.mRight * mFontSize;
        top = cursorY - fontGlyph->mPlaneBounds.mTop * mFontSize;
        bottom = cursorY - fontGlyph->mPlaneBounds.mBottom * mFontSize;

        float tex_left = fontGlyph->mAltasBounds.mLeft;
        float tex_right = fontGlyph->mAltasBounds.mRight;
        float tex_top = fontGlyph->mAltasBounds.mTop;
        float tex_bottom = fontGlyph->mAltasBounds.mBottom;

        // 시계방향

        UI::UIVertex vLeftTop;
        UI::UIVertex vRightTop;
        UI::UIVertex vRightBottom;
        UI::UIVertex vLeftBottom;

        vLeftTop.mPos.r = left;
        vLeftTop.mPos.g = top;

        vLeftTop.mTex.r = tex_left;
        vLeftTop.mTex.g = tex_top;

        vRightTop.mPos.r = right;
        vRightTop.mPos.g = top;

        vRightTop.mTex.r = tex_right;
        vRightTop.mTex.g = tex_top;

        vRightBottom.mPos.r = right;
        vRightBottom.mPos.g = bottom;

        vRightBottom.mTex.r = tex_right;
        vRightBottom.mTex.g = tex_bottom;

        vLeftBottom.mPos.r = left;
        vLeftBottom.mPos.g = bottom;

        vLeftBottom.mTex.r = tex_left;
        vLeftBottom.mTex.g = tex_bottom;

        vLeftTop.mColor = packedColor;
        vRightTop.mColor = packedColor;
        vLeftBottom.mColor = packedColor;
        vRightBottom.mColor = packedColor;

        vLeftTop.mCommonOne = pxRange;
        vRightTop.mCommonOne = pxRange;
        vRightBottom.mCommonOne = pxRange;
        vLeftBottom.mCommonOne = pxRange;

        vertexVec.push_back(vLeftTop);
        vertexVec.push_back(vRightTop);
        vertexVec.push_back(vRightBottom);
        vertexVec.push_back(vLeftBottom);

        // index

        indexVec.push_back(indexOffset + 0);
        indexVec.push_back(indexOffset + 1);
        indexVec.push_back(indexOffset + 3);
        indexVec.push_back(indexOffset + 1);
        indexVec.push_back(indexOffset + 2);
        indexVec.push_back(indexOffset + 3);
        indexOffset += 4;

        // cursor shift

        cursorX += mFontSize * fontGlyph->mAdvance;
    }

    mVertexVec.swap(vertexVec);
    mIndexVec.swap(indexVec);

    // update vertex ,index

    mDirty = false;
    mColorDirty = false;

    if (mOverflowMode == UI::EUITextOverflowMode::eWordWrap)
    {
        GetOwnerUIElement()->SetHeight(cursorY - fontMatrix.mDescender * mFontSize);
    }
}

void UI::UITextComponent::UpdateColor() const
{

    for (int i = 0; i < mVertexVec.size(); ++i)
    {
        mVertexVec[i].mColor = UIColorUtility::PackColor(mColor.mR, mColor.mG, mColor.mB, 1.0f);
    }

    mColorDirty = false;
}

float UI::UITextComponent::GetActualLeft() const
{
    // glm::vec2 size = GetOwnerUIElement()->mTransform.GetSize();

    return mPaddingLeft;
}

float UI::UITextComponent::GetActualRight() const
{
    glm::vec2 size = GetOwnerUIElement()->mTransform.GetSize();
    return size.r - mPaddingRight;
}

float UI::UITextComponent::GetActualTop() const
{
    // glm::vec2 size = GetOwnerUIElement()->mTransform.GetSize();
    return mPaddingTop;
}

float UI::UITextComponent::GetActualBottom() const
{
    glm::vec2 size = GetOwnerUIElement()->mTransform.GetSize();
    return size.g - mPaddingBottom;
}

#include "UiSystem/UIRectTransform.h"
#include "UiSystem/UIElement.h"
#include <glm/gtc/matrix_transform.hpp>

UI::UIRectTransform::UIRectTransform(UIElement *owner)
    : mOwner(owner), mLocalPosition(0, 0), mWorldPosition(0, 0), mSize({1, 1}), mDirty(true)
{
}

void UI::UIRectTransform::SetPositionLocal(const glm::vec2 &pos)
{
    mLocalPosition = pos;
    MarkDirty();
}

void UI::UIRectTransform::SetPositionLocal(float x, float y)
{

    SetPositionLocal({x, y});
}

void UI::UIRectTransform::SetSize(const glm::vec2 &size)
{
    mSize = size;
    MarkDirty();
}

void UI::UIRectTransform::SetSize(float w, float h)
{

    SetSize({w, h});
}

void UI::UIRectTransform::SetHeight(float h)
{

    SetSize(mSize.r, h);
}

void UI::UIRectTransform::SetWidth(float w)
{
    SetSize(w, mSize.g);
}

void UI::UIRectTransform::SetPositionWorld(const glm::vec2 &pos)
{
    mWorldPosition = pos;

    UIElement *parent = mOwner->GetParent();
    if (parent != nullptr)
    {
        UIRectTransform &parentTransform = parent->mTransform;
        parentTransform.UpdateIfDirty();
        SetPositionLocal(mWorldPosition - parentTransform.mWorldPosition);
    }
    else
    {
        SetPositionLocal(mWorldPosition);
    }
}

void UI::UIRectTransform::TranslateLocal(const glm::vec2 &shift)
{
    mLocalPosition += shift;
    MarkDirty();
}

UI::UIRectTransform::~UIRectTransform() {}

const glm::mat4x4 &UI::UIRectTransform::GetWorldMatrix()
{
    UpdateIfDirty();
    return mWorldMatrix;

    // TODO: 여기에 return 문을 삽입합니다.
}

const glm::vec2 &UI::UIRectTransform::GetWorldPosition() const
{

    UpdateIfDirty();
    return mWorldPosition;
}

const glm::vec2 &UI::UIRectTransform::GetLocalPosition() const
{
    return mLocalPosition;
    // TODO: 여기에 return 문을 삽입합니다.
}

const glm::vec2 &UI::UIRectTransform::GetSize() const
{
    return mSize;
}

void UI::UIRectTransform::GetQuadWorldPoints(glm::vec2 *out) const
{

    float w = std::round(mSize.x); //
    float h = std::round(mSize.y); //

    UpdateIfDirty();

    // out[0] = mWorldPosition + glm::vec2(-halfWidth, +halfHeight); // top-left
    // out[1] = mWorldPosition + glm::vec2(+halfWidth, +halfHeight); // top-right
    // out[2] = mWorldPosition + glm::vec2(+halfWidth, -halfHeight); // bottom-right
    // out[3] = mWorldPosition + glm::vec2(-halfWidth, -halfHeight); // bottom-left

    float offset = 0.0f;

    out[0] = (mWorldPosition + glm::vec2{offset, offset});         // top-left
    out[1] = (mWorldPosition + glm::vec2{w + offset, 0 + offset}); // top-right
    out[2] = (mWorldPosition + glm::vec2{w + offset, h + offset}); // bottom-right
    out[3] = (mWorldPosition + glm::vec2{0 + offset, h + offset}); // bottom-left
}

glm::vec2 UI::UIRectTransform::WorldToLocal(const glm::vec2 &worldPosToLocal) const
{
    const glm::vec2 &worldPos = GetWorldPosition();

    return worldPosToLocal - worldPos;
}

glm::vec2 UI::UIRectTransform::WorldToLocal(float x, float y) const
{
    return WorldToLocal({x, y});
}

void UI::UIRectTransform::MarkDirty()
{
    mDirty = true;
}

void UI::UIRectTransform::UpdateIfDirty() const
{

    UIElement *parent = mOwner->GetParent();

    if (parent != nullptr)
    {
        UIRectTransform &parentTransform = parent->mTransform;
        parentTransform.UpdateIfDirty();

        mWorldPosition = mLocalPosition + parentTransform.mWorldPosition;
        // mWorldPosition = glm::round(mWorldPosition);
        UpdateWorldMatrix();
    }
    else if (mDirty)
    {
        mWorldPosition = mLocalPosition;
        //  mWorldPosition = glm::round(mWorldPosition);
        UpdateWorldMatrix();
    }

    mDirty = false;
}

void UI::UIRectTransform::UpdateWorldMatrix() const
{

    mWorldMatrix = glm::translate(glm::mat4(1.0f), {mWorldPosition.x, mWorldPosition.y, 0}) *
                   glm::scale(glm::mat4(1.0f), {mSize.x, mSize.y, 1.0f});
}

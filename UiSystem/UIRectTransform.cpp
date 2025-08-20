#include "UiSystem/UIRectTransform.h"
#include "UiSystem/UIElement.h"

UI::UIRectTransform::UIRectTransform(UIElement *owner) : mOwner(owner) {}

void UI::UIRectTransform::SetPositionLocal(const CoreMath::Vector2 &pos)
{
    mLocalPosition = pos;
    MarkDirty();
}

void UI::UIRectTransform::SetSize(const CoreMath::Vector2 &size)
{
    mSize = size;
    MarkDirty();
}

void UI::UIRectTransform::TranslateLocal(const CoreMath::Vector2 &shift)
{
    mLocalPosition += shift;
    MarkDirty();
}

UI::UIRectTransform::~UIRectTransform() {}

const CoreMath::Matrix4X4 &UI::UIRectTransform::GetWorldMatrix()
{
    UpdateIfDirty();
    return mWorldMatrix;

    // TODO: 여기에 return 문을 삽입합니다.
}

const CoreMath::Vector2 &UI::UIRectTransform::GetWorldPosition() const
{

    UpdateIfDirty();
    return mWorldPosition;
}

const CoreMath::Vector2 &UI::UIRectTransform::GetLocalPosition() const
{
    return mLocalPosition;
    // TODO: 여기에 return 문을 삽입합니다.
}

const CoreMath::Vector2 &UI::UIRectTransform::GetSize() const
{
    return mSize;
}

void UI::UIRectTransform::GetQuadWorldPoints(CoreMath::Vector2 out[4]) const
{

    float halfWidth = mSize.X * 0.5f;
    float halfHeight = mSize.Y * 0.5f;

    UpdateIfDirty();

    out[0] = mWorldPosition + CoreMath::Vector2(-halfWidth, +halfHeight); // top-left
    out[1] = mWorldPosition + CoreMath::Vector2(+halfWidth, +halfHeight); // top-right
    out[2] = mWorldPosition + CoreMath::Vector2(+halfWidth, -halfHeight); // bottom-right
    out[3] = mWorldPosition + CoreMath::Vector2(-halfWidth, -halfHeight); // bottom-left
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
        UpdateWorldMatrix();
    }
    else if (mDirty)
    {
        mWorldPosition = mLocalPosition;
        UpdateWorldMatrix();
    }

    mDirty = false;
}

void UI::UIRectTransform::UpdateWorldMatrix() const
{
    mWorldMatrix = CoreMath::Matrix4X4::MakeTransform({mWorldPosition.X, mWorldPosition.Y, 0}, {0, 0, 0, 1},
                                                      {mSize.X, mSize.Y, 1});
}

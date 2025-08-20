#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreMath/CoreMath.h>
#include <stdint.h>
#include <vector>
namespace UI
{
class UIElement;
class UISYSTEM_API UIRectTransform
{
  public:
    UIRectTransform(UIElement *onwer);
    ~UIRectTransform();
    ;

    void SetPositionLocal(const CoreMath::Vector2 &pos);
    void SetSize(const CoreMath::Vector2 &size);

    void TranslateLocal(const CoreMath::Vector2 &shift);

    const CoreMath::Matrix4X4 &GetWorldMatrix();

    const CoreMath::Vector2 &GetWorldPosition() const;
    const CoreMath::Vector2 &GetLocalPosition() const;

    const CoreMath::Vector2 &GetSize() const;

    // 사각형 4개의 점을 얻는다.(렌더빌더시스템등 여러시스템에서 호출)
    // topleft, topright, bottom right, bottom left순서
    void GetQuadWorldPoints(CoreMath::Vector2 out[4]) const;

  private:
    void MarkDirty();
    void UpdateIfDirty() const;
    void UpdateWorldMatrix() const;

  private:
    CoreMath::Vector2 mLocalPosition;
    CoreMath::Vector2 mSize;

    mutable CoreMath::Vector2 mWorldPosition;
    // CoreMath::Vector2 mWorldSize;

    mutable CoreMath::Matrix4X4 mWorldMatrix;

    mutable bool mDirty = true;

    UIElement *mOwner = nullptr; // UIElement 내부에서 설정
};

} // namespace UI
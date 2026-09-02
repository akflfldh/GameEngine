#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>
namespace UI
{
class UIElement;
class UISYSTEM_API UIRectTransform
{
    friend class UIElement;

  public:
    UIRectTransform(UIElement *onwer);
    ~UIRectTransform();

    const glm::mat4x4 &GetWorldMatrix();

    const glm::vec2 &GetWorldPosition() const;
    const glm::vec2 &GetLocalPosition() const;

    const glm::vec2 &GetSize() const;

    // 사각형 4개의 점을 얻는다.(렌더빌더시스템등 여러시스템에서 호출)
    // topleft, topright, bottom right, bottom left순서
    void GetQuadWorldPoints(glm::vec2 *out) const;

    glm::vec2 WorldToLocal(const glm::vec2 &worldPosToLocal) const;
    glm::vec2 WorldToLocal(float x, float y) const;

  private:
    void MarkDirty();
    void UpdateIfDirty() const;
    void UpdateWorldMatrix() const;

    void SetPositionLocal(const glm::vec2 &pos);
    void SetPositionLocal(float x, float y);
    void SetPositionWorld(const glm ::vec2 &pos);

    void TranslateLocal(const glm::vec2 &shift);

    void SetSize(const glm::vec2 &size);
    void SetSize(float w, float h);
    void SetHeight(float h);
    void SetWidth(float w);

  private:
    glm::vec2 mLocalPosition;
    glm::vec2 mSize;

    mutable glm::vec2 mWorldPosition;
    // CoreMath::Vector2 mWorldSize;

    mutable glm::mat4x4 mWorldMatrix;

    mutable bool mDirty = true;

    UIElement *mOwner = nullptr; // UIElement 내부에서 설정
};

} // namespace UI
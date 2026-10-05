#pragma once

#include <cstddef>

namespace Core
{
class IDrawableOutline
{
  public:
    ~IDrawableOutline() = default;

    // 전체메시 적용
    virtual void SetDrawOutline(bool bDraw) = 0;

    // 서브메시 단일 적용
    virtual void SetDrawOutline(std::size_t subMeshIndex, bool bDraw) = 0;
};
} // namespace Core

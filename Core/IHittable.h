#pragma once
#include <stdint.h>

namespace CoreMath
{
struct Ray;
}

class Component;
class Object;
namespace Core
{

struct HitResult
{
    bool bHit = false;
    float mDistance = 3.402823466e+38F;
    Object *mHitObject = nullptr;
    Component *mHitComponent = nullptr;
};

class IHittable
{
  public:
    virtual ~IHittable() = default;

    virtual bool Raycast(const CoreMath::Ray &ray, Core::HitResult &oHitResult) = 0;
    virtual void DrawDebugCollider() = 0;

  private:
};

} // namespace Core

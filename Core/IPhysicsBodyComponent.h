#pragma once

#include <Physics/PhysicsType.h>



class IPhysicsBodyComponent
{
  public:
    IPhysicsBodyComponent();
    virtual ~IPhysicsBodyComponent() = 0;

    virtual bool IsPhysicsEnabled() const = 0;
    virtual EPhysicsBodyType GetPhysicsBodyType() const = 0;
    virtual bool IsPhysicsGravityEnabled() const = 0;
    virtual float GetPhysicsMass() const = 0;

    // virtual EPhysicsBodyShapeType GetPhysicsCollisionShapeType() const = 0;
    // virtual CoreMath::Vector3 GetPhysicsBoxHalfExtent() const = 0;

  private:
};

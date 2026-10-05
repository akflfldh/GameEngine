#pragma once

#include <Core/CorePhysicsType.h>
#include <Core/CoreType.h>

class IPhysicsBodyComponent
{
  public:
    IPhysicsBodyComponent();
    virtual ~IPhysicsBodyComponent() = 0;

    virtual bool IsPhysicsEnabled() const = 0;
    virtual EPhysicsBodyType GetPhysicsBodyType() const = 0;
    virtual bool IsPhysicsGravityEnabled() const = 0;
    virtual float GetPhysicsMass() const = 0;
    // 브리지가 Body 생성에 전달할 채널 ID다. 채널별 반응 테이블은 컴포넌트가 소유하지 않는다.
    virtual Core::CollisionChannelID GetCollisionChannelID() const = 0;
    virtual void OnCollisionResponse(const CollisionResponseData &data) = 0;
    // virtual EPhysicsBodyShapeType GetPhysicsCollisionShapeType() const = 0;
    // virtual CoreMath::Vector3 GetPhysicsBoxHalfExtent() const = 0;

  private:
};

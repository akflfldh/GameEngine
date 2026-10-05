#pragma once

#include <Core/CorePhysicsType.h>
#include <Core/CoreType.h>
#include <Core/IPhysicsBodyComponent.h>
#include <Core/IPhysicsShapeProvider.h>
#include <Core/SceneComponent.h>

#include "BoxColliderComponent.generated.h"

/// 메시나 렌더 프록시 없이 박스 하나와 바디 설정을 제공하는 물리 컴포넌트다.
/// 로컬 박스 데이터는 이 컴포넌트가, 실제 body/shape는 PhysicsBridgeSystem과 PhysicsScene이 소유한다.
/// 현재는 컴포넌트 하나당 바디 하나이며, 물리 결과는 Entity 루트에 반영한다.
/// 여러 Collider를 하나의 바디에 결합하거나 본별 애니메이션을 따라가는 기능은 담당하지 않는다.
class CORE_API_LIB REFLECT_CLASS(EngineClass) BoxColliderComponent : public SceneComponent,
                                                                     public IPhysicsBodyComponent,
                                                                     public IPhysicsShapeProvider
{
    GENERATED_BODY(BoxColliderComponent)

  public:
    BoxColliderComponent();
    ~BoxColliderComponent() override;

    void SetPhysicsEnabled(bool flag);
    void SetPhysicsBodyType(EPhysicsBodyType type);
    void SetPhysicsGravityEnabled(bool flag);
    void SetPhysicsMass(float mass);

    bool IsPhysicsEnabled() const override;
    EPhysicsBodyType GetPhysicsBodyType() const override;
    bool IsPhysicsGravityEnabled() const override;
    float GetPhysicsMass() const override;

    // 중심과 반크기는 컴포넌트 로컬 공간 값이다. 월드 스케일은 브리지에서 한 번만 적용한다.
    void SetBoxCenter(const CoreMath::Vector3 &center);
    void SetBoxHalfExtent(const CoreMath::Vector3 &halfExtent);
    const CoreMath::Vector3 &GetBoxCenter() const;
    const CoreMath::Vector3 &GetBoxHalfExtent() const;

    size_t GetPhysicsShapeCount() override;
    bool GetPhysicsShapeBuildData(size_t index, PhysicsShapeBuildData &outData) override;
    const PhysicsCollisionPreset &GetPhysicsCollisionPreset() const override;

    void Serialize(Arch &arch) override;
    void SyncPrefabComponentFrom(Component *prefabComponent) override;
    void OnActiveStateChanged(bool state) override;
    void FlushPropertyDirty() override;

    void SetCollisionChannelID(Core::CollisionChannelID id);
    Core::CollisionChannelID GetCollisionChannelID() const override;
    void OnCollisionResponse(const CollisionResponseData &data) override;

    CollisionResponseCallbackSystem mCollisionOverlapBeginCallbackSystem;
    CollisionResponseCallbackSystem mCollisionOverlapEndCallbackSystem;
    CollisionResponseCallbackSystem mCollisionBlockBeginCallbackSystem;
    CollisionResponseCallbackSystem mCollisionBlockEndCallbackSystem;

  protected:
    void OnOwnerObjectAddedToMap() override;
    void OnOwnerObjectRemovedFromMap() override;
    void OnDestoryRequested() override;
    void EndTick(float deltaTime) override;

  private:
    void MarkPhysicsSettingsDirty();
    void UpdateCollisionPreset();
    void RegisterPhysicsBody();
    void UnregisterPhysicsBody();

    PhysicsComponentSettings mPhysicsContext{};
    CoreMath::Vector3 mBoxCenter = {0.0f, 0.0f, 0.0f};
    CoreMath::Vector3 mBoxHalfExtent = {0.5f, 0.5f, 0.5f};
    // 위 박스 값으로 재구성하는 런타임 preset이다. 에셋을 참조하거나 중복 직렬화하지 않는다.
    PhysicsCollisionPreset mCollisionPreset;
    bool mOwnerInMap = false;
    bool mPhysicsRegistered = false;
    bool mPhysicsSettingsDirty = true;

    Core::CollisionChannelID mCollisionChannelID = Core::DefaultCollisionChannelID;
};

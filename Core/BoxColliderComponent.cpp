#include "BoxColliderComponent.h"

#include <Core/Entity.h>
#include <Core/Map.h>
#include <Core/PhysicsBridgeSystem.h>
#include <cmath>

namespace
{
bool IsFiniteVector(const CoreMath::Vector3 &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z);
}

bool IsValidHalfExtent(const CoreMath::Vector3 &value)
{
    return IsFiniteVector(value) && value.X > 0.0f && value.Y > 0.0f && value.Z > 0.0f;
}
} // namespace

BoxColliderComponent::BoxColliderComponent()
{
    UpdateCollisionPreset();
}

BoxColliderComponent::~BoxColliderComponent()
{
    UnregisterPhysicsBody();
}

void BoxColliderComponent::SetPhysicsEnabled(bool flag)
{
    if (mPhysicsContext.mPhysicsEnabled == flag)
        return;
    mPhysicsContext.mPhysicsEnabled = flag;
    MarkPhysicsSettingsDirty();
}

void BoxColliderComponent::SetPhysicsBodyType(EPhysicsBodyType type)
{
    if (type != EPhysicsBodyType::eStatic && type != EPhysicsBodyType::eDynamic && type != EPhysicsBodyType::eKinematic)
        return;
    if (mPhysicsContext.mPhysicsBodyType == type)
        return;
    mPhysicsContext.mPhysicsBodyType = type;
    MarkPhysicsSettingsDirty();
}

void BoxColliderComponent::SetPhysicsGravityEnabled(bool flag)
{
    if (mPhysicsContext.mUseGravity == flag)
        return;
    mPhysicsContext.mUseGravity = flag;
    MarkPhysicsSettingsDirty();
}

void BoxColliderComponent::SetPhysicsMass(float mass)
{
    // 정적 바디도 종류 변경 후 유효한 질량을 사용할 수 있도록 양수만 보관한다.
    if (!std::isfinite(mass) || mass <= 0.0f || mPhysicsContext.mMass == mass)
        return;
    mPhysicsContext.mMass = mass;
    MarkPhysicsSettingsDirty();
}

bool BoxColliderComponent::IsPhysicsEnabled() const
{
    return mPhysicsContext.mPhysicsEnabled;
}
EPhysicsBodyType BoxColliderComponent::GetPhysicsBodyType() const
{
    return mPhysicsContext.mPhysicsBodyType;
}
bool BoxColliderComponent::IsPhysicsGravityEnabled() const
{
    return mPhysicsContext.mUseGravity;
}
float BoxColliderComponent::GetPhysicsMass() const
{
    return mPhysicsContext.mMass;
}

void BoxColliderComponent::SetBoxCenter(const CoreMath::Vector3 &center)
{
    if (!IsFiniteVector(center) || mBoxCenter == center)
        return;
    mBoxCenter = center;
    MarkPhysicsSettingsDirty();
}

void BoxColliderComponent::SetBoxHalfExtent(const CoreMath::Vector3 &halfExtent)
{
    // 반크기가 0 이하인 퇴화 박스나 NaN은 충돌/관성 계산에 넘기지 않는다.
    if (!IsValidHalfExtent(halfExtent) || mBoxHalfExtent == halfExtent)
        return;
    mBoxHalfExtent = halfExtent;
    MarkPhysicsSettingsDirty();
}

const CoreMath::Vector3 &BoxColliderComponent::GetBoxCenter() const
{
    return mBoxCenter;
}
const CoreMath::Vector3 &BoxColliderComponent::GetBoxHalfExtent() const
{
    return mBoxHalfExtent;
}

size_t BoxColliderComponent::GetPhysicsShapeCount()
{
    return 1;
}

bool BoxColliderComponent::GetPhysicsShapeBuildData(size_t index, PhysicsShapeBuildData &outData)
{
    if (index != 0)
        return false;
    outData = PhysicsShapeBuildData{};
    outData.mShapeType = EPhysicsCollisionShapeType::eBox;
    outData.mLocalPosition = mBoxCenter;
    outData.mLocalRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    outData.mWorldPosition = GetTransformWorld().TransformPoint(mBoxCenter);
    outData.mWorldRotation = GetQuaternionWorld();
    outData.mBoxData.mLocalHalfExtent = mBoxHalfExtent;
    return true;
}

const PhysicsCollisionPreset &BoxColliderComponent::GetPhysicsCollisionPreset() const
{
    return mCollisionPreset;
}

void BoxColliderComponent::UpdateCollisionPreset()
{
    // 실제 생성 경로는 legacy BuildData 목록이 아니라 이 preset을 읽는다.
    // 크기에 월드 스케일을 미리 곱하지 않아 PhysicsBridgeSystem의 스케일 적용과 중복되지 않게 한다.
    mCollisionPreset.mShapeList.resize(1);
    auto &shape = mCollisionPreset.mShapeList.front();
    shape = PhysicsCollisionShapeData{};
    shape.mShapeType = EPhysicsCollisionShapeType::eBox;
    shape.mLocalPosition = mBoxCenter;
    shape.mBoxData.mLocalHalfExtents = mBoxHalfExtent;
}

void BoxColliderComponent::MarkPhysicsSettingsDirty()
{
    UpdateCollisionPreset();
    mPhysicsSettingsDirty = true;
    MarkPropertyDirty();
    if (auto map = GetMap())
        map->MarkAssetDirty();
}

void BoxColliderComponent::RegisterPhysicsBody()
{
    auto owner = GetOwnerObject();
    if (mPhysicsRegistered || !mOwnerInMap || !owner || !GetMap() || GetDeadState() || !owner->GetActive() ||
        !IsPhysicsEnabled())
        return;

    auto entity = dynamic_cast<Entity *>(owner);
    if (!entity || !entity->GetRootComponent())
        return;

    // 정적 메시와 동일하게 collider 위치로 body를 만들고 결과는 Entity 루트에 반영한다.
    const auto handle =
        PhysicsBridgeSystem::GetInstance()->RegisterPhysicsBodyComponent(entity->GetRootComponent(), this, this, this);
    mPhysicsRegistered = handle != PhysicsBodyHandleInValid;
}

void BoxColliderComponent::UnregisterPhysicsBody()
{
    if (!mPhysicsRegistered)
        return;
    PhysicsBridgeSystem::GetInstance()->UnregisterPhysicsBodyComponent(this);
    mPhysicsRegistered = false;
}

void BoxColliderComponent::FlushPropertyDirty()
{
    // 현재 bridge에는 shape/질량을 수정하는 API가 없으므로 설정 변경만 재등록한다.
    // 이 경로는 속도 등 시뮬레이션 상태를 초기화한다. 일반 transform 이동은 bridge의 버전 동기화에 맡긴다.
    if (mPhysicsSettingsDirty)
    {
        UnregisterPhysicsBody();
        UpdateCollisionPreset();
        mPhysicsSettingsDirty = false;
    }
    RegisterPhysicsBody();
}

void BoxColliderComponent::EndTick(float deltaTime)
{
    SceneComponent::EndTick(deltaTime);
    // 여러 setter 호출을 한 번의 재등록으로 합친다. map의 physics context가 늦게 준비된 경우도 재시도한다.
    if (mPhysicsSettingsDirty || !mPhysicsRegistered)
        FlushPropertyDirty();
}

void BoxColliderComponent::OnOwnerObjectAddedToMap()
{
    SceneComponent::OnOwnerObjectAddedToMap();
    mOwnerInMap = true;
    FlushPropertyDirty();
}

void BoxColliderComponent::OnOwnerObjectRemovedFromMap()
{
    // owner의 map 연결이 끊기기 전에 bridge가 바디를 찾을 수 있을 때 해제한다.
    UnregisterPhysicsBody();
    mOwnerInMap = false;
    SceneComponent::OnOwnerObjectRemovedFromMap();
}

void BoxColliderComponent::OnDestoryRequested()
{
    UnregisterPhysicsBody();
    mOwnerInMap = false;
    SceneComponent::OnDestoryRequested();
}

void BoxColliderComponent::OnActiveStateChanged(bool state)
{
    SceneComponent::OnActiveStateChanged(state);
    if (state)
        FlushPropertyDirty();
    else
        UnregisterPhysicsBody();
}

void BoxColliderComponent::Serialize(Arch &arch)
{
    if (arch.GetLoadingFlag())
        UnregisterPhysicsBody();
    SceneComponent::Serialize(arch);

    // 이 신규 타입의 설정만 저장한다. runtime preset/등록 상태와 legacy shape 목록은 저장하지 않는다.
    arch << mPhysicsContext.mPhysicsEnabled << mPhysicsContext.mPhysicsBodyType;
    arch << mPhysicsContext.mUseGravity << mPhysicsContext.mMass;
    arch << mBoxCenter << mBoxHalfExtent;
    arch << mCollisionChannelID;

    if (arch.GetLoadingFlag())
    {
        // 비정상 저장값이 physics에 전달되지 않도록 유효한 기본값으로 복원한다.
        if (!IsFiniteVector(mBoxCenter))
            mBoxCenter = {0.0f, 0.0f, 0.0f};
        if (!IsValidHalfExtent(mBoxHalfExtent))
            mBoxHalfExtent = {0.5f, 0.5f, 0.5f};
        if (!std::isfinite(mPhysicsContext.mMass) || mPhysicsContext.mMass <= 0.0f)
            mPhysicsContext.mMass = 1.0f;
        if (mPhysicsContext.mPhysicsBodyType != EPhysicsBodyType::eStatic &&
            mPhysicsContext.mPhysicsBodyType != EPhysicsBodyType::eDynamic &&
            mPhysicsContext.mPhysicsBodyType != EPhysicsBodyType::eKinematic)
            mPhysicsContext.mPhysicsBodyType = EPhysicsBodyType::eStatic;
        UpdateCollisionPreset();
        mPhysicsSettingsDirty = true;
    }
}

void BoxColliderComponent::SyncPrefabComponentFrom(Component *prefabComponent)
{
    auto source = dynamic_cast<BoxColliderComponent *>(prefabComponent);
    if (!source || source == this)
        return;
    SceneComponent::SyncPrefabComponentFrom(prefabComponent);
    SetPhysicsEnabled(source->IsPhysicsEnabled());
    SetPhysicsBodyType(source->GetPhysicsBodyType());
    SetPhysicsGravityEnabled(source->IsPhysicsGravityEnabled());
    SetPhysicsMass(source->GetPhysicsMass());
    SetBoxCenter(source->GetBoxCenter());
    SetBoxHalfExtent(source->GetBoxHalfExtent());
}

void BoxColliderComponent::SetCollisionChannelID(Core::CollisionChannelID id)
{
    if (mCollisionChannelID == id)
        return;
    mCollisionChannelID = id;
    // 채널은 Body 생성 시 복사되므로 기존 물리 설정 재등록 경로로 반영한다.
    // 동일 ID 설정은 재등록을 피하며, 실제 변경 시에는 기존 경로처럼 속도가 초기화된다.
    MarkPhysicsSettingsDirty();
}

Core::CollisionChannelID BoxColliderComponent::GetCollisionChannelID() const
{
    return mCollisionChannelID;
}

void BoxColliderComponent::OnCollisionResponse(const CollisionResponseData &data)
{

    if (data.responseType == ECollisionResponseType::eOverlap)
    {
        switch (data.eventType)
        {
        case ECollisionResponseEventType::eBegin:

            mCollisionOverlapBeginCallbackSystem.ExecuteCallbacks(data);

            break;
        case ECollisionResponseEventType::eEnd:
            mCollisionOverlapEndCallbackSystem.ExecuteCallbacks(data);
            break;
        }
    }
    else if (data.responseType == ECollisionResponseType::eBlock)
    {
        switch (data.eventType)
        {
        case ECollisionResponseEventType::eBegin:
            mCollisionBlockBeginCallbackSystem.ExecuteCallbacks(data);
            break;
        case ECollisionResponseEventType::eEnd:
            mCollisionBlockEndCallbackSystem.ExecuteCallbacks(data);
            break;
        }
    }
}

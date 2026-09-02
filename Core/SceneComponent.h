#pragma once

#include <Core/Component.h>
#include <Core/CoreDllExport.h>
#include <Core/Transform.h>
#include <CoreMath/CoreMath.h>
#include <vector>

#include "SceneComponent.generated.h"

class Map;

class CORE_API_LIB REFLECT_CLASS(EngineClass) SceneComponent : public Component
{
    friend class Map;

    GENERATED_BODY(SceneComponent)
  public:
    SceneComponent();
    virtual ~SceneComponent() override;

    virtual void OnBegin() override;

    // bKeepWorld : true  ->  자식의 월드가 유지되고 로컬이 변경된다.
    // bKeepWorld : false ->  자식의 로컬이 유지되고 자식의 월드가 변경된다.
    void SetParent(SceneComponent *parentCom, bool bKeepWorld = true);
    SceneComponent *GetParent() const;

    void SetScaleLocal(const CoreMath::Vector3 &scaleLocal);
    void SetQuaternionLocal(const CoreMath::Quaternion &quaternionLocal);
    void SetRotationLocal(const CoreMath::Vector3 &rotationLocal);
    void SetPositionLocal(const CoreMath::Vector3 &positionLocal);
    void AddQuaternionLocal(const CoreMath::Quaternion &quaternion);
    void AddRotationLocal(const CoreMath::Vector3 &rotation);
    void TranslationLocal(const CoreMath::Vector3 &shift);

    void SetPositionWorld(const CoreMath::Vector3 &positionWorld);
    void SetScaleWorld(const CoreMath::Vector3 &scaleWorld);
    void SetRotationWorld(const CoreMath::Vector3 &rotationWorld);
    void SetQuaternionWorld(const CoreMath::Quaternion &quaterionWorld);

    void AddMovementWorld(const CoreMath::Vector3 &movement);

    const CoreMath::Vector3 &GetScaleLocal() const;
    const CoreMath::Quaternion &GetQuaternionLocal() const;
    CoreMath::Vector3 GetRotationLocal() const;
    const CoreMath::Vector3 &GetPositionLocal() const;
    const CoreMath::Matrix4X4 &GetTransformLocal() const;

    const CoreMath::Vector3 &GetScaleWorld() const;
    const CoreMath::Quaternion &GetQuaternionWorld() const;
    const CoreMath::Vector3 &GetPositionWorld() const;
    CoreMath::Vector3 GetRotationWorld() const;
    const CoreMath::Matrix4X4 &GetTransformWorld() const;

    // virtual void Serialize(Arch &arch) override;

    const CoreMath::Vector3 GetForwardWorld() const;
    const CoreMath::Vector3 GetUpWorld() const;
    const CoreMath::Vector3 GetRightWorld() const;

    bool IsChildOf(SceneComponent *targetCom);

    virtual void OnTransformChanged();

    virtual void Serialize(Arch &arch) override;

    void ClearChildSceneComponentsForLoad();
    void AddChildSceneComponentForLoad(SceneComponent *com);

    const std::vector<SceneComponent *> &GetChildSceneComponentList() const;

    void SyncPrefabComponentFrom(Component *prefabComponent) override;

    unsigned long long GetTransformVersion() const;

  private:
    // dirty이거나, 부모의 version num와 일치하지않는다면 update한다.
    void UpdateIfDirty() const;
    void UpdateTransformLocal() const;

    // true-> 부모,조상과 동기화되어있다. false, world transform 갱신필요
    bool CheckVersionNum() const;
    void UpdateTransformWorld() const;

    // 기록하고있는 부모버전을 0으로초기화한다.
    // 따라서 다음에는 무조건 월드 트랜스폼을 다시 갱신한다..
    void ResetParentTransformVersion();

    void NotifyTransformPropertyChanged();
    void MarkOwnerMapAssetDirty();

    virtual void OnDestoryRequested() override;

  private:
    REFLECT_PROPERTY()
    Core::Transform mTransform;
    mutable unsigned long long mTransformVersion;
    // 부모가 가진 버전넘버와 비교하여 자신의 transform을 업데이트해야하는지를 결정한다.
    mutable unsigned long long mParentTransformVersion;

    REFLECT_PROPERTY(IsReference)
    SceneComponent *mParentSceneComponent;
    std::vector<SceneComponent *> mChildSceneComponentList;
};

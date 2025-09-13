#pragma once

#include <Core/Component.h>
#include <Core/QuadVector.h>
#include <Core/Transform.h>
namespace Core
{

class SceneComponent : public Component
{

  public:
    SceneComponent();
    virtual ~SceneComponent() override;

    virtual void Start() override;

    void SetParent(SceneComponent *parentCom);
    SceneComponent *GetParent() const;

    void SetScaleLocal(const CoreMath::Vector3 &scaleLocal);
    void SetQuaternionLocal(const CoreMath::Quaternion &quaternionLocal);
    void SetRotationLocal(const CoreMath::Vector3 &rotataionLocal);
    void SetPositionLocal(const CoreMath::Vector3 &positionLocal);

    CoreMath::Vector3 GetScaleLocal() const;
    CoreMath::Quaternion GetQuaternionLocal() const;
    CoreMath::Vector3 GetRotationLocal() const;
    CoreMath::Vector3 GetPositionLocal() const;
    const CoreMath::Matrix4X4 &GetTransformLocal() const;

    CoreMath::Vector3 GetScaleWorld() const;
    CoreMath::Quaternion GetQuaternionWorld() const;
    CoreMath::Vector3 GetPositionWorld() const;
    CoreMath::Vector3 GetRotationWorld() const;
    const CoreMath::Matrix4X4 &GetTransformWorld() const;

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

  private:
    Transform mTransform;
    mutable unsigned long long mTransformVersion;
    // 부모가 가진 버전넘버와 비교하여 자신의 transform을 업데이트해야하는지를 결정한다.
    mutable unsigned long long mParentTransformVersion;

    SceneComponent *mParentSceneComponent;
    QuadVector<SceneComponent *> mChildSceneComponentList;
};
} // namespace Core

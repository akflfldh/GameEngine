#pragma once
#include <Core/SceneComponent.h>

#include <CoreMath/CoreMath.h>
#include <CoreMath/Geometry.h>

#include <CameraComponent.generated.h>

enum class ECameraType : uint8_t
{
    ePerspective = 0,
    eOrthogonal
};

class CORE_API_LIB REFLECT_CLASS(EngineClass) CameraComponent : public SceneComponent
{
    GENERATED_BODY(CameraComponent)
  public:
    CameraComponent();
    virtual ~CameraComponent() override;

    virtual void OnBegin() override;

    // view matrix
    const CoreMath::Matrix4X4 &GetViewMatrix() const;

    float GetFov() const;
    float GetNear() const;
    float GetFar() const;
    float GetHeight() const;

    void SetNear(float z);
    void SetFar(float z);

    ECameraType GetType() const;
    void SetType(ECameraType type);

    virtual void Serialize(Arch &arch) override;

    virtual void SyncPrefabComponentFrom(Component *prefabComponent) override;

  protected:
    void OnOwnerObjectAddedToMap() override;

  private:
    void UpdateIfViewDirtyFlag() const;

  private:
    ECameraType mType;

    REFLECT_PROPERTY()
    float mFovY; // degree

    REFLECT_PROPERTY()
    float mNearZ;

    REFLECT_PROPERTY()
    float mFarZ;

    mutable CoreMath::Matrix4X4 mViewMatrix;
    mutable CoreMath::Matrix4X4 mLastWorldMatrix;

    // 직교투영시 너비
    float mHeight;
};

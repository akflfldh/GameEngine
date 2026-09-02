#pragma once

#include "Physics/PhysicsType.h"

struct PhysicsBoxCollisionQueryData;

preal GetSleepEpsilon();
void SetSleepEpsilon(preal value);

class PhysicsBody
{
  public:
    PhysicsBodyHandle mHandle = PhysicsBodyHandleInValid;
    CoreMath::Vector3 mPosition;
    CoreMath::Quaternion mRotation;
    CoreMath::Vector3 mScale = {1, 1, 1};

    CoreMath::Vector3 mVelocity;        // 속도(로컬)
    CoreMath::Vector3 mAngularVelocity; // 각속도

    // 앞선프레임에서 발생한 가속
    CoreMath::Vector3 mLastFrameAcceleration = {0, 0, 0};

    // 관성텐서
    CoreMath::Matrix4X4 mInertia = CoreMath::Matrix4X4::Identity;

    // 역 관성텐서
    CoreMath::Matrix4X4 mInertiaInverse = CoreMath::Matrix4X4::Identity;

    // 관성텐서 월드
    CoreMath::Matrix4X4 mInertiaWorld = CoreMath::Matrix4X4::Identity;

    // 역 관성텐서 월드
    CoreMath::Matrix4X4 mInertiaInverseWorld = CoreMath::Matrix4X4::Identity;

    float mMass = 1.0f;
    EPhysicsBodyType mBodyType;
    bool mGravity = false;
    CoreMath::Vector3 mForce = {0, 0, 0};  // 물체가 받는 알짜힘
    CoreMath::Vector3 mTorque = {0, 0, 0}; // 알짜 토크

    bool mTransformSyncDirty = false; // 외부 동기화용 더티 플래그

    // 실제 충돌 처리를 할 shapeList
    std::vector<PhysicsShapeHandle> mShapeHandleList;

// 슬립 최적화
#pragma region Sleep

    // 각속도와 선속도를 고려한 물체의 속도를 스칼라로 표현
    preal mMotion = 0.0f;

    // sleep 상태 여부
    bool mIsAwake = true;

    // Sleep상태로 변환이가능한 물체인지여부
    bool mCanSleep = true;

    preal mBaseBias = 0.7f;

#pragma endregion

  public:
    float GetInverseMass() const;

    // 물체의 속도, 각속도가 변하고나서 새롭게 업데이트해야하는 데이터들이 있다 (월드 관성텐서)
    // 그런 데이터들을 업데이트하는 메서드
    void UpdateDerivedData();

    void SetAwake(bool awake = true);

    // 속도가 일정값이하로 떨어지면 Sleep상태로 전환한다 .
    void UpdateAwakeState();

    void UpdateMotion(float deltaTime);

  private:
    void UpdateInertiaIWorld();
};

class PhysicsShape
{
  public:
    // handle
    PhysicsShapeHandle mHandle = PhysicsShapeHandleInValid;

    // Body Handle
    PhysicsBodyHandle mAttachedBodyHandle = PhysicsBodyHandleInValid;

    // shape type
    EPhysicsCollisionShapeType mPhysicsShapeType = EPhysicsCollisionShapeType::eBox;

    // local offset
    CoreMath::Vector3 mLocalPosition; // physicsBody에 상대적인 offset
    CoreMath::Quaternion mLocalRotation;
    CoreMath::Vector3 mLocalScale;

    CoreMath::Vector3 mWorldPosition;
    CoreMath::Quaternion mWorldRotation;

    CoreMath::Vector3 mWorldScale;

    // 스케일은 빠져야할듯?  box의 halfExtent는 scale을 고려해서 설정되서 들어올텐데?
    CoreMath::Matrix4X4 mWorldTransform; // position, rotation만고려된 월드 트랜스폼, (스케일은 포함되지않음)

    // axis를 get하는 메서드필요
    CoreMath::Vector3 GetAxis(int index) const;

    // Shape의 질량중심을 기준으로한 역 관성텐서
    CoreMath::Matrix4X4 mInverseInertiaTensor;

    float mMass = 1.0f;

    virtual void UpdateShapeSpecificData() = 0;

    // 반환 뿐만 아니라 InverseInertiaTensor도  같이 갱신하자
    virtual CoreMath::Matrix4X4 CalculateLocalInertiaTensor() = 0;

    // shape data

    //  PhysicsBoxShapeData mBoxData;
    // PhysicsSphereShapeData mSphereData;
    // PhysicsCapsuleShapeData mCapsuleData;
    // PhysicsConvexMeshShapeData mConvexMeshData;
    // PhysicsTriangleMeshShapeData mTriangleMeshData;
};

class PhysicsBoxShape : public PhysicsShape
{
  public:
    CoreMath::Vector3 mLocalHalfExtent = {0.5f, 0.5f, 0.5f};
    CoreMath::Vector3 mHalfExtent = {0.5f, 0.5f, 0.5f};
    virtual void UpdateShapeSpecificData() override;
    virtual CoreMath::Matrix4X4 CalculateLocalInertiaTensor() override;

    PhysicsBoxCollisionQueryData GetCollisionData() const;

  private:
};

struct PhysicsBoxCollisionQueryData
{
    CoreMath::Vector3 mPosition;
    CoreMath::Quaternion mRotation;
    CoreMath::Vector3 mHalfExtent;

    CoreMath::Matrix4X4 mWorldTransform;

    CoreMath::Vector3 GetAxis(int index) const;
    void Translate(const CoreMath::Vector3 &offset);
};
#pragma once

#include "Physics/PhysicsType.h"

struct ContactPairKey
{
    PhysicsShapeHandle mShapeHandleA = PhysicsShapeHandleInValid;
    PhysicsShapeHandle mShapeHandleB = PhysicsShapeHandleInValid;

    bool operator==(const ContactPairKey &rhs) const
    {
        if (mShapeHandleA == rhs.mShapeHandleA && mShapeHandleB == rhs.mShapeHandleB)
        {
            return true;
        }

        return false;
    }

    static ContactPairKey MakeContactPairKey(PhysicsShapeHandle a, PhysicsShapeHandle b);
};

struct ContactPairKeyHash
{
    size_t operator()(const ContactPairKey &key) const noexcept
    {
        return key.mShapeHandleA ^
               (key.mShapeHandleB + 0x9e3779b97f4a7c15ull + (key.mShapeHandleA << 6) + (key.mShapeHandleA >> 2));
    }
};

enum class EContactFeatureType
{
    eNone = 0,
    eFaceA,
    eFaceB,
    eEdgeEdge
};

struct EContactFeatrue
{
    EContactFeatureType mType = EContactFeatureType ::eNone;

    CoreMath::Vector3 mAxisA; // face , or EdgeA
    CoreMath::Vector3 mAxisB; // EdgeB

    int mFaceIndex = -1;
    int mEdgeAIndex = -1;
    int mEdgeBIndex = -1;
    bool bUseEdgeB = true;
};

/*

충돌접촉 감지로 부터 얻은 정보
*/
class CollisionContact
{

  public:
    CoreMath::Vector3 mPoint;
    CoreMath::Vector3 mNormal; // ShapeB -> ShapeA
    float mPenetration = 0.0f;
    EContactFeatrue mFeature;

  private:
};

struct CollisionContactManifold
{
    PhysicsShape *mShapeA = nullptr;
    PhysicsShape *mShapeB = nullptr;

    PhysicsShapeHandle mShapeAHandle = PhysicsShapeHandleInValid;

    PhysicsShapeHandle mShapeBHandle = PhysicsShapeHandleInValid;

    std::vector<CollisionContact> mContacts;
};

// 충돌 결과 (충돌반응에서 사용할 데이터들)
class PhysicsContact
{
  public:
    // body
    PhysicsBody *mBodyA = nullptr;
    PhysicsBody *mBodyB = nullptr;

    PhysicsShape *mShapeA = nullptr;
    PhysicsShape *mShapeB = nullptr;

    PhysicsShapeHandle mShapeAHandle = PhysicsShapeHandleInValid;
    PhysicsShapeHandle mShapeBHandle = PhysicsShapeHandleInValid;

    // 접촉지점의 상대위치
    CoreMath::Vector3 mRelativeContactPosition[2];

    // 접촉방향 접근 상대 속도
    CoreMath::Vector3 mRelativeVelocity = {0, 0, 0};

    // 충돌 법선 벡터
    CoreMath::Vector3 mNormal = {0, 0, 0};

    // 반발계수
    float mRestitution = 0.4f;

    float mDesiredVelocity = 0; // 필요한 속도 변화량

    // 충돌 지점
    CoreMath::Vector3 mPoint;

    // 관통 깊이
    float mPenetration = 0.0f;

    // 마찰

    EContactFeatrue mContactFeature;

    // 접촉 좌표계 변환행렬
    CoreMath::Matrix4X4 mContactToWorld;

    // 각속도
    CoreMath::Vector3 mAngularVelocity;

  public:
    // 필요한 내부데이터들을 생성 (기저변환행렬, 상대속도 등등 )
    void CalculateInternals(float deltaTime);

    // 접촉 좌표게 변환행렬을 계산한다.
    void CalculateContactBasic();

    // 단위임펄스당 필요한 속도변화량 계산
    // (접촉좌표계기준)
    CoreMath::Matrix4X4 CalculateVelocityPerUnitImpluse();

    // 필요한 속도변화량 계산
    // 현재는 충돌법선방향으로의 스칼라값
    void CalculateVelocity(float deltaTime);

    // 필요한 충격량 계산
    CoreMath::Vector3 CalculateImpulse(float deltaTime);

    void ResolvePenetration(CoreMath::Vector3 linearMove[2], CoreMath::Vector3 angularMove[2]);

    // 충격량을 적용하여 강체의 속도를 변화시킨다.
    void ApplyImpulse(float deltaTime, CoreMath::Vector3 linearChange[2], CoreMath::Vector3 angularChange[2]);

    // 해당 충돌이 발생했을때 강체를 깨운다 .
    // 깨어있는 물체와 충돌한다면 물체는 깨어난다.
    void matchAwakeState();
};

struct ContactManifold
{

    //// body
    // PhysicsBody *mBodyA = nullptr;
    // PhysicsBody *mBodyB = nullptr;

    // vector<PhysicsContact>
    std::vector<PhysicsContact> mContactList;
};

class KinematicContact
{
  public:
    PhysicsBody *mKinematicBody = nullptr;
    PhysicsBody *mOtherBody = nullptr;

    PhysicsBodyHandle mKinematicBodyHandle;
    PhysicsShapeHandle mKinematicShapeHandle;

    PhysicsBodyHandle mOtherBodyHandle;
    PhysicsShapeHandle mOtherShapeHandle;

    CoreMath::Vector3 mPoint;                 // 접촉지점
    CoreMath::Vector3 mNormalTowardKinematic; // 항상 kinematic 방향

    // 관통깊이
    float mPenetration = 0.0f;

    void ResolvePenetration();
};

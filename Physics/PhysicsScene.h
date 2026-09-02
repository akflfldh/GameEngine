#pragma once

#include <Physics/Contact.h>
#include <Physics/PhysicsDllMacro.h>
#include <Physics/PhysicsType.h>
#include <string>
#include <unordered_map>
#include <vector>

class PhysicsShape;
class PhysicsBoxShape;
class PhysicsContact;
struct ContactManifold;
struct CollisionContactManifold;
struct PhysicsBoxCollisionQueryData;
// 현재 에디터에서씬 플레이중이 아닌데도 PyhsicsBody,Shape들을 생성하고있음. 그것을 하지않도록 수정필요
// 각 shape의 충돌감지,부분을 별도의 세부 모듈로 분리필요

// broad phase , narrow phase 로 충돌 감지 단계를 분리할것

class PHYSICS_API PhysicsScene
{
  public:
    PhysicsScene();
    ~PhysicsScene();

    // 물리 시뮬레이션을 수행한다.
    void Update(float deltaTime);

    void SetID(PhysicsSceneID id);
    void SetName(const std::string &name);

    PhysicsSceneID GetID() const;
    std::string GetName() const;

    PhysicsBodyHandle CreatePhysicsBody(const PhysicsBodyDesc &bodyDesc);
    PhysicsShapeHandle CreatePhysicsShape(const PhysicsShapeDesc &shapeDesc);

    void DestroyPhysicsBody(PhysicsBodyHandle handle);
    void DestroyPhysicsShape(PhysicsShapeHandle handle);

    bool SetBodyTransform(PhysicsBodyHandle handle, const CoreMath::Vector3 &pos, const CoreMath::Quaternion &rot);
    bool GetBodyTransform(PhysicsBodyHandle handle, CoreMath::Vector3 &oPos, CoreMath::Quaternion &oRot) const;

    void EnqueueTransformCommand(const PhysicsTransformCommand &command);
    void EnqueueSizeCommand(const PhysicsSizeCommand &command);
    void EnqueueAddForceCommand(const PhysicsAddForceCommand &command);
    void EnqueueSetKinematicVelocityCommand(const PhysicsSetVelocityCommmand &command);

    const PhysicsFrameResult &GetPhysicsFrameResult() const;
    void ClearPhysicsFrameResult();

    CoreMath::Vector3 GetVelocity(PhysicsBodyHandle handle) const;
    float GetMass(PhysicsBodyHandle handle) const;

    PhysicsContact BuildPhysicsContact(PhysicsBody &bodyA, PhysicsBody &bodyB, PhysicsShape &shapeA,
                                       PhysicsShape &shapeB, const CollisionContact &source);

    KinematicContact BuildKinematicContact(PhysicsBody &bodyA, PhysicsBody &bodyB, PhysicsShape &shapeA,
                                           PhysicsShape &shapeB, const CollisionContact &source);

  private:
    PhysicsBody *GetPhysicsBody(PhysicsBodyHandle handle) const;
    PhysicsShape *GetPhysicsShape(PhysicsShapeHandle handle) const;

    void IncreaseNextPhysicsBodyHandle();
    void IncreaseNextPhysicsShapeHandle();

    PhysicsBody *AllocPhysicsBody();
    void BuildPhysicsBody(const PhysicsBodyDesc &bodyDesc, PhysicsBody &oBody);
    void RegisterPhysicsBody(PhysicsBody *body);
    void UnRegisterPhysicsBody(PhysicsBody *body);
    void ReleasePhysicsBodyMemory(PhysicsBody *body);

    PhysicsShape *AllocPhysicsShape(EPhysicsCollisionShapeType shapeTyp);
    void BuildPhysicsShape(PhysicsBody *body, const PhysicsShapeDesc &shapeDesc, PhysicsShape &oShape);
    void RegisterPhysicsShape(PhysicsShape *shape);
    void UnRegisterPhysicsShape(PhysicsShape *shape);
    void ReleasePhysicsShapeMemory(PhysicsShape *shape);

    // 임시메서드 (BODY가 하나의 Shape만 가정
    void SyncBodyFromShape(PhysicsBody *pBody);

    void ClearAll();

    void Step(float deltaTime);
#pragma region Collision

    // 충돌검출
    bool CheckBodyCollision(PhysicsBody &bodyA, PhysicsBody &bodyB);

    // Kinematic-static 충돌검출
    void CheckBodyKinematicStaticCollision();

    // dynamic 충돌검출
    void CheckBodyDynamicCollision();

    // manifold (contact)들을 새로 계산
    void RefreshManifold();

    void KinematicCollisionResolution(float deltaTime);
    // 충돌 반응
    void CollisionResolution(float deltaTime);

    // 관통깊이를 새롭게 계산
    void UpdatePenetrations(std::vector<PhysicsContact> &contactList);

    void ApplyImpluse(PhysicsContact &physicsContact); /* contact */

    bool DectectCollision(PhysicsShape &shapeA, PhysicsShape &shapeB, CollisionContactManifold &oManifoold,
                          bool &oBodySwap) const;

    bool DectectCollisionWithOffset(PhysicsShape &shapeA, const CoreMath::Vector3 &offset, PhysicsShape &shapeB,
                                    CollisionContactManifold &oManifoold, bool &oBodySwap) const;

    bool DetectBoxAndBox(PhysicsBoxCollisionQueryData &boxCollisionDataA,
                         PhysicsBoxCollisionQueryData &boxCollisionDataB, CollisionContactManifold &oManifoold,
                         bool &oBodySwap) const;

    bool CheckOverlap(const PhysicsBoxCollisionQueryData &boxCollisionDataA,
                      const PhysicsBoxCollisionQueryData &boxCollisionDataB, CoreMath::Vector3 axis,
                      CoreMath::Vector3 toCentre, unsigned int index, float &penetration,
                      unsigned int &bestIndex) const;

    // 축에 박스들을 사영후(반대각선을 사영) 관통깊이 반환
    float TryAxis(const PhysicsBoxCollisionQueryData &boxCollisionDataA,
                  const PhysicsBoxCollisionQueryData &boxCollisionDataB, const CoreMath::Vector3 &axis,
                  const CoreMath::Vector3 &toCentre) const;

    void FillContactDataFaceBoxBox(const PhysicsBoxCollisionQueryData &collisionDataA,
                                   const PhysicsBoxCollisionQueryData &collisionDataB, unsigned axisIndex,
                                   const CoreMath::Vector3 &toCentre, float penetration,
                                   CollisionContactManifold &oManifold) const;

    void FillContactDataEdgeBoxBox(const PhysicsBoxCollisionQueryData &collisionDataA,
                                   const PhysicsBoxCollisionQueryData &collisionDataB, unsigned int bestEdgeAIndex,
                                   unsigned int bestEdgeBIndex, bool bUseEdgeB, const CoreMath::Vector3 &toCentre,
                                   float penetration, CollisionContactManifold &oManifold) const;

    void AddManifold(const ContactManifold &manifold);
    void AddRigidBodyManifold(PhysicsBody &bodyA, PhysicsBody &bodyB, const CollisionContactManifold &manifold);
    void AddKinematicContacts(PhysicsBody &bodyA, PhysicsBody &bodyB, const CollisionContactManifold &manifold);

    void RecalculateBodyInertia(PhysicsBody &body);

    // 충돌 처리
    //  void ResolveContact(const PhysicsContact &contact);

#pragma endregion

#pragma region Force

    void CalcForces(PhysicsBody &body);

#pragma endregion

    void UpdateBodyEuler(float deltaTime, PhysicsBody &body);
    void UpdateKinematicBodyEuler(float deltaTime, PhysicsBody &body);

    void CalcInertia(PhysicsBody &body);

    void SyncBodiesFromExternal();
    void SyncBodiesToExternal();

    // 명령 리스트 비우기
    void ClearPhysicsCommand();

    void GetShapeLocalTransformFromBody(const PhysicsBody &body, PhysicsShape &shape);
    void UpdateShapeFromBody(const PhysicsBody &body, PhysicsShape &shape);

    // body의 shape들의 worldTransform을 갱신한다.
    void UpdateShapeWorldTransformFromBody(const PhysicsBody &body, PhysicsShape &shape);

    void RebuildShapesWorldTransformFromBody(const PhysicsBody &body);

    bool IsSleep(const PhysicsBody &bodyA, const PhysicsBody &bodyB) const;

    ECollisionResponsePath ClassifyCollisionPair(const PhysicsBody &bodyA, const PhysicsBody &bodyB) const;

    PhysicsGroundResult &GetOrCreateGroundResult(PhysicsBodyHandle handle, PhysicsShapeHandle shapeHandle);

    void UpdateGroundResult(const KinematicContact &contact);

    void CheckPreviousGroundResult();

    bool CheckGroundNormal(const CoreMath::Vector3 &normal);
    void FillGroundResult(bool bIsGrounded, const CoreMath::Vector3 &groundNormal, PhysicsBodyHandle groundHandle,
                          PhysicsGroundResult &oResult);

  private:
    PhysicsSceneID mID;
    std::string mName;

    std::vector<PhysicsBody *> mPhysicsBodyList;
    std::unordered_map<PhysicsBodyHandle, PhysicsBody *> mPhysicsBodyHandleTable;

    std::vector<PhysicsShape *> mPhysicsShapeList;
    std::unordered_map<PhysicsShapeHandle, PhysicsShape *> mPhysicsShapeHandleTable;

    PhysicsBodyHandle mNextPhysicsBodyHandle;
    PhysicsShapeHandle mNextPhysicsShapeHandle;

    PhysicsSceneForces mSceneForces;

    PhysicsCommandContext mPhysicsCommandContext;

    PhysicsFrameResult mPhysicsFrameResult;

    std::unordered_map<ContactPairKey, ContactManifold, ContactPairKeyHash> mContactManifoldTable;

    /*
    매프레임 초기화
    */
    std::vector<KinematicContact> mKinematicContactList;
};

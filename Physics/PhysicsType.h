#pragma once
#include <CoreBase/CollisionPhysicsType.h>
#include <CoreMath/CoreMath.h>
#include <stdint.h>
#include <vector>

using preal = float;

class PhysicsWorld;
class PhysicsScene;
class PhysicsBody;
class PhysicsShape;

using PhysicsSceneID = uint32_t;
#define PhysicsSceneIDInValid 0

using PhysicsShapeHandle = uint64_t;
#define PhysicsShapeHandleInValid 0

using PhysicsBodyHandle = uint64_t;
#define PhysicsBodyHandleInValid 0

using PhysicsCookedMeshHandle = uint64_t;
#define PhysicsCookedMeshHandleInValid 0

// enum class EPhysicsBodyShapeType : uint32_t
//{
//
//     eNone = 0,
//     eBox,
//     eConvex,
//     eTriangleMesh
// };

enum class EPhysicsBodyType : uint8_t
{

    eStatic = 0,
    eDynamic,
    eKinematic
};

enum class EPhysicsBodyMode
{
    eNone,
    eSelfBody,    // 이 component가 독립 body를 생성
    eAttachToBody // 다른 body에 shape로 붙음
};

struct PhysicsBoxShapeData
{
    CoreMath::Vector3 mLocalHalfExtent = {0.5f, 0.5f, 0.5f};
};

struct PhysicsSphereShapeData
{
    float mRadius = 0.5f;
};

struct PhysicsCapsuleShapeData
{
    float mRadius = 0.5f;
    float mHeight = 1.0f;
};

struct PhysicsConvexMeshShapeData
{
    PhysicsCookedMeshHandle mCookedMeshHandle = PhysicsCookedMeshHandleInValid;
};

struct PhysicsTriangleMeshShapeData
{
    PhysicsCookedMeshHandle mCookedMeshHandle = PhysicsCookedMeshHandleInValid;
};

// class PhysicsShape
//{
//   public:
//     // handle
//     PhysicsShapeHandle mHandle = PhysicsShapeHandleInValid;
//
//     // Body Handle
//     PhysicsBodyHandle mAttachedBodyHandle = PhysicsBodyHandleInValid;
//
//     // shape type
//     EPhysicsShapeType mPhysicsShapeType = EPhysicsShapeType::eBox;
//
//     // local offset
//     CoreMath::Vector3 mLocalPosition; // physicsBody에 상대적인 offset
//     CoreMath::Quaternion mLocalRotation;
//
//     CoreMath::Vector3 mWorldPosition;
//     CoreMath::Quaternion mWorldRotation;
//
//     CoreMath::Matrix4X4 mWorldTransform; // position, scale, rotation받아서 맨처음 설정
//
//     // axis를 get하는 메서드필요
//     CoreMath::Vector3 GetAxis(int index);
//
//     // shape data
//
//     //  PhysicsBoxShapeData mBoxData;
//     // PhysicsSphereShapeData mSphereData;
//     // PhysicsCapsuleShapeData mCapsuleData;
//     // PhysicsConvexMeshShapeData mConvexMeshData;
//     // PhysicsTriangleMeshShapeData mTriangleMeshData;
// };

// struct PhysicsCollisionShape
//{
//     EPhysicsBodyShapeType mShapeType = EPhysicsBodyShapeType::eNone;
//     PhysicsBoxShapeData mBoxData;
//     PhysicsConvexMeshShapeData mConvexMeshData;
//     PhysicsTriangleMeshShapeData mTriangleMeshData;
// };

struct PhysicsShapeDesc
{
    // shape가 부착할 body handle , 설정되지않는다면 shape는 생성되지않는다.
    PhysicsBodyHandle mBodyHandle = PhysicsBodyHandleInValid;
    EPhysicsCollisionShapeType mShapeType = EPhysicsCollisionShapeType::eBox;

    CoreMath::Vector3 mLocalPosition = {0, 0, 0};
    CoreMath::Quaternion mLocalRotation;
    CoreMath::Vector3 mLocalScale = {1, 1, 1};

    CoreMath::Vector3 mWorldPosition = {0, 0, 0};
    CoreMath::Quaternion mWorldRotation;
    CoreMath::Vector3 mWorldScale = {1, 1, 1};

    PhysicsBoxShapeData mBoxData;
    PhysicsSphereShapeData mSphereData;
    PhysicsCapsuleShapeData mCapsuleData;
    PhysicsConvexMeshShapeData mConvexMeshData;
    PhysicsTriangleMeshShapeData mTriangleMeshData;
};

struct PhysicsBodyDesc
{
    EPhysicsBodyType mBodyType = EPhysicsBodyType::eStatic;
    CoreMath::Vector3 mPosition = {0, 0, 0};
    CoreMath::Quaternion mRotation;
    CoreMath::Vector3 mVelocity = {0, 0, 0};
    CoreMath::Vector3 mForce = {0, 0, 0};
    float mMass = 1.0f;
    bool mGravity = false;
};

struct PhysicsSceneForces
{
    CoreMath::Vector3 mGravity = {0.0f, -9.8f, 0.0f};
};

struct PhysicsTransformCommand
{
    PhysicsBodyHandle mBodyHandle = PhysicsBodyHandleInValid;
    PhysicsShapeHandle mShapeHandle = PhysicsShapeHandleInValid;
    CoreMath::Vector3 mWorldPosition;
    CoreMath::Quaternion mWorldRotation;
    CoreMath::Vector3 mWorldScale = {1, 1, 1};
};

struct PhysicsSizeCommand
{
    // PhysicsBodyHandle mBodyHandle;
    PhysicsShapeHandle mShapeHandle;
    CoreMath::Vector3 mSize;
};

struct PhysicsAddForceCommand
{
    PhysicsBodyHandle mBodyHandle;
    CoreMath::Vector3 mForce;
};

struct PhysicsSetVelocityCommmand
{
    PhysicsBodyHandle mBodyHandle;
    CoreMath::Vector3 mVelocity;
};

struct PhysicsCommandContext
{
    std::vector<PhysicsTransformCommand> mTransformCommandList;
    std::vector<PhysicsSizeCommand> mSizeCommnadList;
    std::vector<PhysicsAddForceCommand> mAddForceCommandList;
    std::vector<PhysicsSetVelocityCommmand> mSetVelocityCommandList;
};

struct PhysicsTransformResult
{
    PhysicsBodyHandle mBodyHandle = PhysicsBodyHandleInValid;
    PhysicsShapeHandle mShapeHandle = PhysicsShapeHandleInValid;
    CoreMath::Vector3 mPosition;
    CoreMath::Quaternion mRotation;
};

struct PhysicsGroundResult
{
    PhysicsBodyHandle mBodyHandle = PhysicsBodyHandleInValid;
    PhysicsShapeHandle mShapeHandle = PhysicsShapeHandleInValid;

    bool mIsGrounded = false;
    CoreMath::Vector3 mGroundNormal = {0, 1, 0};

    PhysicsBodyHandle mGroundBodyHandle = PhysicsBodyHandleInValid;
};

struct PhysicsFrameResult
{
    std::vector<PhysicsTransformResult> mTransformResults;
    std::vector<PhysicsGroundResult> mGroundResults;
};

enum class ECollisionResponsePath
{
    eNone = 0,
    eRigidBodySolver,
    eKinematicSolver
};

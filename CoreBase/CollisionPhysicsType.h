#pragma once

#include <CoreBase/Arch.h>
#include <CoreMath/CoreMath.h>
#include <stdint.h>
#include <vector>

enum class EPhysicsCollisionShapeType : uint8_t
{
    eNone = 0,
    eBox,
    eSphere,
    eCapsule,
    eConvexMesh,
    eTriangleMesh
};

struct PhysicsBoxCollisionData
{
    CoreMath::Vector3 mLocalHalfExtents = {0.5f, 0.5f, 0.5f};

    Arch &operator<<(Arch &arch)
    {
        arch << mLocalHalfExtents;
    }
};

inline Arch &operator<<(Arch &arch, PhysicsBoxCollisionData &data)
{
    arch << data.mLocalHalfExtents;

    return arch;
}

struct PhysicsSphereCollisionData
{
    float mRadius = 0.5f;
};

struct PhysicsCollisionShapeData
{

    EPhysicsCollisionShapeType mShapeType = EPhysicsCollisionShapeType::eNone;
    PhysicsBoxCollisionData mBoxData;

    CoreMath::Vector3 mLocalPosition = {0, 0, 0};
    CoreMath::Quaternion mLocalRotation = {0, 0, 0, 1};
    CoreMath::Vector3 mLocalScale = {1, 1, 1};

    /*   Arch &operator<<(Arch &arch)
       {

           arch << mShapeType;
           arch << mBoxData;
           arch << mLocalPosition;
           arch << mLocalRotation;
           arch << mLocalScale;
       }*/
};

inline Arch &operator<<(Arch &arch, PhysicsCollisionShapeData &data)
{
    arch << data.mShapeType;
    arch << data.mBoxData;
    arch << data.mLocalPosition;
    arch << data.mLocalRotation;
    arch << data.mLocalScale;

    return arch;
}

struct PhysicsCollisionPreset
{
    std::vector<PhysicsCollisionShapeData> mShapeList;
};

inline Arch &operator<<(Arch &arch, PhysicsCollisionPreset &data)
{
    arch << data.mShapeList;

    return arch;
}
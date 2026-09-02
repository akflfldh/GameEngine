#pragma once

#include <Core/CoreType.h>
#include <CoreBase/Arch.h>
#include <Physics/PhysicsType.h>

enum class EPhysicsCollisionShapePolicy : uint8_t
{
    eManual = 0,
    eAuto
};

struct PhysicsShapeBuildData
{
    EPhysicsCollisionShapeType mShapeType = EPhysicsCollisionShapeType::eBox;

    CoreMath::Vector3 mLocalPosition = {0, 0, 0};
    CoreMath::Quaternion mLocalRotation;

    CoreMath::Vector3 mWorldPosition = {0, 0, 0};
    CoreMath::Quaternion mWorldRotation;

    PhysicsBoxShapeData mBoxData;
    PhysicsSphereShapeData mSphereData;
    PhysicsCapsuleShapeData mCapsuleData;
    PhysicsConvexMeshShapeData mConvexMeshData;
    PhysicsTriangleMeshShapeData mTriangleMeshData;
};

struct PhysicsComponentSettings
{
    bool mPhysicsEnabled = false;
    EPhysicsBodyType mPhysicsBodyType = EPhysicsBodyType::eStatic;
    bool mUseGravity = true;
    float mMass = 1.0f;

    // 레거시
    // 향후 제거될거임
    EPhysicsCollisionShapePolicy mShapePolicy;
    // 레거시
    std::vector<PhysicsShapeBuildData> mShapeBuildDataList;
};

inline Arch &operator<<(Arch &arch, PhysicsBoxShapeData &data)
{
    arch << data.mLocalHalfExtent;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsSphereShapeData &data)
{
    arch << data.mRadius;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsCapsuleShapeData &data)
{
    arch << data.mRadius;
    arch << data.mHeight;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsConvexMeshShapeData &data)
{
    arch << data.mCookedMeshHandle;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsTriangleMeshShapeData &data)
{
    arch << data.mCookedMeshHandle;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsShapeBuildData &data)
{
    arch << data.mShapeType;
    arch << data.mLocalPosition;
    arch << data.mLocalRotation;
    arch << data.mWorldPosition;
    arch << data.mWorldRotation;
    arch << data.mBoxData;
    arch << data.mSphereData;
    arch << data.mCapsuleData;
    arch << data.mConvexMeshData;
    arch << data.mTriangleMeshData;
    return arch;
}

inline Arch &operator<<(Arch &arch, PhysicsComponentSettings &settings)
{
    arch << settings.mPhysicsEnabled;
    arch << settings.mPhysicsBodyType;
    arch << settings.mUseGravity;
    arch << settings.mMass;
    arch << settings.mShapePolicy;
    arch << settings.mShapeBuildDataList;
    return arch;
}

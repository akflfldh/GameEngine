#pragma once

#include <Core/CoreDllExport.h>
#include <CoreBase/CollisionPhysicsType.h>
#include <Physics/PhysicsType.h>

struct PhysicsShapeBuildData;

class CORE_API_LIB IPhysicsShapeProvider
{
  public:
    IPhysicsShapeProvider();
    virtual ~IPhysicsShapeProvider() = 0;

    // virtual EPhysicsShapeType GetPhysicsCollisionShapeType() const = 0;
    // virtual CoreMath::Vector3 GetPhysicsBoxHalfExtent() const = 0;

    virtual size_t GetPhysicsShapeCount() = 0;
    virtual bool GetPhysicsShapeBuildData(size_t index, PhysicsShapeBuildData &oData) = 0;

    virtual const PhysicsCollisionPreset &GetPhysicsCollisionPreset() const = 0;

  private:
};

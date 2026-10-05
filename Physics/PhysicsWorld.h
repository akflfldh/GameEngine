#pragma once

#include <Physics/PhysicsDllMacro.h>
#include <Physics/PhysicsType.h>
#include <string>
#include <unordered_map>
#include <vector>

class PhysicsScene;

class PHYSICS_API PhysicsWorld
{
  public:
    PhysicsWorld();
    ~PhysicsWorld();

    PhysicsSceneID CreateNewScene();
    PhysicsScene *GetScene(PhysicsSceneID id) const;

    void DestroyScene(PhysicsSceneID id);
    void DestroyScene(PhysicsScene *scene);

    void SetCollisionChannelResponseTable(const PhysicsCollisionChannelResponseTable &table);
    void SetCollisionChannelResponseTable(PhysicsCollisionChannelResponseTable &&table);

  private:
    void ClearAll();

  private:
    std::vector<PhysicsScene *> mSceneList;
    std::unordered_map<PhysicsSceneID, PhysicsScene *> mSceneIDTable;

    PhysicsSceneID mNextSceneID = 1;

    PhysicsCollisionChannelResponseTable mCollisionChannelResponseTable;
};

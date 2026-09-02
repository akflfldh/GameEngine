#include "PhysicsWorld.h"
#include <Physics/PhysicsScene.h>
#include <algorithm>

PhysicsWorld::PhysicsWorld() {}

PhysicsWorld::~PhysicsWorld()
{

    ClearAll();
}

PhysicsSceneID PhysicsWorld::CreateNewScene()
{

    PhysicsScene *scene = new PhysicsScene;

    if (scene == nullptr)
        return PhysicsSceneIDInValid;

    scene->SetID(mNextSceneID++);
    mSceneList.push_back(scene);
    mSceneIDTable[scene->GetID()] = scene;

    return scene->GetID();
}

PhysicsScene *PhysicsWorld::GetScene(PhysicsSceneID id) const
{

    auto it = mSceneIDTable.find(id);

    if (it == mSceneIDTable.end())
    {
        return nullptr;
    }

    return it->second;
}

void PhysicsWorld::DestroyScene(PhysicsSceneID id)
{

    auto it = mSceneIDTable.find(id);

    if (it == mSceneIDTable.end())
    {
        return;
    }

    // 씬 파괴
    DestroyScene(it->second);
}

void PhysicsWorld::DestroyScene(PhysicsScene *scene)
{

    if (scene == nullptr)
        return;

    // 파괴

    auto it = std::find(mSceneList.begin(), mSceneList.end(), scene);

    if (it != mSceneList.end())
    {
        std::iter_swap(it, mSceneList.end() - 1);
        mSceneList.pop_back();
    }

    mSceneIDTable.erase(scene->GetID());

    delete scene;
}

void PhysicsWorld::ClearAll()
{

    std::vector<PhysicsScene *> tempList = mSceneList;

    for (auto scene : tempList)
    {
        DestroyScene(scene);
    }

    mSceneList.clear();
    mSceneIDTable.clear();

    mNextSceneID = 1;
}
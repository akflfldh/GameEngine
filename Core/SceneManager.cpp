#include "SceneManager.h"
#include <World.h>
Core::SceneManager *Core::SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

Core::SceneManager::SceneManager() : mUserWorld(nullptr)
{

    mUserWorld = new World;
}

Core::SceneManager::~SceneManager() {}

void Core::SceneManager::Update(float deltaTime)
{

    if (mUserWorld)
    {
        mUserWorld->Update(deltaTime);
    }
}

void Core::SceneManager::Render() {}

World *Core::SceneManager::GetWorld() const
{
    return mUserWorld;
}
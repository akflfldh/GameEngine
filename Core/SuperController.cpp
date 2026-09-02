#include "Core/SuperController.h"

Core::SuperController::SuperController() : mWorld(nullptr), mBackBufferIndex(0)
{
    mWorld = std::make_unique<World>();
}

Core::SuperController::~SuperController() {}

World *Core::SuperController::GetWorld() const
{
    return mWorld.get();
}

void Core::SuperController::InitWorld() {}

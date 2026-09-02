#include "RenderIDManager.h"

Core::RenderIDManager *Core::RenderIDManager::GetInstance()
{
    static RenderIDManager instance;
    return &instance;
}

Core::RenderIDManager::RenderIDManager() : mNextID(1) {}

Core::RenderIDManager::~RenderIDManager() {}

uint32_t Core::RenderIDManager::AllocID()
{
    return mNextID++;
}

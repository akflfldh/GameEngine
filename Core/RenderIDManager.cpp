#include "RenderIDManager.h"

#include <limits>
#include <stdexcept>

Core::RenderIDManager *Core::RenderIDManager::GetInstance()
{
    static RenderIDManager instance;
    return &instance;
}

Core::RenderIDManager::RenderIDManager() : mNextID(1) {}

Core::RenderIDManager::~RenderIDManager() {}

uint32_t Core::RenderIDManager::AllocID()
{
    // 여러 생성 경로에서도 중복 발급하지 않으며, 범위 소진 시 0이나 이전 ID로 순환하지 않는다.
    uint32_t nextID = mNextID.load(std::memory_order_relaxed);
    for (;;)
    {
        if (nextID == (std::numeric_limits<uint32_t>::max)())
            throw std::overflow_error("RenderID exhausted");

        if (mNextID.compare_exchange_weak(nextID, nextID + 1, std::memory_order_relaxed))
            return nextID;
    }
}

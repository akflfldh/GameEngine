#include "GpuBufferContextSystem.h"
#include "D3DGpuResourceManager/GpuBufferContextSystem.h"
#include "D3DGpuResourceManager/GpuBufferContextSystemImpl.h"
#include "GpuBufferContextSystem.h"
#include <assert.h>

#include <Logger/Logger.h>
GRM::GpuBufferContextSystem *GRM::GpuBufferContextSystem::GetInstance()
{
    static GpuBufferContextSystemImpl instance;
    return &instance;
}

GRM::GpuBufferContextSystem::~GpuBufferContextSystem() {}

GRM::GpuBufferContextSystem::GpuBufferContextSystem() {}

GRM::AllocateRange::AllocateRange() {}

uint32_t GRM::AllocateRange::UseRange(uint32_t usageCount) const
{

    if (mEnd < mNextOffset + usageCount)
    {
        // TODO 걸리는경우가있으니 항상 중단점 활성화
        std::string nextoffsetStr = std::to_string(mNextOffset);
        std::string usageCountStr = std::to_string(usageCount);
        std::string endStr = std::to_string(mEnd);

        LOG_MESSAGE_ERROR("Error", (endStr + "  , " + nextoffsetStr + "  , " + endStr).c_str());

        int a = 2;
        assert(0);
    }
    uint32_t currNextOffset = mNextOffset;
    mNextOffset += usageCount;
    return currNextOffset;
}

void GRM::AllocateRange::Reset()
{

    uint32_t chunkSize = mTotalSize / 3;
    mFrameIndex = (mFrameIndex + 1) % 3;
    mNextOffset = chunkSize * mFrameIndex;
    mEnd = mNextOffset + chunkSize;
}

void GRM::AllocateRange::NextFrame() {}

void GRM::AllocateRange::SetTotalSize(uint32_t size)
{

    mTotalSize = size;
    uint32_t chunkSize = mTotalSize / 3;
    mNextOffset = chunkSize * mFrameIndex;
    mEnd = mNextOffset + chunkSize;
}

uint32_t GRM::AllocateRange::GetCurrentFrameIndex(uint32_t relativeOffset) const
{
    uint32_t chunkSize = mTotalSize / 3;
    uint32_t startOffset = chunkSize * mFrameIndex;

    return startOffset + relativeOffset;
}

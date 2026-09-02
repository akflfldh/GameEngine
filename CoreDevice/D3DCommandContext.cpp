#include "D3DCommandContext.h"
#include <CoreDevice/D3DCoreDevice.h>

Core::D3DCommandContext::D3DCommandContext(D3D12_COMMAND_LIST_TYPE type, int frameCount)
    : mType(type), mFrameCount(frameCount), mLastFrameIndex(-1)
{
}

Core::D3DCommandContext::~D3DCommandContext() {}

void Core::D3DCommandContext::Initialize()
{

    D3DCoreDevice *coreDevice = static_cast<D3DCoreDevice *>(CoreDevice::GetInstance());

    for (int i = 0; i < mFrameCount; ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

        coreDevice->mDevice->CreateCommandAllocator(mType, IID_PPV_ARGS(allocator.GetAddressOf()));

        mCommandAllocatorVec.push_back(allocator);
    }

    coreDevice->mDevice->CreateCommandList(0, mType, mCommandAllocatorVec[0].Get(), nullptr,
                                           IID_PPV_ARGS(&mCommandList));

    mCommandList->Close();
}

void Core::D3DCommandContext::Reset(int frameIndex)
{

    if (mLastFrameIndex != frameIndex)
    {
        mCommandAllocatorVec[frameIndex]->Reset();
        mLastFrameIndex = frameIndex;
    }

    mCommandList->Reset(mCommandAllocatorVec[frameIndex].Get(), nullptr);
}

void Core::D3DCommandContext::Close()
{

    mCommandList->Close();
}

ID3D12GraphicsCommandList *Core::D3DCommandContext::GetCommandList()
{
    return mCommandList.Get();
}

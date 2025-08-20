#include "CoreDevice/D3DCoreDevice.h"


#include <d3d12.h>

Core::D3DCoreDevice::D3DCoreDevice()
{

    HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));

    result = mFactory->EnumAdapters(0, &mAdapter);

    result = D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice));

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.NodeMask = 0;
    commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    result = mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue));

    result = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
}

Core::D3DCoreDevice::~D3DCoreDevice() {}

void Core::D3DCoreDevice::FlushCommandQueue()
{

    mFenceValue++;

    mCommandQueue->Signal(mFence.Get(), mFenceValue);

    if (mFence->GetCompletedValue() < mFenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mFenceValue, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);

        CloseHandle(eventHandle);
    }
}
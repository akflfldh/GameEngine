#include "D3DCoreDevice.h"
#include "CoreDevice/D3DCoreDevice.h"
#include "D3DCoreDevice.h"

#include <d3d12.h>

Core::D3DCoreDevice::D3DCoreDevice()
{

#ifdef _DEBUG
    OutputDebugStringW(L"Before debugController declaration\n");

    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

    OutputDebugStringW(L"After debugController declaration\n");
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }

#endif

    HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&mFactory));

    result = mFactory->EnumAdapters(0, &mAdapter);

    result = D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice));

    // clear value 경고 메세지 무시
    ID3D12InfoQueue *infoQueue = nullptr;
    mDevice->QueryInterface(IID_PPV_ARGS(&infoQueue));
    infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

    D3D12_MESSAGE_ID hideIds[] = {D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE};

    D3D12_INFO_QUEUE_FILTER filter = {};
    filter.DenyList.NumIDs = _countof(hideIds);
    filter.DenyList.pIDList = hideIds;
    infoQueue->AddStorageFilterEntries(&filter);
    infoQueue->Release();

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

    mCommandQueue->Signal(mFence.Get(), mFenceValue);

    if (mFence->GetCompletedValue() < mFenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mFenceValue, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);

        CloseHandle(eventHandle);
    }

    mFenceValue++;
}

uint64_t Core::D3DCoreDevice::GetNextFenceValue()
{
    return mFenceValue;
}

void Core::D3DCoreDevice::IncreaseNextFenceValue()
{

    mFenceValue++;
}

void Core::D3DCoreDevice::WaitFenceValue(uint64_t value)
{

    if (mFence->GetCompletedValue() < value)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(value, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void Core::D3DCoreDevice::SetFence(uint64_t value)
{
    mCommandQueue->Signal(mFence.Get(), value);
}

void Core::D3DCoreDevice::FlushGpu()
{
    IncreaseNextFenceValue();
    uint64_t value = GetNextFenceValue();
    SetFence(value);
    WaitFenceValue(value);
    IncreaseNextFenceValue();
}

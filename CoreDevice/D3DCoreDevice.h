#pragma once

#include "CoreDevice/CoreDevice.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace Core
{

class CORE_DEVICE_API D3DCoreDevice : public CoreDevice
{
  public:
    D3DCoreDevice();
    virtual ~D3DCoreDevice();

    // virtual void Initialize()override;

    Microsoft::WRL::ComPtr<IDXGIFactory4> mFactory;
    Microsoft::WRL::ComPtr<IDXGIAdapter> mAdapter;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

    void FlushCommandQueue();

  private:
    UINT64 mFenceValue = 0;
};

} // namespace Core
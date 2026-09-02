#pragma once

#include "CoreDevice/CoreDevice.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdint.h>
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

    void FlushCommandQueue() override;
    virtual uint64_t GetNextFenceValue() override;
    virtual void IncreaseNextFenceValue() override;
    virtual void WaitFenceValue(uint64_t value) override;

    virtual void SetFence(uint64_t value) override;
    virtual void FlushGpu() override;

  private:
    UINT64 mFenceValue = 1;
};

} // namespace Core
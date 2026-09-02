#pragma once

#ifdef D3DX
#include "CoreDevice/CommandContext.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdint.h>
#include <vector>
#include <wrl.h>

namespace Core
{

class CORE_DEVICE_API D3DCommandContext : public CommandContext
{
  public:
    D3DCommandContext(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT, int frameCount = 3);
    ~D3DCommandContext();

    virtual void Initialize() override;

    // 다음프레임작업준비
    virtual void Reset(int frameIndex) override;

    // 명령  기록 종료
    virtual void Close() override;

    ID3D12GraphicsCommandList *GetCommandList();

  private:
    D3D12_COMMAND_LIST_TYPE mType;
    int mFrameCount;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCommandAllocatorVec;

    int mLastFrameIndex;
};
} // namespace Core

#endif
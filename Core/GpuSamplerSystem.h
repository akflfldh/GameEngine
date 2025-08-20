#pragma once

#include "Core/CoreDllExport.h"
#include <D3DGpuResourceManager/GRMPtr.h>
#include <D3DGpuResourceManager/GpuTypes.h>
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>

namespace Core
{

class CORE_API_LIB GpuSamplerSystem
{
  public:
    static GpuSamplerSystem *GetInstance();

    GpuSamplerSystem(GRM::IGpuResourceManager *gpuResourceManager);
    ~GpuSamplerSystem();
    // 복사 금지
    GpuSamplerSystem(const GpuSamplerSystem &) = delete;
    GpuSamplerSystem &operator=(const GpuSamplerSystem &) = delete;

    GpuSamplerSystem(GpuSamplerSystem &&) = default;
    GpuSamplerSystem &operator=(GpuSamplerSystem &&) = default;

    void LoadShaderSamplerFile(const std::string &filePath);

    // 이 메서드는 renderItem Builder들이 호출할것이다.
    GRM::GRMPtr GetGpuSampler(uint32_t id) const;

  private:
    bool CreateSampler(uint32_t bufferID);

    GRM::ESamplerAddressMode ConvertToSamplerAddressMode(const char *addressMode) const;
    GRM::ESamplerFilterMode ConvertToSamplerFilterMode(const char *filterMode) const;

  private:
    static GpuSamplerSystem *mInstance;
    GRM::IGpuResourceManager *mGpuResourceManager;
    std::unordered_map<uint32_t, GRM::GRMPtr> mGpuSamplerTable;
};

} // namespace Core
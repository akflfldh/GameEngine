#pragma once

#include "Core/GpuBufferContextSystem.h"
#include <memory>
#include <string>
#include <unordered_map>
namespace Core
{

class CORE_API_LIB GpuBufferContextSystemImpl : public GpuBufferContextSystem
{

  public:
    GpuBufferContextSystemImpl();
    virtual ~GpuBufferContextSystemImpl();
    // 복사 금지
    GpuBufferContextSystemImpl(const GpuBufferContextSystemImpl &) = delete;
    GpuBufferContextSystemImpl &operator=(const GpuBufferContextSystemImpl &) = delete;

    GpuBufferContextSystemImpl(GpuBufferContextSystemImpl &&) = default;
    GpuBufferContextSystemImpl &operator=(GpuBufferContextSystemImpl &&) = default;

    virtual void LoadShaderBufferFile(const std::string &filePath) override;

    // 이메서드는 에디터와, 유저 DLL의 버퍼콜백등록 전역함수가 호출한다.
    virtual bool RegisterGpuBufferCallback(uint32_t id,
                                           pDataMemory (*createBufferData)(void *object, void *frameContext)) override;

    //	virtual bool CreateGpuBufferContext(uint32_t id, const GRM::BufferDesc& bufferDesc) override;

    // 이 메서드는 renderItem Builder들이 호출할것이다.
    virtual GpuBufferContext *GetGpuBufferContext(uint32_t id) const override;

  private:
    bool CreateGpuBuffer(uint32_t bufferID, GRM::EBufferUsage mBufferUsage, uint32_t uploadType, uint32_t size);

    uint32_t CacluateConstantBufferSize(uint32_t size) const;

  private:
    GRM::IGpuResourceManager *mGpuResourceManager;
    std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>> mGpuBufferContextTable;
};

} // namespace Core
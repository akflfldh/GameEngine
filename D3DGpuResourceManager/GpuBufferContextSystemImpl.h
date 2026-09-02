#pragma once

#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace JsonParser
{
class Parser;
}

namespace GRM
{

class GPURESOURCE_MANAGER_API GpuBufferContextSystemImpl : public GpuBufferContextSystem
{

  public:
    GpuBufferContextSystemImpl();
    virtual ~GpuBufferContextSystemImpl();
    // 복사 금지
    GpuBufferContextSystemImpl(const GpuBufferContextSystemImpl &) = delete;
    GpuBufferContextSystemImpl &operator=(const GpuBufferContextSystemImpl &) = delete;

    GpuBufferContextSystemImpl(GpuBufferContextSystemImpl &&) = default;
    GpuBufferContextSystemImpl &operator=(GpuBufferContextSystemImpl &&) = default;

    virtual void PreUpdate() override;

    virtual void LoadShaderBufferFile(const std::filesystem::path &filePath) override;

    // 이메서드는 에디터와, 유저 DLL의 버퍼콜백등록 전역함수가 호출한다.
    virtual bool RegisterGpuBufferCallback(uint32_t id);

    //	virtual bool CreateGpuBufferContext(uint32_t id, const GRM::BufferDesc& bufferDesc) override;

    // 이 메서드는 renderItem Builder들이 호출할것이다.
    virtual GpuBufferContext *GetGpuBufferContext(uint32_t id) const override;

    bool CreateBuffer(uint32_t bufferID, GRM::EBufferUsage mBufferUsage, uint32_t uploadType, uint32_t size);
    bool CreateStructuredBuffer(uint32_t bufferID, uint32_t size, bool isBuffersPerFrame);

  private:
    uint32_t CacluateConstantBufferSize(uint32_t size) const;

    bool LoadConstantBuffer(JsonParser::Parser &parser);

  private:
    GRM::IGpuResourceManager *mGpuResourceManager;
    std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>> mGpuBufferContextTable;
    std::vector<GpuConstantBufferContext *> mConstantBufferCacheList;
    std::vector<GpuStructuredBufferContext *> mStructuredBufferCacheList;
};

} // namespace GRM
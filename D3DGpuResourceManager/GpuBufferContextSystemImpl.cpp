#include "D3DGpuResourceManager/GpuBufferContextSystemImpl.h"
#include <assert.h>

#include "D3DGpuResourceManager/GpuSamplerSystem.h"
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <JsonParserWrapping/JsonParser.h>
#include <Logger/Logger.h>
#include <sstream>

GRM::GpuBufferContextSystemImpl::GpuBufferContextSystemImpl()
{
    mGpuResourceManager = GRM::IGpuResourceManager::GetInstance();
}

GRM::GpuBufferContextSystemImpl::~GpuBufferContextSystemImpl() {}

void GRM::GpuBufferContextSystemImpl::PreUpdate()
{

    for (auto gpuBufferContext : mConstantBufferCacheList)
    {

        gpuBufferContext->mAllocateRange.Reset();
    }

    for (auto gpuBufferContext : mStructuredBufferCacheList)
    {
        gpuBufferContext->mCurrFrameIndex = (gpuBufferContext->mCurrFrameIndex + 1) % 3;
    }
}

void GRM::GpuBufferContextSystemImpl::LoadShaderBufferFile(const std::filesystem::path &filePath)
{

    JsonParser::Parser jsonParser;
    bool ret = true;
    ret = jsonParser.ReadFile(filePath.string());
    if (!ret)
    {
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "JsonParser ReadFile 실패");
        assert(0);
    }

    do
    {
        std::string bufferTypeKey = jsonParser.GetKeyFromReadPointer();

        if (bufferTypeKey == "ConstantBuffer")
        {
            // 상수버퍼
            bool ret = LoadConstantBuffer(jsonParser);
        }
        else
        {
            LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "지원하지않는 버퍼타입입니다.");
            assert(0);
            return;
        }

    } while (jsonParser.NextReadPointer());
}

bool GRM::GpuBufferContextSystemImpl::RegisterGpuBufferCallback(uint32_t id)
{
    // GpuBuffer를 생성요청
    // 타입에 맞게 생성해야한다.
    // 또한 공간이 부족하면 추가로 다시 사이즈를 확장할수있게 요청하는 메서드도 제공해야한다.
    std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>>::iterator it = mGpuBufferContextTable.find(id);

    if (it == mGpuBufferContextTable.end())
    {
        std::stringstream errorMessage;
        errorMessage << id << "의 ID를 가진 버퍼가 존재하지않습니다.";
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", errorMessage.str().c_str());
        assert(0);
        return false;
    }

    return true;
}

GRM::GpuBufferContext *GRM::GpuBufferContextSystemImpl::GetGpuBufferContext(uint32_t id) const
{

    std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>>::const_iterator it =
        mGpuBufferContextTable.find(id);

    if (it == mGpuBufferContextTable.end())
    {
        std::stringstream errorMessage;
        errorMessage << id << "의 ID를 가진 버퍼가 존재하지않습니다. 먼저 CreateGpuBuffer를 호출해야합니다.";
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", errorMessage.str().c_str());
        assert(0);
        return nullptr;
    }

    return it->second.get();

    return nullptr;
}

bool GRM::GpuBufferContextSystemImpl::CreateBuffer(uint32_t bufferID, GRM::EBufferUsage bufferUsage,
                                                   uint32_t uploadType, uint32_t size)
{

    std::unordered_map<uint32_t, std::unique_ptr<GpuBufferContext>>::iterator it =
        mGpuBufferContextTable.find(bufferID);

    if (it != mGpuBufferContextTable.end())
    {
        std::stringstream errorMessage;
        errorMessage << bufferID << "의 ID를 가진 버퍼가 이미 존재합니다.";
        LOG_MESSAGE_INFO("GpuBufferContextSystemImpl", errorMessage.str().c_str());
        return false;
    }

    GRM::BufferDesc bufferDesc;
    bufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
    bufferDesc.mBufferUsage = bufferUsage;
    bufferDesc.mData = nullptr;

    if (uploadType == 0)
    {
        // PASS 10
        bufferDesc.mElementDataNum = 256;
    }
    else
    {
        // Object
        bufferDesc.mElementDataNum = 1000;
    }

    if (bufferUsage == GRM::EBufferUsage::eConstantBuffer)
    {
        bufferDesc.mElementDataSize = CacluateConstantBufferSize(size);
        bufferDesc.mBufferSize = bufferDesc.mElementDataNum * CacluateConstantBufferSize(bufferDesc.mElementDataSize);
    }

    GRM::GRMPtr pBuffer = mGpuResourceManager->CreateBuffer(bufferDesc);
    if (pBuffer.getResource() == nullptr)
    {
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "Fun : CreaetGpuBuffer , GpuBuffer리소스 생성실패");
        assert(0);
    }
    std::unique_ptr<GpuConstantBufferContext> gpuBufferContext = std::make_unique<GpuConstantBufferContext>();
    gpuBufferContext->mAllocateRange.SetTotalSize(bufferDesc.mElementDataNum);
    gpuBufferContext->mGpuBuffer = pBuffer;
    gpuBufferContext->mID = bufferID;
    gpuBufferContext->mBufferDesc = bufferDesc;
    // element max개수

    mConstantBufferCacheList.push_back(gpuBufferContext.get());
    mGpuBufferContextTable[bufferID] = std::move(gpuBufferContext);

    return true;
}

bool GRM::GpuBufferContextSystemImpl::CreateStructuredBuffer(uint32_t bufferID, uint32_t size, bool isBuffersPerFrame)
{

    if (mGpuBufferContextTable.find(bufferID) != mGpuBufferContextTable.end())
    {
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl",
                             "Fun : CreaetGpuBuffer , GpuBuffer리소스 생성실패, 이미존재하는 구조적버퍼");
        assert(0);
        return false;
    }
    GRM::BufferDesc bufferDesc;
    bufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
    bufferDesc.mBufferUsage = EBufferUsage::eStructuredBuffer;
    bufferDesc.mData = nullptr;
    bufferDesc.mElementDataNum = 256;
    bufferDesc.mElementDataSize = size;
    //~(15) & (size + 15);
    bufferDesc.mBufferSize = bufferDesc.mElementDataNum * bufferDesc.mElementDataSize;

    int bufferNum = 1;

    if (isBuffersPerFrame)
    {
        bufferNum = 3;
    }
    std::unique_ptr<GpuStructuredBufferContext> gpuBufferContext = std::make_unique<GpuStructuredBufferContext>();

    for (int i = 0; i < bufferNum; ++i)
    {
        GRM::GRMPtr pBuffer = mGpuResourceManager->CreateBuffer(bufferDesc);
        if (pBuffer.getResource() == nullptr)
        {
            LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "Fun : CreaetGpuBuffer , GpuBuffer리소스 생성실패");
            assert(0);
        }

        gpuBufferContext->mGpuBuffersPerFrame.push_back(pBuffer);
    }

    gpuBufferContext->mID = bufferID;
    gpuBufferContext->mBufferDesc = bufferDesc;
    gpuBufferContext->mIsFrameBuffers = isBuffersPerFrame;
    mStructuredBufferCacheList.push_back(gpuBufferContext.get());
    mGpuBufferContextTable[bufferID] = std::move(gpuBufferContext);

    return false;
}

uint32_t GRM::GpuBufferContextSystemImpl::CacluateConstantBufferSize(uint32_t size) const
{

#ifdef D3DX

    return (size + 255) & ~255;

#else
    // DirectX 상수 버퍼는 256바이트 정렬을 권장함.
    // D3DX 매크로가 없을 때도 동일한 정렬 규칙을 적용.
    const uint32_t alignment = 256u;
    return (size + (alignment - 1)) & ~(alignment - 1);
#endif
}

bool GRM::GpuBufferContextSystemImpl::LoadConstantBuffer(JsonParser::Parser &jsonParser)
{
    bool ret = true;

    jsonParser.IntoReadPointer();

    // 상수버퍼들을 읽는다.
    do
    {

        // 한 버퍼 정보들을 읽는다.
        jsonParser.IntoReadPointer();

        GRM::BufferDesc bufferDesc;
        uint32_t bufferID = 0;
        uint32_t uploadType = 0;
        uint32_t size = 0;

        do
        {
            const std::string &bufferConfigKey = jsonParser.GetKeyFromReadPointer();
            if (bufferConfigKey == "ID")
            {

                ret = jsonParser.GetValueFromReadPointer<uint32_t>(bufferID);
                if (!ret)
                {
                    LOG_MESSAGE_ERROR("GpuBufferContextSystem", ".shader.buffer 읽기실패 (bufferID)");
                    assert(0);
                }
            }
            else if (bufferConfigKey == "UploadType")
            {

                ret = jsonParser.GetValueFromReadPointer<uint32_t>(uploadType);
                if (!ret)
                {
                    LOG_MESSAGE_CRITICAL("GpuBufferContextSystem", ".shader.buffer 읽기실패 (UploadType)");
                    assert(0);
                }
            }
            else if (bufferConfigKey == "Size")
            {

                ret = jsonParser.GetValueFromReadPointer<uint32_t>(size);
                if (!ret)
                {
                    LOG_MESSAGE_CRITICAL("GpuBufferContextSystem", ".shader.buffer 읽기 실패(size)");
                    assert(0);
                }
            }

        } while (jsonParser.NextReadPointer());

        ret = CreateBuffer(bufferID, GRM::EBufferUsage::eConstantBuffer, uploadType, size);

        // 읽은 버퍼에서 빠져나온다.
        jsonParser.OutReadPointer();

    } while (jsonParser.NextReadPointer());

    jsonParser.OutReadPointer();

    return true;
}

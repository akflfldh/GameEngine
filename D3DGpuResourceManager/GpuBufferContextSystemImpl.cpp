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

void GRM::GpuBufferContextSystemImpl::LoadShaderBufferFile(const std::string &filePath)
{

    JsonParser::Parser jsonParser;
    bool ret = true;
    ret = jsonParser.ReadFile(filePath);
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

                ret = CreateGpuBuffer(bufferID, GRM::EBufferUsage::eConstantBuffer, uploadType, size);

                // 읽은 버퍼에서 빠져나온다.
                jsonParser.OutReadPointer();

            } while (jsonParser.NextReadPointer());

            jsonParser.OutReadPointer();
        }
        else if (bufferTypeKey == "StructuredBuffer")
        {
            // 구조화된 버퍼
        }
        else
        {
            LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "지원하지않는 버퍼타입입니다.");
            assert(0);
            return;
        }

    } while (jsonParser.NextReadPointer());
}

bool GRM::GpuBufferContextSystemImpl::RegisterGpuBufferCallback(uint32_t id,
                                                                pDataMemory (*createBufferData)(void *object,
                                                                                                void *frameContext))
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

    it->second->createBufferData = createBufferData;

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

bool GRM::GpuBufferContextSystemImpl::CreateGpuBuffer(uint32_t bufferID, GRM::EBufferUsage bufferUsage,
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
        bufferDesc.mElementDataNum = 10;
    }
    else
    {
        bufferDesc.mElementDataNum = 100;
    }

    if (bufferUsage == GRM::EBufferUsage::eConstantBuffer)
    {
        bufferDesc.mElementDataSize = CacluateConstantBufferSize(size);
        bufferDesc.mBufferSize = bufferDesc.mElementDataNum * CacluateConstantBufferSize(bufferDesc.mElementDataSize);
    }
    else if (bufferUsage == GRM::EBufferUsage::eStructuredBuffer)
    {
        bufferDesc.mElementDataSize = size;
        bufferDesc.mBufferSize = bufferDesc.mElementDataNum * bufferDesc.mElementDataSize;
    }

    GRM::GRMPtr pBuffer = mGpuResourceManager->CreateBuffer(bufferDesc);
    if (pBuffer.getResource() == nullptr)
    {
        LOG_MESSAGE_CRITICAL("GpuBufferContextSystemImpl", "Fun : CreaetGpuBuffer , GpuBuffer리소스 생성실패");
        assert(0);
    }

    std::unique_ptr<GpuBufferContext> gpuBufferContext = std::make_unique<GpuBufferContext>();

    gpuBufferContext->mGpuBuffer = pBuffer;
    gpuBufferContext->mID = bufferID;
    gpuBufferContext->mBufferDesc = bufferDesc;
    mGpuBufferContextTable[bufferID] = std::move(gpuBufferContext);

    return true;
}

uint32_t GRM::GpuBufferContextSystemImpl::CacluateConstantBufferSize(uint32_t size) const
{

#ifdef D3DX

    return (size + 255) & ~255;

#endif

    return 0;
}

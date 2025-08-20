#include "Core/GpuSamplerSystem.h"
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <JsonParserWrapping/JsonParser.h>
#include <Logger/Logger.h>
#include <assert.h>

namespace Core
{

static std::unordered_map<std::string, GRM::ESamplerFilterMode> gFilterModeTable;
static std::unordered_map<std::string, GRM::ESamplerAddressMode> gAddressModeTable;

} // namespace Core

Core::GpuSamplerSystem *Core::GpuSamplerSystem::mInstance = nullptr;

Core::GpuSamplerSystem *Core::GpuSamplerSystem::GetInstance()
{
    if (mInstance == nullptr)
    {
        LOG_MESSAGE_CRITICAL("GpuSamplerSystem", "샘플러시스템을 먼저 생성해야합니다.");
        assert(0);
    }

    return mInstance;
}

Core::GpuSamplerSystem::GpuSamplerSystem(GRM::IGpuResourceManager *gpuResourceManager)
    : mGpuResourceManager(gpuResourceManager)
{
    if (mInstance != nullptr)
    {
        LOG_MESSAGE_CRITICAL("GpuSamplerSystem", "샘플러시스템은 한번만 생성해야합니다.");
        assert(0);
    }

    gFilterModeTable["MIN_MAG_MIP_LINEAR"] = GRM::ESamplerFilterMode::eMIN_MAG_MIP_LINEAR;
    gFilterModeTable["MIN_MAG_MIP_POINT"] = GRM::ESamplerFilterMode::eMIN_MAG_MIP_POINT;

    gAddressModeTable["WRAP"] = GRM::ESamplerAddressMode::eWrap;
    gAddressModeTable["CLAMP"] = GRM::ESamplerAddressMode::eClamp;

    mInstance = this;
}

Core::GpuSamplerSystem::~GpuSamplerSystem() {}

void Core::GpuSamplerSystem::LoadShaderSamplerFile(const std::string &filePath)
{

    JsonParser::Parser jsonParser;
    bool ret = true;
    ret = jsonParser.ReadFile(filePath);
    if (!ret)
    {
        LOG_MESSAGE_CRITICAL("GpuSamplerSystem", "JsonParser ReadFile 실패");
        assert(0);
    }

    std::string configKey;
    std::string configValue;
    do
    {

        jsonParser.IntoReadPointer();
        uint32_t samplerID = 0;
        GRM::SamplerDesc samplerDesc;
        bool ret = false;
        do
        {

            configKey = jsonParser.GetKeyFromReadPointer();
            if (configKey == "Name")
            {
            }
            else if (configKey == "ID")
            {

                ret = jsonParser.GetValueFromReadPointer<uint32_t>(samplerID);
                if (ret == false)
                {
                    assert(0);
                }
            }
            else if (configKey == "Filter")
            {

                ret = jsonParser.GetValueFromReadPointer<std::string>(configValue);
                if (ret == false)
                {
                    assert(0);
                }
                samplerDesc.mFilterMode = ConvertToSamplerFilterMode(configValue.c_str());
            }
            else if (configKey == "AddressU")
            {

                ret = jsonParser.GetValueFromReadPointer<std::string>(configValue);
                if (ret == false)
                {
                    assert(0);
                }

                samplerDesc.mAddressU = ConvertToSamplerAddressMode(configValue.c_str());
            }
            else if (configKey == "AddressV")
            {

                ret = jsonParser.GetValueFromReadPointer<std::string>(configValue);
                if (ret == false)
                {
                    assert(0);
                }
                samplerDesc.mAddressV = ConvertToSamplerAddressMode(configValue.c_str());
            }
            else if (configKey == "AddressW")
            {
                ret = jsonParser.GetValueFromReadPointer<std::string>(configValue);
                if (ret == false)
                {
                    assert(0);
                }

                samplerDesc.mAddressW = ConvertToSamplerAddressMode(configValue.c_str());
            }

        } while (jsonParser.NextReadPointer());

        GRM::GRMPtr sampler = mGpuResourceManager->CreateSampler(samplerDesc);
        if (sampler.getResource() == nullptr)
        {
            LOG_MESSAGE_CRITICAL("Sampler", "샘플러 생성 실패");
            assert(0);
        }
        mGpuSamplerTable[samplerID] = sampler;

        jsonParser.OutReadPointer();

    } while (jsonParser.NextReadPointer());
}

GRM::GRMPtr Core::GpuSamplerSystem::GetGpuSampler(uint32_t id) const
{

    std::unordered_map<uint32_t, GRM::GRMPtr>::const_iterator it = mGpuSamplerTable.find(id);
    if (it == mGpuSamplerTable.cend())
        return nullptr;

    return it->second;
}

bool Core::GpuSamplerSystem::CreateSampler(uint32_t samplerID)
{

    return false;
}

GRM::ESamplerAddressMode Core::GpuSamplerSystem::ConvertToSamplerAddressMode(const char *addressMode) const
{

    std::unordered_map<std::string, GRM::ESamplerAddressMode>::const_iterator it = gAddressModeTable.find(addressMode);

    if (it == gAddressModeTable.cend())
    {
        assert(0);
    }

    return it->second;
}

GRM::ESamplerFilterMode Core::GpuSamplerSystem::ConvertToSamplerFilterMode(const char *filterMode) const
{
    std::unordered_map<std::string, GRM::ESamplerFilterMode>::const_iterator it = gFilterModeTable.find(filterMode);

    if (it == gFilterModeTable.cend())
    {
        assert(0);
    }

    return it->second;
}

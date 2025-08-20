#include "pch.h"

#include "RenderSystem/D3DShaderReflectSystem.h"
#include <Logger/Logger.h>
#include <assert.h>
#include <d3d12.h>
#include <d3dcompiler.h>

bool D3DRender::D3DShaderReflectSystem::Reflect(void *compiledShader, size_t size,
                                                std::vector<ShaderResourceReflectInfo> &oReflectShaderResourceVector)
{

    ID3D12ShaderReflection *shaderReflection;
    HRESULT result = D3DReflect(compiledShader, size, IID_PPV_ARGS(&shaderReflection));

    if (FAILED(result))
    {
        LOG_MESSAGE_WARNING("ReflectSystem", "세이더 리플렉션 실패");
        return false;
    }

    // 세이더에 정의된 리소스의 개수 등의 정보를 얻을수있다.
    D3D12_SHADER_DESC shaderDesc;
    result = shaderReflection->GetDesc(&shaderDesc);

    if (FAILED(result))
    {
        LOG_MESSAGE_WARNING("ReflectSystem", "세이더 리플렉션 실패");
        return false;
    }

    UINT shaderVersion = shaderDesc.Version;
    D3D12_SHADER_VERSION_TYPE shaderVersionType = (D3D12_SHADER_VERSION_TYPE)((shaderVersion & 0xFFFF0000) >> 16);

    UINT constantBufferNum = shaderDesc.ConstantBuffers; // 바인딩된 상수버퍼 수
    UINT boundedResourceNum = shaderDesc.BoundResources; // 바인딩된 리소스 수(텍스처+버퍼)

    for (size_t i = 0; i < boundedResourceNum; ++i)
    {

        D3D12_SHADER_INPUT_BIND_DESC inputResourceBindDesc;
        result = shaderReflection->GetResourceBindingDesc(i, &inputResourceBindDesc);

        if (FAILED(result))
        {
            LOG_MESSAGE_WARNING("ReflectSystem", "세이더 리플렉션 실패");
            return false;
        }

        ShaderResourceReflectInfo shaderResourceReflectionInfo;
        shaderResourceReflectionInfo.mName = inputResourceBindDesc.Name;
        shaderResourceReflectionInfo.mResourceDimension =
            ConvertToRenderShaderResourceDimension(inputResourceBindDesc.Dimension);
        shaderResourceReflectionInfo.mResourceType = ConvertToRenderShaderResourceType(inputResourceBindDesc.Type);

        if (shaderResourceReflectionInfo.mResourceType == Render::EShaderResourceType::eConstantBuffer)
        {
            ID3D12ShaderReflectionConstantBuffer *constantBuffeReflect =
                shaderReflection->GetConstantBufferByName(shaderResourceReflectionInfo.mName.c_str());

            D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
            constantBuffeReflect->GetDesc(&shaderBufferDesc);

            shaderResourceReflectionInfo.mSize = shaderBufferDesc.Size;
        }

        shaderResourceReflectionInfo.mRegisterIndex =
            inputResourceBindDesc.BindPoint + inputResourceBindDesc.BindCount - 1;
        shaderResourceReflectionInfo.mRegisterSpace = inputResourceBindDesc.Space;

        oReflectShaderResourceVector.push_back(shaderResourceReflectionInfo);
    }

    // 리소스들의 바인딩정보를 얻는다.(이름,타입, 레지스터 등등 )
    // shaderReflection->GetResourceBindingDesc()
    // shaderReflection->GetResourceBindingDescByName //리소스이름으로 가져올수도있다.
    // 리플렉션과동시에 검증도 가능할수도

    // shaderReflection->GetVariableByName()    //실제 엔진에서 정해놓은대로 올바르게 변수들을 정의했는지도
    // 검증할수있을것

    return true;
}

Render::EShaderResourceDimension D3DRender::D3DShaderReflectSystem::ConvertToRenderShaderResourceDimension(
    D3D_SRV_DIMENSION shaderDimension)
{
    switch (shaderDimension)
    {
    case D3D_SRV_DIMENSION_BUFFER:
    case D3D_SRV_DIMENSION_UNKNOWN:
        return Render::EShaderResourceDimension::eBuffer;

    case D3D_SRV_DIMENSION_TEXTURE2D:
        return Render::EShaderResourceDimension::eTex2D;

    default:
    {
        LOG_MESSAGE_ERROR("ReflectionSystem", "ShaderResourceDimension 변환 실패(정의되지않은 타입)");
        assert(0);
    }
    }
}

Render::EShaderResourceType D3DRender::D3DShaderReflectSystem::ConvertToRenderShaderResourceType(
    D3D_SHADER_INPUT_TYPE shaderInputType)
{

    switch (shaderInputType)
    {
    case D3D_SIT_CBUFFER:

        return Render::EShaderResourceType::eConstantBuffer;

    case D3D_SIT_STRUCTURED:

        return Render::EShaderResourceType::eStructuredBuffer;

    case D3D_SIT_TEXTURE:
        return Render::EShaderResourceType::eTexture;

    case D3D_SIT_SAMPLER:

        return Render::EShaderResourceType::eSampler;

    default:
    {
        LOG_MESSAGE_ERROR("ReflectionSystem", "ShaderResourceType 변환 실패(정의되지않은 타입)");
        assert(0);
    }
    }
}

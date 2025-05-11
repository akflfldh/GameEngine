#include "Effect/RenderPassTwo.h"
#include"Shader/ShaderResource.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"Shader/ShaderResourceConstantBuffer.h"
#include"Shader/ShaderResourceTexture.h"
#include"Shader/ShaderResourceSampler.h"


Quad::RenderPassTwo::~RenderPassTwo()
{

    for(int i=0; i<mShaderResourceVector.size(); ++i)
    { 
        delete mShaderResourceVector[i];
    }
   


}


Quad::RenderPassTwo::RenderPassTwo(const RenderPassTwo& renderPass)
    :mStencilWriteFlag(renderPass.mStencilWriteFlag),mPipelineState(renderPass.mPipelineState),
    mRootSignature(renderPass.mRootSignature),mPrimitiveTopologyType(renderPass.mPrimitiveTopologyType),
    mRenderPassID(renderPass.mRenderPassID)
{
  
    const std::vector<ShaderResource*>  & shaderResourceVector = renderPass.GetShaderResourceVector();
    mShaderResourceVector.resize(shaderResourceVector.size());
    for (int i = 0; i < shaderResourceVector.size(); ++i)
    {
        ShaderResource* shaderResource = nullptr;
        switch (shaderResourceVector[i]->GetShaderResourceType())
        {

        case EShaderResourceType::eConstantBuffer:

            shaderResource = new ShaderResourceConstantBuffer(*(ShaderResourceConstantBuffer*)shaderResourceVector[i]);
            break;

        case EShaderResourceType::eTexture:
            shaderResource = new ShaderResourceTexture(*(ShaderResourceTexture*)shaderResourceVector[i]);

            break;

        case EShaderResourceType::eSampler:
            shaderResource = new ShaderResourceSampler(*(ShaderResourceSampler*)shaderResourceVector[i]);
            break;
        }
        mShaderResourceVector[i] = shaderResource;
    }




}
Microsoft::WRL::ComPtr<ID3D12PipelineState> Quad::RenderPassTwo::GetPipelineState() const
{
    return mPipelineState;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Quad::RenderPassTwo::GetRootSignature() const
{
    return mRootSignature;
}

void Quad::RenderPassTwo::SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState)
{
    mPipelineState = pipelineState;
}

void Quad::RenderPassTwo::SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
{
    mRootSignature = rootSignature;
}

void Quad::RenderPassTwo::AddShaderResourece(ShaderResource* shaderResource)
{


    mShaderResourceVector.push_back(shaderResource);
}

Quad::ShaderResource* Quad::RenderPassTwo::GetShaderResource(const std::wstring& name) const
{
    for (int i = 0; i < mShaderResourceVector.size(); ++i)
    {
        if (mShaderResourceVector[i]->GetName() == name)
            return mShaderResourceVector[i];
    }

    return nullptr;
}

const std::vector<Quad::ShaderResource*>& Quad::RenderPassTwo::GetShaderResourceVector() const
{
    return mShaderResourceVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Quad::RenderPassTwo::SetPrimitiveToplogyType(D3D_PRIMITIVE_TOPOLOGY type)
{
    mPrimitiveTopologyType = type;
}

D3D_PRIMITIVE_TOPOLOGY Quad::RenderPassTwo::GetPrimitiveTopologyType() const
{
    return mPrimitiveTopologyType;
}

Quad::ERenderPassID Quad::RenderPassTwo::GetRenderPassID() const
{
    return mRenderPassID;
}

void Quad::RenderPassTwo::SetRenderPassID(ERenderPassID id)
{
    mRenderPassID = id;
}

void Quad::RenderPassTwo::SetStencilWriteFlag(bool flag)
{
    mStencilWriteFlag = flag;
}


bool Quad::RenderPassTwo::GetStencilWriteFlag() const
{
    return mStencilWriteFlag;
}


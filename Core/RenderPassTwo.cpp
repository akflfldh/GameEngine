#include "Effect/RenderPassTwo.h"
#include"Shader/ShaderResource.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"Shader/ShaderResourceConstantBuffer.h"
#include"Shader/ShaderResourceTexture.h"
#include"Shader/ShaderResourceSampler.h"


Quad::RenderPassTwo::RenderPassTwo()
    : mDestEffect(nullptr),mPipelineState(nullptr),mRootSignature(nullptr),mPrimitiveTopologyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),mRenderPassID(ERenderPassID::eOpaque),
    mCustomRenderTargetTexture(nullptr), mCustomRenderTargetLocalID(-1), mRenderPassDrawType(ERenderPassDrawType::eObjectDraw)
{



}

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
void Quad::RenderPassTwo::SetDestEffect(Effect* effect)
{
    mDestEffect = effect;
}
Quad::Effect* Quad::RenderPassTwo::GetDestEffect() const
{
    return mDestEffect;
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

void Quad::RenderPassTwo::SetCustomRenderTarget(RenderTargetTexture* renderTargetTexture)
{

    mCustomRenderTargetTexture = renderTargetTexture;
}

Quad::RenderTargetTexture* Quad::RenderPassTwo::GetCustomRenderTarget() const
{
    return mCustomRenderTargetTexture;
}

void Quad::RenderPassTwo::SetCustomRenderTargetLocalID(int id)
{
    mCustomRenderTargetLocalID = id;
}

int Quad::RenderPassTwo::GetCustomRenderTargetLocalID() const
{
    return mCustomRenderTargetLocalID;
}

void Quad::RenderPassTwo::AddPreRenderPassCommand(RenderPassCommand* renderPassCommmand)
{
    mPreRenderPassCommandVector.push_back(renderPassCommmand);
}

void Quad::RenderPassTwo::AddPostRenderPassCommand( RenderPassCommand* renderPassCommand)
{
    mPostRenderPassCommandVector.push_back(renderPassCommand);
}

void Quad::RenderPassTwo::SetPreRenderPassCommandVector(const std::vector<RenderPassCommand*>& renderPassCommandVector)
{
    mPreRenderPassCommandVector = renderPassCommandVector;

}

void Quad::RenderPassTwo::SetPreRenderPassCommandVector(std::vector<RenderPassCommand*>&& renderPassCommandVector)
{
    mPreRenderPassCommandVector = std::move(renderPassCommandVector);
}

void Quad::RenderPassTwo::SetPostRenderPassCommandVector(const std::vector<RenderPassCommand*>& renderPassCommandVector)
{
    mPostRenderPassCommandVector = renderPassCommandVector;
}

void Quad::RenderPassTwo::SetPostRenderPassCommandVector(std::vector<RenderPassCommand*>&& renderPassCommandVector)
{
    mPostRenderPassCommandVector = std::move(renderPassCommandVector);
}

const std::vector<Quad::RenderPassCommand*>& Quad::RenderPassTwo::GetPreRenderPassCommandVector() const
{
    return mPreRenderPassCommandVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<Quad::RenderPassCommand*>& Quad::RenderPassTwo::GetPostRenderPassCommandVector() const
{
    return mPostRenderPassCommandVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Quad::RenderPassTwo::SetRenderPassDrawType(ERenderPassDrawType renderPassDrawType)
{

    mRenderPassDrawType = renderPassDrawType;
}

Quad::ERenderPassDrawType Quad::RenderPassTwo::GetRenderPassDrawType() const
{
    return mRenderPassDrawType;
}




#include "RenderPass.h"
#include"header.h"

Microsoft::WRL::ComPtr<ID3D12PipelineState> Quad::RenderPass::GetPipelineState() const
{
    return mPipelineState;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Quad::RenderPass::GetRootSignature() const
{
    return mRootSignature;
}


const std::vector<std::pair<Quad::ShaderResource*, int>>& Quad::RenderPass::GetShaderResourcePerPassVector() const
{
    return mShaderResourcePerPassVector;
    // TODO: ���⿡ return ���� �����մϴ�.
}

const std::vector<std::pair<Quad::ShaderResource*, int>>& Quad::RenderPass::GetShaderResourcePerObjectVector() const
{

    return mShaderResourcePerObjectVector;
    // TODO: ���⿡ return ���� �����մϴ�.
}

void Quad::RenderPass::SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState)
{
    mPipelineState = pipelineState;
}

void Quad::RenderPass::SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
{
    mRootSignature = rootSignature;

}

//const std::vector<std::pair<std::wstring, int>>& Quad::RenderPass::GetShaderResourcePerPassVector() const
//{
//    return mShaderResourcePerPassVector;
//    // TODO: ���⿡ return ���� �����մϴ�.
//}
//
//const std::vector<std::pair<std::wstring, int>>& Quad::RenderPass::GetShaderResourcePerObjectVector() const
//{
//
//    return mShaderResourcePerObjectVector;
//    // TODO: ���⿡ return ���� �����մϴ�.
//}

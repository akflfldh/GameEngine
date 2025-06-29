#include "GraphicPipelineStateGeneratorHelper.h"

#include"Utility/DoException.h"

Quad::GraphicPipelineStateGeneratorHelper::GraphicPipelineStateGeneratorHelper()
{
}

Quad::GraphicPipelineStateGeneratorHelper::~GraphicPipelineStateGeneratorHelper()
{
}

void Quad::GraphicPipelineStateGeneratorHelper::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	mDevice = device;
}

void Quad::GraphicPipelineStateGeneratorHelper::StartGenerateGraphicPipelineState(ID3D12RootSignature* rootSignature)
{
	memset(&mGraphicsPipelineStateDesc, 0, sizeof(mGraphicsPipelineStateDesc));

	mGraphicsPipelineStateDesc.pRootSignature = rootSignature;
	mGraphicsPipelineStateDesc.NumRenderTargets = 1;
	mGraphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	
	mGraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT{});
	mGraphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	mGraphicsPipelineStateDesc.SampleDesc.Count = 1;
	mGraphicsPipelineStateDesc.SampleDesc.Quality = 0;
	mGraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mGraphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	mGraphicsPipelineStateDesc.SampleMask = 0xFFFFFFFF;
	mGraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	mGraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

void Quad::GraphicPipelineStateGeneratorHelper::SetShaderCode(const void* vsBytecode, SIZE_T vsBytecodeLength, const void* psBytecode, SIZE_T psBytecodeLength, const void* gsBytecode, SIZE_T gsBytecodeLength, const void* dsBytecode, SIZE_T dsBytecodeLength, const void* hsBytecode, SIZE_T hsBytecodeLength)
{


	mGraphicsPipelineStateDesc.VS.pShaderBytecode = vsBytecode;
	mGraphicsPipelineStateDesc.VS.BytecodeLength = vsBytecodeLength;


	mGraphicsPipelineStateDesc.PS.pShaderBytecode = psBytecode;
	mGraphicsPipelineStateDesc.PS.BytecodeLength = psBytecodeLength;

	mGraphicsPipelineStateDesc.GS.pShaderBytecode = gsBytecode;
	mGraphicsPipelineStateDesc.GS.BytecodeLength = gsBytecodeLength;

	mGraphicsPipelineStateDesc.DS.pShaderBytecode = dsBytecode;
	mGraphicsPipelineStateDesc.DS.BytecodeLength = dsBytecodeLength;

	mGraphicsPipelineStateDesc.HS.pShaderBytecode = hsBytecode;
	mGraphicsPipelineStateDesc.HS.BytecodeLength = hsBytecodeLength;



}

void Quad::GraphicPipelineStateGeneratorHelper::SetVSCode(const void* vsByteCode, SIZE_T vsByteCodeLength)
{
	mGraphicsPipelineStateDesc.VS.pShaderBytecode = vsByteCode;
	mGraphicsPipelineStateDesc.VS.BytecodeLength = vsByteCodeLength;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetPSCode(const void* psByteCode, SIZE_T psByteCodeLength)
{
	mGraphicsPipelineStateDesc.PS.pShaderBytecode = psByteCode;
	mGraphicsPipelineStateDesc.PS.BytecodeLength = psByteCodeLength;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetGSCode(const void* gsByteCode, SIZE_T gsByteCodeLength)
{

	mGraphicsPipelineStateDesc.GS.pShaderBytecode = gsByteCode;
	mGraphicsPipelineStateDesc.GS.BytecodeLength = gsByteCodeLength;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetDSCode(const void* dsByteCode, SIZE_T dsByteCodeLength)
{

	mGraphicsPipelineStateDesc.DS.pShaderBytecode = dsByteCode;
	mGraphicsPipelineStateDesc.DS.BytecodeLength = dsByteCodeLength;
}

void Quad::GraphicPipelineStateGeneratorHelper::SetHSCode(const void* hsByteCode, SIZE_T hsByteCodeLength)
{
	mGraphicsPipelineStateDesc.HS.pShaderBytecode = hsByteCode;
	mGraphicsPipelineStateDesc.HS.BytecodeLength = hsByteCodeLength;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetInputLayout(const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements)
{
	mGraphicsPipelineStateDesc.InputLayout.pInputElementDescs = pInputElementDescs;
	mGraphicsPipelineStateDesc.InputLayout.NumElements = NumElements;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType)
{
	mGraphicsPipelineStateDesc.PrimitiveTopologyType = primitiveTopologyType;


}

void Quad::GraphicPipelineStateGeneratorHelper::SetRenderTargetNum(int num)
{
	mGraphicsPipelineStateDesc.NumRenderTargets = num;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetRenderTargetViewFormat(int index, DXGI_FORMAT format)
{
	mGraphicsPipelineStateDesc.RTVFormats[index] = format;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetDepthStencilViewFormat(DXGI_FORMAT format)
{
	mGraphicsPipelineStateDesc.DSVFormat = format;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetDepthStencilViewDesc(BOOL depthEnable, D3D12_DEPTH_WRITE_MASK depthWriteMask, D3D12_COMPARISON_FUNC depthFunc, BOOL stencilEnable, UINT8 stencilReadMask, UINT8 stencilWriteMask, D3D12_DEPTH_STENCILOP_DESC frontFace, D3D12_DEPTH_STENCILOP_DESC backFace)
{

	mGraphicsPipelineStateDesc.DepthStencilState.DepthEnable = depthEnable;
	mGraphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = depthWriteMask;
	mGraphicsPipelineStateDesc.DepthStencilState.DepthFunc = depthFunc;
	mGraphicsPipelineStateDesc.DepthStencilState.StencilEnable = stencilEnable;
	mGraphicsPipelineStateDesc.DepthStencilState.StencilReadMask = stencilReadMask;
	mGraphicsPipelineStateDesc.DepthStencilState.StencilWriteMask = stencilWriteMask;
	mGraphicsPipelineStateDesc.DepthStencilState.FrontFace = frontFace;
	mGraphicsPipelineStateDesc.DepthStencilState.BackFace = backFace;




}

void Quad::GraphicPipelineStateGeneratorHelper::SetDepthStencilViewDesc(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	mGraphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

}

void Quad::GraphicPipelineStateGeneratorHelper::SetBlendDesc(bool alphaToCoverageEnable, bool IndependentBlendEnable, const std::vector<D3D12_RENDER_TARGET_BLEND_DESC>& renderTargetVector)
{

	mGraphicsPipelineStateDesc.BlendState.AlphaToCoverageEnable = alphaToCoverageEnable;
	mGraphicsPipelineStateDesc.BlendState.IndependentBlendEnable = IndependentBlendEnable;


	int num = min(8, renderTargetVector.size());

	for (int i = 0; i < num; ++i)
	{
		mGraphicsPipelineStateDesc.BlendState.RenderTarget[i] = renderTargetVector[i];
	}



}

void Quad::GraphicPipelineStateGeneratorHelper::SetRasterizerCullModeDesc(D3D12_CULL_MODE cullMode)
{
	mGraphicsPipelineStateDesc.RasterizerState.CullMode = cullMode;
}

void Quad::GraphicPipelineStateGeneratorHelper::SetRasterizerFillModeDesc(D3D12_FILL_MODE fillMode)
{
	mGraphicsPipelineStateDesc.RasterizerState.FillMode = fillMode;

}

Microsoft::WRL::ComPtr < ID3D12PipelineState >Quad::GraphicPipelineStateGeneratorHelper::EndGenerateGraphicPipelineState()
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;


	ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&mGraphicsPipelineStateDesc, IID_PPV_ARGS(pipelineState.GetAddressOf())));


	return pipelineState;


}

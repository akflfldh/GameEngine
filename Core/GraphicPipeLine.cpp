#include "Core/GraphicPipeLine.h"

namespace Quad
{
	void GraphicPipeLine::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList, const std::string & effectFile,
		const std::string & vs, const std::string & ps )
	{
		mGraphicsCommandList = graphicsCommandList;
		mEffectFile = effectFile;

		//하드코딩이지만 파일에서 읽어온정보다 라고생각하자.


		D3D12_ROOT_PARAMETER rootParameter[4];	//개수 타입 

		// object constant
		rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		D3D12_DESCRIPTOR_RANGE range[4];

		range[0].BaseShaderRegister = 0;
		range[0].NumDescriptors = 1;
		range[0].OffsetInDescriptorsFromTableStart = 0;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range[0].RegisterSpace = 0;
		rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameter[0].DescriptorTable.pDescriptorRanges = &range[0];

		//pass constant
		rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		range[1].BaseShaderRegister = 1;
		range[1].NumDescriptors = 1;
		range[1].OffsetInDescriptorsFromTableStart = 0;
		range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range[1].RegisterSpace = 0;
		rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParameter[1].DescriptorTable.pDescriptorRanges = &range[1];

		//diffuse texture

		rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		range[2].BaseShaderRegister = 0;
		range[2].NumDescriptors = 1;
		range[2].OffsetInDescriptorsFromTableStart = 0;
		range[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[2].RegisterSpace = 0;
		rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParameter[2].DescriptorTable.pDescriptorRanges = &range[2];

		//sampler 

		rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		range[3].BaseShaderRegister = 0;
		range[3].NumDescriptors = 1;
		range[3].OffsetInDescriptorsFromTableStart = 0;
		range[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		range[3].RegisterSpace = 0;
		rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
		rootParameter[3].DescriptorTable.pDescriptorRanges = &range[3];


		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters = _countof(rootParameter);
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pParameters = rootParameter;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);


		ThrowIfFailed(hr);

		ThrowIfFailed(device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));



		//shader compile

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
	//	std::ifstream fin("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\effect\\common_vs.cso", std::ios_base::binary);
		//std::ifstream fin("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\effect\\Text_vs.cso", std::ios_base::binary);
		std::ifstream fin(vs, std::ios_base::binary);

		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		ThrowIfFailed(D3DCreateBlob(size, &blob));
		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();


		Microsoft::WRL::ComPtr<ID3DBlob> blob2;
		//std::ifstream fin2("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\effect\\common_ps.cso", std::ios_base::binary);
		//std::ifstream fin2("C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\effect\\Text_ps.cso", std::ios_base::binary);
		fin.open(ps, std::ios_base::binary);
		//std::ifstream fin2(ps, std::ios_base::binary);


		fin.seekg(0, std::ios_base::end);
		size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		ThrowIfFailed(D3DCreateBlob(size, &blob2));
		fin.read((char*)blob2->GetBufferPointer(), size);
		fin.close();


		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(psoDesc));

		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.VS.pShaderBytecode = blob->GetBufferPointer();
		psoDesc.VS.BytecodeLength = blob->GetBufferSize();
		psoDesc.PS.pShaderBytecode = blob2->GetBufferPointer();
		psoDesc.PS.BytecodeLength = blob2->GetBufferSize();
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.NodeMask = 0;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.InputLayout.NumElements = 2;
		/*{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEX",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONEINDEXONE",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,32,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONEINDEXTWO",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,48,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONEWEIGHTONE",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,64,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "BONEWEIGHTTWO",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,80,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },*/

		std::vector< D3D12_INPUT_ELEMENT_DESC> inputElementVector(2);
		inputElementVector[0].SemanticName = "POSITION";
		inputElementVector[0].SemanticIndex = 0;
		inputElementVector[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementVector[0].InputSlot = 0;
		inputElementVector[0].AlignedByteOffset = 0;
		inputElementVector[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementVector[0].InstanceDataStepRate = 0;

		inputElementVector[1].SemanticName = "TEX";
		inputElementVector[1].SemanticIndex = 0;
		inputElementVector[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementVector[1].InputSlot = 0;
		inputElementVector[1].AlignedByteOffset = 12;
		inputElementVector[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementVector[1].InstanceDataStepRate = 0;

		psoDesc.InputLayout.pInputElementDescs = inputElementVector.data();
		psoDesc.InputLayout.NumElements = inputElementVector.size();



		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));

		//pipline object















	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicPipeLine::GetPipelineState() const
	{
		return mPipelineState;
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicPipeLine::GetRootSignature() const
	{
		return mRootSignature;
	}


}
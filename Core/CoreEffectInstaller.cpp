#include "CoreEffectInstaller.h"

#include"ResourceManager/EffectManager/EffectManager.h"
#include"Effect/RenderPassTwo.h"
#include"Utility/DoException.h"

#include"RootSignatureGeneratorHelper.h"
#include"GraphicPipelineStateGeneratorHelper.h"

#include"Shader/ShaderResourceConstantBuffer.h"
#include"Shader/ShaderResourceTexture.h"

#include"Buffer/ConstantBufferManager.h"

#include"RenderPassCommand.h"
#include"Shader/ShaderResourceType.h"

#include<exception>

#include"Effect/Effect.h"

Quad::CoreEffectInstaller::CoreEffectInstaller()
	:mRenderTargetToBackBuffereffect(nullptr)
{
}

Quad::CoreEffectInstaller::~CoreEffectInstaller()
{
}

void Quad::CoreEffectInstaller::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
	const std::string& effectFolderPath)
{
	mDevice = device;
	mEffectFolderPath = effectFolderPath;
	InstallTestEffect();
	InstallRenderTargetToBackBufferEffect();

}

//Quad::Effect* Quad::CoreEffectInstaller::GetRenderTargetToBackBufferEffect()const
//{
//	//return mRenderTargetToBackBuffereffect;
//	return nullptr;
//
//}



void Quad::CoreEffectInstaller::InstallTestEffect()
{


	Effect* testEffect = EffectManager::CreateEffect("TestEffect");

	testEffect->AddCustomTexture(ETextureType::eRenderTargetTexture,{ 0.3,0.3 });		//0번 텍스처
	testEffect->AddCustomTexture(ETextureType::eDefaultTexture,{ 1,1 }); //꼭 renderTarget이 아니여도 texture 를 추가할수있게하자 .




	std::vector<RenderPassTwo*> renderPassVector(2);


	renderPassVector[0] = new RenderPassTwo;
	renderPassVector[0]->SetCustomRenderTargetLocalID(0);
	

	renderPassVector[1] = new RenderPassTwo;


	//emissive write render pass
	SetTestEffectRenderPass0(renderPassVector[0]);


	//bloom render pass
	SetTestEffectRenderPass1(renderPassVector[1]);


	
	




	testEffect->SetStaticMeshRenderPassVector(std::move(renderPassVector));




	//testEffect->SetSkinningMeshRenderPassVector(std::move(renderPassVector));



}

void Quad::CoreEffectInstaller::SetTestEffectRenderPass0(RenderPassTwo* renderPass)
{
	RenderPassCommandFactory* renderPassCommandFactory = RenderPassCommandFactory::GetInstance();

	renderPass->SetRenderPassDrawType(ERenderPassDrawType::eObjectDraw);


	//pre command 

	//0번 텍스처를 Generic Read에서 render target으로  상태 변경

	RenderPassResourceStateTransitionCommand* emissiveTextureStateTransitionCommand = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));


	emissiveTextureStateTransitionCommand->SetTextureLocalID(0);
	emissiveTextureStateTransitionCommand->SetStateTransition(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	renderPass->AddPreRenderPassCommand(emissiveTextureStateTransitionCommand);

	//렌더타겟 설정 명령


	RenderPassSetRenderTargetCommand* emissiveTextureRenderTargetSetCommand = static_cast<RenderPassSetRenderTargetCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eRenderTargetSetting));


	emissiveTextureRenderTargetSetCommand->SetDepthStencilBufferLocalID(-1);
	emissiveTextureRenderTargetSetCommand->SetRenderTargetTextureLocalID(0);

	renderPass->AddPreRenderPassCommand(emissiveTextureRenderTargetSetCommand);






	//즉 책임져야하는거지
	//각각의 렌더패스는 시작할때 렌더타겟변경할수있고

	//post에서는 만약 변경했다면 다시 디폴트 렌더타켓으로 변경하도록 하자
	//그래야 다음의 다른 렌더패스들이 올바르게 이어서 사용할수있다.





	// post command 

	//기본 렌더타겟의 내용을 임시 텍스처에 복사하기위한 상태전이 

	//기본렌더타겟을 복사소스상태로
	RenderPassResourceStateTransitionCommand* resourceStateTransition1 = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));

	resourceStateTransition1->SetStateTransition(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
	resourceStateTransition1->SetTextureLocalID(-1);
	renderPass->AddPostRenderPassCommand(resourceStateTransition1);

	//임시 텍스처를 복사 목적 상태로
	RenderPassResourceStateTransitionCommand* resourceStateTransition2 = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));

	resourceStateTransition2->SetStateTransition(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
	resourceStateTransition2->SetTextureLocalID(1);
	renderPass->AddPostRenderPassCommand(resourceStateTransition2);

	//복사명령
	RenderPassTextureCopyCommnad* textureCopyCommand = static_cast<RenderPassTextureCopyCommnad*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eTextureCopy));

	textureCopyCommand->SetSourceTextureLocalID(-1);
	textureCopyCommand->SetDestTextureLocalID(1);

	//씬 장면을 복사 
	renderPass->AddPostRenderPassCommand(textureCopyCommand);



	//다시 기본렌더타켓을 원래상태로 명령
	RenderPassResourceStateTransitionCommand* resourceStateTransition3 = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));

	resourceStateTransition3->SetStateTransition(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	resourceStateTransition3->SetTextureLocalID(-1);
	renderPass->AddPostRenderPassCommand(resourceStateTransition3);



	//임시 텍스처를 읽기 상태로 명령
	RenderPassResourceStateTransitionCommand* resourceStateTransition4 = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));

	resourceStateTransition4->SetStateTransition(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	resourceStateTransition4->SetTextureLocalID(1);
	renderPass->AddPostRenderPassCommand(resourceStateTransition4);



	//Emissive texture를 읽기 상태로 
	RenderPassResourceStateTransitionCommand* resourceStateTransition5 = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eResourceStateTransition));

	resourceStateTransition5->SetStateTransition(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	resourceStateTransition5->SetTextureLocalID(0);
	renderPass->AddPostRenderPassCommand(resourceStateTransition5);



	//기본 렌더타켓을 렌더타켓으로설정

	RenderPassSetRenderTargetCommand* defaultRenderTargetSetCommand = static_cast<RenderPassSetRenderTargetCommand*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eRenderTargetSetting));


	defaultRenderTargetSetCommand->SetRenderTargetTextureLocalID(-1);
	defaultRenderTargetSetCommand->SetDepthStencilBufferLocalID(-1);

	renderPass->AddPostRenderPassCommand(resourceStateTransition5);




	//root signature
	RootSignatureGeneratorHelper* rootSignatureGeneratorHelper = RootSignatureGeneratorHelper::GetInstance();
	GraphicPipelineStateGeneratorHelper* graphicPipelineStateGeneratorHelper = GraphicPipelineStateGeneratorHelper::GetInstance();





	//emissive write pass root signature;
	rootSignatureGeneratorHelper->StartGenerateRootSignature(3, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, nullptr);

	//object constant buffer
	rootSignatureGeneratorHelper->SetRootParameter(0, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);

	rootSignatureGeneratorHelper->SetRootParameterTable(0, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(0, 0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);


	//pass constant buffer
	rootSignatureGeneratorHelper->SetRootParameter(1, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(1, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(1, 0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0);

	//material constant buffer
	rootSignatureGeneratorHelper->SetRootParameter(2, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(2, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(2, 0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2, 0);


	Microsoft::WRL::ComPtr<ID3D12RootSignature> emssiveWritePassRootSignature = rootSignatureGeneratorHelper->EndGenerateRootSignature();






	//piplineState





	graphicPipelineStateGeneratorHelper->StartGenerateGraphicPipelineState(emssiveWritePassRootSignature.Get());


	//compileShader
	std::string emissiveWriteVsHLSLPath = mEffectFolderPath + "\\EmissiveVS.cso";
	size_t emissiveWriteVsSize = 0;

	std::string emissiveWritePsHLSLPath = mEffectFolderPath + "\\EmissivePS.cso";
	size_t emissiveWritePsSize = 0;

	std::unique_ptr<char[]> emissiveWriteVsBuffer = Utility::ReadBinaryFileAll(emissiveWriteVsHLSLPath.c_str(), emissiveWriteVsSize);

	std::unique_ptr<char[]> emissiveWriteVPsBuffer = Utility::ReadBinaryFileAll(emissiveWritePsHLSLPath.c_str(), emissiveWritePsSize);

	if (emissiveWriteVsBuffer == nullptr || emissiveWriteVPsBuffer == nullptr)
	{
		OutputDebugStringW(L"Emissive vs or ps 읽기 실패\n");
		assert(0);
	}

	graphicPipelineStateGeneratorHelper->SetShaderCode(emissiveWriteVsBuffer.get(), emissiveWriteVsSize,
		emissiveWriteVPsBuffer.get(), emissiveWritePsSize);

	//input layout 

	//D3D12_INPUT_LAYOUT_DESC emissiveWritePassInputLayout;
	std::vector< D3D12_INPUT_ELEMENT_DESC> emissiveWritePassInputElementVector(1);

	//posL
	emissiveWritePassInputElementVector[0].SemanticName = "POSITION";
	emissiveWritePassInputElementVector[0].SemanticIndex = 0;
	emissiveWritePassInputElementVector[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	emissiveWritePassInputElementVector[0].InputSlot = 0;
	emissiveWritePassInputElementVector[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	emissiveWritePassInputElementVector[0].AlignedByteOffset = 0;
	emissiveWritePassInputElementVector[0].InstanceDataStepRate = 0;

	//emissiveWritePassInputLayout.NumElements = 1;
	//emissiveWritePassInputLayout.pInputElementDescs = emissiveWritePassInputElementVector.data();

	graphicPipelineStateGeneratorHelper->SetInputLayout(emissiveWritePassInputElementVector.data(), emissiveWritePassInputElementVector.size());

	//depthStencilDesc

	CD3DX12_DEPTH_STENCIL_DESC emissiveWritePassDepthStencilDesc(CD3DX12_DEFAULT{});
	emissiveWritePassDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicPipelineStateGeneratorHelper->SetDepthStencilViewDesc(emissiveWritePassDepthStencilDesc);

	graphicPipelineStateGeneratorHelper->SetDepthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);



	Microsoft::WRL::ComPtr<ID3D12PipelineState> emissiveWritePassPipelineState = graphicPipelineStateGeneratorHelper->EndGenerateGraphicPipelineState();




	renderPass->SetPipelineState(emissiveWritePassPipelineState);
	renderPass->SetPrimitiveToplogyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);




	//shader resource


	//object constant buffer
	ShaderResourceConstantBuffer* objectConstantBuffer = new ShaderResourceConstantBuffer(L"emissiveObjectConstantBuffer", 0, 0);
	objectConstantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(sizeof(DirectX::XMFLOAT4X4)));

	std::vector<ShaderResourceVariable> objectConstantBufferShaderResourceVariableVector(1);
	//gWorld 
	objectConstantBufferShaderResourceVariableVector[0].mName = L"gWorld";
	objectConstantBufferShaderResourceVariableVector[0].mElementNum = 1;
	objectConstantBufferShaderResourceVariableVector[0].mOffsetInShaderResource = 0;
	objectConstantBufferShaderResourceVariableVector[0].mSize = sizeof(DirectX::XMFLOAT4X4);
	//shaderResourceVariableVector[0].mClass =

	objectConstantBuffer->Initialize(std::move(objectConstantBufferShaderResourceVariableVector), sizeof(DirectX::XMFLOAT4X4));

	renderPass->AddShaderResourece(objectConstantBuffer);
	//pass constant buffer
	// 
	//gViewProj
	ShaderResourceConstantBuffer* passConstantBuffer = new ShaderResourceConstantBuffer(L"emissivePassConstantBuffer", 1, 0);
	passConstantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(sizeof(DirectX::XMFLOAT4X4)));

	std::vector<ShaderResourceVariable> passConstantBufferShaderResourceVariableVector(1);
	//gWorld 
	passConstantBufferShaderResourceVariableVector[0].mName = L"gViewProj";
	passConstantBufferShaderResourceVariableVector[0].mElementNum = 1;
	passConstantBufferShaderResourceVariableVector[0].mOffsetInShaderResource = 0;
	passConstantBufferShaderResourceVariableVector[0].mSize = sizeof(DirectX::XMFLOAT4X4);
	//shaderResourceVariableVector[0].mClass =

	passConstantBuffer->Initialize(std::move(passConstantBufferShaderResourceVariableVector), sizeof(DirectX::XMFLOAT4X4));

	renderPass->AddShaderResourece(passConstantBuffer);




	//material constant buffer



	ShaderResourceConstantBuffer* materialConstantBuffer = new ShaderResourceConstantBuffer(L"emissivePassConstantBuffer", 1, 0);
	materialConstantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(sizeof(DirectX::XMFLOAT4X4)));

	std::vector<ShaderResourceVariable> materialConstantBufferShaderResourceVariableVector(1);

	materialConstantBufferShaderResourceVariableVector[0].mName = L"gEmissive";
	materialConstantBufferShaderResourceVariableVector[0].mElementNum = 1;
	materialConstantBufferShaderResourceVariableVector[0].mOffsetInShaderResource = 0;
	materialConstantBufferShaderResourceVariableVector[0].mSize = sizeof(DirectX::XMFLOAT3);
	//shaderResourceVariableVector[0].mClass =

	materialConstantBuffer->Initialize(std::move(materialConstantBufferShaderResourceVariableVector), sizeof(DirectX::XMFLOAT3));

	renderPass->AddShaderResourece(materialConstantBuffer);

	renderPass->SetRenderPassID(ERenderPassID::eEmissiveWrite);
	renderPass->SetRootSignature(emssiveWritePassRootSignature);




























}

void Quad::CoreEffectInstaller::SetTestEffectRenderPass1(RenderPassTwo* renderPass)
{

	RenderPassCommandFactory* renderPassCommandFactory = RenderPassCommandFactory::GetInstance();


	renderPass->SetRenderPassDrawType(ERenderPassDrawType::eEntirePostDraw);


	//pre command 

	//0 ,1 텍스처들을 srv 로서 바인딩 
	//emissive texture
	RenderPassTextureBind * renderPassTextureBindCommand1  =static_cast<RenderPassTextureBind*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eTextureBind));

	renderPassTextureBindCommand1->SetTextureSrv(EShaderResourceDescriptorType::eShaderResourceSrv, 0, 1);
	renderPass->AddPreRenderPassCommand(renderPassTextureBindCommand1);
	//원본copy texture
	RenderPassTextureBind* renderPassTextureBindCommand2 = static_cast<RenderPassTextureBind*>(renderPassCommandFactory->CreateRenderPassCommand(ERenderPassCommandType::eTextureBind));


	renderPassTextureBindCommand2->SetTextureSrv(EShaderResourceDescriptorType::eShaderResourceSrv, 1, 2);
	renderPass->AddPreRenderPassCommand(renderPassTextureBindCommand2);


	//pose command



	RootSignatureGeneratorHelper* rootSignatureGeneratorHelper = RootSignatureGeneratorHelper::GetInstance();
	GraphicPipelineStateGeneratorHelper* graphicPipelineStateGeneratorHelper = GraphicPipelineStateGeneratorHelper::GetInstance();

	//root signature

	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescVector(1);

	staticSamplerDescVector[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	staticSamplerDescVector[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].MipLODBias = 0;
	staticSamplerDescVector[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	staticSamplerDescVector[0].MinLOD = 0;
	staticSamplerDescVector[0].MaxLOD = 0;
	staticSamplerDescVector[0].ShaderRegister = 0;
	staticSamplerDescVector[0].RegisterSpace = 0;
	staticSamplerDescVector[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	rootSignatureGeneratorHelper->StartGenerateRootSignature(3, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, &staticSamplerDescVector);
	

	//pass constant
	rootSignatureGeneratorHelper->SetRootParameter(0, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(0, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(0, 0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);
	
	
	//texture 1
	rootSignatureGeneratorHelper->SetRootParameter(1, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(1, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(1, 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);



	//texture 2
	rootSignatureGeneratorHelper->SetRootParameter(2, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(2, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(2, 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature =	rootSignatureGeneratorHelper->EndGenerateRootSignature();

	//graphic pipeline 

	graphicPipelineStateGeneratorHelper->StartGenerateGraphicPipelineState(rootSignature.Get());

	//shader compile

	std::string emissiveBloomVsHLSLPath = mEffectFolderPath + "\\EmissiveBloomVS.cso";
	//size_t emissiveBloomVsSize = 0;

	std::string emissiveBloomPsHLSLPath = mEffectFolderPath + "\\EmissiveBloomPS.cso";
	//size_t emissiveBloomPsSize = 0;

	//std::unique_ptr<char[]> emissiveBloomVsBuffer = Utility::ReadBinaryFileAll(emissiveBloomVsHLSLPath.c_str(), emissiveBloomVsSize);

	//std::unique_ptr<char[]> emissiveBloomVPsBuffer = Utility::ReadBinaryFileAll(emissiveBloomPsHLSLPath.c_str(), emissiveBloomPsSize);

	//if (emissiveBloomVsBuffer == nullptr || emissiveBloomVPsBuffer == nullptr)
	//{
	//	OutputDebugStringW(L"Emissive bloom vs or ps 읽기 실패\n");
	//	assert(0);
	//}

	//graphicPipelineStateGeneratorHelper->SetShaderCode(emissiveBloomVsBuffer.get(), emissiveBloomVsSize,
	//	emissiveBloomVPsBuffer.get(), emissiveBloomPsSize);

	std::vector<std::unique_ptr<char[]>> hlslBufferVector;

	LoadCompiledHLSL(emissiveBloomVsHLSLPath, emissiveBloomPsHLSLPath, graphicPipelineStateGeneratorHelper, hlslBufferVector);




	//input layout
	std::vector< D3D12_INPUT_ELEMENT_DESC> emissiveBloomPassInputElementVector(2);
	//posH
	emissiveBloomPassInputElementVector[0].SemanticName = "POSITION";
	emissiveBloomPassInputElementVector[0].SemanticIndex = 0;
	emissiveBloomPassInputElementVector[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	emissiveBloomPassInputElementVector[0].InputSlot = 0;
	emissiveBloomPassInputElementVector[0].AlignedByteOffset = 0;
	emissiveBloomPassInputElementVector[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	emissiveBloomPassInputElementVector[0].InstanceDataStepRate = 0;



	//tex
	emissiveBloomPassInputElementVector[1].SemanticName = "TEX";
	emissiveBloomPassInputElementVector[1].SemanticIndex = 0;
	emissiveBloomPassInputElementVector[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	emissiveBloomPassInputElementVector[1].InputSlot = 0;
	emissiveBloomPassInputElementVector[1].AlignedByteOffset = 24;
	emissiveBloomPassInputElementVector[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	emissiveBloomPassInputElementVector[1].InstanceDataStepRate = 0;


	graphicPipelineStateGeneratorHelper->SetInputLayout(emissiveBloomPassInputElementVector.data(), 2);





	//다른 속성 설정

	CD3DX12_DEPTH_STENCIL_DESC emissiveBloomPassDepthStencilDesc(CD3DX12_DEFAULT{});
	emissiveBloomPassDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	graphicPipelineStateGeneratorHelper->SetDepthStencilViewDesc(emissiveBloomPassDepthStencilDesc);

	graphicPipelineStateGeneratorHelper->SetDepthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);



	//create pipeline

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState =	graphicPipelineStateGeneratorHelper->EndGenerateGraphicPipelineState();


	//shader resource 생성


	ShaderResourceConstantBuffer* passConstantBuffer = new ShaderResourceConstantBuffer(L"emissiveBloomPassConstantBuffer", 1, 0);
	passConstantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(sizeof(float) * 2));

	std::vector<ShaderResourceVariable> passConstantBufferShaderResourceVariableVector(2);
	//gWorld 
	passConstantBufferShaderResourceVariableVector[0].mName = L"gClientWidth";
	passConstantBufferShaderResourceVariableVector[0].mElementNum = 1;
	passConstantBufferShaderResourceVariableVector[0].mOffsetInShaderResource = 0;
	passConstantBufferShaderResourceVariableVector[0].mSize = sizeof(float);


	passConstantBufferShaderResourceVariableVector[1].mName = L"gClientHeight";
	passConstantBufferShaderResourceVariableVector[1].mElementNum = 1;
	passConstantBufferShaderResourceVariableVector[1].mOffsetInShaderResource = sizeof(float);
	passConstantBufferShaderResourceVariableVector[1].mSize = sizeof(float);



	passConstantBuffer->Initialize(std::move(passConstantBufferShaderResourceVariableVector), sizeof(float) * 2);


	renderPass->AddShaderResourece(passConstantBuffer);



	ShaderResourceTexture* emissiveMap = new ShaderResourceTexture(L"emissiveShaderResourceTexture", 0, 0, EShaderResourceTextureType::eT2D);
	emissiveMap->SetShaderResourceVisibility(EShaderResourceVisibility::ePixel);

	renderPass->AddShaderResourece(emissiveMap);


	ShaderResourceTexture* copiedSceneMap = new ShaderResourceTexture(L"copiedSceneShaderResourrceTexture", 0, 0, EShaderResourceTextureType::eT2D);
	copiedSceneMap->SetShaderResourceVisibility(EShaderResourceVisibility::ePixel);

	renderPass->AddShaderResourece(copiedSceneMap);



	renderPass->SetPipelineState(pipelineState);
	renderPass->SetRootSignature(rootSignature);


	//rendepass ID 우선순위도 설정

	renderPass->SetRenderPassID(ERenderPassID::eBloom);


}

void Quad::CoreEffectInstaller::LoadCompiledHLSL(const std::string& vsFilePath, const std::string& psFilePath, GraphicPipelineStateGeneratorHelper* graphicPipelineStateGeneratorHelper, std::vector<std::unique_ptr<char[]>>& oRawBufferVector)
{


	size_t vsSize = 0;

	size_t psSize = 0;

	std::unique_ptr<char[]> vsBuffer;
	std::unique_ptr<char[]> psBuffer;


	if (vsFilePath.size() != 0)
	{
		vsBuffer = Utility::ReadBinaryFileAll(vsFilePath.c_str(), vsSize);
		if (vsBuffer == nullptr)
		{
			OutputDebugStringW(L"버텍스 셰이더 읽기 실패\n");
			throw std::runtime_error("버텍스 셰이더 읽기 실패");
		}

		graphicPipelineStateGeneratorHelper->SetVSCode(vsBuffer.get(), vsSize);
	}


	if (psFilePath.size() != 0)
	{
		psBuffer = Utility::ReadBinaryFileAll(psFilePath.c_str(), psSize);
	
		if (psBuffer == nullptr)
		{
			throw std::runtime_error("픽셀 셰이더 읽기 실패");
		}

		graphicPipelineStateGeneratorHelper->SetPSCode(psBuffer.get(), psSize);

	}


	//if (vsBuffer == nullptr || psBuffer == nullptr)
	//{
	//	OutputDebugStringW(L"Emissive bloom vs or ps 읽기 실패\n");
	//	assert(0);
	//}


	/*graphicPipelineStateGeneratorHelper->SetShaderCode(vsBuffer.get(), vsSize,
		psBuffer.get(), psSize);*/


	oRawBufferVector.push_back(std::move(vsBuffer));
	oRawBufferVector.push_back(std::move(psBuffer));


}

void Quad::CoreEffectInstaller::InstallRenderTargetToBackBufferEffect()
{
	Effect* testEffect = EffectManager::CreateEffect("RenderTargetToBackBufferEffect");
	
	std::vector<RenderPassTwo*> renderPassVector(1);
	renderPassVector[0] = new RenderPassTwo;

	//static sampler 

	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescVector(1);

	staticSamplerDescVector[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	staticSamplerDescVector[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDescVector[0].MipLODBias = 0;
	staticSamplerDescVector[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	staticSamplerDescVector[0].MinLOD = 0;
	staticSamplerDescVector[0].MaxLOD = 0;
	staticSamplerDescVector[0].ShaderRegister = 0;
	staticSamplerDescVector[0].RegisterSpace = 0;
	staticSamplerDescVector[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;




	//rootsignature
	RootSignatureGeneratorHelper* rootSignatureGeneratorHelper = RootSignatureGeneratorHelper::GetInstance();
	
	rootSignatureGeneratorHelper->StartGenerateRootSignature(2, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, &staticSamplerDescVector);


	//일단 값들을 위한
	rootSignatureGeneratorHelper->SetRootParameter(0, D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS);
	rootSignatureGeneratorHelper->SetRootParameterConstants(0, 0, 0, 4);


	//텍스처 바인딩을 위한 
	rootSignatureGeneratorHelper->SetRootParameter(1, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
	rootSignatureGeneratorHelper->SetRootParameterTable(1, 1);
	rootSignatureGeneratorHelper->SetRootParameterTableRange(1, 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature =	rootSignatureGeneratorHelper->EndGenerateRootSignature();
	
	


	//pipeline 
	GraphicPipelineStateGeneratorHelper* graphicPipelineStateGeneratorHelper = GraphicPipelineStateGeneratorHelper::GetInstance();

	graphicPipelineStateGeneratorHelper->StartGenerateGraphicPipelineState(rootSignature.Get());

	

	//hlsl
	const std::string vsFilePath = mEffectFolderPath + "\\RenderTargetToBackBufferEffectVS.cso";
	const std::string psFilePath = mEffectFolderPath + "\\RenderTargetToBackBufferEffectPS.cso";

	std::vector<std::unique_ptr<char[]>> hlslBufferVector;

	LoadCompiledHLSL(vsFilePath, psFilePath, graphicPipelineStateGeneratorHelper, hlslBufferVector);



	//input layout

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector(2);

	//pos
	inputElementDescVector[0].SemanticName = "POSITION";
	inputElementDescVector[0].SemanticIndex = 0;
	inputElementDescVector[0].AlignedByteOffset = 0;
	inputElementDescVector[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescVector[0].InputSlot = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescVector[0].InstanceDataStepRate = 0;
	inputElementDescVector[0].InputSlot = 0;

	
	inputElementDescVector[1].SemanticName = "TEX";
	inputElementDescVector[1].SemanticIndex = 0;
	inputElementDescVector[1].AlignedByteOffset = 12+12;	//POS+ NORMAL 
	inputElementDescVector[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescVector[1].InputSlot = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescVector[1].InstanceDataStepRate = 0;
	inputElementDescVector[1].InputSlot = 0;

	graphicPipelineStateGeneratorHelper->SetInputLayout(inputElementDescVector.data(),2);




	//depthStencilDesc
	CD3DX12_DEPTH_STENCIL_DESC	depthStencilDesc(CD3DX12_DEFAULT{});
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	graphicPipelineStateGeneratorHelper->SetDepthStencilViewDesc(depthStencilDesc);

	//rasterizer
	//CD3DX12_RASTERIZER_DESC reasterizerDesc(CD3DX12_DEFAULT{});
	//reasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	graphicPipelineStateGeneratorHelper->SetRasterizerCullModeDesc(D3D12_CULL_MODE_NONE);


	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicPipelineState = graphicPipelineStateGeneratorHelper->EndGenerateGraphicPipelineState();



	renderPassVector[0]->SetPipelineState(graphicPipelineState);
	renderPassVector[0]->SetRootSignature(rootSignature);

	testEffect->SetStaticMeshRenderPassVector(std::move(renderPassVector));
	//rendepass ID 우선순위도 설정

	//renderPassVector[0]->SetRenderPassID(ERenderPassID::eBloom);







}

#include<Windows.h>
#include "EffectLoader.h"
#include<fstream>
#include"Effect/Effect.h"
#include<regex>
#include"RawResourceType.h"
#include"Utility.h"
#include"EffectUtility.h"
#include"ResourceManager/EffectManager/EffectManager.h"
#include"ShaderManager.h"
#include"Effect/RenderPassTwo.h"
#include"Core/Shader.h"
#include"Buffer/ConstantBufferManager.h"
#include"Shader/ShaderResourceTexture.h"
#include"HeapManager/SamplerManager.h"
#include"Shader/ShaderResourceSampler.h"

#include"Shader/ShaderResourceConstantBuffer.h"


void Quad::EffectLoader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	mDevice = device;
	InitBlendEnumUnMap();
	InitStencilEnumUnMap();
	InitComparisonFuncEnumUnMap();


}

bool Quad::EffectLoader::Load(const std::string& filePath)
{

	RawEffectDataTwo effectData;

	effectData.mEffectName = Utility::GetFileNameFromPath(filePath);





	std::ifstream fin(filePath);
	if (!fin.is_open())
	{
		//디버깅 창에출력
		MessageBox(nullptr, L"effect파일 로드 실패", L"error", 0);
		return false;
	}

	std::string oneLineStr;

	std::regex passPatern("PASS[[:blank:]]*");
	std::regex blankPatern("[[:blank:]]*");


	std::getline(fin, oneLineStr);
	while (fin.good())
	{

		if (std::regex_match(oneLineStr, blankPatern))
		{
			std::getline(fin, oneLineStr);
			continue;
		}else if (std::regex_match(oneLineStr, passPatern))
		{
			//render pass 읽기 시작.
			RawRenderPassDataTwo rawRenderPassDataTwo;
			if (LoadRenderPassTwo(fin, rawRenderPassDataTwo))
				effectData.mRenderPassDataVector.push_back(rawRenderPassDataTwo);

		}

		std::getline(fin, oneLineStr);

	}



	ImplementEffect(effectData);


	return true;
}


bool Quad::EffectLoader::LoadRenderPassTwo(std::ifstream& fin, RawRenderPassDataTwo& oRawRenderPassDataTwo)
{


	std::regex blankPattern("[[:blank:]]*");
	std::regex leftBracePattern("[[:blank:]]*\\{[[:blank:]]*");
	std::regex rightBracePattern("[[:blank:]]*\\}[[:blank:]]*");
	std::regex shaderPattern("[[:blank:]]*([[:upper:]]+)[[:blank:]]*");
	std::regex graphicsPipelineCfgPattern("[[:blank:]]*([[:upper:]]+)[[:blank:]]*:[[:blank:]]*([[:w:]_]+)[[:blank:]]*");

		
		//STENCILENABLE: True


	std::smatch subMatch;
	std::string oneLineStr;
	std::getline(fin, oneLineStr);


	bool braceStart = false;


	while (fin.good())
	{

		//빈칸
		if (std::regex_match(oneLineStr, blankPattern))
		{
			std::getline(fin, oneLineStr);
			continue;
		}
		

		//{
		if (std::regex_match(oneLineStr, leftBracePattern))
		{

			if (braceStart == false)
			{
				braceStart = true;
			}
			else
			{
				//에러 디버깅 창에 출력
				MessageBox(nullptr, L"effect파일 구문 오류", L"error", 0);
				assert(0);
				return false;
			}
		}else if (std::regex_match(oneLineStr, rightBracePattern))//}
		{

			if (braceStart == true)
			{
				return true;
			}
			else
			{
				//에러 디버깅 창에 출력
				MessageBox(nullptr, L"effect파일 구문 오류", L"error", 0);
				assert(0);
				return false;
			}

		}//shader
		else if (std::regex_match(oneLineStr, subMatch, shaderPattern))
		{

			const std::string& shaderName = subMatch[1];
			if (shaderName == "VS")
			{

				RawShaderDataTwo rawShaderDataTwo;
				if (LoadShaderDataTwo(fin, rawShaderDataTwo))
				{
					oRawRenderPassDataTwo.mVertexShaderData = std::move(rawShaderDataTwo);
				}

			}
			else if (shaderName == "PS")
			{
				RawShaderDataTwo rawShaderDataTwo;
				if (LoadShaderDataTwo(fin, rawShaderDataTwo))
				{
					oRawRenderPassDataTwo.mPixelShaderData = std::move(rawShaderDataTwo);
				}

			}
			else {

				//에러 디버깅 창에 출력
				MessageBox(nullptr, L"effect파일 구문(shader이름) 오류", L"error", 0);
				assert(0);
				return false;
			}
		}
		else if (std::regex_match(oneLineStr, subMatch, graphicsPipelineCfgPattern))
		{
			const std::string& key = subMatch[1];
			const std::string& value = subMatch[2];

			oRawRenderPassDataTwo.mGraphicsPipelineCfgVector.push_back(std::pair{ key,value });

		}

		
		std::getline(fin, oneLineStr);







	}




	return true;






}

bool Quad::EffectLoader::LoadShaderDataTwo(std::ifstream& fin, RawShaderDataTwo& oRawShaderDataTwo)
{




	std::regex blankPattern("[[:blank:]]*");
	std::regex leftBracePattern("[[:blank:]]*\\{[[:blank:]]*");
	std::regex rightBracePattern("[[:blank:]]*\\}[[:blank:]]*");
	std::regex cfgPattern("[[:blank:]]*([[:alpha:]]+)[[:blank:]]*:[[:blank:]]*\"([[:alnum:]+\\._]+)\"[[:blank:]]*");

	std::smatch subMatch;



	std::string oneLineStr;
	std::getline(fin, oneLineStr);

	bool braceStart = false;
	while (fin.good())
	{

		if (std::regex_match(oneLineStr, blankPattern))
		{
			std::getline(fin, oneLineStr);
			continue;
		}

		if (std::regex_match(oneLineStr, leftBracePattern))
		{
			if (braceStart == false)
			{
				braceStart = true;
			}
			else
			{
				//에러 디버깅 창에 출력
				MessageBox(nullptr, L"effect파일 구문 오류", L"error", 0);
				assert(0);
				return false;
			}


		}
		else if (std::regex_match(oneLineStr, rightBracePattern))//}
		{

			if (braceStart == true)
			{
				return true;
			}
			else
			{
				//에러 디버깅 창에 출력
				MessageBox(nullptr, L"effect파일 구문 오류", L"error", 0);
				assert(0);
				return false;
			}

		}
		else if (std::regex_match(oneLineStr, subMatch, cfgPattern))
		{

			const std::string& cfgName = subMatch[1];
			const std::string& cfgValue = subMatch[2];

			if (cfgName == "File")
			{
				oRawShaderDataTwo.mFile = cfgValue;
			}
			else if (cfgName == "EntryPoint")
			{
				oRawShaderDataTwo.mEntryPoint = cfgValue;

			}
			else if (cfgName == "Version")
			{
				oRawShaderDataTwo.mVersion = cfgValue;
			}
			else
			{
				//디버깅 창 출력
				MessageBox(nullptr, L"effect파일 구문(셰이더)요류", L"error", 0);
				assert(0);
				return false;
			}

		}


		std::getline(fin, oneLineStr);
	}




	return true;
}

void Quad::EffectLoader::ImplementEffect(RawEffectDataTwo& effectData)
{
	Effect * effect = EffectManager::CreateEffect(effectData.mEffectName);

	if (effect == nullptr)
		return;


	//effect클래스가 존재하면된다.
	std::vector<RenderPassTwo* >renderPassVector;
	for (int i = 0; i < effectData.mRenderPassDataVector.size(); ++i)
	{
		RenderPassTwo* renderPass = CreateRenderPass(effectData.mRenderPassDataVector[i], false);
		renderPassVector.push_back(renderPass);
	}



	std::vector<RenderPassTwo*> renderPassVectorS;//skinning버전
	for (int i = 0; i < effectData.mRenderPassDataVector.size(); ++i)
	{
		RenderPassTwo* renderPass = CreateRenderPass(effectData.mRenderPassDataVector[i], true);
		renderPassVectorS.push_back(renderPass);
	}



	if (effect != nullptr)
	{
		effect->SetStaticMeshRenderPassVector(std::move(renderPassVector));
		effect->SetSkinningMeshRenderPassVector(std::move(renderPassVectorS));
	}





}

 Quad::RenderPassTwo  * Quad::EffectLoader::CreateRenderPass(RawRenderPassDataTwo& rawRenderPassTwoData, bool isSkinningShader)
{

	 RenderPassTwo* renderPass = new RenderPassTwo;

	 ID3DInclude* pInclude = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	 UINT compileFlags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	 UINT compileFlags2 = 0;

	 std::vector<Shader*> shaderVector;
	 Shader* vertexShader = nullptr;

	 std::vector<D3D_SHADER_MACRO> macroVector;
	 D3D_SHADER_MACRO* pMarco = nullptr;
	 std::string shaderMeshType = "Static";
	 if (isSkinningShader)
	 {
		 shaderMeshType = "Skinning";
		 D3D_SHADER_MACRO skinningMarco;
		 skinningMarco.Name = "SKINNING";
		 skinningMarco.Definition = 0;

		 macroVector.push_back(skinningMarco);
		 macroVector.push_back({ NULL,NULL });

		 pMarco = macroVector.data();
	 }





	 if (rawRenderPassTwoData.mVertexShaderData.mFile.size() != 0)
	 {
		 const std::string& file = rawRenderPassTwoData.mVertexShaderData.mFile;
		 const std::string& entryPoint = rawRenderPassTwoData.mVertexShaderData.mEntryPoint;
		 const std::string& version = rawRenderPassTwoData.mVertexShaderData.mVersion;

		 //컴파일하기전에  이미 컴파일한 세이더인지 점검
		 Shader* shader = ShaderManager::GetShader(file, entryPoint, version, shaderMeshType);

		 if (shader == nullptr)
		 {


			 Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = CompileShader(file, pMarco, pInclude, entryPoint, version, compileFlags1, compileFlags2);
			 shader = new Shader;
			 shader->Initialize(file, entryPoint, version, shaderMeshType, shaderByteCode, EShaderType::eVS);
			 ShaderManager::AddShader(shader);
		 }

		 // shader resource 생성
		 ShaderResourceReflection(renderPass, shader, EShaderResourceVisibility::eVertex);
		 shaderVector.push_back(shader);
		 vertexShader = shader;
	 }


	 if (rawRenderPassTwoData.mPixelShaderData.mFile.size() != 0)
	 {

		 const std::string& file = rawRenderPassTwoData.mPixelShaderData.mFile;
		 const std::string& entryPoint = rawRenderPassTwoData.mPixelShaderData.mEntryPoint;
		 const std::string& version = rawRenderPassTwoData.mPixelShaderData.mVersion;

		 //컴파일하기전에  이미 컴파일한 세이더인지 점검
		 Shader* shader = ShaderManager::GetShader(file, entryPoint, version, shaderMeshType);

		 if (shader == nullptr)
		 {
			 Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = CompileShader(file, pMarco, pInclude, entryPoint, version, compileFlags1, compileFlags2);
			 shader = new Shader;
			 shader->Initialize(file, entryPoint, version, shaderMeshType, shaderByteCode, EShaderType::ePS);
			 ShaderManager::AddShader(shader);
		 }

		 // shader resource 생성
		 ShaderResourceReflection(renderPass, shader, EShaderResourceVisibility::ePixel);
		 shaderVector.push_back(shader);
	 }


	 //create root signature 
	 // 

	 CreateRootSignature(renderPass);

	 CreateGraphicsPipeline(rawRenderPassTwoData, renderPass, shaderVector, vertexShader);





	 //effect파일의여러 옵션들을 읽어서 ,
	 //최종적으로 pipeline생성
	 //create pipeline 






	 return renderPass;

}

Microsoft::WRL::ComPtr<ID3DBlob> Quad::EffectLoader::CompileShader(const std::string& fileName, const D3D_SHADER_MACRO* pMacro, ID3DInclude* pInclude, const std::string& entryPoint, const std::string& version, UINT flags1, UINT flag2)
{


	//std::string entryPoint;
	//std::string version;

	//icu::UnicodeString ustr(entryPointW.c_str());
	//ustr.toUTF8String(entryPoint);

	//ustr = versionW.c_str();
	//ustr.toUTF8String(version);
	std::wstring filenNameW = Utility::ConvertToWString(fileName,true);


	Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode;
	ID3DBlob* errorByteCode;
	HRESULT ret = D3DCompileFromFile(filenNameW.c_str(), pMacro, pInclude, entryPoint.c_str(), version.c_str(), flags1, flag2,
		shaderByteCode.GetAddressOf(), &errorByteCode);



	if (errorByteCode != nullptr)
	{
		//디버그창에 출력
	}

	ThrowIfFailed(ret);		//일단다음과같이 처리, 나중에는 계속 엔진이 진행되도록해야한다.
	return shaderByteCode;


}

void Quad::EffectLoader::ShaderResourceReflection(RenderPassTwo* renderPass, Shader* shader, EShaderResourceVisibility shaderResourceVisibility)
{

	Microsoft::WRL::ComPtr< ID3D12ShaderReflection> shaderReflectionInterface;
	D3DReflect(shader->GetShaderByteCode().Get()->GetBufferPointer(), shader->GetShaderByteCode().Get()->GetBufferSize(), IID_PPV_ARGS(shaderReflectionInterface.GetAddressOf()));

	D3D12_SHADER_DESC shaderDesc;
	shaderReflectionInterface->GetDesc(&shaderDesc);
	UINT shaderResourceNums = shaderDesc.BoundResources;



	for (UINT i = 0; i < shaderResourceNums; ++i)
	{

		D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
		shaderReflectionInterface->GetResourceBindingDesc(i, &shaderInputBindDesc);

		const std::wstring shaderResourceName = Utility::ConvertToWString(shaderInputBindDesc.Name, true);
		if (renderPass->GetShaderResource(shaderResourceName) != nullptr)
		{

			//이미 리소수가 존재함으로 새로 만들필요없이
			//두개이상의 셰이더에서 이 리소스에 접근함으로 셰이더의 가시성설정을 ALL로 변경한다.
			ShaderResource* shaderResource = renderPass->GetShaderResource(shaderResourceName);
			shaderResource->SetShaderResourceVisibility(EShaderResourceVisibility::eAll);
			continue;
		}
		int registerIndex = shaderInputBindDesc.BindPoint;
		int registerSpace = shaderInputBindDesc.Space;


		switch (shaderInputBindDesc.Type)
		{

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER://상수버퍼
		{
			ID3D12ShaderReflectionConstantBuffer* pShaderReflectionConstantBuffer = shaderReflectionInterface->GetConstantBufferByName(shaderInputBindDesc.Name);




			D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
			pShaderReflectionConstantBuffer->GetDesc(&shaderBufferDesc);

			UINT bufferSize = shaderBufferDesc.Size;
			UINT variableNum = shaderBufferDesc.Variables;


			ShaderResourceConstantBuffer* shaderResourceCosntantBuffer =
				new ShaderResourceConstantBuffer(shaderResourceName, registerIndex, registerSpace);
			SetObjectOrPassFlag(shaderResourceCosntantBuffer);
			std::vector<ShaderResourceVariable> variableVector(variableNum);


			//리소스내의 변수
			for (UINT variableIndex = 0; variableIndex < variableNum; ++variableIndex)
			{
				ID3D12ShaderReflectionVariable* pShaderReflectionVariable = pShaderReflectionConstantBuffer->GetVariableByIndex(variableIndex);

				D3D12_SHADER_VARIABLE_DESC shaderVariableDesc;
				pShaderReflectionVariable->GetDesc(&shaderVariableDesc); //그 변수의 이름,사이즈정보가있고

				//그변수의 타입정보 
				ID3D12ShaderReflectionType* pVariableType = pShaderReflectionVariable->GetType();

				//ID3D12ShaderReflectionType* pBaseVariableType = pVariableType->GetBaseClass();
				D3D12_SHADER_TYPE_DESC shaderTypeDesc;
				pVariableType->GetDesc(&shaderTypeDesc);

				variableVector[variableIndex].mName = Utility::ConvertToWString(shaderVariableDesc.Name, true);
				variableVector[variableIndex].mOffsetInShaderResource = shaderVariableDesc.StartOffset;
				variableVector[variableIndex].mSize = shaderVariableDesc.Size;
				variableVector[variableIndex].mElementNum = shaderTypeDesc.Elements;
				variableVector[variableIndex].mClass = shaderTypeDesc.Class;
				variableVector[variableIndex].mTypeName = Utility::ConvertToWString(shaderTypeDesc.Name, true);


				//변수가 구조체인경우

				//구조체를서술하는 객체를생성한다.

				//구조체의 크기, 정렬크기를 계산해준다(변수들의타입을 보고)
				//이미있는구조체라면 무시
				if (variableVector[variableIndex].mClass == D3D_SVC_STRUCT && shaderResourceCosntantBuffer->GetShaderResourceStructDesc(variableVector[variableIndex].mTypeName) == nullptr)
				{

					ShaderResourceStructTypeDesc* pShadarResourceStructTypeDesc = new ShaderResourceStructTypeDesc;
					//구조체기본설정
					pShadarResourceStructTypeDesc->mName = variableVector[variableIndex].mTypeName;
					pShadarResourceStructTypeDesc->mVariableVector.resize(shaderTypeDesc.Members);

					int structSize = 0;
					//구조체내부변수들
					for (int memberIndex = 0; memberIndex < shaderTypeDesc.Members; ++memberIndex)
					{

						ID3D12ShaderReflectionType* pMemberType = pVariableType->GetMemberTypeByIndex(memberIndex);

						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mName = Utility::ConvertToWString(pVariableType->GetMemberTypeName(memberIndex), true);

						//D3D12_SHADER_VARIABLE_DESC

						D3D12_SHADER_TYPE_DESC memberTypeDesc;
						pMemberType->GetDesc(&memberTypeDesc);


						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mOffsetInShaderResource =
							memberTypeDesc.Offset;
						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mClass = memberTypeDesc.Class;
						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mElementNum = memberTypeDesc.Elements;
						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mTypeName = Utility::ConvertToWString(memberTypeDesc.Name, true);



						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mSize = 0;
						pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mSize = GetDefaultVariableSize(memberTypeDesc);


					}

					pShadarResourceStructTypeDesc->mSize = pShadarResourceStructTypeDesc->mVariableVector[shaderTypeDesc.Members - 1].mOffsetInShaderResource + pShadarResourceStructTypeDesc->mVariableVector[shaderTypeDesc.Members - 1].mSize;

					pShadarResourceStructTypeDesc->mAlignedSize = (variableVector[variableIndex].mSize - pShadarResourceStructTypeDesc->mSize) / (variableVector[variableIndex].mElementNum - 1);

					//int lastMemeberIndex = shaderTypeDesc.Members - 1;
					////마지막멤버의 offset+마지막멤버의 크기 = 구조체의 총크기
					//pShadarResourceStructTypeDesc->mSize = pShadarResourceStructTypeDesc->mVariableVector[lastMemeberIndex].mOffsetInShaderResource + pShadarResourceStructTypeDesc->mVariableVector[lastMemeberIndex].mSize;





					shaderResourceCosntantBuffer->AddShaderResourceStructDesc(pShadarResourceStructTypeDesc);

				}

			}




			//변수들의타입,안에들어있는 변수들의 종류,등등이올바른지 점검한다.


			shaderResourceCosntantBuffer->Initialize(std::move(variableVector), bufferSize);

			shaderResourceCosntantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(bufferSize));

			shaderResourceCosntantBuffer->SetShaderResourceVisibility(shaderResourceVisibility);
			renderPass->AddShaderResourece(shaderResourceCosntantBuffer);

		}
		break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:

		{
			EShaderResourceTextureType eShaderResourceTextureType;
			switch (shaderInputBindDesc.Dimension)
			{
			case D3D_SRV_DIMENSION_TEXTURE2D:
				eShaderResourceTextureType = EShaderResourceTextureType::eT2D;
				break;
			}
			ShaderResourceTexture* shaderResourceTexture = new ShaderResourceTexture(
				shaderResourceName, registerIndex, registerSpace, eShaderResourceTextureType);


			shaderResourceTexture->SetShaderResourceVisibility(shaderResourceVisibility);
			shaderResourceTexture->SetObjectOrPassFlag(true);
			renderPass->AddShaderResourece(shaderResourceTexture);

		}

		break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:


		{

			ViewIndex viewIndex = InvalidViewIndex;

			viewIndex =	SamplerManager::GetSamplerViewIndex(shaderResourceName);
			if (viewIndex == InvalidViewIndex)
			{
				MessageBox(nullptr, L"샘플러이름이 잘못됨", L"error", 0);
				assert(0);
			}

			/*if (shaderResourceName == L"gSamMinMagMipLinear")
			{
				viewIndex= mSamplerManager.GetSamplerViewIndex(shaderResourceName);

			}
			else
			{
				MessageBox(nullptr, L"샘플러이름이 잘못됨", L"error", 0);
				assert(0);
			}*/

			ShaderResourceSampler* shaderResourceSampler = new ShaderResourceSampler(shaderResourceName, registerIndex, registerSpace, viewIndex);

			shaderResourceSampler->SetShaderResourceVisibility(shaderResourceVisibility);
			shaderResourceSampler->SetObjectOrPassFlag(false);
			renderPass->AddShaderResourece(shaderResourceSampler);

		}
		break;
		}


	}
}

void Quad::EffectLoader::SetObjectOrPassFlag(ShaderResourceConstantBuffer* shaderResourceConstantBuffer)
{
	const std::wstring& name = shaderResourceConstantBuffer->GetName();
	if (name == L"ObjectConstant" || name == L"MaterialConstant")
	{
		shaderResourceConstantBuffer->SetObjectOrPassFlag(true);
	}
	else if (name == L"PassConstant" || name == L"LightConstant")
	{
		shaderResourceConstantBuffer->SetObjectOrPassFlag(false);
	}

}

UINT Quad::EffectLoader::GetDefaultVariableSize(const D3D12_SHADER_TYPE_DESC& variableTypeDesc)
{
	D3D_SHADER_VARIABLE_CLASS variableClass = variableTypeDesc.Class;
	int variableElementSize = GetDefaultVariableElementSize(variableTypeDesc.Type);


	int row = variableTypeDesc.Rows;
	int column = variableTypeDesc.Columns;

	int elementNum = variableTypeDesc.Elements;

	switch (variableClass)
	{

	case D3D_SVC_SCALAR:
		if (elementNum == 0)
		{
			return variableElementSize;
		}
		else
		{
			return 16 * elementNum;
		}

	case D3D_SVC_VECTOR:
		if (elementNum == 0)
		{
			return variableElementSize * column;
		}
		else
		{
			return 16 * column * elementNum;
		}
	case D3D_SVC_MATRIX_ROWS:
		if (elementNum == 0)
		{
			return variableElementSize * column * row;
		}
		else
		{
			return 16 * 4 * row * elementNum;
		}
	case D3D_SVC_MATRIX_COLUMNS:
		if (elementNum == 0)
		{
			return variableElementSize * column * row;
		}
		else
		{
			return 16 * 4 * column * elementNum;
		}
	}






	return 0;
}

UINT Quad::EffectLoader::GetDefaultVariableElementSize(D3D_SHADER_VARIABLE_TYPE type)
{
	switch (type)
	{
	case   D3D_SVT_BOOL:

	case D3D_SVT_INT:
	case D3D_SVT_FLOAT:
	case D3D_SVT_UINT:
		return 4;
	case D3D_SVT_UINT8:
		return 1;
	case D3D_SVT_DOUBLE:
		return 8;
	}
	return 0;
}


bool Quad::EffectLoader::CreateRootSignature(Quad::RenderPassTwo* renderPass)
{
	const std::vector<ShaderResource*>& shaderResourceVector = renderPass->GetShaderResourceVector();
	std::vector<D3D12_ROOT_PARAMETER> mRootParameterVector(shaderResourceVector.size());
	std::vector< D3D12_ROOT_DESCRIPTOR_TABLE> mRootDescriptorTableVector(shaderResourceVector.size());
	std::vector< D3D12_DESCRIPTOR_RANGE > mRootDescriptorRangeVector(shaderResourceVector.size());

	for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
	{
		ShaderResource* currentShaderResource = shaderResourceVector[shaderResourceIndex];
		D3D12_ROOT_PARAMETER& currentRootParameter = mRootParameterVector[shaderResourceIndex];
		D3D12_DESCRIPTOR_RANGE& currentDescriptorRange = mRootDescriptorRangeVector[shaderResourceIndex];


		currentRootParameter.ShaderVisibility = ConvertD3DShaderVisibility(currentShaderResource->GetShaderResourceVisibility());
		currentRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//일단 고정


		currentDescriptorRange.BaseShaderRegister = currentShaderResource->GetRegisterIndex();
		currentDescriptorRange.NumDescriptors = 1;
		currentDescriptorRange.OffsetInDescriptorsFromTableStart = 0;
		currentDescriptorRange.RangeType = ConvertD3DShaderRangeType(currentShaderResource);
		currentDescriptorRange.RegisterSpace = currentShaderResource->GetRegisterSpace();

		currentRootParameter.DescriptorTable.NumDescriptorRanges = 1;
		currentRootParameter.DescriptorTable.pDescriptorRanges = &currentDescriptorRange;

	}


	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.NumParameters = mRootParameterVector.size();
	rootSignatureDesc.pParameters = mRootParameterVector.data();
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//루트시그니처 직렬화
	ID3DBlob* serializedRootSignatureBlob;
	ID3DBlob* errorBlob;
	HRESULT ret = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSignatureBlob, &errorBlob);

	if (errorBlob != nullptr)
	{
		MessageBox(nullptr, L"루트시그니처 직렬화 에러", L"error", 0);
		assert(0);
	}
	ThrowIfFailed(ret);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	ThrowIfFailed(mDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));

	renderPass->SetRootSignature(rootSignature);
	return true;


}

bool Quad::EffectLoader::CreateGraphicsPipeline(const RawRenderPassDataTwo& rawRenderPassTwoData,
	RenderPassTwo* renderPass, const std::vector<Shader*>& shaderVector,
	Shader* vertexShader)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc;


	ZeroMemory(&graphicsPipelineStateDesc, sizeof(graphicsPipelineStateDesc));

	graphicsPipelineStateDesc.NodeMask = 0;
	graphicsPipelineStateDesc.pRootSignature = renderPass->GetRootSignature().Get();
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//effect파일의 옵션
	renderPass->SetPrimitiveToplogyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
	graphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	graphicsPipelineStateDesc.SampleMask = 0xFFFFFFFF;		//이거 0하면 렌더링안됨
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
	//graphicsPipelineStateDesc.RasterizerState.FillMode =
	//graphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	graphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
//	graphicsPipelineStateDesc.RasterizerState.FrontCounterClockwise = D3D12_CULL_MODE_BACK;
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;

	SetGraphicsPipelineCfg(renderPass, graphicsPipelineStateDesc, rawRenderPassTwoData.mGraphicsPipelineCfgVector);

	//input layout 
	Microsoft::WRL::ComPtr< ID3D12ShaderReflection> shaderReflectionInterface;
	D3DReflect(vertexShader->GetShaderByteCode().Get()->GetBufferPointer(), vertexShader->GetShaderByteCode().Get()->GetBufferSize(), IID_PPV_ARGS(shaderReflectionInterface.GetAddressOf()));

	D3D12_SHADER_DESC shaderDesc;
	shaderReflectionInterface->GetDesc(&shaderDesc);
	UINT inputParameterNums = shaderDesc.InputParameters;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector;
	inputElementDescVector.resize(inputParameterNums);
	for (UINT inputParameterIndex = 0; inputParameterIndex < inputParameterNums; ++inputParameterIndex)
	{
		D3D12_SIGNATURE_PARAMETER_DESC pSignatureParameterDesc;
		//디버그 출력할것
		ThrowIfFailed(shaderReflectionInterface->GetInputParameterDesc(inputParameterIndex, &pSignatureParameterDesc));

		inputElementDescVector[inputParameterIndex].SemanticName = pSignatureParameterDesc.SemanticName;
		inputElementDescVector[inputParameterIndex].SemanticIndex = pSignatureParameterDesc.SemanticIndex;
		inputElementDescVector[inputParameterIndex].InputSlot = 0;
		inputElementDescVector[inputParameterIndex].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElementDescVector[inputParameterIndex].InstanceDataStepRate = 0;
		inputElementDescVector[inputParameterIndex].Format = GetInputElementFormat(pSignatureParameterDesc.SemanticName,
			pSignatureParameterDesc.ComponentType, pSignatureParameterDesc.Mask);

		inputElementDescVector[inputParameterIndex].AlignedByteOffset = GetInputElementOffset(pSignatureParameterDesc.SemanticName);
	}

	graphicsPipelineStateDesc.InputLayout.NumElements = inputElementDescVector.size();//이것도 reflection;
	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescVector.data();//이것도 reflection;






	//shader

	for (int i = 0; i < shaderVector.size(); ++i)
	{

		Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = shaderVector[i]->GetShaderByteCode();

		switch (shaderVector[i]->GetShaderType())
		{
		case EShaderType::eVS:
			graphicsPipelineStateDesc.VS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

			graphicsPipelineStateDesc.VS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
			break;

		case EShaderType::ePS:
			graphicsPipelineStateDesc.PS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

			graphicsPipelineStateDesc.PS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();

			break;

		case EShaderType::eGS:
			graphicsPipelineStateDesc.GS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

			graphicsPipelineStateDesc.GS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
			break;
		case EShaderType::eDS:

			graphicsPipelineStateDesc.DS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

			graphicsPipelineStateDesc.DS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
			break;
		case EShaderType::eHS:
			graphicsPipelineStateDesc.HS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

			graphicsPipelineStateDesc.HS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
			break;
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
	ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState.GetAddressOf())));


	////Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//{
	//	debugController->EnableDebugLayer();
	//}

	renderPass->SetPipelineState(graphicsPipelineState);

	return true;
}

void Quad::EffectLoader::SetGraphicsPipelineCfg(RenderPassTwo* renderPass,
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, const std::vector<std::pair<std::string, std::string>>& graphicsPipelineCfgVector)
{

	for (int i = 0; i < graphicsPipelineCfgVector.size(); ++i)
	{
		const std::string& key = graphicsPipelineCfgVector[i].first;
		const std::string& value = graphicsPipelineCfgVector[i].second;

		if (key == "STENCILENABLE")
		{
			graphicsPipelineStateDesc.DepthStencilState.StencilEnable = Utility::ConvertBoolValue(value);
			//	graphicsPipelineStateDesc.DepthStencilState.StencilEnable = Utility::ConvertBoolValue(value);
		}
		else if (key == "STENCILFAILOP")
		{
			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = ConvertStencilOP(value, renderPass);
			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp;

		}
		else if (key == "STENCILDEPTHFAILOP")
		{
			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = ConvertStencilOP(value, renderPass);
			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp;
		}
		else if (key == "STENCILPASSOP")
		{
			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = ConvertStencilOP(value, renderPass);
			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp;
		}
		else if (key == "STENCILFUNC")
		{
			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = ConvertComparisionFunEnum(value);
			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFunc = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc;
		}
		else if (key == "PRIMITIVETOPOLOGYTYPE")
		{

			graphicsPipelineStateDesc.PrimitiveTopologyType = ConvertPrimitiveTopologyType(value);
			renderPass->SetPrimitiveToplogyType(ConvertCommandListPrimitiveTopologyType(value));

		}
		else if (key == "FILLMODE")
		{
			if (value == "WIREFRAME")
			{
				graphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
			}
		}
		else if (key == "CULLMODE")
		{

			graphicsPipelineStateDesc.RasterizerState.CullMode = ConvertCullMode(value);

		}
		else if (key == "DEPTHFUNC")
		{
			graphicsPipelineStateDesc.DepthStencilState.DepthFunc = ConvertComparisonFuncEnum(value);
		}
		else if (key == "DEPTHENABLE")
		{
			graphicsPipelineStateDesc.DepthStencilState.DepthEnable = Utility::ConvertBoolValue(value);
		}
		else if (key == "RENDERPASSID")
		{
			renderPass->SetRenderPassID(ConvertRenderPassID(value));
		}
		else if (key == "BLENDOP")
		{
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = ConvertD3DBlendOpEnum(value);
		}
		else if (key == "BLENDENABLE")
		{
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = Utility::ConvertBoolValue(value);
		}
		else if (key == "SRCBLEND")
		{
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = ConvertBlendFactorEnum(value);
		}
		else if (key == "DESTBLEND")
		{
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = ConvertBlendFactorEnum(value);
		}



	}









}

D3D12_SHADER_VISIBILITY Quad::EffectLoader::ConvertD3DShaderVisibility(EShaderResourceVisibility visibility)
{
	switch (visibility)
	{

	case EShaderResourceVisibility::eAll:
		return D3D12_SHADER_VISIBILITY_ALL;

	case EShaderResourceVisibility::ePixel:

		return D3D12_SHADER_VISIBILITY_PIXEL;

	case EShaderResourceVisibility::eVertex:

		return D3D12_SHADER_VISIBILITY_VERTEX;

	case EShaderResourceVisibility::eDomain:
		return D3D12_SHADER_VISIBILITY_DOMAIN;

	case EShaderResourceVisibility::eGeometry:
		return D3D12_SHADER_VISIBILITY_GEOMETRY;

	case EShaderResourceVisibility::eHull:

		return D3D12_SHADER_VISIBILITY_HULL;


	case EShaderResourceVisibility::eMesh:

		return D3D12_SHADER_VISIBILITY_MESH;

	case EShaderResourceVisibility::eAmplification:

		return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
	}


}

D3D12_DESCRIPTOR_RANGE_TYPE Quad::EffectLoader::ConvertD3DShaderRangeType(ShaderResource* shaderResource)
{
	switch (shaderResource->GetShaderResourceType())
	{
	case EShaderResourceType::eConstantBuffer:

		return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	case EShaderResourceType::eTexture:

		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	case EShaderResourceType::eSampler:

		return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	}


}

UINT Quad::EffectLoader::GetInputElementOffset(const std::string& sementicName)
{

	if (sementicName == "POSITION")
	{
		return 0;
	}
	else if (sementicName == "NORMAL")
	{
		return 12;
	}
	else if (sementicName == "TEX")
	{
		return 24;
	}
	else if (sementicName == "BONEWEIGHTONE")
	{
		return 24 + 8;
	}
	else if (sementicName == "BONEWEIGHTTWO")
	{
		return 48;
	}
	else if (sementicName == "BONEINDEXONE")
	{
		return 64;
	}
	else if (sementicName == "BONEINDEXTWO")
	{
		return 80;
	}


	return 0;
}

DXGI_FORMAT Quad::EffectLoader::GetInputElementFormat(const std::string& sementicName, D3D_REGISTER_COMPONENT_TYPE componentType, BYTE mask)
{

	if (sementicName == "POSITION")
	{
		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0111))
		{
			//디버그출력 

			OutputDebugString(L"input layout의 POSITION 시맨틱에 해당하는 속성의 타입이틀리다.\n");
			assert(0);
		}

		return DXGI_FORMAT_R32G32B32_FLOAT;
	}
	else if (sementicName == "NORMAL")
	{
		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0111))
		{
			//디버그출력 

			OutputDebugString(L"input layout의 NORMAL 시맨틱에 해당하는 속성의 타입이틀리다.\n");
			assert(0);
		}



		return DXGI_FORMAT_R32G32B32_FLOAT;
	}
	else if (sementicName == "TEX")
	{
		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0011))
		{
			//디버그출력 

			OutputDebugString(L"input layout의 TEX 시맨틱에 해당하는 속성의 타입이틀리다.\n");
			assert(0);
		}



		return DXGI_FORMAT_R32G32_FLOAT;
	}
	else if (sementicName == "BONEWEIGHTONE" || sementicName == "BONEWEIGHTTWO")
	{
		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b1111))
		{
			//디버그출력 

			OutputDebugString(L"input layout의 BONEWEIGHTONE 또는 BONEWEIGHTTWO 시맨틱에 해당하는 속성의 타입이틀리다.\n");
			assert(0);
		}
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	else if (sementicName == "BONEINDEXONE" || sementicName == "BONEINDEXTWO")
	{
		if (componentType != D3D_REGISTER_COMPONENT_SINT32 || (mask != 0b1111))
		{
			//디버그출력 

			OutputDebugString(L"input layout의 BONEINDEXONE 또는 BONEINDEXTWO 시맨틱에 해당하는 속성의 타입이틀리다.\n");
			assert(0);
		}
		return DXGI_FORMAT_R32G32B32A32_SINT;


	}

}



D3D12_STENCIL_OP Quad::EffectLoader::ConvertStencilOP(const std::string& str, RenderPassTwo* renderPass) const
{
	if (str == "KEEP")
	{
		return D3D12_STENCIL_OP_KEEP;
	}
	else
	{
		renderPass->SetStencilWriteFlag(true);

		if (str == "ZERO")
		{
			return D3D12_STENCIL_OP_ZERO;
		}
		else if (str == "REPLACE")
		{
			return D3D12_STENCIL_OP_REPLACE;
		}
		else if (str == "INCR_SAT")
		{
			return D3D12_STENCIL_OP_INCR_SAT;
		}
		else if (str == "DESCR_SAT")
		{
			return D3D12_STENCIL_OP_DECR_SAT;
		}
		else if (str == "INVERT")
		{
			return D3D12_STENCIL_OP_INVERT;

		}
		else if (str == "INCR")
		{
			return D3D12_STENCIL_OP_INCR;
		}
		else if (str == "DECR")
		{
			return D3D12_STENCIL_OP_DECR;
		}
		else
		{
			//디버그 출력
			return D3D12_STENCIL_OP_KEEP;
		}
	}
}

D3D12_COMPARISON_FUNC Quad::EffectLoader::ConvertComparisionFunEnum(const std::string& str) const
{
	if (str == "NEVER")
	{
		return D3D12_COMPARISON_FUNC_NEVER;
	}
	else if (str == "LESS")
	{
		return D3D12_COMPARISON_FUNC_LESS;
	}
	else if (str == "EQUAL")
	{
		return D3D12_COMPARISON_FUNC_EQUAL;
	}
	else if (str =="LESS_EQUAL")
	{
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}
	else if (str == "GREATER")
	{
		return D3D12_COMPARISON_FUNC_GREATER;
	}
	else if (str == "NOT_EQUAL")
	{
		return D3D12_COMPARISON_FUNC_GREATER;
	}
	else if (str == "GREATER_EQUAL")
	{
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	}
	else if (str == "ALWAYS")
	{
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}
	else
	{
		//디버그 출력
		return D3D12_COMPARISON_FUNC_LESS;
	}

}

D3D12_PRIMITIVE_TOPOLOGY_TYPE Quad::EffectLoader::ConvertPrimitiveTopologyType(const std::string& str) const
{
	if (str == "POINTLIST")
	{
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	}
	else if (str == "LINELIST" || str == "LINESTRIP")
	{
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	else if (str == "TRIANGLELIST" || str == "TRIANGLESTRIP")
	{
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	else if (str =="PATCH")//패치는 다시고려할것
	{
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	}
	else
	{
		//디버그출력
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}

}

D3D_PRIMITIVE_TOPOLOGY Quad::EffectLoader::ConvertCommandListPrimitiveTopologyType(const std::string& str) const
{

	if (str == "POINTLIST")
	{
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	}
	else if (str == "LINELIST")
	{
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	}
	else if (str =="LINESTRIP")
	{
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	}
	else if (str == "TRIANGLESTRIP")
	{
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
	else if (str == "TRIANGLELIST")
	{
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
	else if (str == "PATCH")//패치는 다시고려할것
	{
		//return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	}
	else
	{
		//디버그출력
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}





D3D12_BLEND Quad::EffectLoader::ConvertBlendFactorEnum(const std:: string& str)
{
	//default
	D3D12_BLEND blendEnum = D3D12_BLEND_ZERO;

	std::unordered_map<std::string, D3D12_BLEND>::iterator it = mBlendFactorEnumUnMap.find(str);
	if (it != mBlendFactorEnumUnMap.end())
		blendEnum = it->second;
	else
	{
		MessageBox(nullptr, L"blendEnum is not correct", L"error", 0);
	}

	return blendEnum;
}

D3D12_BLEND_OP Quad::EffectLoader::ConvertBlendOpEnum(const std::string& str)
{
	//default
	D3D12_BLEND_OP blendOp = D3D12_BLEND_OP_ADD;

	std::unordered_map<std::string, D3D12_BLEND_OP>::iterator it = mBlendOpEnumUnMap.find(str);
	if (it != mBlendOpEnumUnMap.end())
	{
		blendOp = it->second;
	}
	else
	{
		MessageBox(nullptr, L"Blend Op is not correct", L"error", 0);
	}

	return blendOp;
}

D3D12_STENCIL_OP Quad::EffectLoader::ConvertStencilOpEnum(const std::string& str)
{

	std::unordered_map<std::string, D3D12_STENCIL_OP>::iterator it = mStencilOpEnumUnMap.find(str);
	if (it == mStencilOpEnumUnMap.end())
	{
		MessageBox(nullptr, L"StencilOp is not correct ", L"error", 0);
		return D3D12_STENCIL_OP_KEEP;
	}

	return it->second;

}

D3D12_COMPARISON_FUNC Quad::EffectLoader::ConvertComparisonFuncEnum(const std::string& str)
{
	std::unordered_map<std::string, D3D12_COMPARISON_FUNC>::iterator it = mComparisonFuncEnumUnMap.find(str);

	if (it == mComparisonFuncEnumUnMap.end())
	{
		MessageBox(nullptr, L"ComparisonFunc is not correct ", L"error ", 0);
		return D3D12_COMPARISON_FUNC_LESS;
	}
	return it->second;
}

D3D12_SRV_DIMENSION Quad::EffectLoader::ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension)
{

	switch (dimension)
	{
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		return D3D12_SRV_DIMENSION_TEXTURE2D;

	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
		return D3D12_SRV_DIMENSION_BUFFER;

	}
	MessageBox(nullptr, L"srv dimension변환에서 처리미룬것이 있다", L"error", 0);
	assert(-1);

}

//void Quad::EffectLoader::FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource)
//{
//	oSrv.Texture2D.MipLevels = resource->GetDesc().MipLevels;
//	oSrv.Texture2D.MostDetailedMip = 0;
//	oSrv.Texture2D.ResourceMinLODClamp = 0.0;
//	oSrv.Texture2D.PlaneSlice = 0;
//
//}


D3D12_CULL_MODE Quad::EffectLoader::ConvertCullMode(const std::string& str)
{
	if (str == "NONE")
		return D3D12_CULL_MODE_NONE;
	else if (str == "FRONT")
		return D3D12_CULL_MODE_FRONT;
	else if (str == "BACK")
		return D3D12_CULL_MODE_BACK;

}

Quad::ERenderPassID Quad::EffectLoader::ConvertRenderPassID(const std::string& str) const
{

	if (str == "GIZMO")
	{
		return ERenderPassID::eGizmo;
	}
	else if (str == "TRANSPARENT")
	{
		return ERenderPassID::eTransparent;
	}
	else if (str == "OPAQUE")
	{
		return ERenderPassID::eOpaque;
	}
	else if (str == "SHADOWMAP")
	{
		return ERenderPassID::eShadowMap;
	}
	else
	{
		return ERenderPassID::eOpaque;
	}

}

D3D12_BLEND_OP Quad::EffectLoader::ConvertD3DBlendOpEnum(const std::string& str)
{

	if (str == "ADD")
	{
		return D3D12_BLEND_OP_ADD;
	}
	else if (str == "SUBTRACT")
	{
		return D3D12_BLEND_OP_SUBTRACT;
	}
	else if (str == "REV_SUBTRACT")
	{
		return D3D12_BLEND_OP_REV_SUBTRACT;
	}
	else if (str == "MIN")
	{
		return D3D12_BLEND_OP_MIN;
	}
	else if (str == "MAX")
	{
		return D3D12_BLEND_OP_MAX;
	}
	else
	{
		MessageBox(0, L"BlendOp가 정확하지않다.", L"error", 0);
		assert(-1);
	}


	
}

void Quad::EffectLoader::InitBlendEnumUnMap()
{
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_ZERO",D3D12_BLEND_ZERO });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_ONE",D3D12_BLEND_ONE });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_SRC_COLOR",D3D12_BLEND_SRC_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_SRC_COLOR",D3D12_BLEND_INV_SRC_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_SRC_ALPHA",D3D12_BLEND_SRC_ALPHA });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_SRC_ALPHA",D3D12_BLEND_INV_SRC_ALPHA });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_DEST_ALPHA",D3D12_BLEND_DEST_ALPHA });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_DEST_ALPHA",D3D12_BLEND_INV_DEST_ALPHA });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_DEST_COLOR",D3D12_BLEND_DEST_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_DEST_COLOR",D3D12_BLEND_INV_DEST_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_SRC_ALPHA_SAT",D3D12_BLEND_SRC_ALPHA_SAT });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_BLEND_FACTOR",D3D12_BLEND_BLEND_FACTOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_BLEND_FACTOR",D3D12_BLEND_INV_BLEND_FACTOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_SRC1_COLOR",D3D12_BLEND_SRC1_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_SRC1_COLOR",D3D12_BLEND_INV_SRC1_COLOR });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_SRC1_ALPHA",D3D12_BLEND_SRC1_ALPHA });
	mBlendFactorEnumUnMap.insert({ "D3D12_BLEND_INV_SRC1_ALPHA",D3D12_BLEND_INV_SRC1_ALPHA });
	//	mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_ALPHA_FACTOR",D3D12_BLEND_ALPHA_FACTOR });
	//	mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_ALPHA_FACTOR",D3D12_BLEND_INV_ALPHA_FACTOR });


	mBlendOpEnumUnMap.insert({ "D3D12_BLEND_OP_ADD",D3D12_BLEND_OP_ADD });
	mBlendOpEnumUnMap.insert({ "D3D12_BLEND_OP_SUBTRACT",D3D12_BLEND_OP_SUBTRACT });
	mBlendOpEnumUnMap.insert({ "D3D12_BLEND_OP_REV_SUBTRACT",D3D12_BLEND_OP_REV_SUBTRACT });
	mBlendOpEnumUnMap.insert({ "D3D12_BLEND_OP_MIN",D3D12_BLEND_OP_MIN });
	mBlendOpEnumUnMap.insert({ "D3D12_BLEND_OP_MAX",D3D12_BLEND_OP_MAX });

}

void Quad::EffectLoader::InitStencilEnumUnMap()
{
	mStencilOpEnumUnMap.insert({ "KEEP",D3D12_STENCIL_OP_KEEP });
	mStencilOpEnumUnMap.insert({ "ZERO",D3D12_STENCIL_OP_ZERO });
	mStencilOpEnumUnMap.insert({ "REPLACE",D3D12_STENCIL_OP_REPLACE });
	mStencilOpEnumUnMap.insert({ "INCR_SAT",D3D12_STENCIL_OP_INCR_SAT });
	mStencilOpEnumUnMap.insert({ "DECR_SAT",D3D12_STENCIL_OP_DECR_SAT });
	mStencilOpEnumUnMap.insert({ "INVERT",D3D12_STENCIL_OP_INVERT });
	mStencilOpEnumUnMap.insert({ "INCR",D3D12_STENCIL_OP_INCR });
	mStencilOpEnumUnMap.insert({ "DECR",D3D12_STENCIL_OP_DECR });


}

void Quad::EffectLoader::InitComparisonFuncEnumUnMap()
{
	mComparisonFuncEnumUnMap.insert({ "NEVER",D3D12_COMPARISON_FUNC_NEVER });
	mComparisonFuncEnumUnMap.insert({ "LESS",D3D12_COMPARISON_FUNC_LESS });
	mComparisonFuncEnumUnMap.insert({ "EQUAL",D3D12_COMPARISON_FUNC_EQUAL });
	mComparisonFuncEnumUnMap.insert({ "LESS_EQUAL",D3D12_COMPARISON_FUNC_LESS_EQUAL });
	mComparisonFuncEnumUnMap.insert({ "GREATER",D3D12_COMPARISON_FUNC_GREATER });
	mComparisonFuncEnumUnMap.insert({ "NOT_EQUAL",D3D12_COMPARISON_FUNC_NOT_EQUAL });
	mComparisonFuncEnumUnMap.insert({ "GREATER_EQUAL",D3D12_COMPARISON_FUNC_GREATER_EQUAL });
	mComparisonFuncEnumUnMap.insert({ "ALWAYS",D3D12_COMPARISON_FUNC_ALWAYS });

}
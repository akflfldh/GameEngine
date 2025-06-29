#include "EffectParser.h"

#include<Parser/JsonParser.h>

#include"EffectKeyDef.h"


static const std::string kVSName = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderNameKey::eVS);
static const std::string kPSName = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderNameKey::ePS);
static const std::string kGSName = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderNameKey::eGS);

static const std::string kFileNameKey = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderKey::eFile);
static const std::string kEntryPointKey = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderKey::eEntryPoint);
static const std::string kVersionKey = Quad::EffectKeyDef::toString(Quad::EffectKeyDef::EShaderKey::eVersion);



Quad::EffectParser::EffectParser()
{
}

Quad::EffectParser::~EffectParser()
{
}

void Quad::EffectParser::ReadStart(const std::string& filePath)
{
	JsonParser::ReadFile(filePath);
	JsonParser::ReadStart();


	size_t passNum = JsonParser::GetObjectNum();


	mEffectNodeVector.clear();
	mEffectNodeVector.resize(passNum);
	
	for (size_t i = 0; i < passNum; ++i)
	{
		JsonParser::DescendIntoObjectOrArray();

		ReadPass(i);

		JsonParser::AscendOutofObjectOrArray();

	}














}

void Quad::EffectParser::GetRawEffectData(RawEffectDataTwo& rawEffectDataTwo)
{


	size_t passNum = mEffectNodeVector.size();

	std::vector<RawRenderPassDataTwo> rawRenderPassDataTwoVector(passNum);

	for (size_t i = 0; i < passNum; ++i)
	{

		RawRenderPassDataTwo& currRawRenderPassData = rawRenderPassDataTwoVector[i];

		//shader 설정
		for (const auto& shaderElement : mEffectNodeVector[i].mShaderObjectVector)
		{
			const std::string shaderNameKey = shaderElement.first;
			const ShaderObject& shaderObject = shaderElement.second;
			ToRawShaderData(shaderNameKey, shaderObject , currRawRenderPassData);
		}
		
		//config 설정
		currRawRenderPassData.mGraphicsPipelineCfgVector =  std::move(mEffectNodeVector[i].mRenderPassSettingVector);
	
	}

	rawEffectDataTwo.mRenderPassDataVector = std::move(rawRenderPassDataTwoVector);


}


void Quad::EffectParser::ReadPass(int passIndex)
{
	size_t tagNum = JsonParser::GetMemeberNum();
	tagNum -= 1;//TYPE은 항상맨앞에 오고 따로 처리함으로 1을 뺀다.

	std::string typeKey = EffectKeyDef::toString(EffectKeyDef::EPassKey::eType);
	std::string typeValue;

	JsonParser::Read(typeKey, typeValue);
	JsonParser::IncrementCurrentIndex();
	
	ReadShader(passIndex);

	ReadConfig(passIndex);


}

void Quad::EffectParser::ReadShader(int passIndex)
{
	const std::string shaderKey = EffectKeyDef::toString(EffectKeyDef::EPassKey::eShader);
	JsonParser::DescendIntoObjectOrArray(shaderKey);

	size_t shaderNum = JsonParser::GetMemeberNum();

	for (size_t i = 0; i < shaderNum; ++i)
	{
		const std::string ShaderName =	 JsonParser::GetCurrMemberKey();
		JsonParser::DescendIntoObjectOrArray(ShaderName);

	
		std::string fileNameValue, entryPointValue, versionValue;

		JsonParser::Read(kFileNameKey, fileNameValue);
		JsonParser::Read(kEntryPointKey, entryPointValue);
		JsonParser::Read(kVersionKey, versionValue);

		mEffectNodeVector[passIndex].mShaderObjectVector[ShaderName].mShaderSettingUnMap[kFileNameKey] = fileNameValue;
		mEffectNodeVector[passIndex].mShaderObjectVector[ShaderName].mShaderSettingUnMap[kEntryPointKey] = entryPointValue;
		mEffectNodeVector[passIndex].mShaderObjectVector[ShaderName].mShaderSettingUnMap[kVersionKey] = versionValue;


		JsonParser::AscendOutofObjectOrArray();

	}

	JsonParser::AscendOutofObjectOrArray();

}

void Quad::EffectParser::ReadConfig(int passIndex)
{
	const std::string configKey = EffectKeyDef::toString(EffectKeyDef::EPassKey::eConfig);
	JsonParser::DescendIntoObjectOrArray(configKey);

	size_t configNum = JsonParser::GetMemeberNum();

	
	if (configNum != 0)
	{
		for (size_t i = 0; i < configNum; ++i)
		{
			std::string key = JsonParser::GetCurrMemberKey();
			std::string value;
			JsonParser::Read(key, value);
			JsonParser::IncrementCurrentIndex();


			mEffectNodeVector[passIndex].mRenderPassSettingVector.push_back({ key,value });
		}
	}


	JsonParser::AscendOutofObjectOrArray();

}



void Quad::EffectParser::ToRawShaderData(const std::string& shaderName, const ShaderObject& shaderObject, RawRenderPassDataTwo& rawRenderPassData)
{
	using shaderSettingUnmap = std::unordered_map<std::string, std::string>;

	shaderSettingUnmap::const_iterator itFileName = shaderObject.mShaderSettingUnMap.find(kFileNameKey);
	if (itFileName == shaderObject.mShaderSettingUnMap.end())
	{	
		OutputDebugString(L"EffectParser에서 실패\n");
		assert(0);
	}

	shaderSettingUnmap::const_iterator itEntryPoint = shaderObject.mShaderSettingUnMap.find(kEntryPointKey);
	if (itEntryPoint == shaderObject.mShaderSettingUnMap.end())
	{
		OutputDebugString(L"EffectParser에서 실패\n");
		assert(0);
	}


	shaderSettingUnmap::const_iterator itVersion = shaderObject.mShaderSettingUnMap.find(kVersionKey);
	if (itVersion == shaderObject.mShaderSettingUnMap.end())
	{
		OutputDebugString(L"EffectParser에서 실패\n");
		assert(0);
	}





	RawShaderDataTwo* rawShaderData = nullptr;
	if (shaderName == kVSName)
	{
		rawShaderData = &rawRenderPassData.mVertexShaderData;
	}
	else if (shaderName == kPSName)
	{
		rawShaderData = &rawRenderPassData.mPixelShaderData;
	}
	else if (shaderName == kGSName)
	{
		rawShaderData = &rawRenderPassData.mGeometryShaderData;
	}



	rawShaderData->mFile = itFileName->second;
	rawShaderData->mEntryPoint = itEntryPoint->second;
	rawShaderData->mVersion = itVersion->second;



}



#pragma once


#include<vector>
#include<unordered_map>
#include<string>
#include<functional>
#include"RawResourceType.h"
#include"CoreDllExport.h"


namespace Quad
{


	class CORE_API_LIB EffectParser
	{
	public:
		EffectParser();
		~EffectParser();

		EffectParser(const EffectParser&) = delete;
		EffectParser & operator=(const EffectParser&) = delete;

		EffectParser(EffectParser&&) = default;
		EffectParser& operator=(EffectParser&&) = default;

		struct ShaderObject
		{
		//	std::vector<std::pair<std::string, std::string>> mShaderSettingVector;
			std::unordered_map<std::string, std::string> mShaderSettingUnMap;

		};

		struct EffectNode
		{
			std::string mType;
			//<shader name, shader setting value> 
			std::unordered_map<std::string, ShaderObject>  mShaderObjectVector;
			//<settingName, value>
			std::vector<std::pair<std::string, std::string >> mRenderPassSettingVector;
		};

		void ReadStart(const std::string& filePath);

		void GetRawEffectData(RawEffectDataTwo& rawEffectDataTwo);
		//void Read();

	private:

		void ReadPass(int passIndex);

		void ReadShader(int passIndex);
		void ReadConfig(int passIndex);


		void ToRawShaderData(const std::string& shdaerName, const ShaderObject& shaderObject, RawRenderPassDataTwo& rawRenderPassData);

	private:
		std::vector<EffectNode> mEffectNodeVector;
		std::unordered_map<std::string, std::function<void(void)>> mShaderObjectReadFunctionTable;
			





	};


}
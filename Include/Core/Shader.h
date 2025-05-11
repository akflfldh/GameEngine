#pragma once



#include"Predeclare.h"
#include"header.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	enum class EShaderType
	{
		eVS=0,
        ePS,
		eGS,
	    eDS,
        eHS
	};


	class CORE_API_LIB Shader
	{
	public:
		Shader()=default;
		~Shader() = default;

		bool operator==(const Shader& shader);
		void Initialize(const std::string& file, const std::string& entryPoint, const std::string& version, const std::string& shaderMeshType,
			Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode,EShaderType shaderType);
		void Initialize(std::string&& file, std::string&& entryPoint, std::string&& version, std::string&& shaderMeshType,
			Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode, EShaderType shaderType);

		Microsoft::WRL::ComPtr<ID3DBlob> GetShaderByteCode()const;

		const std::string& GetFileName()const;
		const std::string& GetEntryPoint()const;
		const std::string& GetVersion() const;
		EShaderType GetShaderType() const;


	private:
		std::string mFile;
		std::string mEntryPoint;
		std::string mVersion;
		std::string mShaderMeshType;
		Microsoft::WRL::ComPtr<ID3DBlob> mShaderByteCode;

		EShaderType mShaderType;

		//shade resource

	};
}


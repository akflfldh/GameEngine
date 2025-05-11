#include "Shader.h"

bool Quad::Shader::operator==(const Shader& shader)
{
	if ((mFile == shader.mFile) && (mEntryPoint == shader.mEntryPoint) && (mVersion == shader.mVersion))
	{
		return true;
	}
	return false;

}

void Quad::Shader::Initialize(const std::string& file, const std::string& entryPoint, const std::string& version, const std::string& shaderMeshType,
	Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode, EShaderType shaderType)
{
	mFile = file;
	mEntryPoint = entryPoint;
	mVersion = version;
	mShaderMeshType = shaderMeshType;
	mShaderByteCode = shaderByteCode;
	mShaderType = shaderType;
}

void Quad::Shader::Initialize(std::string&& file, std::string&& entryPoint, std::string&& version, std::string&& shaderMeshType,
	Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode, EShaderType shaderType)
{
	mFile = std::move(file);
	mEntryPoint = std::move(entryPoint);
	mVersion = std::move(version);
	mShaderMeshType = std::move(shaderMeshType);
	mShaderByteCode = shaderByteCode;
	mShaderType = shaderType;

}

Microsoft::WRL::ComPtr<ID3DBlob> Quad::Shader::GetShaderByteCode() const
{
	return mShaderByteCode;
}

const std::string& Quad::Shader::GetFileName() const
{
	return mFile;
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::string& Quad::Shader::GetEntryPoint() const
{
	return mEntryPoint;
	// TODO: 여기에 return 문을 삽입합니다.
}

const std::string& Quad::Shader::GetVersion() const
{
	return mVersion;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::EShaderType Quad::Shader::GetShaderType() const
{
	return mShaderType;
}



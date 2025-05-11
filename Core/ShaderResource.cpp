#include "Shader/ShaderResource.h"



Quad::ShaderResource::ShaderResource(EShaderResourceType type, const std::wstring& name,
	char registerIndex, char registerSpace)
	:mType(type),mName(name),mRegisterIndex(registerIndex),mRegisterSpace(registerSpace)
{

}

Quad::ShaderResource::~ShaderResource()
{

}

const std::wstring& Quad::ShaderResource::GetName() const
{

	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::EShaderResourceType Quad::ShaderResource::GetShaderResourceType() const
{
	return mType;
}



char Quad::ShaderResource::GetRegisterSpace() const
{
	return mRegisterSpace;
}

char Quad::ShaderResource::GetRegisterIndex() const
{
	return mRegisterIndex;
}

void Quad::ShaderResource::SetShaderResourceVisibility(EShaderResourceVisibility shaderResourceVisibility)
{
	mShaderResourceVisibility = shaderResourceVisibility;
}

Quad::EShaderResourceVisibility Quad::ShaderResource::GetShaderResourceVisibility() const
{
	return mShaderResourceVisibility;
}

void Quad::ShaderResource::SetObjectOrPassFlag(bool flag)
{
	mObjectOrPassFlag = flag;
}

bool Quad::ShaderResource::GetObjectOrPassFlag() const
{
	return mObjectOrPassFlag;
}

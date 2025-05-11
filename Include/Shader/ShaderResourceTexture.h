#pragma once

#include"Shader/ShaderResource.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	enum class  EShaderResourceTextureType
	{
		eT2D = 0,

	};

	class CORE_API_LIB ShaderResourceTexture:public ShaderResource
	{
	public:
		ShaderResourceTexture(const std::wstring& name, char registerIndex, char registerSpace, EShaderResourceTextureType textureType);

	


		~ShaderResourceTexture() = default;
	private:
		EShaderResourceTextureType mShaderResourceTextureType;


	};

}

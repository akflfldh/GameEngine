#include "Shader/ShaderResourceTexture.h"

Quad::ShaderResourceTexture::ShaderResourceTexture(const std::wstring& name, char registerIndex, char registerSpace,
	EShaderResourceTextureType textureType)
	:ShaderResource(EShaderResourceType::eTexture,name,registerIndex,registerSpace), mShaderResourceTextureType(textureType)
{
}

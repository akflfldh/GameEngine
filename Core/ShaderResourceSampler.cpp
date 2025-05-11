#include "Shader/ShaderResourceSampler.h"

Quad::ShaderResourceSampler::ShaderResourceSampler(const std::wstring& name, char registerIndex, char registerSpace,
	ViewIndex viewIndex)
	:ShaderResource(EShaderResourceType::eSampler,name,registerIndex,registerSpace),mViewIndex(viewIndex)
{


}

ViewIndex Quad::ShaderResourceSampler::GetViewIndex() const
{

	return mViewIndex;
}

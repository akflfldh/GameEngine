#pragma once

#include"MaterialType.h"
#include<string>
#include<d3d12.h>
#include"RenderSystemDllMacro.h"
namespace D3DRender
{
	
	struct ShaderResourceReflectInfo
	{
		std::string mName;
		Render::EShaderResourceDimension mResourceDimension;
		Render::EShaderResourceType mResourceType;
		
		int mRegisterIndex;
		int mRegisterSpace;
		
	};
	
	class RENDER_SYSTEM_API D3DShaderReflectSystem
	{
	public:
		static bool Reflect(void* compiledShader, size_t size , std::vector< ShaderResourceReflectInfo> & oReflectShaderResourceVector);

			
	private:

		static Render::EShaderResourceDimension ConvertToRenderShaderResourceDimension(D3D_SRV_DIMENSION shaderDimension);
		static Render::EShaderResourceType ConvertToRenderShaderResourceType(D3D_SHADER_INPUT_TYPE shaderInputType);




	


	private:


		D3DShaderReflectSystem();


	};

}

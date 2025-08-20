#pragma once

#include "RenderSystem/MaterialType.h"
#include "RenderSystem/RenderSystemDllMacro.h"
#include <d3d12.h>
#include <string>

namespace D3DRender
{

struct ShaderResourceReflectInfo
{
    std::string mName;
    Render::EShaderResourceDimension mResourceDimension;
    Render::EShaderResourceType mResourceType;

    int mRegisterIndex;
    int mRegisterSpace;

    size_t mSize; // 바인딩된 리소스의 크기(바이트단위) 일단은 Buffer에서만 유효하다.
};

class RENDER_SYSTEM_API D3DShaderReflectSystem
{
  public:
    static bool Reflect(void *compiledShader, size_t size,
                        std::vector<ShaderResourceReflectInfo> &oReflectShaderResourceVector);

  private:
    static Render::EShaderResourceDimension ConvertToRenderShaderResourceDimension(D3D_SRV_DIMENSION shaderDimension);
    static Render::EShaderResourceType ConvertToRenderShaderResourceType(D3D_SHADER_INPUT_TYPE shaderInputType);

  private:
    D3DShaderReflectSystem();
};

} // namespace D3DRender

#pragma once

#include "RenderSystem/MaterialType.h"
#include "RenderSystem/RenderSystemDllMacro.h"
#include "RenderSystem/RenderType.h"
namespace Render
{

class RENDER_SYSTEM_API IMaterialManager
{
  public:
    static IMaterialManager *GetInstance();
    IMaterialManager();
    virtual ~IMaterialManager() = 0;

    // virtual Render::MaterialID CreateMaterial(const Render::CreationMaterialInfo &creationMaterialInfo) = 0;

    //	bool GetMaterialItem(Render::MaterialID materialID, D3DMaterialItem& oMaterialItem) const;

    static void SetMaterialManagerImpl(IMaterialManager *pImpl);

    // 머터리얼에서 사용하는 리소스정보를 가져온다
    virtual const Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(Render::MaterialID matID) = 0;
    virtual const Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(const char *materialName) const = 0;
    virtual Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(const char *materialName) = 0;

    virtual Render::MaterialID CreateMaterial(const MaterialGenerationInfo &info) = 0;
    virtual Render::MaterialID CreateMaterialDirectly(const MaterialGenerationInfo
                                                          &info /*, uint8_t *pShader,
                              size_t shaderSize*/) = 0;

  protected:
  private:
    static IMaterialManager *mImpl;
};

} // namespace Render

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

    virtual Render::MaterialID CreateComputeMaterial(const ComputeMaterialGenerationInfo &info) = 0;

    /// Master Root Signature가 semantic에 할당한 shader register와 root binding 위치를 조회한다.
    /// 등록되지 않은 semantic이면 nullptr을 반환하며 record의 수명은 MaterialManager가 소유한다.
    virtual const MaterialBindingRecord *FindMasterBindingRecord(EMasterRootBindingSemantic semantic) const = 0;

  protected:
  private:
    static IMaterialManager *mImpl;
};

} // namespace Render

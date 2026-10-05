#pragma once

#include <Core/IInterface.h>
#include <Core/IDrawableOutline.h>
#include <Core/SceneComponent.h>
#include <CoreAsset/AssetPtr.h>

#include <CoreMath/CoreMath.h>
#include <memory>
#include <string>
#include <vector>

#include "SkeletalMeshComponent.generated.h"

namespace CoreAsset
{
class Material;
class SkinningMesh;
struct SkinBinding;
} // namespace CoreAsset

class AnimatorComponent;

namespace Core
{
class SkeletalMeshRenderProxy;
}

/// Scene 안에 SkinningMesh 에셋과 인스턴스별 서브메시 머터리얼 선택을 배치하는 컴포넌트다.
/// 메시별 최종 행렬의 CPU cache와 render proxy는 소유하지만 에셋 자체, Animator의 재생 상태,
/// RenderFrontend의 frame snapshot과 GPU resource는 소유하지 않는다.
class CORE_API_LIB REFLECT_CLASS(EngineClass) SkeletalMeshComponent : public SceneComponent,
                                                                      public Core::IDrawableOutline,
                                                                      public Core::IRenderableComponent
{
    GENERATED_BODY(SkeletalMeshComponent)

  public:
    SkeletalMeshComponent();
    ~SkeletalMeshComponent() override;

    void SetMesh(CoreAsset::SkinningMesh *mesh);
    void SetMesh(const std::string &meshAssetName);
    void SetMesh(CoreAsset::AssetID meshAssetID);

    CoreAsset::SkinningMesh *GetSkinningMesh() const;
    const CoreAsset::SkinBinding *GetSkinBinding() const;

    const std::vector<CoreAsset::AssetPtr> &GetSubMeshMaterialList() const;
    void SetSubMeshMaterial(size_t index, CoreAsset::AssetPtr material);
    void SetSubMeshMaterial(size_t index, CoreAsset::AssetID materialAssetID);
    void SetSubMeshMaterial(size_t index, CoreAsset::Material *material);

    void SetDrawOutline(bool bDraw) override;
    void SetDrawOutline(size_t subMeshIndex, bool bDraw) override;

    void Serialize(Arch &arch) override;

    void OnTransformChanged() override;
    void OnActiveStateChanged(bool state) override;
    void FlushPropertyDirty() override;

    void SetRenderID(uint32_t id) override;
    void UpdateRenderProxy();

  protected:
    void OnOwnerObjectAddedToMap() override;
    void OnOwnerObjectRemovedFromMap() override;
    void EndTick(float deltaTime) override;

    virtual void OnMeshChanged();

    void UpdateAnimationFinalMatrix();
    virtual void OnDestoryRequested() override;

  private:
    AnimatorComponent *GetAnimatorComponent() const;
    void RegisterRenderProxy();
    void UnRegisterRenderProxy();
    void NotifyOutlineToManager();

    // Identity 행렬로 리셋
    void ResetFinalMatrix();

  private:
    REFLECT_PROPERTY()
    CoreAsset::AssetPtr mSkinningMeshPtr;

    /// 공유 mesh 에셋을 변경하지 않고 컴포넌트 인스턴스마다 선택하는 서브메시 머터리얼 목록이다.
    REFLECT_PROPERTY()
    std::vector<CoreAsset::AssetPtr> mSubMeshMaterialList;

    std::vector<bool> mSubMeshDrawOutlineFlagList;

    std::unique_ptr<Core::SkeletalMeshRenderProxy> mRenderProxy;
    bool mRenderProxyRegistered = false;

    /// 현재는 mesh 변경 시 만드는 identity 초기 cache이며, 실제 유효한 skin palette는 Animator 계산 성공 후 채워야
    /// 한다.
    std::vector<CoreMath::Matrix4X4> mFinalMatrixList;

    // Raycast/AABB는 pose에 따라 변하는 skinned bounds 정책이 정해진 뒤 추가해야 한다.
    // Physics/debug collider는 skeletal 전용 collision data와 갱신 정책이 없으므로 현재 책임에 포함하지 않는다.
};

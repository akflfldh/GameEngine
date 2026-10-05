#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <vector>

#include "SkeletalMeshComponentUIReflectPanel.generated.h"

namespace UI
{
class UIText;
}

class Component;
class SkeletalMeshComponent;
class UIFoldoutPanel;
class UIAssetSlotPanel;

/// SkeletalMeshComponent의 인스턴스별 메시·머터리얼 편집 UI다.
/// 대상 컴포넌트나 에셋을 소유하지 않으며, 선택 변경 시 바인딩을 갱신하고 기존 Inspector pool에 반환된다.
/// Transform은 공통 SceneComponent Inspector가 담당하고, Skeleton/clip 재생 상태는 이 패널의 책임이 아니다.
class REFLECT_CLASS(EngineClass) SkeletalMeshComponentUIReflectPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(SkeletalMeshComponentUIReflectPanel)

  public:
    SkeletalMeshComponentUIReflectPanel();
    ~SkeletalMeshComponentUIReflectPanel() override;

    void Build(Component *component);
    void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    void Release() override;

  protected:
    void OnBegin() override;
    void OnTransformChanged(UI::ETransformChangeType type) override;

  private:
    void SetMesh(CoreAsset::AssetID id);
    void SetSubMeshMaterial(CoreAsset::AssetID id, size_t index);
    void RefreshComponentMesh();
    UIAssetSlotPanel *CreateSubMaterialPanel(size_t index);

    SkeletalMeshComponent *mDestMeshComponent = nullptr;
    UIFoldoutPanel *mMeshFoldPanel = nullptr;
    UI::UIImage *mMeshPanel = nullptr;
    UI::UIText *mMeshText = nullptr;
    UI::UIImage *mMaterialPanel = nullptr;
    std::vector<UIAssetSlotPanel *> mSubMaterialPanels;
};

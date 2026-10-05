#pragma once

#include <CoreAsset/AssetType.h>
#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>

#include "AnimatorComponentUIReflectPanel.generated.h"

namespace UI
{
class UIText;
}

class AnimatorComponent;
class Component;
class UIFoldoutPanel;

/// AnimatorComponent의 Skeleton 에셋 참조를 편집하는 Inspector 패널이다.
/// 컴포넌트와 에셋은 소유하지 않으며, 재생 handle과 clip 상태는 런타임 Animator가 관리한다.
/// 패널 pool 재사용 시 이전 컴포넌트와의 바인딩을 해제한다.
class REFLECT_CLASS(EngineClass) AnimatorComponentUIReflectPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(AnimatorComponentUIReflectPanel)

  public:
    AnimatorComponentUIReflectPanel();
    ~AnimatorComponentUIReflectPanel() override;

    void Build(Component *component);
    void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    void Release() override;

  protected:
    void OnBegin() override;
    void OnTransformChanged(UI::ETransformChangeType type) override;

  private:
    void SetSkeleton(CoreAsset::AssetID id);
    void RefreshSkeleton();

    AnimatorComponent *mDestAnimatorComponent = nullptr;
    UIFoldoutPanel *mSkeletonFoldPanel = nullptr;
    UI::UIText *mSkeletonText = nullptr;
};

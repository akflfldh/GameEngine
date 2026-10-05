#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <cstddef>

#include "LightComponentUIReflectPanel.generated.h"

class Component;
class LightComponent;
class UIFoldoutPanel;
class UIBoolPanel;
class UIDropdown;
class UIReflectFloatPanel;
class UIReflectVector3Panel;

/// 조명 타입과 속성을 컴포넌트 setter로 편집하는 전용 Inspector 패널이다.
/// 대상 LightComponent와 렌더 프록시는 소유하지 않으며 Transform은 상위 공통 패널에 맡긴다.
/// 자식 UI는 Canvas가 소유하고 이 패널은 ReflectPanelFactory pool에서 재사용된다.
/// Enabled는 직렬화되지 않는 런타임 상태의 미리보기이며 저장 속성과 구분한다.
class REFLECT_CLASS(EngineClass) LightComponentUIReflectPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(LightComponentUIReflectPanel)
  public:
    LightComponentUIReflectPanel();
    ~LightComponentUIReflectPanel() override;

    void Build(Component *component);
    void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    void Release() override;
    void Update(float deltaTime) override;

  protected:
    void OnBegin() override;
    void OnTransformChanged(UI::ETransformChangeType type) override;

  private:
    void BindFields();
    void CommitEdit();
    void UpdateTypeVisibility();
    size_t GetLightTypeIndex() const;
    void SetLightTypeByIndex(size_t index);

    LightComponent *mDestLightComponent = nullptr;
    UIFoldoutPanel *mLightPanel = nullptr;
    UIBoolPanel *mLightEnabledPanel = nullptr;
    UIDropdown *mLightTypeDropdown = nullptr;
    UIReflectVector3Panel *mStrengthPanel = nullptr;
    UIReflectFloatPanel *mFalloffStartPanel = nullptr;
    UIReflectFloatPanel *mFalloffEndPanel = nullptr;
    UIReflectFloatPanel *mSpotPowerPanel = nullptr;
    // 외부 변경 동기화 시 펼쳐진 드롭다운을 매 프레임 닫지 않도록 마지막 표시 타입을 기억한다.
    size_t mDisplayedLightTypeIndex = 0;
};

#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>

#include "BoxColliderComponentUIReflectPanel.generated.h"

class Component;
class BoxColliderComponent;
class UIFoldoutPanel;
class UIBoolPanel;
class UIDropdown;
class UIReflectFloatPanel;
class UIReflectVector3Panel;

/// BoxCollider의 로컬 형상과 바디 설정을 편집하는 Inspector 패널이다.
/// 대상 컴포넌트/물리 바디는 소유하지 않으며, 변경은 컴포넌트 setter를 통해 반영한다.
/// 위치·회전·스케일은 상위 Inspector의 공통 SceneComponent Transform 패널이 담당한다.
/// 자식 UI는 Canvas가 소유하고 이 패널은 기존 ReflectPanelFactory pool에서 재사용된다.
class REFLECT_CLASS(EngineClass) BoxColliderComponentUIReflectPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(BoxColliderComponentUIReflectPanel)
  public:
    BoxColliderComponentUIReflectPanel();
    ~BoxColliderComponentUIReflectPanel() override;

    void Build(Component *component);
    void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    void Release() override;

  protected:
    void OnBegin() override;
    void OnTransformChanged(UI::ETransformChangeType type) override;

  private:
    void BindFields();
    void CommitEdit();
    size_t GetPhysicsBodyTypeIndex() const;
    void SetPhysicsBodyTypeByIndex(size_t index);

    void OnChangedCollisionChannelID();

    BoxColliderComponent *mDestColliderComponent = nullptr;
    UIFoldoutPanel *mBoxPanel = nullptr;
    UIFoldoutPanel *mPhysicsPanel = nullptr;
    UIReflectVector3Panel *mBoxCenterPanel = nullptr;
    UIReflectVector3Panel *mBoxHalfExtentPanel = nullptr;
    UIBoolPanel *mPhysicsEnabledPanel = nullptr;
    UIDropdown *mPhysicsBodyTypeDropdown = nullptr;
    UIBoolPanel *mPhysicsGravityPanel = nullptr;
    UIReflectFloatPanel *mPhysicsMassPanel = nullptr;
    UIDropdown *mCollisionChannelDropdown = nullptr;
};

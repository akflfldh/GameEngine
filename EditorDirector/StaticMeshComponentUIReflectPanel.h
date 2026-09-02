#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>

#include "StaticMeshComponentUIReflectPanel.generated.h"

namespace UI
{
class UIImage;
class UIVerticalLayoutComponent;
class UIText;
} // namespace UI

namespace CoreAsset
{
class StaticMesh;
}

class StaticMeshComponent;
class Component;
class UIBoolPanel;
class UIDropdown;
class UIReflectFloatPanel;
struct DragPayload;

class REFLECT_CLASS(EngineClass) StaticMeshComponentUIReflectPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(StaticMeshComponentUIReflectPanel)
  public:
    StaticMeshComponentUIReflectPanel();
    ~StaticMeshComponentUIReflectPanel();

    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    virtual void Release() override;

    void Build(Component *com);

  protected:
    virtual void OnBegin() override;

    UI::UIImage *CreateSubMaterialPanel();
    void DropMaterialPayload(const DragPayload &payload, int index);

    void BuildComponentMesh();
    void BuildPhysicsProperties();
    void SetMesh(CoreAsset::AssetID id);

    void SetSubMaterialPanelNameText(UI::UIImage *subMaterialPanel, const std::string &name);
    void RefreshPhysicsProperties();
    size_t GetPhysicsBodyTypeIndex() const;
    void SetPhysicsBodyTypeByIndex(size_t index);

  private:
    StaticMeshComponent *mDestMeshComponent = nullptr;
    UI::UIImage *mMeshPanel = nullptr;
    UI::UIText *mMeshText = nullptr;
    UI::UIImage *mMaterialPanel = nullptr;
    UI::UIImage *mPhysicsPanel = nullptr;
    UIBoolPanel *mPhysicsEnabledPanel = nullptr;
    UIDropdown *mPhysicsBodyTypeDropdown = nullptr;
    UIBoolPanel *mPhysicsGravityPanel = nullptr;
    UIReflectFloatPanel *mPhysicsMassPanel = nullptr;

    std::vector<UI::UIImage *> mSubMaterialPanelList;
    UI::UIVerticalLayoutComponent *mVerticalLayoutCom = nullptr;
};

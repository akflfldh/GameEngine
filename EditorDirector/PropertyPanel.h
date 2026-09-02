#pragma once

#include <EditorDirector/UIScrollBox.h>

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/GlobalOverlayType.h>

#include "PropertyPanel.generated.h"

using OnSelectedEntityCallbackSystem = Core::MultiCallbackSystem<>;

namespace UI
{
class UIImage;
class UIEditBox;
}; // namespace UI

class Object;
class Entity;
class Component;

class UIReflectPanel;
class BaseSelectionManager;

class REFLECT_CLASS(EngineClass) PropertyPanel : public UIScrollBox
{
    GENERATED_BODY(PropertyPanel)
  public:
    PropertyPanel();
    virtual ~PropertyPanel();

    void Initialize(BaseSelectionManager *selectionManager);

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    Entity *GetEntity() const;

    void OnSelectedObject(Object *object);
    void OnSelectedComponent(Component *com);

    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

    // OnSelectedEntityCallbackSystem mOnSelectedObjectCallbackSystem;

  private:
    // UI::UIImage *CreateBackgroundPanel(const char *name, const UI::UIColor &color, int height);

    // std::vector<UI::UIEditBox *> CreateVector3Panel(UI::UIImage *parent, const std::string &tagText,
    //                                                 const std::function<const CoreMath::Vector3 &()> &getter,
    //                                                 const std::function<void(const CoreMath::Vector3 &)> &setter,
    //                                                 int heightIndex);

    // void CreateAssetPanel(UI::UIImage *parent, const std::string &tagText, EDragDropType payloadType,
    //                        int assetIndex = 0);

    //  void OnFinishTransformInput();
    //   void OnStartTransformInput();

  private:
    BaseSelectionManager *mSelectionManager = nullptr;

    Entity *mSelectedEntity;
    UIReflectPanel *mReflectPanel;
    std::vector<UI::UIImage *> mMaterialPanelList;
};

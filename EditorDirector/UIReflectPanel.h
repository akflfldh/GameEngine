#pragma once

#include <CoreMath/CoreMath.h>
#include <UiSystem/UIElement.h>
#include <string>
#include <vector>

#include "UIReflectPanel.generated.h"
namespace UI
{
class UICanvas;
class UIVerticalLayoutComponent;
class UIImageComponent;
class UIImage;
class UIElement;
class UIEditBox;
class UIText;
} // namespace UI

class Object;
class Component;
class SceneComponent;
namespace Quad
{
class ClassInfo;
struct PropertyInfo;
class ReflectionSystem;
} // namespace Quad

class UIReflectSinglePrimitivePanel;
class UIReflectVector3Panel;
class UIReflectBoolPanel;

enum class ETransformTargetType
{
    eObject = 0,
    eComponent
};

class TransformReflectPanel
{
  public:
    TransformReflectPanel();
    ~TransformReflectPanel();

    void Update(float deltaTime);
    void Initialize(UI::UICanvas *canvas, ETransformTargetType targetType);

    void SetTargetObject(Object *object);
    void SetTargetComponent(Component *component);

    void SetParent(UI::UIElement *parent);
    void SetActive(bool state);

    bool GetActive() const;

    void ClearBinding();

  private:
    void BuildPanels(UI::UICanvas *canvas);
    void Rebind();
    SceneComponent *ResolveSceneComponent() const;
    void ResetPanel(UIReflectVector3Panel *panel);

  private:
    ETransformTargetType mTargetType = ETransformTargetType::eComponent;

    UI::UIImage *mBaseBackgroundPanel = nullptr;
    UIReflectVector3Panel *mScalePanel = nullptr;
    UIReflectVector3Panel *mRotationPanel = nullptr;
    UIReflectVector3Panel *mPositionPanel = nullptr;

    Object *mTargetObject = nullptr;
    Component *mTargetComponent = nullptr;
};

class REFLECT_CLASS(EngineClass) UIReflectPanel : public UI::UIElement
{
    GENERATED_BODY(UIReflectPanel)

  public:
    UIReflectPanel();
    virtual ~UIReflectPanel();
    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void SetTargetObject(Object *object);

    void OnSelectedComponent(Component *com);
    void SetTargetComponent(Component *com);

    void Clear();
    void Rebuild();

    // 기본타입이 아니라면 nullptr 리턴
    UI::UIImage *CreateSinglePrimitivePanel(const std::string &tagName, Quad::PropertyInfo *property);

    UIReflectVector3Panel *CreateVector3Panel(const std::string &tagName, Quad::PropertyInfo *property);

    UIReflectBoolPanel *CreateBoolPanel(const std::string &tagName, Quad::PropertyInfo *property);
    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

  private:
    UI::UIImage *CreateBackgroundPanel();

    void BuildObjectProperties(Object *object);
    void BuildObjectComponentReferenceProperties(Object *object);
    void BuildObjectComponentPanels(Object *object);
    void BuildInspectorForRootSceneComponent(Object *object, SceneComponent *rootCom);
    bool BuildEngineComponentClass(Component *com, Quad::ClassInfo *comClassInfo);

    void BuildComponentTransformPanel(SceneComponent *com);
    void BuildComponentPanel(Component *com);
    void BuildComponentProperties(Component *com);

    bool IsRootSceneComponent(Component *com) const;

    bool IsComponentReferenceProperty(Quad::PropertyInfo *propertyInfo) const;

    void BuildReflectProperties(void *targetMemory, Quad::ClassInfo *classInfo, const char *stopClassName);

  private:
    UI::UIImageComponent *mBackgroundImageComponent;
    UI::UIVerticalLayoutComponent *mVerticalLayoutComponent;

    Object *mTargetObject = nullptr;
    Component *mSelectedComponent = nullptr;
    // std::vector<UIReflectSinglePrimitivePanel *> mPrimitivePanelList;
    // std::vector<UIReflectVector3Panel *> mVector3PanelList;
    // std::vector<UIReflectBoolPanel *> mBoolPanelList;

    std::vector<UI::UIElement *> mReflectPanelList;
    Quad::ReflectionSystem *mReflectionSystem = nullptr;

    TransformReflectPanel mTransformReflectPanel;
};

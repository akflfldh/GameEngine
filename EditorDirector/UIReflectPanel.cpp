#include "UIReflectPanel.h"

#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/SceneComponent.h>
#include <EditorDirector/UIReflectBoolPanel.h>
#include <EditorDirector/UIReflectSinglePrimitivePanel.h>
#include <EditorInspectorUtility.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <StaticMeshComponentUIReflectPanel.h>
#include <UIReflectPanelFactory.h>
#include <UIReflectVector3Panel.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

TransformReflectPanel::TransformReflectPanel() {}

TransformReflectPanel::~TransformReflectPanel() {}

void TransformReflectPanel::Update(float deltaTime)
{
    if (!GetActive())
    {
        return;
    }

    if (mScalePanel)
    {
        mScalePanel->RefreshFromSource();
    }

    if (mRotationPanel)
    {
        mRotationPanel->RefreshFromSource();
    }

    if (mPositionPanel)
    {
        mPositionPanel->RefreshFromSource();
    }
}

void TransformReflectPanel::Initialize(UI::UICanvas *canvas, ETransformTargetType targetType)
{
    mTargetType = targetType;

    mBaseBackgroundPanel = canvas->CreateUIElement<UI::UIImage>("BaseBackgroundPanel");
    mBaseBackgroundPanel->SetSize(600, 300);
    mBaseBackgroundPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    BuildPanels(canvas);
    ClearBinding();
}

void TransformReflectPanel::SetTargetObject(Object *object)
{
    mTargetObject = object;

    Rebind();
}

void TransformReflectPanel::SetTargetComponent(Component *component)
{
    mTargetComponent = component;
    Rebind();
}

void TransformReflectPanel::SetParent(UI::UIElement *parent)
{
    if (mBaseBackgroundPanel)
    {
        mBaseBackgroundPanel->SetParent(parent);
    }
}

void TransformReflectPanel::SetActive(bool state)
{
    if (mBaseBackgroundPanel)
    {
        mBaseBackgroundPanel->SetActiveFlag(state);
    }
}

bool TransformReflectPanel::GetActive() const
{
    if (mBaseBackgroundPanel)
    {
        return mBaseBackgroundPanel->GetActiveFlag();
    }

    return false;
}

void TransformReflectPanel::ClearBinding()
{
    mTargetObject = nullptr;
    mTargetComponent = nullptr;

    ResetPanel(mScalePanel);
    ResetPanel(mRotationPanel);
    ResetPanel(mPositionPanel);
}

void TransformReflectPanel::BuildPanels(UI::UICanvas *canvas)
{
    auto createPanel = [this](const char *name, const char *tagText) -> UIReflectVector3Panel *
    {
        UIReflectVector3Panel *panel = mBaseBackgroundPanel->CreateChildUIElement<UIReflectVector3Panel>(name);
        panel->SetColor(0.4f, 0.4f, 0.4f);
        panel->SetWidth(mBaseBackgroundPanel->mTransform.GetSize().r);
        panel->SetTagText(tagText);
        return panel;
    };

    mScalePanel = createPanel("ScalePanel", "Scale");
    mRotationPanel = createPanel("RotationPanel", "Rotation");
    mPositionPanel = createPanel("PositionPanel", "Position");
}

void TransformReflectPanel::Rebind()
{
    SceneComponent *sceneCom = ResolveSceneComponent();

    if (!sceneCom)
    {
        ResetPanel(mScalePanel);
        ResetPanel(mRotationPanel);
        ResetPanel(mPositionPanel);
        return;
    }

    if (mScalePanel)
    {
        mScalePanel->BindVector3([sceneCom]() { return sceneCom->GetScaleLocal(); },
                                 [sceneCom](const CoreMath::Vector3 &value) { sceneCom->SetScaleLocal(value); });

        mScalePanel->SetCommitNotifier([sceneCom]() { Quad::CommitInspectorEdit(sceneCom); });
    }

    if (mRotationPanel)
    {
        mRotationPanel->BindVector3([sceneCom]() { return sceneCom->GetRotationLocal(); },
                                    [sceneCom](const CoreMath::Vector3 &value) { sceneCom->SetRotationLocal(value); });
        mRotationPanel->SetCommitNotifier([sceneCom]() { Quad::CommitInspectorEdit(sceneCom); });
    }

    if (mPositionPanel)
    {
        mPositionPanel->BindVector3([sceneCom]() { return sceneCom->GetPositionLocal(); },
                                    [sceneCom](const CoreMath::Vector3 &value) { sceneCom->SetPositionLocal(value); });
        mPositionPanel->SetCommitNotifier([sceneCom]() { Quad::CommitInspectorEdit(sceneCom); });
    }
}

SceneComponent *TransformReflectPanel::ResolveSceneComponent() const
{
    if (mTargetType == ETransformTargetType::eComponent)
    {
        return dynamic_cast<SceneComponent *>(mTargetComponent);
    }

    Entity *entity = dynamic_cast<Entity *>(mTargetObject);
    if (!entity)
    {
        return nullptr;
    }

    return entity->GetComponent<SceneComponent>();
}

void TransformReflectPanel::ResetPanel(UIReflectVector3Panel *panel)
{
    if (panel)
    {
        panel->Unbind();
        panel->ClearDisplay();
    }
}

UIReflectPanel::UIReflectPanel()
{
    mBackgroundImageComponent = CreateUIComponent<UI::UIImageComponent>("BackgroundImageCom");
    mVerticalLayoutComponent = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    mReflectionSystem = Quad::ReflectionSystem::GetInstance();
}

UIReflectPanel::~UIReflectPanel() {}

void UIReflectPanel::OnBegin()
{
    UI::UIElement::OnBegin();

    mTransformReflectPanel.Initialize(GetDestCanvas(), ETransformTargetType::eComponent);
    mTransformReflectPanel.SetActive(false);
    mTransformReflectPanel.SetParent(this);
}

void UIReflectPanel::Update(float deltaTime)
{
    UI::UIElement::Update(deltaTime);
    if (mTransformReflectPanel.GetActive())
    {
        mTransformReflectPanel.Update(deltaTime);
    }
}

void UIReflectPanel::SetTargetObject(Object *object)
{
    mSelectedComponent = nullptr;

    if (mTargetObject == object)
    {
        return;
    }

    mTargetObject = object;

    Clear();

    if (mTargetObject)
    {
        Rebuild();
    }

    if (mVerticalLayoutComponent)
    {
        mVerticalLayoutComponent->CalculateLayout();
    }
}

void UIReflectPanel::OnSelectedComponent(Component *com)
{
    if (mSelectedComponent == com)
    {
        return;
    }

    Clear();

    mSelectedComponent = com;
    if (mSelectedComponent == nullptr)
        return;

    if (IsRootSceneComponent(com))
    {
        SceneComponent *rootSceneCom = static_cast<SceneComponent *>(com);
        Object *ownerObject = com->GetOwnerObject();
        BuildInspectorForRootSceneComponent(ownerObject, rootSceneCom);
        return;
    }
    if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
    {
        BuildComponentTransformPanel(sceneCom);
    }

    BuildComponentPanel(com);
}

void UIReflectPanel::SetTargetComponent(Component *com)
{

    OnSelectedComponent(com);
}

void UIReflectPanel::Clear()
{
    auto reflectPanelFactory = UIReflectPanelFactory::GetInstance();

    for (auto panel : mReflectPanelList)
    {
        reflectPanelFactory->ReleaseReflectPanel(panel);
    }

    mReflectPanelList.clear();

    mTransformReflectPanel.SetActive(false);
    mTransformReflectPanel.SetParent(nullptr);
    mTransformReflectPanel.ClearBinding();
    // mSelectedComponent = nullptr;
}

void UIReflectPanel::Rebuild()
{

    if (mTargetObject == nullptr)
    {
        return;
    }
    BuildObjectProperties(mTargetObject);

    if (Entity *entity = dynamic_cast<Entity *>(mTargetObject))
    {
        BuildComponentTransformPanel(entity->GetRootComponent());
    }

    BuildObjectComponentPanels(mTargetObject);
}

void UIReflectPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    int a = 2;
}

UI::UIImage *UIReflectPanel::CreateBackgroundPanel()
{
    auto panel = CreateChildUIElement<UI::UIImage>("BasePanel");
    panel->SetSize(400, 120);
    return panel;
}

void UIReflectPanel::BuildObjectProperties(Object *object)
{
    if (object == nullptr)
        return;

    const char *className = object->GetRunTimeClassName();
    Quad::ClassInfo *classInfo = mReflectionSystem->FindClassInfo(className);

    BuildReflectProperties(object, classInfo, "Entity");
    BuildObjectComponentReferenceProperties(object);
}

void UIReflectPanel::BuildObjectComponentReferenceProperties(Object *object)
{

    if (object == nullptr)
        return;

    const char *className = object->GetRunTimeClassName();

    Quad::ClassInfo *classInfo = mReflectionSystem->FindClassInfo(className);

    if (classInfo == nullptr)
        return;

    while (1)
    {
        if (classInfo == nullptr || (strcmp(classInfo->GetTypeName(), "Entity") == 0) ||
            (strcmp(classInfo->GetTypeName(), "Object") == 0))
        {
            break;
        }

        for (Quad::PropertyInfo *property : classInfo->GetDeclaredPropertyVector())
        {
            if (property == nullptr)
                continue;

            if (!IsComponentReferenceProperty(property))
                continue;

            Component *com = property->GetValue<Component *>(object);
            if (com == nullptr)
                continue;

            BuildComponentPanel(com);
        }

        const auto &parentClassList = classInfo->GetParentClassList();
        if (!parentClassList.empty())
        {
            classInfo = mReflectionSystem->FindClassInfo(parentClassList[0].c_str());
        }
        else
        {
            break;
        }
    }
}

void UIReflectPanel::BuildObjectComponentPanels(Object *object)
{
    if (object == nullptr)
        return;

    for (auto com : object->GetComponentList())
    {
        if (com->GetComponentFlag() == Core::EComponentFlag::eEngineAdded)
        {
            BuildComponentPanel(com);
        }
    }
}

void UIReflectPanel::BuildInspectorForRootSceneComponent(Object *object, SceneComponent *rootCom)
{

    if (object == nullptr || rootCom == nullptr)
    {
        return;
    }

    BuildObjectProperties(object);
    BuildComponentTransformPanel(rootCom);
    BuildComponentPanel(rootCom);
}

bool UIReflectPanel::BuildEngineComponentClass(Component *com, Quad::ClassInfo *comClassInfo)
{
    auto reflectPanelFactory = UIReflectPanelFactory::GetInstance();

    std::string componentName = comClassInfo->GetTypeName();

    if (componentName == "StaticMeshComponent")
    {
        auto panel = reflectPanelFactory->GetStaticMeshPanel(this);
        panel->Build(com);
        panel->SetWidth(mTransform.GetSize().r);
        mReflectPanelList.push_back(panel);
        return true;
    }

    return false;
}

void UIReflectPanel::BuildComponentTransformPanel(SceneComponent *com)
{
    mTransformReflectPanel.SetTargetComponent(com);
    mTransformReflectPanel.SetActive(true);
    mTransformReflectPanel.SetParent(this);
}

void UIReflectPanel::BuildComponentPanel(Component *com)
{
    if (com == nullptr)
        return;

    Quad::ClassInfo *comClassInfo = mReflectionSystem->FindClassInfo(com->GetRunTimeClassName());

    if (comClassInfo == nullptr)
    {
        return;
    }

    bool bBuildEngineCom = false;
    // 두개의 분리된 관문  엔진클래스로 설정된경우는 특수하게 처리하는 경우
    if (comClassInfo->IsEngineClass())
    {
        bBuildEngineCom = BuildEngineComponentClass(com, comClassInfo);
    }

    if (bBuildEngineCom == false)
    {
        // 일반적으로 리플렉션 프로퍼티를 순회하면서 구축
        BuildComponentProperties(com);
    }
}

void UIReflectPanel::BuildComponentProperties(Component *com)
{
    if (com == nullptr)
    {
        return;
    }

    Quad::ClassInfo *classInfo = mReflectionSystem->FindClassInfo(com->GetRunTimeClassName());
    BuildReflectProperties(com, classInfo, "Component");
}

bool UIReflectPanel::IsRootSceneComponent(Component *com) const
{
    if (com == nullptr)
        return false;

    SceneComponent *sceneComponent = dynamic_cast<SceneComponent *>(com);
    if (sceneComponent == nullptr)
        return false;

    Entity *entity = dynamic_cast<Entity *>(com->GetOwnerObject());
    if (entity == nullptr)
        return false;

    if (entity->GetRootComponent() != sceneComponent)
        return false;

    return true;
}

bool UIReflectPanel::IsComponentReferenceProperty(Quad::PropertyInfo *propertyInfo) const
{
    if (propertyInfo == nullptr)
        return false;

    if (propertyInfo->mIsPointerType == false)
        return false;

    Quad::ClassInfo *classInfo = mReflectionSystem->FindClassInfo(propertyInfo->mOriginType);

    if (classInfo == nullptr)
        return false;

    return classInfo->IsAncestorClass("Component");
}

void UIReflectPanel::BuildReflectProperties(void *targetMemory, Quad::ClassInfo *classInfo, const char *stopClassName)
{

    if (classInfo == nullptr)
    {
        return;
    }

    while (1)
    {
        if (classInfo == nullptr || strcmp(classInfo->GetTypeName(), stopClassName) == 0)
        {
            break;
        }

        for (Quad::PropertyInfo *property : classInfo->GetDeclaredPropertyVector())
        {
            if (property == nullptr)
            {
                continue;
            }

            if (IsComponentReferenceProperty(property))
            {
                continue;
            }

            std::vector<UI::UIElement *> panelList = UIReflectPanelFactory::GetInstance()->GetReflectPanel(
                targetMemory, this, property, property->mPropertyName);

            if (!panelList.empty())
            {
                for (UI::UIElement *panel : panelList)
                {
                    panel->SetWidth(mTransform.GetSize().r);
                    mReflectPanelList.push_back(panel);
                }
            }
        }

        const auto &parentClassList = classInfo->GetParentClassList();
        if (!parentClassList.empty())
        {
            classInfo = mReflectionSystem->FindClassInfo(parentClassList[0].c_str());
        }
        else
        {
            break;
        }
    }
}

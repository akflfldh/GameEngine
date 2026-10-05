#include "UIReflectPanelFactory.h"
#include <EditorDirector/EditorUIUtility.h>
#include <AnimatorComponentUIReflectPanel.h>
#include <BoxColliderComponentUIReflectPanel.h>
#include <LightComponentUIReflectPanel.h>
#include <Core/Component.h>
#include <EditorDirector/UIReflectFloatPanel.h>
#include <EditorDirector/UIReflectVectorPanel.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <StaticMeshComponentUIReflectPanel.h>
#include <SkeletalMeshComponentUIReflectPanel.h>
#include <UIReflectBoolPanel.h>
#include <UIReflectSinglePrimitivePanel.h>
#include <UIReflectVector3Panel.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIImage.h>
UIReflectPanelFactory *UIReflectPanelFactory::GetInstance()
{
    static UIReflectPanelFactory instance;
    return &instance;
}

UIReflectPanelFactory::UIReflectPanelFactory() {}

UIReflectPanelFactory::~UIReflectPanelFactory() {}

std::vector<UI::UIElement *> UIReflectPanelFactory::GetReflectPanel(void *targetMemory, UI::UIElement *parentElement,
                                                                    Quad::PropertyInfo *property,
                                                                    const std::string &tagName)
{
    if (property == nullptr)
        return {};

    std::vector<UI::UIElement *> panelList;
    UI::UIImage *panel = nullptr;
    if (std::strcmp(property->mType, "bool") == 0)
    {
        UIReflectBoolPanel *boolPanel = nullptr;
        if (mBoolPanelPool.empty())
        {
            // Create
            boolPanel = EditorUIUtility::Create<UIReflectBoolPanel>(parentElement, "BoolPanel");
            // boolPanel->SetColor({0.4f, 0.4f, 0.4f});
            boolPanel->mReturnToPoolCallback = [this](UI::UIElement *element)
            { mBoolPanelPool.push_back(static_cast<UIReflectBoolPanel *>(element)); };
        }

        else
        {

            boolPanel = mBoolPanelPool.back();
            mBoolPanelPool.pop_back();
        }
        boolPanel->SetTagText(tagName);
        boolPanel->SetActiveFlag(true);
        boolPanel->SetParent(parentElement);
        panel = boolPanel;
    }
    else if (std::strcmp(property->mType, "float") == 0)
    {

        UIReflectFloatPanel *floatPanel = nullptr;
        if (mBoolPanelPool.empty())
        {
            // Create
            floatPanel = EditorUIUtility::CreateFloatField(parentElement, "FloatPanel");
            // floatPanel->SetColor({0.4f, 0.4f, 0.4f});
            floatPanel->mReturnToPoolCallback = [this](UI::UIElement *element)
            { mFloatPanelPool.push_back(static_cast<UIReflectFloatPanel *>(element)); };
        }
        else
        {

            floatPanel = mFloatPanelPool.back();
            mFloatPanelPool.pop_back();
        }
        floatPanel->SetTagText(tagName);
        floatPanel->SetActiveFlag(true);
        floatPanel->SetParent(parentElement);
        panel = floatPanel;
    }
    else if (property->mIsBuiltinType)
    {
        UIReflectSinglePrimitivePanel *primitivePanel = nullptr;
        if (mSinglePrimitivePanelPool.empty())
        {
            primitivePanel = EditorUIUtility::Create<UIReflectSinglePrimitivePanel>(parentElement, "SinglePrimtivePanel");
            // primitivePanel->SetColor({0.4f, 0.4f, 0.4f});
            primitivePanel->mReturnToPoolCallback = [this](UI::UIElement *element)
            { mSinglePrimitivePanelPool.push_back(static_cast<UIReflectSinglePrimitivePanel *>(element)); };
        }
        else
        {
            primitivePanel = mSinglePrimitivePanelPool.back();
            mSinglePrimitivePanelPool.pop_back();
        }
        primitivePanel->SetTagText(tagName);
        primitivePanel->SetActiveFlag(true);
        primitivePanel->SetParent(parentElement);
        panel = primitivePanel;
    }
    else if (std::strcmp(property->mType, "Vector3") == 0)
    {

        UIReflectVector3Panel *vector3Panel = nullptr;
        if (mVector3PanelPool.empty())
        {
            // Create
            vector3Panel = EditorUIUtility::CreateVector3Field(parentElement, "ReflectVector3Panel");
            // vector3Panel->SetColor({0.4f, 0.4f, 0.4f});
            vector3Panel->mReturnToPoolCallback = [this](UI::UIElement *element)
            { mVector3PanelPool.push_back(static_cast<UIReflectVector3Panel *>(element)); };
        }
        else
        {
            vector3Panel = mVector3PanelPool.back();
            mVector3PanelPool.pop_back();
        }
        vector3Panel->SetTagText(tagName);
        vector3Panel->SetActiveFlag(true);
        vector3Panel->SetParent(parentElement);
        panel = vector3Panel;
    }
    else if (property->mIsTemplateType)
    {
        UIReflectVectorPanel *vectorPanel = nullptr;
        if (std::strcmp(property->mTemplateTypeName, "vector") == 0)
        {
            if (mVectorPanelPool.empty())
            {

                vectorPanel = EditorUIUtility::Create<UIReflectVectorPanel>(parentElement, "ReflectVectorPanel");
                vectorPanel->SetColor({0.4f, 0.4f, 0.4f});
                vectorPanel->mReturnToPoolCallback = [this](UI::UIElement *element)
                { mVectorPanelPool.push_back(static_cast<UIReflectVectorPanel *>(element)); };
            }
            else
            {
                vectorPanel = mVectorPanelPool.back();
                mVectorPanelPool.pop_back();
            }
            //   vectorPanel->SetTagText(tagName);
            vectorPanel->SetActiveFlag(true);
            vectorPanel->SetParent(parentElement);
            vectorPanel->SetHeaderText(tagName);
            panel = vectorPanel;
        }
    }
    else if (property->mIsPointerType)
    {
    }

    if (panel)
    {
        panelList.push_back(panel);
    }

    if (panel)
    {
        IPropertyBindable *IBindable = dynamic_cast<IPropertyBindable *>(panel);
        if (IBindable)
        {
            IBindable->BindProperty(targetMemory, property);
        }
    }

    return panelList;
}

void UIReflectPanelFactory::ReleaseReflectPanel(UI::UIElement *element)
{
    element->SetActiveFlag(false);
    element->SetParent(nullptr);
    IPropertyBindable *IBindable = dynamic_cast<IPropertyBindable *>(element);
    if (IBindable)
    {
        IBindable->Release();
        IBindable->mReturnToPoolCallback(element);
    }
}

StaticMeshComponentUIReflectPanel *UIReflectPanelFactory::GetStaticMeshPanel(UI::UIElement *parentElement)
{
    StaticMeshComponentUIReflectPanel *panel = nullptr;
    if (mStaticMeshPanelPool.empty())
    {

        panel = EditorUIUtility::Create<StaticMeshComponentUIReflectPanel>(parentElement, "StaticMeshPanel");
        panel->mReturnToPoolCallback = [this](UI::UIElement *element)
        { mStaticMeshPanelPool.push_back(static_cast<StaticMeshComponentUIReflectPanel *>(element)); };
    }
    else
    {
        panel = mStaticMeshPanelPool.back();
        ActivatePanel(panel, parentElement);
        mStaticMeshPanelPool.pop_back();
    }

    return panel;
}

SkeletalMeshComponentUIReflectPanel *UIReflectPanelFactory::GetSkeletalMeshPanel(UI::UIElement *parentElement)
{
    SkeletalMeshComponentUIReflectPanel *panel = nullptr;
    if (mSkeletalMeshPanelPool.empty())
    {
        panel = EditorUIUtility::Create<SkeletalMeshComponentUIReflectPanel>(parentElement, "SkeletalMeshPanel");
        panel->mReturnToPoolCallback = [this](UI::UIElement *element)
        { mSkeletalMeshPanelPool.push_back(static_cast<SkeletalMeshComponentUIReflectPanel *>(element)); };
    }
    else
    {
        panel = mSkeletalMeshPanelPool.back();
        ActivatePanel(panel, parentElement);
        mSkeletalMeshPanelPool.pop_back();
    }

    return panel;
}

AnimatorComponentUIReflectPanel *UIReflectPanelFactory::GetAnimatorPanel(UI::UIElement *parentElement)
{
    AnimatorComponentUIReflectPanel *panel = nullptr;
    if (mAnimatorPanelPool.empty())
    {
        panel = EditorUIUtility::Create<AnimatorComponentUIReflectPanel>(parentElement, "AnimatorPanel");
        panel->mReturnToPoolCallback = [this](UI::UIElement *element)
        { mAnimatorPanelPool.push_back(static_cast<AnimatorComponentUIReflectPanel *>(element)); };
    }
    else
    {
        panel = mAnimatorPanelPool.back();
        ActivatePanel(panel, parentElement);
        mAnimatorPanelPool.pop_back();
    }

    return panel;
}

BoxColliderComponentUIReflectPanel *UIReflectPanelFactory::GetBoxColliderPanel(UI::UIElement *parentElement)
{
    // 기존 컴포넌트 패널과 같은 pool 경로를 사용하며 Release에서 이전 대상 바인딩을 해제한다.
    BoxColliderComponentUIReflectPanel *panel = nullptr;
    if (mBoxColliderPanelPool.empty())
    {
        panel = EditorUIUtility::Create<BoxColliderComponentUIReflectPanel>(parentElement, "BoxColliderPanel");
        panel->mReturnToPoolCallback = [this](UI::UIElement *element)
        { mBoxColliderPanelPool.push_back(static_cast<BoxColliderComponentUIReflectPanel *>(element)); };
    }
    else
    {
        panel = mBoxColliderPanelPool.back();
        ActivatePanel(panel, parentElement);
        mBoxColliderPanelPool.pop_back();
    }
    return panel;
}

LightComponentUIReflectPanel *UIReflectPanelFactory::GetLightPanel(UI::UIElement *parentElement)
{
    // 다른 전용 컴포넌트 패널과 동일하게 Canvas 소유 UI를 pool에서 재사용한다.
    LightComponentUIReflectPanel *panel = nullptr;
    if (mLightPanelPool.empty())
    {
        panel = EditorUIUtility::Create<LightComponentUIReflectPanel>(parentElement, "LightComponentPanel");
        panel->mReturnToPoolCallback = [this](UI::UIElement *element)
        { mLightPanelPool.push_back(static_cast<LightComponentUIReflectPanel *>(element)); };
    }
    else
    {
        panel = mLightPanelPool.back();
        ActivatePanel(panel, parentElement);
        mLightPanelPool.pop_back();
    }
    return panel;
}

void UIReflectPanelFactory::ActivatePanel(UI::UIElement *panel, UI::UIElement *parentElement)
{

    panel->SetParent(parentElement);
    panel->SetActiveFlag(true);
}

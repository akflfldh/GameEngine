#include "UIReflectPanelFactory.h"
#include <Core/Component.h>
#include <EditorDirector/UIReflectVectorPanel.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionPropertyInfo.h>
#include <StaticMeshComponentUIReflectPanel.h>
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
            boolPanel = parentElement->CreateChildUIElement<UIReflectBoolPanel>("BoolPanel");
            boolPanel->SetColor({0.4f, 0.4f, 0.4f});
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
    else if (property->mIsBuiltinType)
    {
        UIReflectSinglePrimitivePanel *primitivePanel = nullptr;
        if (mSinglePrimitivePanelPool.empty())
        {
            primitivePanel = parentElement->CreateChildUIElement<UIReflectSinglePrimitivePanel>("SinglePrimtivePanel");
            primitivePanel->SetColor({0.4f, 0.4f, 0.4f});
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
            vector3Panel = parentElement->CreateChildUIElement<UIReflectVector3Panel>("ReflectVector3Panel");
            vector3Panel->SetColor({0.4f, 0.4f, 0.4f});
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

                vectorPanel = parentElement->CreateChildUIElement<UIReflectVectorPanel>("ReflectVectorPanel");
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

        panel = parentElement->CreateChildUIElement<StaticMeshComponentUIReflectPanel>("StaticMeshPanel");
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

void UIReflectPanelFactory::ActivatePanel(UI::UIElement *panel, UI::UIElement *parentElement)
{

    panel->SetParent(parentElement);
    panel->SetActiveFlag(true);
}

#include "PropertyPanel.h"
#include <Core/Entity.h>
#include <Core/StaticMeshComponent.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIReflectPanel.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIText.h>
PropertyPanel::PropertyPanel() : mSelectedEntity(nullptr), mReflectPanel(nullptr) {}

PropertyPanel::~PropertyPanel() {}

void PropertyPanel::OnBegin()
{

    UIScrollBox::OnBegin();

    float propertyPanelWidth = mTransform.GetSize().r;
    float propertyPanelHeight = mTransform.GetSize().g;

    SetLayout(EUIScrollLayout::eVertical);

    // User Reflect Panel
    mReflectPanel = CreateChildUIElement<UIReflectPanel>("ReflectPanel");
    mReflectPanel->SetWidth(propertyPanelWidth);

    AddItem(mReflectPanel);
}

void PropertyPanel::Update(float deltaTime)
{
    UIScrollBox::Update(deltaTime);
}

Entity *PropertyPanel::GetEntity() const
{
    return mSelectedEntity;
}
void PropertyPanel::OnSelectedObject(Object *object)
{
    mSelectedEntity = static_cast<Entity *>(object);

    mReflectPanel->SetTargetObject(mSelectedEntity);

    ForceUpdateLayout();

    //  mOnSelectedObjectCallbackSystem.ExecuteCallbacks();
}

void PropertyPanel::OnSelectedComponent(Component *com)
{

    if (mReflectPanel == nullptr)
        return;

    mReflectPanel->OnSelectedComponent(com);
    ForceUpdateLayout();
}

void PropertyPanel::Initialize(BaseSelectionManager *selectionManager)
{

    mSelectionManager = selectionManager;
}

void PropertyPanel::OnTransformChanged(UI::ETransformChangeType type)
{

    UIScrollBox::OnTransformChanged(type);

    if (UI::ETransformChangeType::eAll == type || UI::ETransformChangeType::eSize == type)
    {
        if (mReflectPanel)
        {
            mReflectPanel->SetWidth(GetWidth());
        }
    }
}

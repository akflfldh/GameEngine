#include "PrefabWorkSpaceData.h"
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/SceneComponent.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UIReflectPanel.h>
#include <UISearchSelectBox.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include "MaterialWorkSpaceManager.h"

// #include<ReflectSystem/ReflectionSystem.h>

Quad::PrefabComponentPanel::PrefabComponentPanel() {}
Quad::PrefabComponentPanel::~PrefabComponentPanel() {}



void Quad::PrefabComponentPanel::Initialize(UI::UICanvas *canvas, BaseSelectionManager *selectionManager)
{

    RebuildComponentList();

    mTopPanel = canvas->CreateUIElement<UI::UIImage>("TopPanel");
    mTopPanel->SetSize(400, 800);
    mTopPanel->SetColor(0.3f, 0.3f, 0.3f);

    mComponentScrollPanel.Initialize(canvas, selectionManager, 400, 700);
    mComponentScrollPanel.SetUIParent(mTopPanel);
    mComponentScrollPanel.SetPositionLocal(0, 100);

    mComponentGenerationButton = mTopPanel->CreateChildUIElement<UI::UIButton>("ComGenerationButton");
    mComponentGenerationButton->SetPositionLocal(200, 50);
    mComponentGenerationButton->SetSize(50, 50);
    mComponentGenerationButton->mUIImageComponent->UseTexture();
    mComponentGenerationButton->mUIImageComponent->SetTexture("Engine/plus");
    mComponentGenerationButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float) { OnClickedComponentGenerationButton(); });

    CreateComponentAddSearchSelectBox(mTopPanel, 0, 50);
}

void Quad::PrefabComponentPanel::Update(float DeltaTime)
{

    mComponentScrollPanel.Update(DeltaTime);
}

void Quad::PrefabComponentPanel::SetPositionLocal(float x, float y)
{

    if (mTopPanel)
    {
        mTopPanel->SetPositionLocal(x, y);
    }
}

void Quad::PrefabComponentPanel::SetObject(Object *object)
{
    mTargetObject = object;
    mComponentScrollPanel.SetObject(object);
}

void Quad::PrefabComponentPanel::OnSelectedComponent(Component *com)
{
    mComponentScrollPanel.OnSelectedComponent(com);
}

void Quad::PrefabComponentPanel::RebuildComponentList()
{

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    mComponentList.clear();

    mComponentList.push_back("CameraComponent");
    mComponentList.push_back("StaticMeshComponent");

    for (auto classInfo : reflectionSystem->GetUserClassList())
    {
        mComponentList.push_back(classInfo->GetTypeName());
    }
}

void Quad::PrefabComponentPanel::CreateComponentAddSearchSelectBox(UI::UIElement *parent, float posX, float posY)
{

    mComponentAddSearchSelectBox = parent->CreateChildUIElement<UISearchSelectBox>("ComponentAddSearchSelectBox");
    mComponentAddSearchSelectBox->SetWidth(200.0f);
    mComponentAddSearchSelectBox->SetPositionLocal(posX, posY);
    mComponentAddSearchSelectBox->mOnSelectedItemChangedCallbackSystem.Register([this](uint32_t itemID)
                                                                                { mSelectedComponentItemID = itemID; });
    // reflect component class

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    std::vector<SearchSelectItem> items;
    for (size_t i = 0; i < mComponentList.size(); ++i)
    {

        SearchSelectItem item;
        item.mLabel = mComponentList[i];
        item.mUniqueNum = static_cast<uint32_t>(i);

        items.push_back(item);
    }

    mComponentAddSearchSelectBox->SetItems(items);
}

void Quad::PrefabComponentPanel::OnClickedComponentGenerationButton()
{

    if (mSelectedComponentItemID == UINT_MAX)
        return;

    if (mSelectedComponentItemID >= mComponentList.size())
        return;

    AddComponentToPrefab(mComponentList[mSelectedComponentItemID]);

    if (mComponentAddSearchSelectBox)
    {
        mComponentAddSearchSelectBox->ClearItems();
    }
}

void Quad::PrefabComponentPanel::AddComponentToPrefab(const std::string &componentName)
{

    if (mTargetObject == nullptr)
        return;

    mOnRequestAddComponentCallbackSystem.ExecuteCallbacks(componentName);
}

Quad::PrefabPropertyPanel::PrefabPropertyPanel() {}

Quad::PrefabPropertyPanel::~PrefabPropertyPanel() {}

void Quad::PrefabPropertyPanel::Initialize(UI::UICanvas *canvas)
{

    mReflectPanel = canvas->CreateUIElement<UIReflectPanel>("ReflectPanel");
    mReflectPanel->SetSize(600, 2000);
    mReflectPanel->SetHorizontalPivotSide(UI::EUIPosPivotHorizontal::eRight);
    mReflectPanel->SetHorizontalPivotOffset(0.0f);

    mReflectPanel->SetVerticalPivotSide(UI::EUIPosPivotVertical::eTop);
    mReflectPanel->SetVerticalPivotOffset(50.0f);
}

void Quad::PrefabPropertyPanel::SetPositionLocal(float x, float y)
{

    if (mReflectPanel)
    {
        mReflectPanel->SetPositionLocal(x, y);
    }
}

void Quad::PrefabPropertyPanel::SetObject(Object *object)
{

    if (mReflectPanel)
    {
        mReflectPanel->SetTargetObject(object);
    }
}

void Quad::PrefabPropertyPanel::OnSelectedComponent(Component *com)
{

    if (mReflectPanel)
    {
        mReflectPanel->OnSelectedComponent(com);
    }
}

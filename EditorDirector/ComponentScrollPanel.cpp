#include "ComponentScrollPanel.h"
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/SceneComponent.h>
#include <EditorSelectionManager.h>
#include <UIDragSourceComponent.h>
#include <UIDropTargetComponent.h>
#include <UIHierarchyItem.h>
#include <UIScrollBox.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UITextButton.h>
#include <unordered_map>

ComponentScrollPanel::ComponentScrollPanel()
    : mItemHeight(40.0f), mComponentAddedCallbackID(Core::CallbackIDNone),
      mComponentRemovedCallbackID(Core::CallbackIDNone)
{
}

ComponentScrollPanel::~ComponentScrollPanel()
{
    UnbindObjectComponentEvents();

    for (auto item : mComponentItemList)
    {
        delete item;
    }

    for (auto item : mComponentItemPool)
    {
        delete item;
    }
}

void ComponentScrollPanel::Initialize(UI::UICanvas *canvas, BaseSelectionManager *selectionManager,
                                      float scrollBoxWidth, float scrollBoxHeight)
{
    mCanvas = canvas;
    mSelectionManager = selectionManager;

    mScrollBox = canvas->CreateUIElement<UIScrollBox>("ScrollBox");
    mScrollBox->SetSize(scrollBoxWidth, scrollBoxHeight);

    for (int i = 0; i < 10; ++i)
    {
        mComponentItemPool.push_back(CreateItem());
    }

    mSelectionManager->mOnSelectedComponentCallbackSystem.Register([this](Component *com)
                                                                   { OnSelectedComponent(com); });
}

void ComponentScrollPanel::Update(float DeltaTime)
{

    if (mObject == nullptr)
        return;

    if (mObject->IsComponentTreeDirty() == false)
        return;

    Refresh();

    mObject->ClearComponentTreeDirty();
}

void ComponentScrollPanel::SetObject(Object *object)
{

    if (mObject != nullptr && mObject == object)
    {
        Refresh();
        return;
    }

    // UnbindObjectComponentEvents();
    mObject = object;
    // BindObjectComponentEvents(mObject);
    Refresh();
}

void ComponentScrollPanel::SetUIParent(UI::UIElement *parent)
{

    if (mScrollBox)
    {
        mScrollBox->SetParent(parent);
    }
}

void ComponentScrollPanel::SetPositionLocal(float x, float y)
{

    if (mScrollBox)
    {
        mScrollBox->SetPositionLocal(x, y);
    }
}

void ComponentScrollPanel::OnSelectedComponent(Component *com)
{

    if (mCurrentSelectedComponent)
    {

        // 선택상태 false로
        auto it = std::find_if(mComponentItemList.begin(), mComponentItemList.end(),
                               [this](ComponentItem *item)
                               {
                                   if (item->GetComponent() == mCurrentSelectedComponent)

                                   {
                                       return true;
                                   }

                                   return false;
                               });
        if (it != mComponentItemList.end())
        {
            ChangeToSelectedComponentState(*it, false);
        }
    }

    mCurrentSelectedComponent = com;
    if (mCurrentSelectedComponent)
    {

        auto it = std::find_if(mComponentItemList.begin(), mComponentItemList.end(),
                               [this](ComponentItem *item)
                               {
                                   if (item->GetComponent() == mCurrentSelectedComponent)

                                   {
                                       return true;
                                   }

                                   return false;
                               });
        if (it != mComponentItemList.end())
        {
            ChangeToSelectedComponentState(*it, true);
        }
        // 선택상태로
    }
}

void ComponentScrollPanel::Refresh()
{
    ResetScrollBox();

    if (mObject == nullptr)
        return;

    // component 순회하면서 item추가/
    std::unordered_map<Component *, ComponentItem *> table;
    for (auto com : mObject->GetComponentList())
    {

        if (com == nullptr || com->GetDeadState())
            continue;

        table[com] = GetAvailableItem();
        table[com]->SetText(com->GetInstanceName());
        table[com]->SetComponent(com);
    }

    std::vector<UI::UIElement *> topComitemlist;
    for (auto com : mObject->GetComponentList())
    {

        if (com == nullptr || com->GetDeadState())
            continue;

        if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
        {
            if (SceneComponent *parentSceneCom = sceneCom->GetParent())
            {
                auto parentIt = table.find(parentSceneCom);
                if (parentIt != table.end())
                {

                    auto uiItem = parentIt->second->GetUIItem();
                    uiItem->AddItem(table[com]->GetUIItem());
                }
                else
                {
                    topComitemlist.push_back(table[com]->GetUIItem());
                }
            }
            else
            {
                topComitemlist.push_back(table[com]->GetUIItem());
            }
        }
        else
        {
            topComitemlist.push_back(table[com]->GetUIItem());
        }
    }

    mScrollBox->AddItemList(topComitemlist);

    mScrollBox->ForceUpdateLayout();
}

void ComponentScrollPanel::BindObjectComponentEvents(Object *object)
{
    if (object == nullptr)
        return;

    mComponentAddedCallbackID =
        object->mOnComponentAddedCallbackSystem.Register([this](Component *com) { OnObjectComponentAdded(com); });

    mComponentRemovedCallbackID =
        object->mOnComponentRemovedCallbackSystem.Register([this](Component *com) { OnObjectComponentRemoved(com); });
}

void ComponentScrollPanel::UnbindObjectComponentEvents()
{
    if (mObject == nullptr)
        return;

    if (mComponentAddedCallbackID != Core::CallbackIDNone)
    {
        mObject->mOnComponentAddedCallbackSystem.UnRegister(mComponentAddedCallbackID);
        mComponentAddedCallbackID = Core::CallbackIDNone;
    }

    if (mComponentRemovedCallbackID != Core::CallbackIDNone)
    {
        mObject->mOnComponentRemovedCallbackSystem.UnRegister(mComponentRemovedCallbackID);
        mComponentRemovedCallbackID = Core::CallbackIDNone;
    }
}

void ComponentScrollPanel::OnObjectComponentAdded(Component *com)
{

    Refresh();

    if (mSelectionManager && com)
    {
        mSelectionManager->SetSelectedComponent(com);
    }
}

void ComponentScrollPanel::OnObjectComponentRemoved(Component *com)
{

    if (mCurrentSelectedComponent == com)
    {
        mCurrentSelectedComponent = nullptr;

        if (mSelectionManager)
            mSelectionManager->SetSelectedComponent(nullptr);
    }

    // The component is still in the object list while this callback is running.
    // The dirty update will rebuild the final list after Object::DestroyDeadComponents() finishes.
}

ComponentItem *ComponentScrollPanel::GetAvailableItem()
{
    ComponentItem *item = nullptr;
    if (mComponentItemPool.empty() != false)
    {

        item = mComponentItemPool.back();
        mComponentItemPool.pop_back();
    }
    else
    {
        item = CreateItem();
    }

    if (item)
    {
        item->SetActive(true);
        mComponentItemList.push_back(item);
    }

    return item;
}

ComponentItem *ComponentScrollPanel::CreateItem()
{
    ComponentItem *item = new ComponentItem;
    item->Initialize(mCanvas, 1, mItemHeight);
    item->SetActive(false);
    item->GetUIItem()->mOnClickedHeaderPanelCallbackSystem.Register([this, item]() { OnClickedComponentItem(item); });

    item->mDroppedComponentItemCallbackSystem.Register([this](Component *target, Component *source)
                                                       { OnDroppedComponent(target, source); });
    return item;
}

void ComponentScrollPanel::ResetScrollBox()
{
    mScrollBox->RemoveItemAll(false);
    for (auto item : mComponentItemList)
    {

        item->Reset();
        mComponentItemPool.push_back(item);
    }
    mComponentItemList.clear();
}

void ComponentScrollPanel::OnClickedComponentItem(ComponentItem *componentItem)
{
    mSelectionManager->SetSelectedComponent(componentItem->GetComponent());
}

void ComponentScrollPanel::ChangeToSelectedComponentState(ComponentItem *comItem, bool state)
{
    comItem->UpdateVisual(state);
}

void ComponentScrollPanel::OnDroppedComponent(Component *targetComponent, Component *sourceComponent)
{

    SceneComponent *draggedScene = dynamic_cast<SceneComponent *>(sourceComponent);
    SceneComponent *targetScene = dynamic_cast<SceneComponent *>(targetComponent);

    // 싸이클구조 검증
    if (!CanReparentComponent(draggedScene, targetScene))
        return;

    draggedScene->SetParent(targetScene);

    //  Refresh();
}

bool ComponentScrollPanel::CanReparentComponent(SceneComponent *childComponent, SceneComponent *parentComponent) const
{

    // sceneCom이 아니라면 불가능
    if (childComponent == nullptr || parentComponent == nullptr)
        return false;

    if (childComponent == parentComponent)
        return false;

    if (childComponent->GetOwnerObject() == nullptr || parentComponent->GetOwnerObject() == nullptr)
        return false;
    // 서로 다른 오브젝트 소속이라면 불가능
    if (childComponent->GetOwnerObject() != parentComponent->GetOwnerObject())
        return false;

    // 부모가 자식의 자식이라면 불가능
    if (parentComponent->IsChildOf(childComponent))
    {
        return false;
    }

    // child는 루트씬컴포넌트 불가능
    if (Entity *entity = dynamic_cast<Entity *>(childComponent->GetOwnerObject()))
    {

        if (entity->GetRootComponent() == childComponent)
        {
            return false;
        }
    }

    return true;
}

ComponentItem::ComponentItem() {}

ComponentItem::~ComponentItem() {}

void ComponentItem::Initialize(UI::UICanvas *canvas, float width, float height)
{

    if (canvas)
    {
        mUIItem = canvas->CreateUIElement<UIHierarchyItem>("item");
        if (mUIItem)
        {
            mUIItem->SetHeaderHeight(40.0f);
            mUIItem->SetSize(width, height);

            mUIItem->SetColor(0.4f, 0.4f, 0.4f);
            mUIItem->SetHeaderColor(0.4f, 0.4f, 0.4f);

            auto headerPanel = mUIItem->GetHeaderPanel();
            if (headerPanel == nullptr)
                return;

            mDragSourceComponent = headerPanel->CreateUIComponent<UIDragSourceComponent>("DragSourceComponent");
            mDropTargetComponent = headerPanel->CreateUIComponent<UIDropTargetComponent>("DropTargetComponent");

            if (mDragSourceComponent)
            {
                mDragSourceComponent->SetPreviewInputListener(true);
                mDragSourceComponent->SetDepthValue(1);
            }

            if (mDropTargetComponent)
            {
                mDropTargetComponent->SetDepthValue(1);
                mDropTargetComponent->SetDragDropPayloadType(EDragDropType::eComponent);

                mDropTargetComponent->mOnDroppedPayloadCallbackSystem.Register(
                    [this](const DragPayload &payload)
                    {
                        if (payload.mType != EDragDropType::eComponent)
                            return;

                        if (mComponent == nullptr || payload.mComponent == nullptr)
                            return;

                        mDroppedComponentItemCallbackSystem.ExecuteCallbacks(mComponent, payload.mComponent);
                    });
            }
        }
    }
}

void ComponentItem::Reset()
{

    if (mUIItem)
    {

        mUIItem->RemoveItemAll();
        mUIItem->SetActiveFlag(false);
    }
}

UIHierarchyItem *ComponentItem::GetUIItem() const
{
    return mUIItem;
}
void ComponentItem::SetText(const std::string &text)
{

    if (mUIItem)
    {
        mUIItem->SetHeaderText(text);
    }
}

void ComponentItem::SetActive(bool flag)
{

    if (mUIItem)
    {
        mUIItem->SetActiveFlag(flag);
    }
}

void ComponentItem::SetComponent(Component *com)
{

    mComponent = com;

    if (mDragSourceComponent)
    {
        DragPayload payload;
        payload.mType = EDragDropType::eComponent;
        payload.mComponent = com;
        mDragSourceComponent->SetPayload(payload);
    }
}

Component *ComponentItem::GetComponent() const
{
    return mComponent;
}

void ComponentItem::UpdateVisual(bool bSelect)
{

    if (bSelect)
    {
        mUIItem->SetHeaderColor(0.1F, 0.1F, 0.4F);
    }
    else
    {
        mUIItem->SetHeaderColor(0.4f, 0.4f, 0.4f);
    }
}

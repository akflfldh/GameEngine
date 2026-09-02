#include "ObjectHierarchyPanel.h"
#include <Core/Map.h>
#include <Core/Object.h>
#include <EditorDirector/EditorProjectManager.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <EditorDirector/UIScrollBox.h>
#include <ObjectHierarchyItem.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>

#include <EditorDeleteCommand.h>

ObjectHierarchyPanel::ObjectHierarchyPanel()
    : mScrollPanel(nullptr), mSelectedObject(nullptr), mSelectedItem(nullptr), bSelectedItemDirty(false)
{
    mImageCom = CreateUIComponent<UI::UIImageComponent>("ImageCom");
    mImageCom->NotUseTexture();
    mImageCom->SetColor(0.2, 0.7, 0.2);

    mVerticalLayoutCom = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    SetSize(300, 500);
}

ObjectHierarchyPanel::~ObjectHierarchyPanel()
{

    if (mMap != nullptr)
    {
        mMap->mObjectAddedCallbackSystem.UnRegister(mAddedCallbackID);
        mMap->mObjectRemovedCallbackSystem.UnRegister(mRemovedCallbackID);
    }
}

void ObjectHierarchyPanel::Initialize(BaseSelectionManager *selectionManager)
{

    mSelectionManager = selectionManager;
}

void ObjectHierarchyPanel::InitMap(Map *map)
{

    if (mMap)
    {
        // 기존맵에대해서는 어차피 그냥 UnRegisterAll할것이니 문제없을듯.
        // 즉 다른곳에서 map에 등록된 콜백을 모두 제거 하는 함수를 호출해서 처리함.

        mMap->mObjectRemovedCallbackSystem.UnRegister(mRemovedCallbackID);
        mMap->mObjectAddedCallbackSystem.UnRegister(mAddedCallbackID);
    }

    mMap = map;

    if (mMap)
    {
        mAddedCallbackID = mMap->mObjectAddedCallbackSystem.Register<ObjectHierarchyPanel>(
            this, &ObjectHierarchyPanel::OnAddedObjectToMap);

        mRemovedCallbackID = mMap->mObjectRemovedCallbackSystem.Register<ObjectHierarchyPanel>(
            this, &ObjectHierarchyPanel::OnRemovedObjectFromMap);
    }
    InitList();
}

void ObjectHierarchyPanel::OnBegin()
{
    UI::UIElement::OnBegin();

    const glm::vec2 size = mTransform.GetSize();

    auto titleElement = GetDestCanvas()->CreateUIElement<UI::UIElement>("Title");

    auto titleImageCom = titleElement->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    titleImageCom->NotUseTexture();
    titleImageCom->SetColor(1, 0, 0);

    titleElement->SetSize(size.r, 60);

    titleElement->SetParent(this);

    mScrollPanel = GetDestCanvas()->CreateUIElement<UIScrollBox>("ScrollBox");
    int a = 2;

    mScrollPanel->SetParent(this);

    SetScrollPanelSize(size.r, 400);
    SetScrollPanelColor(1, 1, 0);

    mVerticalLayoutCom->CalculateLayout();

    InitList();
    Quad::EditorProjectManager::GetInstance()->mOnOpendMapCallbackSystem.Register(this, &ObjectHierarchyPanel::InitMap);
}

void ObjectHierarchyPanel::SetScrollPanelSize(float w, float h)
{
    if (mScrollPanel)
    {
        mScrollPanel->SetSize(w, h);

        mVerticalLayoutCom->CalculateLayout();
    }
}

void ObjectHierarchyPanel::SetScrollPanelColor(float r, float g, float b)
{

    if (mScrollPanel)
        mScrollPanel->SetBackgrounColor(r, g, b);
}

void ObjectHierarchyPanel::InitList()
{

    if (mMap == nullptr)
        return;

    std::vector<UI::UIElement *> preItemList = mScrollPanel->RemoveItemAll(false);

    for (auto item : preItemList)
        ReleaseObjectHierarchyItem(static_cast<ObjectHierarchyItem *>(item));

    const std::vector<Object *> &objectList = mMap->GetEntityList();

    std::vector<UI::UIElement *> itemList;

    //<object ,item >
    std::unordered_map<Object *, ObjectHierarchyItem *> ItemTable;
    for (auto object : objectList)
    {

        if (object->HasObjectFlag(Core::EObjectFlag::eHidden))
            continue;
        // UIItemElement 생성
        // image component
        // text component

        ObjectHierarchyItem *item = GetObjectHierarchyItem();

        item->SetObject(object);

        // object name text set
        item->SetHeaderText(object->GetObjectName());

        itemList.push_back(item);

        ItemTable[object] = item;
    }

    std::vector<UI::UIElement *> topItemList;
    for (auto object : objectList)
    {

        if (object->HasObjectFlag(Core::EObjectFlag::eHidden))
            continue;

        auto item = ItemTable[object];

        auto parent = object->GetParent();
        if (parent)
        {
            auto parentItem = ItemTable[parent];
            parentItem->AddItem(item);
        }
        else
        {
            topItemList.push_back(item);
        }
    }

    mScrollPanel->AddItemList(topItemList);
}

void ObjectHierarchyPanel::OnAddedObjectToMap(Object *object)
{

    if (mScrollPanel == nullptr)
        return;

    auto canvas = GetDestCanvas();
    ObjectHierarchyItem *item = GetObjectHierarchyItem();
    // UI::UIElement *item =  canvas->CreateUIElement<UI::UIElement>("item");

    //  auto imageCom = item->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    //  imageCom->NotUseTexture();
    //  imageCom->SetColor(1, 1, 0);

    // auto textCom = item->CreateUIComponent<UI::UITextComponent>("textCom");
    // textCom->SetFontSize(41);
    // float h = textCom->GetLineHeight();
    //// setup size
    // item->SetSize(400, h);

    // object name text set

    item->SetObject(object);

    // object name text set
    item->SetHeaderText(object->GetObjectName());

    mScrollPanel->AddItem(item);
}

void ObjectHierarchyPanel::OnRemovedObjectFromMap(Object *object)
{

    if (mSelectedObject == object)
    {
        mSelectedItem = nullptr;
        mSelectedObject = nullptr;

        mOnSelectedObjectRemovedCallbackSystem.ExecuteCallbacks(object);
    }

    ObjectHierarchyItem *parentObjectItem = nullptr;
    ObjectHierarchyItem *targetObjectItem = nullptr;

    parentObjectItem = FindObjectItem(object->GetParent());
    targetObjectItem = FindObjectItem(object);

    /* std::vector<Object *> objectHierarchyList;
     Object *parentObject = object;
     while (parentObject)
     {
         objectHierarchyList.push_back(parentObject);
         parentObject = parentObject->GetParent();
     }

     ObjectHierarchyItem *targetObjectItem = nullptr;
     ObjectHierarchyItem *targetObjectParentItem = nullptr;
     const auto &itemList = mScrollPanel->GetItemList();

     Object *targetObject = objectHierarchyList.back();
     objectHierarchyList.pop_back();
     for (int i = 0; i < itemList.size(); ++i)
     {

         ObjectHierarchyItem *objectItem = static_cast<ObjectHierarchyItem *>(itemList[i]);

         if (objectItem->GetObject() == targetObject)
         {
             targetObjectItem = objectItem;
             break;
         }
     }

     if (targetObjectItem == nullptr)
         return;

     while (!objectHierarchyList.empty())
     {

         const std::vector<UI::UIElement *> &itemList = targetObjectItem->GetItemList();

         targetObject = objectHierarchyList.back();
         objectHierarchyList.pop_back();
         for (auto e : itemList)
         {

             ObjectHierarchyItem *objectItem = static_cast<ObjectHierarchyItem *>(e);
             if (objectItem->GetObject() == targetObject)
             {
                 targetObjectParentItem = targetObjectItem;

                 targetObjectItem = objectItem;
                 break;
             }
         }
     }*/

    if (parentObjectItem)
    {
        // 부모에서 targetObjectItem 제거
        parentObjectItem->RemoveItem(targetObjectItem);
    }

    if (targetObjectItem)
    {
        //        targetObjectItem포함 , 자손들 모두 제거

        std::queue<UI::UIElement *> queue;

        queue.push(targetObjectItem);

        while (!queue.empty())
        {

            ObjectHierarchyItem *item = static_cast<ObjectHierarchyItem *>(queue.front());
            queue.pop();

            for (auto child : item->GetItemList())
            {
                queue.push(child);
            }
            item->RemoveItemAll();
            ReleaseObjectHierarchyItem(item);
        }
    }
}

void ObjectHierarchyPanel::OnSelectedObject(Object *object)
{
    mSelectedObject = object;
    if (object)
    {
        // mSelectedObject = object;
        mOnSelectedObjectCallbackSystem.ExecuteCallbacks(mSelectedObject);

        ObjectHierarchyItem *item = FindObjectItem(object);

        if (bSelectedItemDirty && item)
        {
            UpdateItemVisual(item);
        }

        bSelectedItemDirty = true;
    }
    else
    {
        // mSelectedObject = object;
        UpdateItemVisual(nullptr);
    }
}

void ObjectHierarchyPanel::OnSelectedItem(ObjectHierarchyItem *item, bool bNotify)
{

    UpdateItemVisual(item);

    if (item)
    {
        RequestKeyboardCaptureInput(nullptr);
    }

    bSelectedItemDirty = false;

    auto selectionManager = EditorSelectionManager::GetInstance();
    if (selectionManager && bNotify)
    {
        Object *selectedObject = item ? item->GetObject() : nullptr;
        selectionManager->SetSelectedObject(selectedObject);
    }
}

void ObjectHierarchyPanel::OnKeyDown(const Quad::EKeyCode &key)
{

    UI::UIElement::OnKeyDown(key);

    if (key == Quad::EKeyCode::eDel)
    {
        EditorDeleteCommand::Execute(mSelectionManager);

        // if (mSelectedObject && mMap)
        //{

        //    // TODO Object에 Destroy (virtual로 )만들기
        //    // Destory에서 dead flag설정
        //    // RequestDestoryEntity 호출
        //    //  DestoryList에등록
        //    // 마지막, 첫 프레임에 DestoryList에있는 Object들을 제거,일반 테이블에서도 제거
        //    // Component도 제거
        //    // RenderProxy와 연관있는 친구들 처리적절한지 확인
        //    // 해당오브젝트의 자식들 일단 모두연쇄적으로 제거

        //    mSelectedObject->Destroy();

        //    mSelectedObject = nullptr;
        //}
    }
}

ObjectHierarchyItem *ObjectHierarchyPanel::GetObjectHierarchyItem()
{
    if (!mItemPool.empty())
    {
        auto item = mItemPool.back();
        item->SetActiveFlag(true);
        mItemPool.pop_back();
        return item;
    }

    ObjectHierarchyItem *item = GetDestCanvas()->CreateUIElement<ObjectHierarchyItem>("item");
    item->mOnClickedHeaderPanelCallbackSystem.Register([this, item]() { OnSelectedItem(item); });
    item->SetHeaderColor(0.3f, 0.2f, 0.7f);
    item->mOnDroppedObjectItemCallbackSystem.Register(
        [this, item](ObjectHierarchyItem *otherItem)
        {
            // 자신의부모 ,자기자신 or 자손들이라면 아무일도 일어나지않는다.

            // 오직 부모가 아닌 조상, or 다른 object의경우에만 부모변경
            ChangeObjectParent(otherItem, item);
        });
    item->SetHeaderFontSize(21);

    float h = item->GetHeaderLineHeight() + 20.0f;
    //   item->SetSize(1.0f, h);
    item->SetHeaderHeight(h);
    // setup size

    return item;
}

void ObjectHierarchyPanel::ReleaseObjectHierarchyItem(ObjectHierarchyItem *item)
{

    if (item == nullptr)
        return;

    item->SetActiveFlag(false);
    item->SetObject(nullptr);
    mItemPool.push_back(item);
}

void ObjectHierarchyPanel::UpdateItemVisual(ObjectHierarchyItem *newItem)
{

    if (mSelectedItem)
    {
        mSelectedItem->SetHeaderColor(0.3f, 0.2f, 0.7f);
    }

    mSelectedItem = newItem;
    if (mSelectedItem)
    {
        mSelectedItem->SetHeaderColor(0.2f, 0.2f, 0.2f);
    }
}

void ObjectHierarchyPanel::ChangeObjectParent(ObjectHierarchyItem *childItem, ObjectHierarchyItem *newParentItem)
{

    if (childItem == newParentItem)
    {
        return;
    }

    // 새로운부모의 조상중에 childItem이있다면 역전관계임으로 바로 리턴

    UI::UIElement *t = newParentItem;

    while (t != nullptr)
    {
        if (t == childItem)
        {
            return;
        }
        t = t->GetParent();
    }

    Object *childObject = childItem->GetObject();
    if (newParentItem)
    {
        newParentItem->AddItem(childItem);
        Object *parentObject = newParentItem->GetObject();
        childObject->SetParent(parentObject);
    }
    else
    {
        mScrollPanel->AddItem(childItem);
        childObject->SetParent(nullptr);
    }
}

ObjectHierarchyItem *ObjectHierarchyPanel::FindObjectItem(Object *object)
{
    if (object == nullptr)
        return nullptr;

    std::vector<Object *> objectHierarchyList;

    Object *parentObject = object;

    while (parentObject)
    {
        objectHierarchyList.push_back(parentObject);
        parentObject = parentObject->GetParent();
    }

    parentObject = objectHierarchyList.back();
    objectHierarchyList.pop_back();

    ObjectHierarchyItem *parentObjItem = nullptr;
    for (auto item : mScrollPanel->GetItemList())
    {
        ObjectHierarchyItem *objItem = static_cast<ObjectHierarchyItem *>(item);

        if (objItem->GetObject() == parentObject)
        {
            parentObjItem = objItem;
            break;
        }
    }

    if (parentObjItem == nullptr)
    {

        return nullptr;
    }

    if (objectHierarchyList.empty())
    {
        return parentObjItem;
    }

    ObjectHierarchyItem *targetObjectItem = nullptr;
    while (!objectHierarchyList.empty() && parentObjItem)
    {

        Object *childObject = objectHierarchyList.back();
        objectHierarchyList.pop_back();

        ObjectHierarchyItem *foundItem = nullptr;

        for (auto item : parentObjItem->GetItemList())
        {

            ObjectHierarchyItem *objItem = static_cast<ObjectHierarchyItem *>(item);

            if (objItem->GetObject() == childObject)
            {

                foundItem = objItem;
                break;
            }
        }

        if (foundItem == nullptr)
            return nullptr;

        parentObjItem = foundItem;
    }

    return parentObjItem;
}

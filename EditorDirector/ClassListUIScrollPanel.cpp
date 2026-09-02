#include "ClassListUIScrollPanel.h"
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UIHierarchyItem.h>
#include <UIScrollBox.h>
#include <UiSystem/UICanvas.h>
ClassListUIScrollPanel::ClassListUIScrollPanel() : mScrollBox(nullptr) {}

ClassListUIScrollPanel::~ClassListUIScrollPanel() {}

void ClassListUIScrollPanel::OnBegin()
{

    UIImage::OnBegin();
    float width = mTransform.GetSize().x;
    float height = mTransform.GetSize().y;

    mScrollBox = CreateChildUIElement<UIScrollBox>("ScrollBox");
    mScrollBox->SetSize(width, height);
    mScrollBox->SetBackgrounColor(mScrollPanelColor.mR, mScrollPanelColor.mG, mScrollPanelColor.mB);

    // init list

    auto reflectsystem = Quad::ReflectionSystem::GetInstance();
    const std::vector<Quad::ClassInfo *> classList = reflectsystem->GetUserClassList();

    // className, <item * , parentClassName>
    std::unordered_map<std::string, std::pair<UIHierarchyItem *, std::string>> itemTable;

    auto objectItem = GetItem();
    objectItem->SetHeaderText("Object");
    itemTable["Object"] = {objectItem, ""};
    mItemTable["Object"] = objectItem;

    auto entityItem = GetItem();
    entityItem->SetHeaderText("Entity");
    itemTable["Entity"] = {entityItem, "Object"};
    objectItem->AddItem(entityItem);
    mItemTable["Object"] = entityItem;

    auto controllableEntityItem = GetItem();
    controllableEntityItem->SetHeaderText("ControllableEntity");
    itemTable["ControllableEntity"] = {controllableEntityItem, "Entity"};
    entityItem->AddItem(controllableEntityItem);
    mItemTable["ControllableEntity"] = controllableEntityItem;

    auto CharacterItem = GetItem();
    CharacterItem->SetHeaderText("Character");
    itemTable["Character"] = {CharacterItem, "ControllableEntity"};
    controllableEntityItem->AddItem(CharacterItem);
    mItemTable["Character"] = CharacterItem;

    // width는 scrollBox에서 설정되서 모든 item들로 계층적으로 전파되니 마지막으로수행
    // mScrollBox->AddItem(objectItem);

    for (auto classInfo : classList)
    {
        const auto &parentList = classInfo->GetParentClassList();

        if (parentList.empty() == false)
        {
            const char *parentClassName = parentList[0].c_str();

            auto item = GetItem();
            const char *className = classInfo->GetTypeName();
            item->SetHeaderText(className);
            mItemTable[className] = item;

            itemTable[classInfo->GetTypeName()] = {item, parentClassName};
        }
    }

    for (const auto &e : itemTable)
    {
        const std::string &parentClassName = e.second.second;

        auto parentIt = itemTable.find(parentClassName);

        if (parentIt != itemTable.end())
        {
            UIHierarchyItem *parentItem = parentIt->second.first;
            parentItem->AddItem(e.second.first);
        }
    }

    mScrollBox->AddItem(objectItem);
}

UIHierarchyItem *ClassListUIScrollPanel::GetItem()
{

    auto canvas = GetDestCanvas();
    UIHierarchyItem *item = canvas->CreateUIElement<UIHierarchyItem>("Item");

    item->SetHeaderFontSize(20.0f);
    item->SetHeaderHeight(30.0f);
    item->mOnClickedHeaderPanelCallbackSystem.Register(
        [this, item]() { mOnClickedClassItemCallbackSystem.ExecuteCallbacks(item->GetHeaderText()); });

    return item;
}

void ClassListUIScrollPanel::SetScrollPanelColor(float r, float g, float b)
{
    mScrollPanelColor.mR = r;
    mScrollPanelColor.mG = g;
    mScrollPanelColor.mB = b;

    if (mScrollBox)
    {

        mScrollBox->SetBackgrounColor(mScrollPanelColor.mR, mScrollPanelColor.mG, mScrollPanelColor.mB);
    }
}
void ClassListUIScrollPanel::Refresh() {}

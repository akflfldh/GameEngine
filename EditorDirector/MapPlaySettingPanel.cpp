#include <Core/Map.h>
#include <EditorDirector/MapPlaySettingPanel.h>
#include <EditorDirector/UISearchSelectBox.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
MapPlaySettingPanel::MapPlaySettingPanel() {}

MapPlaySettingPanel::~MapPlaySettingPanel() {}

void MapPlaySettingPanel::OnBegin()
{

   /* auto uiPopupManager = UI::UIPopupManager::GetInstance();
    mPopupScopeID = uiPopupManager->CreatePopupScope(this);*/

    SetColor(0.4f, 0.4f, 0.4f);
    SetWidth(600.0f);

    float subPanelPosY = 10.0f;

    CreateTitle(subPanelPosY);
    subPanelPosY = 50.0f;
    CreatePlayerObjectUI(subPanelPosY);

    subPanelPosY = 100.0f;
    CreatePlayerControllerUI(subPanelPosY);
}

void MapPlaySettingPanel::SetObjectSourceList(const std::vector<Core::ObjectSourceCandidate> &list)
{

    mObjectSourceList = list;

    std::vector<SearchSelectItem> items;

    for (size_t i = 0; i < mObjectSourceList.size(); ++i)
    {
        SearchSelectItem item;
        item.mLabel = mObjectSourceList[i].mDisplayName;
        item.mUniqueNum = static_cast<uint32_t>(i);
        items.push_back(std::move(item));
    }

    if (mSearchObjectSelectBox)
    {
        mSearchObjectSelectBox->SetItems(items);
    }

    RefreshSelectedObjectSource();
}
void MapPlaySettingPanel::SetObjectControllerSourceList(const std::vector<Core::ObjectSourceCandidate> &list)
{

    mObjectControllerSourceList = list;

    std::vector<SearchSelectItem> items;

    for (size_t i = 0; i < mObjectControllerSourceList.size(); ++i)
    {
        SearchSelectItem item;
        item.mLabel = mObjectControllerSourceList[i].mDisplayName;
        item.mUniqueNum = static_cast<uint32_t>(i);
        items.push_back(std::move(item));
    }

    if (mSearchControllerSelectBox)
    {
        mSearchControllerSelectBox->SetItems(items);
    }

    RefreshSelectedObjectControllerSource();
}

void MapPlaySettingPanel::BindMap(Map *map)
{
    // item selected callback 호출시 이 targetMap에 해당 setting정보 전달
    mTargetMap = map;
    RefreshSelectedObjectSource();
    RefreshSelectedObjectControllerSource();
}

// void MapPlaySettingPanel::RebuildObjectSourceCandidates(const std::string &subStr)
//{
//     std::vector<SearchSelectItem> items;
//     // text와 부분 앞문자열매칭에 성공하는 리스트를 가져온다.
//
//     if (subStr == "")
//     {
//         mSearchSelectBox->SetItems(items);
//         return;
//     }
//
//     for (size_t i = 0; i < mObjectSourceList.size(); ++i)
//     {
//
//         const auto &source = mObjectSourceList[i];
//
//         if (!source.mDisplayName.starts_with(subStr))
//             continue;
//
//         SearchSelectItem item;
//         item.mLabel = source.mDisplayName;
//         item.mUniqueNum = i;
//         items.push_back(std::move(item));
//     }
//
//     mSearchSelectBox->SetItems(items);
// }

void MapPlaySettingPanel::SetSelectedObjectSource(uint32_t itemID)
{
    if (mTargetMap == nullptr)
        return;
    if (itemID >= mObjectSourceList.size())
        return;

    mSelectedObjectItemID = itemID;
    // mObjectSourceList[itemID];
    const Core::ObjectSourceCandidate &source = mObjectSourceList[itemID];

    switch (source.mType)
    {
    case Core::ESpawnObjectSourceType::eClass:
        mTargetMap->SetDefaultPlayerObjectClass(source.mClassName);
        break;

    case Core::ESpawnObjectSourceType::ePrefab:
        mTargetMap->SetDefaultPlayerObjectPrefab(source.mPrefabID);
        break;

    default:
        mTargetMap->ClearDefaultPlayerObject();
        break;
    }
}

void MapPlaySettingPanel::SetSelectedControllerSource(uint32_t itemID)
{

    if (mTargetMap == nullptr)
        return;
    if (itemID >= mObjectControllerSourceList.size())
        return;

    mSelectedControllerItemID = itemID;
    const Core::ObjectSourceCandidate &source = mObjectControllerSourceList[itemID];

    switch (source.mType)
    {
    case Core::ESpawnObjectSourceType::eClass:
        mTargetMap->SetDefaultPlayerControllerClass(source.mClassName);
        break;
    default:
        mTargetMap->ClearDefaultPlayerController();
        break;
    }
}

void MapPlaySettingPanel::CreatePlayerObjectUI(float posY)
{
    float posX = 0;

    auto objectPlayerTag = CreateChildUIElement<UI::UIText>("ObjectPlayerTag");
    objectPlayerTag->SetFontSize(20.0F);
    objectPlayerTag->SetText("플레이어 오브젝트");
    posX = 10;

    objectPlayerTag->SetPositionLocal(posX, posY);

    mSearchObjectSelectBox = CreateChildUIElement<UISearchSelectBox>("searchBox");
    mSearchObjectSelectBox->SetWidth(100.0f);
    mSearchObjectSelectBox->mOnSelectedItemChangedCallbackSystem.Register([this](uint32_t itemID)
                                                                          { SetSelectedObjectSource(itemID); });
    mSearchObjectSelectBox->SetDepthValue(1);

    posX += 200.0f;
    mSearchObjectSelectBox->SetPositionLocal(posX, posY);

    // mSearchObjectSelectBox->SetPopupScope(mPopupScopeID);
}

void MapPlaySettingPanel::CreatePlayerControllerUI(float posY)
{

    float posX = 0;
    auto tag = CreateChildUIElement<UI::UIText>("PlayerControllerTag");
    tag->SetFontSize(20.0F);
    tag->SetText("플레이어 컨트롤러");
    posX = 10;

    tag->SetPositionLocal(posX, posY);

    mSearchControllerSelectBox = CreateChildUIElement<UISearchSelectBox>("PlayerControllerSearchBox");
    mSearchControllerSelectBox->SetWidth(100.0f);
    mSearchControllerSelectBox->mOnSelectedItemChangedCallbackSystem.Register([this](uint32_t itemID)
                                                                              { SetSelectedControllerSource(itemID); });

    posX += 200.0f;
    mSearchControllerSelectBox->SetPositionLocal(posX, posY);

    // mSearchControllerSelectBox->SetPopupScope(mPopupScopeID);
}

void MapPlaySettingPanel::CreateTitle(float posY)
{

    auto titleText = CreateChildUIElement<UI::UIText>("Title");
    titleText->SetText("게임모드");
    titleText->SetPositionLocal(10, posY);
    titleText->SetTextColor({1, 1, 1});

    auto exitButton = CreateChildUIElement<UI::UIButton>("ExitButton");
    exitButton->SetSize(40, 40);
    exitButton->mUIImageComponent->UseTexture();
    exitButton->mUIImageComponent->SetTexture("Engine/Exit");

    exitButton->SetPositionLocal(500 - exitButton->mTransform.GetSize().x, 0.0f);
    exitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { SetActiveFlag(false); });
}

void MapPlaySettingPanel::RefreshSelectedObjectSource()
{

    if (mTargetMap == nullptr || mSearchObjectSelectBox == nullptr)
        return;

    const Core::GameModeSetting &setting = mTargetMap->GetGameModeSetting();

    if (setting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::eNone)
    {
        mSelectedObjectItemID = UINT32_MAX;
        mSearchObjectSelectBox->ClearSelectedText();
        return;
    }

    for (size_t i = 0; i < mObjectSourceList.size(); ++i)
    {

        const auto &source = mObjectSourceList[i];

        bool matched = false;

        if (setting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::eClass)
        {
            matched = source.mType == Core::ESpawnObjectSourceType::eClass &&
                      source.mClassName == setting.mDefaultPlayerObjectClassName;
        }
        else if (setting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::ePrefab)
        {
            matched = source.mType == Core::ESpawnObjectSourceType::ePrefab &&
                      source.mPrefabID == setting.mDefaultPlayerObjectPrefabID;
        }

        if (matched)
        {
            mSelectedObjectItemID = static_cast<uint32_t>(i);
            mSearchObjectSelectBox->SetSelectedText(source.mDisplayName);
            return;
        }
    }

    mSelectedObjectItemID = UINT_MAX;
    mSearchObjectSelectBox->ClearSelectedText();
}

void MapPlaySettingPanel::RefreshSelectedObjectControllerSource()
{

    if (mTargetMap == nullptr || mSearchControllerSelectBox == nullptr)
        return;

    const Core::GameModeSetting &setting = mTargetMap->GetGameModeSetting();
    if (setting.mDefaultPlayerControllerClassName.empty())
    {
        mSelectedControllerItemID = UINT32_MAX;
        mSearchControllerSelectBox->ClearSelectedText();
        return;
    }
    for (size_t i = 0; i < mObjectControllerSourceList.size(); ++i)
    {

        const auto &source = mObjectControllerSourceList[i];

        if (source.mClassName == setting.mDefaultPlayerControllerClassName)
        {
            mSelectedControllerItemID = static_cast<uint32_t>(i);
            mSearchControllerSelectBox->SetSelectedText(source.mClassName);
            return;
        }
    }

    mSelectedControllerItemID = UINT_MAX;
    mSearchControllerSelectBox->ClearSelectedText();
}

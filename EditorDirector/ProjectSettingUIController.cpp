#include "ProjectSettingUIController.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/Map.h>
#include <Core/ProjectConfig.h>
#include <Core/World.h>
#include <EditorDirector/CollisionChannelSettingPanel.h>
#include <EditorDirector/EditorAssetManager.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorSceneManager.h>
#include <EditorDirector/UIDropTargetComponent.h>
#include <EditorDirector/UIDropdown.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIUtility.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
#include <algorithm>

void ProjectSettingUIController::Initialize(UI::UICanvas *canvas)
{
    mCanvas = canvas;
    mToolbar = EditorUIUtility::Create<UI::UIImage>(canvas, "ProjectSettingToolbar");
    EditorUIUtility::ApplyPreset(mToolbar, UI::EUIStyleRole::eNone);
    mToolbar->SetColor(UI::UIColor::DarkGray);

    auto *backButton = EditorUIUtility::CreateSmallTextButton(mToolbar, "ProjectSettingBackButton");
    // EditorUIUtility::ApplyPreset(backButton, UI::EUIStyleRole::eNone);

    backButton->SetWidth(200.0);
    backButton->SetPositionLocal(8.0f, 5.0f);
    backButton->mTextComponent->SetPaddingLeft(10.0F);
    backButton->mTextComponent->SetPaddingTop(5.0F);
    //  backButton->mUIImageComponent->SetColor(UI::UIColor::DimGray);
    backButton->mTextComponent->SetText("에디터로 돌아가기");
    // EditorUIUtility의 공통 폰트 규격 유지: backButton->mTextComponent->SetFontSize(18.0f);
    backButton->mUIImageComponent->SetUseBorderFlag(true);
    backButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [](float, float) { Quad::EditorDirector::GetInstance()->ChangeToDefaultEditWorkSpace(); });

    auto *title = EditorUIUtility::CreateLabel(mToolbar, "ProjectSettingTitle");
    EditorUIUtility::ApplyPreset(title, UI::EUIStyleRole::eNone);
    // EditorUIUtility의 기본 높이 유지: title->SetSize(300.0f, 40.0f);
    title->SetWidth(300.0f);
    title->SetPositionLocal(250.0f, 5.0f);
    title->SetText("프로젝트 설정");
    // EditorUIUtility의 공통 폰트 규격 유지: title->SetFontSize(24.0f);

    mSettingListPanel = EditorUIUtility::Create<UI::UIImage>(canvas, "ProjectSettingListPanel");
    EditorUIUtility::ApplyPreset(mSettingListPanel, UI::EUIStyleRole::eNone);
    mSettingListPanel->SetColor(UI::UIColor::DarkGray);
    auto *listTitle = EditorUIUtility::CreateLabel(mSettingListPanel, "ProjectSettingListTitle");
    EditorUIUtility::ApplyPreset(listTitle, UI::EUIStyleRole::eNone);
    // EditorUIUtility의 기본 높이 유지: listTitle->SetSize(240.0f, 32.0f);
    listTitle->SetWidth(240.0f);
    listTitle->SetPositionLocal(12.0f, 10.0f);
    listTitle->SetText("설정 목록");
    // EditorUIUtility의 공통 폰트 규격 유지: listTitle->SetFontSize(20.0f);

    // VerticalLayout은 소유 패널의 높이를 자식 높이 합으로 바꾸므로 제목과 전체 배경에서 분리한다.
    mSettingButtonListPanel = EditorUIUtility::Create<UI::UIImage>(mSettingListPanel, "ProjectSettingButtonListPanel");
    EditorUIUtility::ApplyPreset(mSettingButtonListPanel, UI::EUIStyleRole::eNone);
    mSettingButtonListPanel->SetColor(UI::UIColor::DarkGray);
    mSettingButtonListPanel->SetSize(0.0f, 0.0f);
    mSettingButtonListPanel->SetPositionLocal(12.0f, 52.0f);
    mSettingListLayout =
        mSettingButtonListPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("ProjectSettingListVerticalLayout");

    mContentPanel = EditorUIUtility::Create<UI::UIImage>(canvas, "ProjectSettingContentPanel");
    EditorUIUtility::ApplyPreset(mContentPanel, UI::EUIStyleRole::eNone);
    mContentPanel->SetColor(0.22f, 0.22f, 0.24f);
    mEmptyMessage = EditorUIUtility::CreateLabel(mContentPanel, "ProjectSettingEmptyMessage");
    EditorUIUtility::ApplyPreset(mEmptyMessage, UI::EUIStyleRole::eNone);
    mEmptyMessage->SetPositionLocal(20.0f, 20.0f);
    // EditorUIUtility의 기본 높이 유지: mEmptyMessage->SetSize(600.0f, 60.0f);
    mEmptyMessage->SetWidth(600.0f);
    mEmptyMessage->SetText("아직 등록된 설정 항목이 없습니다.");
    // EditorUIUtility의 공통 폰트 규격 유지: mEmptyMessage->SetFontSize(20.0f);

    CreateCollisionChannelSettingPanel();
    CreateMapSettingPanel();

    UpdateLayout();
}

bool ProjectSettingUIController::AddSettingPanel(const std::string &name, UI::UIElement *panel)
{
    if (!mCanvas || !panel || name.empty() || panel->GetDestCanvas() != mCanvas)
        return false;
    for (const auto &entry : mSettings)
    {
        if (entry.mName == name || entry.mPanel == panel)
            return false;
    }

    // 목록의 콜백은 패널 포인터 대신 이름으로 선택한다. 설정 UI와 데이터 시스템의 결합을 이곳에 넣지 않는다.
    panel->SetParent(mContentPanel);
    panel->SetPositionLocal(0.0f, 0.0f);
    panel->SetSize(mContentPanel->GetWidth(), mContentPanel->GetHeight());
    panel->SetActiveFlag(false);
    auto *button = EditorUIUtility::CreateSmallTextButton(mSettingButtonListPanel, name.c_str());

    UI::UIControlStyleOverride visual;
    visual.mBackgroundColor = UI::UIColor::DarkGray;
    visual.mHoverColor = UI::UIColor::LightGray;
    button->SetStyleOverride(visual);

    button->SetWidth(mSettingButtonListPanel->GetWidth());
    button->SetPositionLocal(0.0f, 0.0f);
    button->mTextComponent->SetText(name);
    // EditorUIUtility의 공통 폰트 규격 유지: button->mTextComponent->SetFontSize(20.0f);
    button->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, name](float, float)
                                                                    { SelectSetting(name); });

    mSettings.push_back({name, panel, button});
    // Begin 전 등록도 가능하므로 자식 추가 알림에만 의존하지 않고 최종 버튼 크기로 한 번 배치한다.
    mSettingListLayout->CalculateLayout();
    if (mSettings.size() == 1)
        SelectSetting(name);
    return true;
}

bool ProjectSettingUIController::SelectSetting(const std::string &name)
{
    const auto selected = std::find_if(mSettings.begin(), mSettings.end(),
                                       [&name](const SettingEntry &entry) { return entry.mName == name; });
    if (selected == mSettings.end())
        return false;

    for (auto &entry : mSettings)
    {
        const bool active = entry.mName == name;
        entry.mPanel->SetActiveFlag(active);
        //    entry.mButton->mUIImageComponent->SetColor(active ? UI::UIColor::DimGray : UI::UIColor::DarkGray);
    }
    mEmptyMessage->SetActiveFlag(false);
    return true;
}

void ProjectSettingUIController::CreateCollisionChannelSettingPanel()
{
    auto *panel = EditorUIUtility::Create<CollisionChannelSettingPanel>(mCanvas, "CollisionChannelSettingPanel");
    AddSettingPanel("충돌채널", panel);
}

void ProjectSettingUIController::CreateMapSettingPanel()
{

    auto *panel = EditorUIUtility::Create<UI::UIImage>(mCanvas, "MapSettingPanel");
    auto vericalLayoutCom = panel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalCom");
    vericalLayoutCom->SetSyncWidthFlag(true);

    AddSettingPanel("맵", panel);

    auto row1 = EditorUIUtility::CreateHorizontalRow(panel, "headerRow", 0.0f, false);
    EditorUIUtility::CreateLabel(row1, "title", "맵 설정", 100.0f);

    auto row2 = EditorUIUtility::CreateHorizontalRow(panel, "headerRow", 0.0f, false);
    EditorUIUtility::CreateLabel(row2, "startMapTagText", "시작 맵", 100.0F);

    auto startMapDropdown = EditorUIUtility::CreateDropdown(row2, "StartMapDropdown");

    auto editorSceneManager = Quad::EditorSceneManager::GetInstance();
    std::vector<std::string> mapList = editorSceneManager->GetUserMapNameList();
    startMapDropdown->SetItemList(mapList);
    startMapDropdown->SetSelectedIndex(0, false);
    startMapDropdown->SetWidth(200.0f);

    editorSceneManager->mOnUserMapAddedCallbackSystem.Register(
        [startMapDropdown, editorSceneManager](Map *map)
        {
            std::vector<std::string> mapList = editorSceneManager->GetUserMapNameList();
            startMapDropdown->SetItemList(mapList);
        });

    startMapDropdown->mOnSelectedItemChangedCallbackSystem.Register(
        [startMapDropdown, editorSceneManager](size_t index)
        {
            std::string mapName;
            bool ret = startMapDropdown->GetSelectedText(mapName);

            if (ret)
            {
                auto mapList = editorSceneManager->GetUserMapList();
                for (auto map : mapList)
                {
                    if (map->GetName().c_str() == mapName)
                    {
                        Quad::ProjectConfig::GetInstance()->SetStartMapID(map->GetID());
                    }
                }
            }
        });

    // 선택한다음에
}

void ProjectSettingUIController::UpdateLayout()
{
    if (!mCanvas)
        return;
    const auto size = mCanvas->GetWindowSize();
    if (size.X == mLayoutWidth && size.Y == mLayoutHeight)
        return;
    mLayoutWidth = size.X;
    mLayoutHeight = size.Y;

    // UI 전용 창 전체를 사용한다. 크기가 바뀔 때만 갱신하여 매 프레임 불필요한 Transform dirty를 만들지 않는다.
    const float toolbarHeight = 50.0f;
    const float listWidth = std::min(280.0f, std::max(0.0f, size.X));
    const float contentHeight = std::max(0.0f, size.Y - toolbarHeight);
    const float contentWidth = std::max(0.0f, size.X - listWidth);
    mToolbar->SetSize(size.X, toolbarHeight);
    mSettingListPanel->SetPositionLocal(0.0f, toolbarHeight);
    mSettingListPanel->SetSize(listWidth, contentHeight);
    mSettingButtonListPanel->SetWidth(std::max(0.0f, listWidth - 24.0f));
    mContentPanel->SetPositionLocal(listWidth, toolbarHeight);
    mContentPanel->SetSize(contentWidth, contentHeight);
    for (auto &entry : mSettings)
    {
        entry.mButton->SetWidth(std::max(0.0f, listWidth - 24.0f));
        entry.mPanel->SetSize(contentWidth, contentHeight);
    }
    mSettingListLayout->CalculateLayout();
}

#include "CollisionChannelSettingPanel.h"
#include <EditorDirector/EditorUIUtility.h>

#include <EditorDirector/UIBoolPanel.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIScrollBox.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <algorithm>

namespace
{
UI::UIText *CreateLabel(UI::UIElement *parent, const char *name, const std::string &text, float x, float y)
{
    auto *label = EditorUIUtility::CreateLabel(parent, name);
    EditorUIUtility::ApplyPreset(label, UI::EUIStyleRole::eNone);
    // EditorUIUtility의 기본 높이 유지: label->SetSize(220.0f, 32.0f);
    label->SetWidth(220.0f);
    label->SetPositionLocal(x, y);
    // EditorUIUtility의 공통 폰트 규격 유지: label->SetFontSize(18.0f);
    label->SetText(text);
    return label;
}
} // namespace

CollisionChannelSettingPanel::CollisionChannelSettingPanel()
{
    // 전체 콘텐츠의 크기는 ProjectSettingUIController가 관리한다. 테마의 행 높이로 덮어쓰지 않는다.
    SetStyleRole(UI::EUIStyleRole::eNone);
    SetColor(UI::UIColor::Gray);
}

CollisionChannelSettingPanel::~CollisionChannelSettingPanel() = default;

void CollisionChannelSettingPanel::OnBegin()
{
    UI::UIImage::OnBegin();
    if (mChannelDropdown)
        return;
    // UIScrollBox/BoolPanel의 내부 자식은 Begin에서 준비된다. 그 이후에 행을 연결한다.
    CreateControls();
    UpdatePanelLayout();
    RefreshChannels();
}

void CollisionChannelSettingPanel::CreateControls()
{
    CreateLabel(this, "SelectedChannelLabel", "선택 채널", 20.0f, 20.0f);
    mChannelDropdown = EditorUIUtility::CreateDropdown(this, "CollisionChannelDropdown");
    mChannelDropdown->SetWidth(260.0f);
    mChannelDropdown->SetPositionLocal(120.0f, 20.0f);
    mChannelDropdown->SetRenderLayer(UI::EUIRenderLayer::ePopup);
    mChannelDropdown->mOnSelectedItemChangedCallbackSystem.Register([this](size_t index) { SelectChannel(index); });

    mChannelDropdown->SetDepthValue(1);

    mDeleteButton = EditorUIUtility::CreateSmallTextButton(this, "DeleteCollisionChannelButton");
    mDeleteButton->SetWidth(130.0f);
    mDeleteButton->SetPositionLocal(400.0f, 20.0f);
    mDeleteButton->mTextComponent->SetText("선택 채널 삭제");
    mDeleteButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float)
                                                                           { DeleteSelectedChannel(); });

    CreateLabel(this, "NewChannelLabel", "새 채널", 20.0f, 75.0f);
    mChannelNameInput = EditorUIUtility::CreateTextInput(this, "NewCollisionChannelName");
    mChannelNameInput->SetWidth(260.0f);
    mChannelNameInput->SetPositionLocal(120.0f, 75.0f);
    mAddButton = EditorUIUtility::CreateSmallTextButton(this, "AddCollisionChannelButton");
    mAddButton->SetWidth(100.0f);
    mAddButton->SetPositionLocal(400.0f, 75.0f);
    mAddButton->mTextComponent->SetText("채널 추가");
    mAddButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            if (AddChannel(mChannelNameInput->GetText()))
                mChannelNameInput->SetText("");
        });

    mStatusText = CreateLabel(this, "CollisionChannelStatus", "", 20.0f, 120.0f);
    mStatusText->SetWidth(700.0f);
    mResponseHeader = EditorUIUtility::Create<UI::UIImage>(this, "CollisionResponseHeader");
    EditorUIUtility::ApplyPreset(mResponseHeader, UI::EUIStyleRole::eNone);
    mResponseHeader->SetColor(UI::UIColor::DarkGray);
    mResponseHeader->SetPositionLocal(20.0f, 165.0f);
    mResponseHeader->SetSize(600.0f, 35.0f);
    CreateLabel(mResponseHeader, "TargetChannelLabel", "상대 채널", 10.0f, 5.0f);
    CreateLabel(mResponseHeader, "IgnoreLabel", "Ignore", 270.0f, 5.0f);
    CreateLabel(mResponseHeader, "OverlapLabel", "Overlap", 370.0f, 5.0f);
    CreateLabel(mResponseHeader, "BlockLabel", "Block", 470.0f, 5.0f);

    mResponseScroll = EditorUIUtility::Create<UIScrollBox>(this, "CollisionResponseRows");
    EditorUIUtility::ApplyPreset(mResponseScroll, UI::EUIStyleRole::eNone);
    mResponseScroll->SetLayout(EUIScrollLayout::eVertical);
    mResponseScroll->SetBackgrounColor(UI::UIColor::DarkGray);
    mResponseScroll->SetPositionLocal(20.0f, 200.0f);
}

const CollisionChannelSettingPanel::ChannelEntry *CollisionChannelSettingPanel::FindChannel(uint64_t id) const
{
    const auto it =
        std::find_if(mChannels.begin(), mChannels.end(), [id](const ChannelEntry &entry) { return entry.mID == id; });
    return it == mChannels.end() ? nullptr : &*it;
}

void CollisionChannelSettingPanel::RefreshChannels()
{
    if (!mChannelDropdown)
        return;
    auto *system = Core::CollisionChannelSystem::GetInstance();
    auto names = system->GetAllChannelName();
    // unordered_map의 순회 순서는 UI 순서가 아니다. 선택은 이름 정렬 후에도 ID로 보존한다.
    std::sort(names.begin(), names.end());
    mChannels.clear();
    for (const auto &name : names)
    {
        uint64_t id = 0;
        if (system->GetChannelID(name, id))
            mChannels.push_back({id, name});
    }
    names.clear();
    size_t selectedIndex = 0;
    for (size_t i = 0; i < mChannels.size(); ++i)
    {
        names.push_back(mChannels[i].mName);
        if (mChannels[i].mID == mSelectedChannelID)
            selectedIndex = i;
    }

    mChannelDropdown->SetItemList(names);
    const bool hasChannels = !mChannels.empty();
    mChannelDropdown->SetActiveFlag(hasChannels);
    mDeleteButton->SetActiveFlag(hasChannels);
    mSelectedChannelID = hasChannels ? mChannels[selectedIndex].mID : 0;
    if (hasChannels)
        mChannelDropdown->SetSelectedIndex(selectedIndex, false);
    // 기존 행은 지연 삭제하고 참조를 비운다. 새 행의 콜백은 ID만 캡처한다.
    RebuildResponseRows();
    RefreshResponseValues();
    if (!hasChannels)
        mStatusText->SetText("등록된 채널이 없습니다. 새 채널을 추가하세요.");
    else
        mStatusText->SetText("변경 내용은 프로젝트 저장 시 저장됩니다.");
}

void CollisionChannelSettingPanel::RebuildResponseRows()
{
    // Destroy는 DEAD 표시만 하며 실제 해제는 UIManager가 한다. 즉시 삭제하거나 포인터를 재사용하지 않는다.
    mResponseScroll->RemoveItemAll();
    mResponseRows.clear();
    for (const auto &channel : mChannels)
    {
        ChannelResponseRow row;
        row.mTargetChannelID = channel.mID;
        row.mRowPanel = EditorUIUtility::Create<UI::UIImage>(this, "CollisionResponseRow");
        EditorUIUtility::ApplyPreset(row.mRowPanel, UI::EUIStyleRole::eNone);
        row.mRowPanel->SetColor(UI::UIColor::DarkGray);
        row.mRowPanel->SetSize(600.0f, 45.0f);
        CreateLabel(row.mRowPanel, "ChannelNameText", channel.mName, 10.0f, 10.0f);

        row.mIgnoreBox = EditorUIUtility::CreateBoolField(row.mRowPanel, "IgnoreBox");
        row.mOverlapBox = EditorUIUtility::CreateBoolField(row.mRowPanel, "OverlapBox");
        row.mBlockBox = EditorUIUtility::CreateBoolField(row.mRowPanel, "BlockBox");
        UIBoolPanel *boxes[] = {row.mIgnoreBox, row.mOverlapBox, row.mBlockBox};
        const ECollisionResponseType responses[] = {ECollisionResponseType::eIgnore, ECollisionResponseType::eOverlap,
                                                    ECollisionResponseType::eBlock};
        for (size_t i = 0; i < 3; ++i)
        {
            boxes[i]->SetTagText("");
            boxes[i]->SetWidth(100.0f);
            boxes[i]->SetPositionLocal(220.0f + 100.0f * static_cast<float>(i), 0.0f);
            boxes[i]->mOnValueChanged.Register(
                [this, id = channel.mID, response = responses[i]](bool)
                {
                    // 독립된 bool 세 개가 아닌 단일 Response다. 재클릭으로 모두 해제되지 않도록 선택을 복원한다.
                    ChangeResponse(id, response);
                });
        }
        mResponseScroll->AddItem(row.mRowPanel);
        mResponseRows.push_back(row);
    }
    mResponseScroll->ForceUpdateLayout();
}

void CollisionChannelSettingPanel::SelectChannel(size_t index)
{
    if (index >= mChannels.size())
        return;
    mSelectedChannelID = mChannels[index].mID;
    RefreshResponseValues();
}

void CollisionChannelSettingPanel::RefreshResponseValues()
{
    const auto *selected = FindChannel(mSelectedChannelID);
    Core::CollisionChannelInfo info;
    if (!selected || !Core::CollisionChannelSystem::GetInstance()->GetCollisionChannelInfo(selected->mName, info))
        return;
    for (auto &row : mResponseRows)
    {
        const auto it = info.mResponseTypeTable.find(row.mTargetChannelID);
        const auto response = it == info.mResponseTypeTable.end() ? ECollisionResponseType::eIgnore : it->second;
        // 표시 갱신은 알림 없이 처리하여 다른 체크 해제에서 변경 콜백이 연쇄 호출되지 않게 한다.
        row.mIgnoreBox->SetCheckValue(response == ECollisionResponseType::eIgnore, false);
        row.mOverlapBox->SetCheckValue(response == ECollisionResponseType::eOverlap, false);
        row.mBlockBox->SetCheckValue(response == ECollisionResponseType::eBlock, false);
    }
}

void CollisionChannelSettingPanel::ChangeResponse(Core::CollisionChannelID targetChannelID,
                                                  ECollisionResponseType response)
{
    const auto *selected = FindChannel(mSelectedChannelID);
    const auto *target = FindChannel(targetChannelID);
    if (!selected || !target)
        return;
    // 표시 텍스트/UI 인스턴스 이름이 아니라 채널 목록의 실제 이름을 기존 시스템 API에 전달한다.
    if (!Core::CollisionChannelSystem::GetInstance()->ChangeChannelResponse(selected->mName, target->mName, response))
    {
        RefreshChannels();
        mStatusText->SetText("채널 정보가 변경되었습니다. 목록을 다시 불러왔습니다.");
        return;
    }
    RefreshResponseValues();
}

bool CollisionChannelSettingPanel::AddChannel(const std::string &name)
{
    if (!mChannelDropdown)
        return false;
    const auto first = name.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        mStatusText->SetText("새 채널 이름을 입력하세요.");
        return false;
    }
    const std::string channelName = name.substr(first, name.find_last_not_of(" \t\r\n") - first + 1);
    auto *system = Core::CollisionChannelSystem::GetInstance();
    if (!system->CreateNewChannel(channelName))
    {
        mStatusText->SetText("같은 이름의 채널이 이미 있습니다.");
        return false;
    }
    system->GetChannelID(channelName, mSelectedChannelID);
    RefreshChannels();
    return true;
}

bool CollisionChannelSettingPanel::DeleteSelectedChannel()
{
    const auto *selected = FindChannel(mSelectedChannelID);
    if (!selected)
        return false;
    if (!Core::CollisionChannelSystem::GetInstance()->DeleteChannel(selected->mName))
    {
        RefreshChannels();
        mStatusText->SetText("삭제할 채널을 찾을 수 없습니다.");
        return false;
    }
    // 삭제된 ID는 재사용하지 않는다. 남은 첫 항목을 선택하거나 빈 목록 상태로 전환한다.
    mSelectedChannelID = 0;
    RefreshChannels();
    return true;
}

void CollisionChannelSettingPanel::UpdatePanelLayout()
{
    const auto size = GetSize();
    if (!mResponseScroll || (size.X == mLayoutWidth && size.Y == mLayoutHeight))
        return;
    mLayoutWidth = size.X;
    mLayoutHeight = size.Y;
    const float width = std::max(0.0f, size.X - 40.0f);
    mResponseHeader->SetWidth(width);
    mResponseScroll->SetSize(width, std::max(0.0f, size.Y - 220.0f));
    for (auto &row : mResponseRows)
        mResponseScroll->AddItem(row.mRowPanel);
    mResponseScroll->ForceUpdateLayout();
}

void CollisionChannelSettingPanel::Update(float deltaTime)
{
    const bool visible = GetActiveFlag() && GetDestCanvas() && GetDestCanvas()->GetActiveFlag();
    if (!visible)
    {
        mWasVisible = false;
        return;
    }
    // 다른 설정 화면/작업 공간에서 돌아올 때 재조회한다. 매 프레임 목록을 파괴/재생성하지 않는다.
    if (!mWasVisible)
        RefreshChannels();
    mWasVisible = true;
    UpdatePanelLayout();
    UI::UIImage::Update(deltaTime);
}

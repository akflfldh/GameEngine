#pragma once

#include <Core/CollisionChannelSystem.h>
#include <Core/CoreType.h>
#include <UiSystem/UIImage.h>
#include <string>
#include <vector>

#include "CollisionChannelSettingPanel.generated.h"

class UIDropdown;
class UIBoolPanel;
class UIScrollBox;
namespace UI
{
class UIText;
class UITextButton;
class UIEditBox;
} // namespace UI

// 충돌 채널의 선택/추가/삭제와 반응표 편집을 담당하는 프로젝트 설정 콘텐츠다.
// 실제 채널과 반응값은 CollisionChannelSystem이 소유하며, 이 패널은 선택 ID와 표시용 목록만 유지한다.
// 행과 자식 UI의 수명은 Canvas/UIManager가 관리한다. 저장은 기존 프로젝트 저장 경로에 맡긴다.
class REFLECT_CLASS(EngineClass) CollisionChannelSettingPanel : public UI::UIImage
{
    GENERATED_BODY(CollisionChannelSettingPanel)
  public:
    CollisionChannelSettingPanel();
    ~CollisionChannelSettingPanel() override;
    void OnBegin() override;
    void Update(float deltaTime) override;

    void RefreshChannels();
    bool AddChannel(const std::string &name);
    bool DeleteSelectedChannel();

  private:
    struct ChannelEntry
    {
        Core::CollisionChannelID mID = 0;
        std::string mName;
    };

    // 상대 채널 ID는 정렬/추가/삭제로 변하는 Dropdown 인덱스나 UI 인스턴스 이름과 구분한다.
    // UI 포인터는 비소유 참조이며 행을 재구성할 때 함께 비운다.
    struct ChannelResponseRow
    {
        Core::CollisionChannelID mTargetChannelID = 0;
        UI::UIImage *mRowPanel = nullptr;
        UIBoolPanel *mIgnoreBox = nullptr;
        UIBoolPanel *mOverlapBox = nullptr;
        UIBoolPanel *mBlockBox = nullptr;
    };

    void CreateControls();
    void RebuildResponseRows();
    void RefreshResponseValues();
    void SelectChannel(size_t index);
    void ChangeResponse(Core::CollisionChannelID targetChannelID, ECollisionResponseType response);
    void UpdatePanelLayout();
    const ChannelEntry *FindChannel(Core::CollisionChannelID id) const;

    UIDropdown *mChannelDropdown = nullptr;
    UI::UIEditBox *mChannelNameInput = nullptr;
    UI::UITextButton *mAddButton = nullptr;
    UI::UITextButton *mDeleteButton = nullptr;
    UI::UIText *mStatusText = nullptr;
    UI::UIImage *mResponseHeader = nullptr;
    UIScrollBox *mResponseScroll = nullptr;
    std::vector<ChannelEntry> mChannels;
    std::vector<ChannelResponseRow> mResponseRows;
    uint64_t mSelectedChannelID = 0;
    float mLayoutWidth = -1.0f;
    float mLayoutHeight = -1.0f;
    bool mWasVisible = false;
};

#pragma once

#include <string>
#include <vector>

namespace UI
{
class UICanvas;
class UIElement;
class UIImage;
class UIText;
class UITextButton;
class UIVerticalLayoutComponent;

} // namespace UI

// 프로젝트 설정 화면의 목록과 콘텐츠 전환만 담당한다.
// UI 수명은 Canvas/UIManager가 관리하며, 설정 데이터나 CollisionChannelSystem은 소유하지 않는다.
class ProjectSettingUIController
{
  public:
    void Initialize(UI::UICanvas *canvas);
    void UpdateLayout();
    // 같은 Canvas에서 생성한 패널을 등록한다. 실제 설정 읽기/수정은 해당 패널의 담당자가 연결한다.
    bool AddSettingPanel(const std::string &name, UI::UIElement *panel);
    bool SelectSetting(const std::string &name);

  private:
    void CreateCollisionChannelSettingPanel();
    void CreateMapSettingPanel();

  private:
    struct SettingEntry
    {
        std::string mName;
        UI::UIElement *mPanel = nullptr;
        UI::UITextButton *mButton = nullptr;
    };

    UI::UICanvas *mCanvas = nullptr;
    UI::UIImage *mToolbar = nullptr;
    UI::UIImage *mSettingListPanel = nullptr;
    // 배경 높이는 창에 맞춰 유지하고, 내부 버튼 목록만 자식 높이의 합으로 늘어난다.
    UI::UIImage *mSettingButtonListPanel = nullptr;
    UI::UIVerticalLayoutComponent *mSettingListLayout = nullptr;
    UI::UIImage *mContentPanel = nullptr;
    UI::UIText *mEmptyMessage = nullptr;
    std::vector<SettingEntry> mSettings;
    float mLayoutWidth = -1.0f;
    float mLayoutHeight = -1.0f;
};

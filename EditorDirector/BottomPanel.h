#pragma once

#include <UiSystem/UIImage.h>
#include <string>
#include <vector>

#include "BottomPanel.generated.h"

namespace UI
{
class UITextButton;
class UIVerticalLayoutComponent;
}
namespace QuadLF
{
class LogicalFolder;
}
class UIAssetBrowser;
class LogPanel;

// 에디터 하단의 공통 높이 조절과 콘텐츠 전환을 담당한다.
// UIManager가 자식 UI의 수명을 관리하며, 이 패널은 자식의 배치와 활성 상태만 제어한다.
// Asset Browser의 파일 처리와 LogPanel의 메시지 표시는 각 콘텐츠가 담당한다.
class REFLECT_CLASS(EngineClass) BottomPanel : public UI::UIImage
{
    GENERATED_BODY(BottomPanel)
  public:
    BottomPanel();
    virtual ~BottomPanel() override;
    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;
    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

    void SetInitFolder(QuadLF::LogicalFolder *folder);
    UIAssetBrowser *GetAssetBrowser() const;
    LogPanel *GetLogPanel() const;

    // 같은 Canvas의 콘텐츠를 전달한다. ContentPanel 아래로 연결하며 패널을 직접 삭제하지 않는다.
    bool AddContent(const std::string &name, UI::UIElement *panel);
    bool SetActiveContent(const std::string &name);
    void ResizeContentHeight(float deltaY);

  private:
    void CreateResizeHeightHandle();
    void UpdatePanelLayout();

    struct ContentEntry
    {
        std::string mName;
        UI::UIElement *mPanel = nullptr;
        UI::UITextButton *mButton = nullptr;
    };

    UI::UIVerticalLayoutComponent *mVerticalLayoutComponent = nullptr;
    UI::UIImage *mResizeHeightHandle = nullptr;
    UI::UIImage *mTabPanel = nullptr;
    UI::UIElement *mContentPanel = nullptr;
    UIAssetBrowser *mAssetBrowser = nullptr;
    LogPanel *mLogPanel = nullptr;
    QuadLF::LogicalFolder *mInitFolder = nullptr;
    std::vector<ContentEntry> mContents;
    UI::UIElement *mActiveContent = nullptr;

    // 콘텐츠의 높이는 탭 선택과 무관하다. 세로 레이아웃은 핸들·탭·콘텐츠 영역의 높이만 합산한다.
    float mContentHeight = 500.0f;
    float mMinContentHeight = 100.0f;
    float mMaxContentHeight = 1000.0f;
    float mResizeHandleHeight = 8.0f;
    float mTabHeight = 32.0f;
    bool mUpdatingLayout = false;
    bool mPanelLayoutDirty = true;
    float mLayoutWidth = -1.0f;
    float mLayoutContentHeight = -1.0f;
};

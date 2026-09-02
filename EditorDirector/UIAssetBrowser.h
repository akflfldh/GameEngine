#pragma once

#include <UiSystem/UIElement.h>
#include <vector>

#include "UIVerticalLayoutComponent.generated.h"

namespace UI
{
class UIVerticalLayoutComponent;
class UIEditBox;
} // namespace UI

namespace QuadLF
{
class LogicalFolder;
class LogicalNode;
class LogicalFile;
} // namespace QuadLF

class UIScrollBox;
class UIDirectoryTree;
class UIFileItem;
class UISelectableComponent;
class UISplitterPanel;
class REFLECT_CLASS(EngineClass) UIAssetBrowser : public UI::UIElement
{
    GENERATED_BODY(UIAssetBrowser)
  public:
    UIAssetBrowser();
    virtual ~UIAssetBrowser();
    virtual void Update(float deltaTime);

    void SetInitFolder(QuadLF::LogicalFolder *folder);
    void SelectFolderProgrammtically(QuadLF::LogicalFolder *newFolder);

    void ResizeBrowserHeight(float deltaY);

    void SetMaxBodyHeight(float y);
    void SetMinBodyHeight(float y);

  private:
    virtual void OnBegin() override;

    void CreateBrowserResizeHeightHandle();
    void CreateToolbar();
    void CreateNavigationBar();
    void CreateBody();

    void OnSelectedFile(QuadLF::LogicalNode *file);

    void OnSelectedNewFolder(QuadLF::LogicalFolder *newFolder);

    void NavigateBack(float x, float y);
    void NavigateForward(float x, float y);
    void NavigateToFolder(QuadLF::LogicalFolder *folder, bool addToHistory);

    void FinishEditingFileItem();

    void OpenFileItem(UIFileItem *fileItem);

    void OnClickedFileItemText(UIFileItem *fileItem, float mousePosX, float mousePosY);
    void OnSelectedFileItem(bool flag, UISelectableComponent *selectableCom);

    UIFileItem *CreateFileItem();

    void OnCreatedFile(QuadLF::LogicalFile *newFile, QuadLF::LogicalFolder *parentFolder);
    void OnRemovedFile(QuadLF::LogicalFile *file, QuadLF::LogicalFolder *preParentFolder);
    void OnDraggingResizeHandle(float deltaX, float deltaY);

  private:
    UI::UIVerticalLayoutComponent *mVerticalLayoutComponent;
    UIScrollBox *mFilePanel;
    UIDirectoryTree *mDirectoryTreePanel;
    UISplitterPanel *mBodyPanel;
    UIElement *mToolbar;
    UIElement *mNavigationBar;

    float mToolbarMaxHeight = 50.0f;
    float mBodyMaxHeight = 400.0f;
    float mBodyMinHeight = 200.0f;

    std::vector<QuadLF::LogicalFolder *> mNavigationHistoryFolderList;
    int mCurrentHistoryIndex = -1;

    UI::UIEditBox *mGlobalFileEditBox;

    // 현재 선택된&&  텍스트 수정중인 파일아이템
    UIFileItem *mCurrEditingFileItem;
    UISelectableComponent *mFileItemSelectableCom;

    QuadLF::LogicalFolder *mCurrFolder = nullptr;
};

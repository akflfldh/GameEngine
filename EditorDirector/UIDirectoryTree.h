#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>
#include <vector>

#include "UIDirectoryTree.generated.h"

namespace QuadLF
{
class LogicalFileSystem;
class LogicalFolder;
class LogicalNode;
} // namespace QuadLF

class UITreeNode;

using OnSelectedNewFolderCallbackSystem = Core::MultiCallbackSystem<QuadLF::LogicalNode *>;

class REFLECT_CLASS(EngineClass) UIDirectoryTree : public UI::UIElement
{
    GENERATED_BODY(UIDirectoryTree)

  public:
    UIDirectoryTree();
    virtual ~UIDirectoryTree();
    virtual void Update(float deltaTime) override;
    virtual void OnBegin() override;

    void SetFileSystem(QuadLF::LogicalFileSystem *fileSystem);

    void InvalidateLayout();

    // void UpdateTree(fileSystem);

    void SetSelectedNode(UITreeNode *node);

    // SetSelectedNode의 콜백 비호출 버전 (AssetBrowser가 사용)
    void SetSelectedFolderProgrammatically(QuadLF::LogicalFolder *folder);

    OnSelectedNewFolderCallbackSystem mOnSelectedNewFolderCallbackSystem;

    UITreeNode *FindTreeNodeByFolder(QuadLF::LogicalFolder *folder);
    virtual bool IsPointInsideDefault(float x, float y) const;

  private:
    void RefreshLayout();

    void OnAddedFolder(QuadLF::LogicalFolder *newNode, QuadLF::LogicalFolder *parentNod);

    void RebuildTree();

    QuadLF::LogicalFileSystem *mFileSystem;
    std::vector<UITreeNode *> mRootNodes;

    UITreeNode *mSelectedTreeNode;

    bool mDirtyFlag;
};

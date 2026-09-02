#include "UIDirectoryTree.h"
#include "UITreeNode.h"
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <LogicalFileSystem/LogicalNode.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElementPtr.h>
#include <stack>
UIDirectoryTree::UIDirectoryTree() : mDirtyFlag(true), mSelectedTreeNode(nullptr) {}

UIDirectoryTree::~UIDirectoryTree() {}

void UIDirectoryTree::Update(float deltaTime)
{

    if (mDirtyFlag)
    {
        RefreshLayout();
    }
}

void UIDirectoryTree::OnBegin()
{

    RefreshLayout();
}

void UIDirectoryTree::SetFileSystem(QuadLF::LogicalFileSystem *fileSystem)
{

    mFileSystem = fileSystem;
    mFileSystem->mOnCreatedFolderCallbackSystem.Register(this, &UIDirectoryTree::OnAddedFolder);
    RebuildTree();
}

void UIDirectoryTree::InvalidateLayout()
{

    mDirtyFlag = true;
}

void UIDirectoryTree::SetSelectedNode(UITreeNode *node)
{
    // 같은노드 선택은 무시
    if (node == mSelectedTreeNode)
        return;

    // 기존에 선택되었더 tree node  - >false
    if (mSelectedTreeNode)
    {
        mSelectedTreeNode->SetSelected(false);
    }
    mSelectedTreeNode = node;
    mSelectedTreeNode->SetSelected(true);

    // BrowserPanel에게 알린다 - > FilePanel로 전달
    if (node)
        mOnSelectedNewFolderCallbackSystem.ExecuteCallbacks(node->GetDestLogicalFolder());
}

void UIDirectoryTree::SetSelectedFolderProgrammatically(QuadLF::LogicalFolder *folder)
{

    // 찾는다
    UITreeNode *node = FindTreeNodeByFolder(folder);

    if (node == nullptr)
        return;

    // 같은노드 선택은 무시
    if (node == mSelectedTreeNode)
        return;

    // 기존에 선택되었더 tree node  - >false
    if (mSelectedTreeNode)
    {
        mSelectedTreeNode->SetSelected(false);
    }
    mSelectedTreeNode = node;
    mSelectedTreeNode->SetSelected(true);
}

UITreeNode *UIDirectoryTree::FindTreeNodeByFolder(QuadLF::LogicalFolder *folder)
{
    std::queue<UITreeNode *> queue;

    for (auto rootNode : mRootNodes)
    {
        queue.push(rootNode);
    }

    while (!queue.empty())
    {
        auto treeNode = queue.front();
        queue.pop();

        if (treeNode->GetDestLogicalFolder() == folder)
            return treeNode;

        const std::vector<UITreeNode *> &children = treeNode->GetChildNodes();
        for (auto child : children)
        {
            queue.push(child);
        }
    }

    return nullptr;
}

bool UIDirectoryTree::IsPointInsideDefault(float x, float y) const
{

    bool ret = UIElement::IsPointInsideDefault(x, y);
    int a = 2;
    return ret;
}

void UIDirectoryTree::RefreshLayout()
{

    float currentY = 0.0f;

    for (auto node : mRootNodes)
    {

        node->SetPositionLocal(0, currentY);
        float childHeight = node->LayoutRecursive();
        currentY += childHeight;
    }

    SetHeight(currentY);

    mDirtyFlag = false;
}

void UIDirectoryTree::OnAddedFolder(QuadLF::LogicalFolder *newFolderNode, QuadLF::LogicalFolder *parentNode)
{

    UITreeNode *parentFolderNode = FindTreeNodeByFolder(parentNode);
    if (parentFolderNode == nullptr)
    {
        return;
    }

    // 관계 설정
    auto canvas = GetDestCanvas();
    auto childTreeNode = canvas->CreateUIElement<UITreeNode>(newFolderNode->GetName().c_str());
    childTreeNode->SetParent(parentFolderNode);
    childTreeNode->SetDestDirectoryTree(this);
    childTreeNode->SetSize(600, 50);
    childTreeNode->SetItemHeight(50.0f);
    childTreeNode->SetLogicalFolder(newFolderNode);
    (parentFolderNode)->AddChildNode(childTreeNode);

    mDirtyFlag = true;
}

void UIDirectoryTree::RebuildTree()
{
    mRootNodes.clear();

    auto canvas = GetDestCanvas();
    auto rootFolder = mFileSystem->GetRootFolder();
    std::queue<QuadLF::LogicalFolder *> queue;
    std::queue<UITreeNode *> treeNodeQueue;

    // 실제 루프폴더들 rootFolder 이건은 최상위 가상 루트폴더
    std::vector<QuadLF::LogicalFolder *> rootFolderList;
    //= rootFolder->GetChildFolderNodeList();

    rootFolderList.push_back(mFileSystem->GetEngineFolder());
    rootFolderList.push_back(mFileSystem->GetAssetFolder());
    rootFolderList.push_back(mFileSystem->GetCXXFolder());

    for (auto folder : rootFolderList)
    {
        auto treeNode = canvas->CreateUIElement<UITreeNode>(folder->GetName().c_str());
        queue.push(folder);
        treeNodeQueue.push(treeNode);

        treeNode->SetParent(this);
        treeNode->SetDestDirectoryTree(this);
        treeNode->SetItemHeight(50.0f);
        treeNode->SetLogicalFolder(folder);

        mRootNodes.push_back(treeNode);
    }

    while (!queue.empty())
    {
        auto folder = queue.front();
        queue.pop();

        auto treeNodeParent = treeNodeQueue.front();
        treeNodeQueue.pop();

        const std::vector<QuadLF::LogicalFolder *> &childFolderList = folder->GetChildFolderNodeList();

        for (auto childFolder : childFolderList)
        {
            auto childTreeNode = canvas->CreateUIElement<UITreeNode>(childFolder->GetName().c_str());
            childTreeNode->SetItemHeight(50.0f);
            childTreeNode->SetLogicalFolder(childFolder);
            childTreeNode->SetDestDirectoryTree(this);

            treeNodeParent->AddChildNode(childTreeNode);
            queue.push(childFolder);
            treeNodeQueue.push(childTreeNode);
        }
    }

    InvalidateLayout();
}

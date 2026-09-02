#pragma once

#include <UiSystem/UIElement.h>

#include <vector>

#include "UITreeNode.generated.h"

namespace QuadLF
{
class LogicalFolder;
}

namespace UI
{
class UIImage;
class UIButton;
} // namespace UI

// TODO line height, text font size등 상수로 고정해서 사용하자

class UIDirectoryTree;

class REFLECT_CLASS(EngineClass) UITreeNode : public UI::UIElement
{
    GENERATED_BODY(UITreeNode)
  public:
    UITreeNode();
    virtual ~UITreeNode();
    virtual void Update(float deltaTime) override;

    virtual void OnBegin() override;

    void UpdateLayout();

    void SetItemHeight(float h);
    void SetItemWidth(float w);

    // return : 자식,자손들의 모든 높이합
    float LayoutRecursive();

    void ExpandTree(float x, float y);
    void SetDestDirectoryTree(UIDirectoryTree *tree);

    void AddChildNode(UITreeNode *childNode);
    void SetLogicalFolder(QuadLF::LogicalFolder *logicalFolder);
    QuadLF::LogicalFolder *GetDestLogicalFolder() const;

    const std::vector<UITreeNode *> &GetChildNodes() const;

    // Directory가 호출한다.
    void SetSelected(bool flag);

  private:
    void Init();
    void CreateHeader();
    void CreateChildrenPanel();

    void UpdateText();

    void OnHeaderClicked(float x, float y);

  private:
    UI::UIElement *mHeader;
    UI::UIElement *mChildrenPanel;
    UI::UIButton *mHeaderExpandButton;
    UI::UIImage *mHeaderIcon;
    UI::UIElement *mHeaderText;
    std::vector<UITreeNode *> mChildrenNodes;

    UIDirectoryTree *mDestDirectoryTree;

    QuadLF::LogicalFolder *mDestLogicalFolder;
    bool mIsExpanded = false;

    bool mInit = false;
    float mItemWidth = 600.0f;
    float mItemHeight = 20.0f;
};

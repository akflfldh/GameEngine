#include "UITreeNode.h"
#include <EditorDirector/UIDirectoryTree.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElementPtr.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UITextComponent.h>
UITreeNode::UITreeNode() : mHeaderExpandButton(nullptr), mHeaderIcon(nullptr), mHeaderText(nullptr), mIsExpanded(false)
{
}

UITreeNode::~UITreeNode() {}

void UITreeNode::Update(float deltaTime) {}

void UITreeNode::OnBegin()
{

    // CreateChildrenBody
    //  UpdateLayout();

    Init();
}
void UITreeNode::Init()
{
    if (mInit)
        return;

    CreateHeader();
    CreateChildrenPanel();
    UpdateLayout();

    SetWidth(mItemWidth);
    mInit = true;
}
void UITreeNode::CreateHeader()
{

    auto canvas = GetDestCanvas();

    auto Header = canvas->CreateUIElement<UI::UIElement>("Header");

    auto headerImageCom = Header->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    headerImageCom->NotUseTexture();
    headerImageCom->SetColor(1.0f, 0.3f, 0.3f);

    auto headerButtonCom = Header->CreateUIComponent<UI::UIButtonComponent>("buttonCom");
    headerButtonCom->mButtonClickCallbackSystem.Register(this, &UITreeNode::OnHeaderClicked);

    Header->SetParent(this);
    Header->SetPositionLocal(0, 0);
    Header->SetSize(mItemWidth, mItemHeight);

    mHeader = Header;

    // Expand Button
    auto ExpandButton = canvas->CreateUIElement<UI::UIButton>("ExpandButton");
    ExpandButton->mUIImageComponent->UseTexture();
    ExpandButton->mUIImageComponent->SetTexture("Engine/ArrowRight");

    ExpandButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(this, &UITreeNode::ExpandTree);

    ExpandButton->SetPositionLocal(5, 5);
    ExpandButton->SetSize(mItemHeight - 10, mItemHeight - 10);
    ExpandButton->SetParent(Header);
    mHeaderExpandButton = ExpandButton;

    // Folder Icon
    auto FolderIcon = canvas->CreateUIElement<UI::UIImage>("Icon");
    //    FolderIcon->mImageCom->NotUseTexture();
    FolderIcon->mImageCom->UseTexture();
    FolderIcon->mImageCom->SetTexture("Engine/Folder");

    FolderIcon->SetSize(mItemHeight - 10, mItemHeight - 10);
    // FolderIcon->mImageCom->SetColor(0, 0, 1.0f);
    FolderIcon->SetParent(Header);
    FolderIcon->SetPositionLocal(mItemHeight, 5);
    mHeaderIcon = FolderIcon;
    mHeaderIcon->SetOnlyVisible(true);

    // Text
    auto TextPanel = canvas->CreateUIElement<UI::UIElement>("Text");
    auto TextCom = TextPanel->CreateUIComponent<UI::UITextComponent>("TextCom");

    TextPanel->SetSize(200, mItemHeight);
    TextPanel->SetPositionLocal(mItemHeight * 2, 0);

    TextCom->SetFontSize(mItemHeight);
    TextCom->SetText("Test TextPanel");

    TextPanel->SetParent(Header);
    TextPanel->SetOnlyVisible(true);

    mHeaderText = TextPanel;
}

void UITreeNode::CreateChildrenPanel()
{

    auto canvas = GetDestCanvas();

    mChildrenPanel = canvas->CreateUIElement<UI::UIElement>("ChildrenPanel");

    // auto bodyImageCom = Header->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    // // headerImageCom->NotUseTexture();
    //   headerImageCom->SetColor(0.3f, 0.3f, 0.3f);

    mChildrenPanel->SetParent(this);
    mChildrenPanel->SetPositionLocal(0, mItemHeight);
    mChildrenPanel->SetSize(0, 0);
}
void UITreeNode::UpdateText()
{

    if (mDestLogicalFolder)
    {
        UI::UITextComponent *texCom = nullptr;
        mHeaderText->GetComponents(&texCom, 1);

        texCom->SetText(mDestLogicalFolder->GetName());
    }
}
void UITreeNode::OnHeaderClicked(float x, float y)
{

    // Directory에게 알린다.
    if (mDestDirectoryTree)
    {
        mDestDirectoryTree->SetSelectedNode(this);
    }
}
void UITreeNode::SetItemHeight(float h)
{
    mItemHeight = h;
}
void UITreeNode::SetItemWidth(float w)
{

    mItemWidth = w;
}
float UITreeNode::LayoutRecursive()
{
    Init();

    // header 는항상 맨위
    mHeader->SetHeight(mItemHeight);

    mHeaderExpandButton->SetSize(mItemHeight - 10, mItemHeight - 10);
    mHeaderExpandButton->SetPositionLocal(5, 5);

    mHeaderIcon->SetSize(mItemHeight - 10, mItemHeight - 10);
    mHeaderIcon->SetPositionLocal(mItemHeight, 5);

    mHeaderText->SetSize(200, mItemHeight);
    mHeaderText->SetPositionLocal(mItemHeight * 2, 0);

    UI::UITextComponent *textCom = nullptr;
    mHeaderText->GetComponents<UI::UITextComponent>(&textCom, 1);

    if (textCom)
    {
        textCom->SetFontSize(mItemHeight - 20);
    }

    float currentLocalY = mItemHeight;

    if (mIsExpanded && !mChildrenNodes.empty())
    {
        // 자식 패널의 시작 위치는 헤더 바로 밑
        mChildrenPanel->SetPositionLocal(0, mItemHeight);

        float panelHeight = 0.0f;

        for (UITreeNode *child : mChildrenNodes)
        {
            child->SetPositionLocal(0, panelHeight);

            float childHeight = child->LayoutRecursive();

            panelHeight += childHeight;
        }

        mChildrenPanel->SetHeight(panelHeight);

        // 내 전체 높이에 추가
        currentLocalY += panelHeight;
    }
    else
    {
        mChildrenPanel->SetHeight(0.0f);
    }

    SetHeight(currentLocalY);

    return currentLocalY;
}
void UITreeNode::ExpandTree(float x, float y)
{
    mIsExpanded = !mIsExpanded;

    if (mIsExpanded)
    {
        mHeaderExpandButton->mUIImageComponent->SetTexture("Engine/ArrowDown");
    }
    else
    {
        mHeaderExpandButton->mUIImageComponent->SetTexture("Engine/ArrowRight");
    }

    // DirectoryTree  API호출해서 Layout update
    mDestDirectoryTree->InvalidateLayout();
}
void UITreeNode::SetDestDirectoryTree(UIDirectoryTree *tree)
{

    mDestDirectoryTree = tree;
}
void UITreeNode::AddChildNode(UITreeNode *childNode)
{

    childNode->SetParent(this);
    mChildrenNodes.push_back(childNode);
}

void UITreeNode::SetLogicalFolder(QuadLF::LogicalFolder *logicalFolder)
{

    mDestLogicalFolder = logicalFolder;
    UpdateText();
}

QuadLF::LogicalFolder *UITreeNode::GetDestLogicalFolder() const
{
    return mDestLogicalFolder;
}

const std::vector<UITreeNode *> &UITreeNode::GetChildNodes() const
{
    return mChildrenNodes;
    // TODO: 여기에 return 문을 삽입합니다.
}

void UITreeNode::SetSelected(bool flag)
{
    UI::UIImageComponent *imageCom = nullptr;
    mHeader->GetComponents<UI::UIImageComponent>(&imageCom, 1);

    if (flag)
    {

        if (imageCom)
        {
            imageCom->SetColor(0.2f, 0.2f, 0.2f);
        }
    }
    else
    {
        if (imageCom)
        {
            imageCom->SetColor(1.0f, 0.3f, 0.3f);
        }
    }
}

void UITreeNode::UpdateLayout()
{

    SetSize(mItemWidth, mItemHeight * (1));
}

#include "UIAssetBrowser.h"
#include <Core/Prefab.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/GlobalOverlayManager.h>
#include <EditorDirector/UIFileItem.h>
#include <EditorDirector/UIGridLayoutComponent.h>
#include <EditorDirector/UIScrollBox.h>
#include <EditorDirector/UISelectableComponent.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
#include <PrefabWorkSpaceManager.h>
#include <UIDirectoryTree.h>
#include <UISplitterPanel.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIElementPtr.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMouseDragComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UITextInputComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
UIAssetBrowser::UIAssetBrowser()
    : mFilePanel(nullptr), mDirectoryTreePanel(nullptr), mGlobalFileEditBox(nullptr), mCurrEditingFileItem(nullptr),
      mFileItemSelectableCom(nullptr), mBodyPanel(nullptr), mBodyMinHeight(1.0f), mToolbar(nullptr)
{

    mVerticalLayoutComponent = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

UIAssetBrowser::~UIAssetBrowser() {}

void UIAssetBrowser::OnBegin()
{
    UI::UIElement::OnBegin();

    auto logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();
    logicalFileSystem->mOnCreatedFileCallbackSystem.Register(
        [this](QuadLF::LogicalFile *file, QuadLF::LogicalFolder *parentFolder) { OnCreatedFile(file, parentFolder); });
    logicalFileSystem->mOnRemovedFileCallbackSystem.Register(
        [this](QuadLF::LogicalFile *file, QuadLF::LogicalFolder *parentFolder) { OnRemovedFile(file, parentFolder); });

    CreateBrowserResizeHeightHandle();
    CreateToolbar();

    CreateNavigationBar();
    CreateBody();

    mVerticalLayoutComponent->CalculateLayout();

    if (mCurrFolder)
        SelectFolderProgrammtically(mCurrFolder);
}
void UIAssetBrowser::Update(float deltaTime)
{
    UI::UIElement::Update(deltaTime);
}

void UIAssetBrowser::SelectFolderProgrammtically(QuadLF::LogicalFolder *newFolder)
{
    OnSelectedNewFolder(newFolder);
}

void UIAssetBrowser::CreateBrowserResizeHeightHandle()
{

    auto resizeHandle = CreateChildUIElement<UI::UIElement>("ResizeHandle");
    auto imageCom = resizeHandle->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    auto dragCom = resizeHandle->CreateUIComponent<UI::UIMouseDragComponent>("dragCom");

    dragCom->mOnHoverCallbackSystem.Register([imageCom]() { imageCom->SetColor(0.5f, 0.5f, 0.5f); });
    dragCom->mOnReleaseHoverCallbackSystem.Register([imageCom]() { imageCom->SetColor(0.3f, 0.3f, 0.3f); });

    dragCom->mOnDragStartedCallbackSystem.Register([imageCom]() { imageCom->SetColor(0.7f, 0.7f, 0.7f); });
    dragCom->mOnDraggedCallbackSystem.Register([this](const UI::UIMouseDragContext &context)
                                               { OnDraggingResizeHandle(context.mDeltaX, context.mDeltaY); });
    dragCom->mOnDragEndededCallbackSystem.Register([imageCom]() { imageCom->SetColor(0.5f, 0.5f, 0.5f); });

    imageCom->NotUseTexture();
    imageCom->SetColor(0.3, 0.3, 0.3);
    float width = mTransform.GetSize().x;
    resizeHandle->SetSize(width, 8);
    resizeHandle->SetPositionLocal(0, 0);

    //   toolbar->SetParent(this);
}

void UIAssetBrowser::CreateToolbar()
{
    float width = mTransform.GetSize().x;

    auto canvas = GetDestCanvas();
    auto toolbar = canvas->CreateUIElement<UI::UIElement>("Toolbar");
    auto imageCom = toolbar->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    imageCom->NotUseTexture();
    imageCom->SetColor(0.2, 0.2, 0.2);

    toolbar->SetSize(width, mToolbarMaxHeight);
    toolbar->SetPositionLocal(0, 0);

    toolbar->SetParent(this);
    mToolbar = toolbar;
}

void UIAssetBrowser::CreateNavigationBar()
{
    float width = mTransform.GetSize().x;

    auto canvas = GetDestCanvas();
    auto bar = canvas->CreateUIElement<UI::UIElement>("Navigationobar");
    auto imageCom = bar->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    imageCom->NotUseTexture();
    imageCom->SetColor(0.27, 0.27, 0.27);

    bar->SetSize(width, mToolbarMaxHeight);

    auto backButton = bar->CreateChildUIElement<UI::UIButton>("BackButton");
    auto forwardButton = bar->CreateChildUIElement<UI::UIButton>("ForwardButton");

    backButton->SetSize(40, 40);
    forwardButton->SetSize(40, 40);

    backButton->SetPositionLocal(10, 0);
    forwardButton->SetPositionLocal(
        backButton->mTransform.GetLocalPosition().r + backButton->mTransform.GetSize().r + 10, 0);

    backButton->mUIImageComponent->UseTexture();
    forwardButton->mUIImageComponent->UseTexture();
    backButton->mUIImageComponent->SetTexture("Engine/ArrowLeft");
    forwardButton->mUIImageComponent->SetTexture("Engine/ArrowRight");

    backButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(this, &UIAssetBrowser::NavigateBack);
    forwardButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(this, &UIAssetBrowser::NavigateForward);

    bar->SetParent(this);
    mNavigationBar = bar;
}

void UIAssetBrowser::CreateBody()
{
    float width = mTransform.GetSize().x;
    auto canvas = GetDestCanvas();
    auto body = canvas->CreateUIElement<UISplitterPanel>("Body");
    auto imageCom = body->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    imageCom->NotUseTexture();
    imageCom->SetColor(0.4, 0.4, 0.4);

    body->SetSize(width, mBodyMaxHeight);
    body->SetPositionLocal(0, 100);

    body->SetParent(this);

    auto folderPanel = canvas->CreateUIElement<UIDirectoryTree>("folderPanel");
    auto folderPanelImageCom = folderPanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    folderPanel->SetFileSystem(QuadLF::LogicalFileSystem::GetInstance());

    folderPanel->SetSize(600, mBodyMaxHeight);
    folderPanelImageCom->NotUseTexture();
    folderPanelImageCom->SetColor(0.35f, 0.35f, 0.35f);

    folderPanel->mOnSelectedNewFolderCallbackSystem.Register(this, &UIAssetBrowser::OnSelectedFile);

    mDirectoryTreePanel = folderPanel;

    auto filePanel = canvas->CreateUIElement<UIScrollBox>("folderPanel");
    //   auto filePanelImageCom = filePanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    //  auto filePanelGridLayoutCom = filePanel->CreateUIComponent<UIGridLayoutComponent>("GridLayoutCom");
    filePanel->SetLayout(EUIScrollLayout::eGrid);

    // filePanelImageCom->NotUseTexture();
    //  filePanelImageCom->SetColor(0.3f, 0.3f, 0.3f);
    filePanel->SetSize(width / 2, 300);

    filePanel->mOnBackgroudClickedCallbackSystem.Register(
        [this]()
        {
            if (mFileItemSelectableCom)
            {
                mFileItemSelectableCom->SetSelect(false, true);
            }
        }

    );

    mFilePanel = filePanel;

    // 순서중요 그리기 화가 순서
    body->SetSecondChildElement(filePanel);
    body->SetFirstChildElement(folderPanel);

    mGlobalFileEditBox = canvas->CreateUIElement<UI::UIEditBox>("GlobalEditBox");
    mGlobalFileEditBox->SetFontSize(20.0f);
    mGlobalFileEditBox->SetSize(50, 50);
    mGlobalFileEditBox->SetActiveFlag(false);
    mGlobalFileEditBox->SetOverflowMode(UI::EUITextOverflowMode::eWordWrap);
    mGlobalFileEditBox->SetUseScissorRect(true);
    mGlobalFileEditBox->SetTextColor(0.0f, 0.0f, 0.0f);
    mGlobalFileEditBox->SetBackgroundColor(1.0f, 1.0f, 1.0f);

    mGlobalFileEditBox->mOnLostKeyboardFocusCallbackSystem.Register([this]() { FinishEditingFileItem(); });
    mBodyPanel = body;
}

void UIAssetBrowser::OnSelectedFile(QuadLF::LogicalNode *node)
{
    if (node->GetNodeType() == QuadLF::ELogicalNodeType::eFolder)
    {
        // 폴더
        NavigateToFolder(static_cast<QuadLF::LogicalFolder *>(node), true);
    }
    else
    {
        // 파일

        QuadLF::LogicalFile *file = static_cast<QuadLF::LogicalFile *>(node);
        CoreAsset::EAssetType assetType = file->GetAssetInfo().mAssetType;

        switch (assetType)
        {

        case CoreAsset::EAssetType::ePrefab:
        {
            auto editorDirector = Quad::EditorDirector::GetInstance();

            Prefab *prefab = static_cast<Prefab *>(
                CoreAsset::AssetManager::GetInstance()->GetAsset<Prefab>(file->GetAssetInfo().mAssetID).Get());

            PrefabWorkSpaceManager::GetInstance()->SetPrefab(prefab);

            editorDirector->ChangeToPrefabEditWorkSpace();
        }
        break;
        case CoreAsset::EAssetType::eMaterial:
        {
            auto editorDirector = Quad::EditorDirector::GetInstance();
            CoreAsset::Material *material =
                static_cast<CoreAsset::Material *>(CoreAsset::AssetManager::GetInstance()
                                                       ->GetAsset<CoreAsset::Material>(file->GetAssetInfo().mAssetID)
                                                       .Get());

            editorDirector->ChangeToMaterialEditWorkSpace(material);
        }
        break;
        }
    }
}

void UIAssetBrowser::OnSelectedNewFolder(QuadLF::LogicalFolder *newFolder)
{
    mCurrFolder = newFolder;

    auto canvas = GetDestCanvas();
    // Item Element 들에대한 Pool 이필요하고
    // 에셋수만큼 FilePanel에 자식 Element를 추가 또는 제거 해줘야한다.

    // 그리고 그 ItemElement에 보여지는 이미지 ,텍스트등을 새로 선택한 폴더의 에셋파일들의 내용으로 교체만한다.

    const std::vector<QuadLF::LogicalNode *> &childNodeList = newFolder->GetChildNodeList();

    int requiredCount = childNodeList.size(); // 새로운 폴더의 파일수

    const auto &itemList = mFilePanel->GetItemList();
    int currentCount = itemList.size(); // 현재 아이템수 (파일수 )

    // item수가 모자라면 생성해서 추기

    if (currentCount < requiredCount)
    {
        size_t createCount = requiredCount - currentCount;

        for (int i = 0; i < createCount; ++i)
        {
            auto item = CreateFileItem();
            if (item == nullptr)
            {
                --i;
                continue;
            }
            mFilePanel->AddItem(item);
        }
    }

    // 위에서 추가해서 무효화되었을수도있기에
    auto &activeUIItems = mFilePanel->GetItemList();

    // . 순회하면서 데이터 교체
    for (size_t i = 0; i < activeUIItems.size(); ++i)
    {

        // TODO
        // 텍스트 요소, 텍스처 등등 교체하게될거다.
        UIFileItem *item = static_cast<UIFileItem *>(activeUIItems[i]);

        if (i < requiredCount)
        {
            // 활성화
            // 데이터 교체
            item->SetLogicalFileNode(childNodeList[i]);
            item->SetActiveFlag(true);
        }
        else
        {
            // 남는거는 비활성화
            item->SetActiveFlag(false);
        }
    }

    //  레이아웃 갱신 요청
    mFilePanel->ForceUpdateLayout();
    if (mVerticalLayoutComponent)
    {
        mVerticalLayoutComponent->CalculateLayout();
    }
}

void UIAssetBrowser::NavigateBack(float x, float y)
{
    if (mCurrentHistoryIndex == 0)
        return;

    mCurrentHistoryIndex -= 1;
    NavigateToFolder(mNavigationHistoryFolderList[mCurrentHistoryIndex], false);
}

void UIAssetBrowser::NavigateForward(float x, float y)
{
    if (mNavigationHistoryFolderList.size() - 1 > mCurrentHistoryIndex)
    {

        ++mCurrentHistoryIndex;
        NavigateToFolder(mNavigationHistoryFolderList[mCurrentHistoryIndex], false);
    }
}

void UIAssetBrowser::NavigateToFolder(QuadLF::LogicalFolder *folder, bool addToHistory)
{
    if (!folder)
        return;

    if (addToHistory)
    {
        // 새로운 폴더라면 이후의 히스토리는 모두 제거
        //
        if (mNavigationHistoryFolderList.size() > 0)
            mNavigationHistoryFolderList.erase(mNavigationHistoryFolderList.begin() + mCurrentHistoryIndex + 1,
                                               mNavigationHistoryFolderList.end());

        mNavigationHistoryFolderList.push_back(folder);
        mCurrentHistoryIndex = mNavigationHistoryFolderList.size() - 1;
    }

    // TreeNode->SetSelectedNewFolder() //노 콜백버전
    mDirectoryTreePanel->SetSelectedFolderProgrammatically(folder);

    OnSelectedNewFolder(folder);
}

void UIAssetBrowser::FinishEditingFileItem()
{
    mGlobalFileEditBox->SetActiveFlag(false);
    mCurrEditingFileItem->mFileTextElement->SetActiveFlag(true);

    // 실제로 이름이 바뀔수있는가 판정해야함.

    bool bCanChangeFileName = false;

    if (bCanChangeFileName)
    {
        // 가능하다면 바뀜,  , 에셋파일일경우 에셋데이터까지 변경
        // 폴더라면, 폴더데이터까지 변경s
        std::string newName = mGlobalFileEditBox->GetText();
        mCurrEditingFileItem->mFileTextElement->SetText(newName);
    }
    else
    {
        std::string message = "you can't change the name :" + mGlobalFileEditBox->GetText();
        GlobalOverlayManager::GetInstance()->ShowMessageBox(message);
    }

    mCurrEditingFileItem = nullptr;
}

void OpenFileItem(UIFileItem *fileItem) {}

void UIAssetBrowser::OnCreatedFile(QuadLF::LogicalFile *newFile, QuadLF::LogicalFolder *parentFolder)
{

    if (mCurrFolder != parentFolder)
        return;

    SelectFolderProgrammtically(parentFolder);
}

void UIAssetBrowser::OnRemovedFile(QuadLF::LogicalFile *file, QuadLF::LogicalFolder *preParentFolder)
{

    if (preParentFolder != mCurrFolder)
        return;

    SelectFolderProgrammtically(preParentFolder);
}

UIFileItem *UIAssetBrowser::CreateFileItem()
{
    auto canvas = GetDestCanvas();
    if (canvas == nullptr)
        return nullptr;

    auto item = canvas->CreateUIElement<UIFileItem>("item");
    //   auto com = item->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    item->mOnFileOpendCallbackSystem.Register([this](QuadLF::LogicalNode *node) { OnSelectedFile(node); });

    // 눌렀을때 Global EditBox로 대체
    item->mFileTextButtonComponent->mButtonClickCallbackSystem.Register([item, this](float x, float y)
                                                                        { OnClickedFileItemText(item, x, y); });

    UISelectableComponent *selectableCom = item->GetSelectableComponent();
    selectableCom->mOnSelectedCallbackSystem.Register([this, selectableCom](bool flag)
                                                      { OnSelectedFileItem(flag, selectableCom); });

    return item;
}

void UIAssetBrowser::OnClickedFileItemText(UIFileItem *fileItem, float mousePosX, float mousePosY)
{
    // 선택된상태인지 확인하고
    if (mFileItemSelectableCom->GetOwnerUIElement() != fileItem)
    {
        return;
    }

    mGlobalFileEditBox->SetActiveFlag(true);
    mGlobalFileEditBox->GetTextInputComponent()->RequestKeyboardFocus();

    mCurrEditingFileItem = fileItem;
    glm::vec2 textPosWorld = fileItem->mFileTextElement->mTransform.GetWorldPosition();
    fileItem->mFileTextElement->SetActiveFlag(false);
    mGlobalFileEditBox->SetPositionWorld(textPosWorld);
    mGlobalFileEditBox->SetActiveFlag(true);
    mGlobalFileEditBox->SetSize(fileItem->mFileTextElement->mTransform.GetSize());
    mGlobalFileEditBox->SetText(fileItem->mFileTextElement->GetTextComponent()->GetText());
    mGlobalFileEditBox->SetCursorPosByWorldPos(mousePosX, mousePosY);
}

void UIAssetBrowser::OnSelectedFileItem(bool flag, UISelectableComponent *selectableCom)
{
    if (flag)
    {
        UISelectableComponent *preSelectableCom = mFileItemSelectableCom;
        mFileItemSelectableCom = selectableCom; // 중요
        if (preSelectableCom)
        {
            preSelectableCom->SetSelect(false, true);
        }
    }
    else
    {
        // 외부요인으로인해 선택해제.
        // 현재 선택된 것이 동일할때만 nullptr처리
        if (mFileItemSelectableCom == selectableCom)
        {
            mFileItemSelectableCom = nullptr;
        }
    }
}

void UIAssetBrowser::SetInitFolder(QuadLF::LogicalFolder *folder)
{
    mCurrFolder = folder;
}

void UIAssetBrowser::ResizeBrowserHeight(float deltaY)
{

    float bodyHeight = std::clamp(mBodyPanel->GetHeight() - deltaY, mBodyMinHeight, mBodyMaxHeight);
    mBodyPanel->SetHeight(bodyHeight);
    mDirectoryTreePanel->SetHeight(bodyHeight);
    mFilePanel->SetHeight(bodyHeight);

    float toolbarHeight = (bodyHeight) / mBodyMaxHeight * mToolbarMaxHeight;
    mToolbar->SetHeight(toolbarHeight);
    mNavigationBar->SetHeight(toolbarHeight);

    mBodyPanel->UpdateLayout();
    mVerticalLayoutComponent->CalculateLayout();
}

void UIAssetBrowser::SetMaxBodyHeight(float y)
{

    mBodyMaxHeight = y;
}

void UIAssetBrowser::SetMinBodyHeight(float y)
{

    mBodyMinHeight = y;
    if (mBodyMinHeight < 1.0F)
        mBodyMinHeight = 1.0F;
}

void UIAssetBrowser::OnDraggingResizeHandle(float deltaX, float deltaY)
{

    ResizeBrowserHeight(deltaY);
}

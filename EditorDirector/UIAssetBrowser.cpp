#include "UIAssetBrowser.h"
#include <EditorDirector/EditorUIUtility.h>
#include <Core/Map.h>
#include <Core/Prefab.h>
#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AnimationTransitionSet.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <EditorDirector/EditorAssetManager.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorProjectManager.h>
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
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UITextInputComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
#include <algorithm>
#include <cmath>

UIAssetBrowser::UIAssetBrowser()
    : mFilePanel(nullptr), mDirectoryTreePanel(nullptr), mGlobalFileEditBox(nullptr), mCurrEditingFileItem(nullptr),
      mFileItemSelectableCom(nullptr), mBodyPanel(nullptr), mBodyMinHeight(1.0f), mToolbar(nullptr),
      mFileOptionPanel(nullptr), mNavigationBar(nullptr)
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

    CreateToolbar();

    CreateNavigationBar();
    CreateBody();
    CreateFileOptionPanel();

    // Canvas 직속 팝업과 이름 편집창도 탭 비활성화에 맞춰 닫는다.
    mOnActiveElementCallbackSystem.Register([this](bool active)
                                             {
                                                 if (!active)
                                                     CloseTransientPanels();
                                             });

    if (auto canvas = GetDestCanvas())
    {
        // 구독 해제는 별도로 다루되, 브라우저 제거 후에는 기존 UI handle로 유효성을 확인한다.
        canvas->mOnInputActivationChangedCallbackSystem.Register(
            [browser = UI::UIElementPtr<UIAssetBrowser>(this)](bool active)
            {
                if (!active)
                {
                    if (auto instance = browser.Get())
                        instance->CloseOptionsPanel();
                }
            });
    }

    SetContentSize(GetWidth(), GetHeight());

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

void UIAssetBrowser::CreateToolbar()
{
    float width = mTransform.GetSize().x;

    auto canvas = GetDestCanvas();
    auto toolbar = EditorUIUtility::CreateSectionHeader(canvas, "Toolbar");

    // auto imageCom = toolbar->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    toolbar->mImageCom->NotUseTexture();
    //    imageCom->SetColor(0.2, 0.2, 0.2);

    toolbar->SetSize(width, mToolbarMaxHeight);
    toolbar->SetPositionLocal(0, 0);

    toolbar->SetParent(this);
    mToolbar = toolbar;
}

void UIAssetBrowser::CreateNavigationBar()
{
    float width = mTransform.GetSize().x;

    auto canvas = GetDestCanvas();
    auto bar = EditorUIUtility::CreateSectionHeader(canvas, "Navigationobar");

    //    auto imageCom = bar->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    bar->mImageCom->NotUseTexture();
    // bar->mImageCom->SetColor(0.27, 0.27, 0.27);

    bar->SetWidth(width);
    //    bar->SetSize(width, mToolbarMaxHeight);

    auto backButton = EditorUIUtility::CreateSmallButton(bar, "BackButton");
    auto forwardButton = EditorUIUtility::CreateSmallButton(bar, "ForwardButton");


    // backButton->SetSize(40, 40);
    // forwardButton->SetSize(40, 40);

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
    auto body = EditorUIUtility::Create<UISplitterPanel>(canvas, "Body");
    auto imageCom = body->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    imageCom->NotUseTexture();
    imageCom->SetColor(0.4, 0.4, 0.4);

    body->SetSize(width, mBodyMaxHeight);
    body->SetPositionLocal(0, 100);

    body->SetParent(this);

    auto folderPanel = EditorUIUtility::Create<UIDirectoryTree>(canvas, "folderPanel");
    auto folderPanelImageCom = folderPanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    folderPanel->SetFileSystem(QuadLF::LogicalFileSystem::GetInstance());

    folderPanel->SetSize(600, mBodyMaxHeight);
    folderPanelImageCom->NotUseTexture();
    folderPanelImageCom->SetColor(0.35f, 0.35f, 0.35f);

    folderPanel->mOnSelectedNewFolderCallbackSystem.Register(this, &UIAssetBrowser::OnSelectedFile);

    mDirectoryTreePanel = folderPanel;

    auto filePanel = EditorUIUtility::Create<UIScrollBox>(canvas, "folderPanel");
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

    mGlobalFileEditBox = EditorUIUtility::CreateTextInput(canvas, "GlobalEditBox");
    // EditorUIUtility의 공통 폰트 규격 유지: mGlobalFileEditBox->SetFontSize(20.0f);
    // EditorUIUtility의 기본 높이 유지: mGlobalFileEditBox->SetSize(50, 50);
    mGlobalFileEditBox->SetWidth(50);
    mGlobalFileEditBox->SetActiveFlag(false);
    mGlobalFileEditBox->SetOverflowMode(UI::EUITextOverflowMode::eWordWrap);
    mGlobalFileEditBox->SetUseScissorRect(true);
    // EditorUIUtility의 기본 색상 유지: mGlobalFileEditBox->SetTextColor(0.0f, 0.0f, 0.0f);
    // EditorUIUtility의 기본 색상 유지: mGlobalFileEditBox->SetBackgroundColor(1.0f, 1.0f, 1.0f);

    mGlobalFileEditBox->mOnLostKeyboardFocusCallbackSystem.Register([this]() { FinishEditingFileItem(); });
    mBodyPanel = body;
}

void UIAssetBrowser::CreateFileOptionPanel()
{

    auto canvas = GetDestCanvas();

    mFileOptionPanel = EditorUIUtility::CreatePanel(canvas, "FileOptionPanel");


    mFileOptionPanel->SetHeight(900.0f);
    mFileOptionPanel->SetWidth(400.0f);

    mFileOptionPanel->SetActiveFlag(false);

    canvas->mOnPreviewMouseDownCallbackSystem.Register(
        [browser = UI::UIElementPtr<UIAssetBrowser>(this)](UI::UIElement *hitElement)
        {
            auto instance = browser.Get();
            if (!instance || !instance->mFileOptionPanel || !instance->mFileOptionPanel->GetActiveFlag())
                return;

            // 메뉴 자신과 중첩된 항목은 유지한다. nullptr(빈 공간)와 다른 UI 클릭은 닫는다.
            if (!UI::UIManager::GetInstance()->IsSameOrDescendant(hitElement, instance->mFileOptionPanel))
                instance->CloseOptionsPanel();
        });

    canvas->mOnInputActivationChangedCallbackSystem.Register([this](bool inputActiveState) { CloseOptionsPanel(); });

    mFileOptionPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");

    auto assetDuplicateButton = EditorUIUtility::CreateSmallTextButton(mFileOptionPanel, "AssetDuplicateButton");
    assetDuplicateButton->mTextComponent->SetText("복사하기");
    assetDuplicateButton->SetWidth(mFileOptionPanel->GetWidth());

    assetDuplicateButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this](float, float)
        {
            CloseOptionsPanel();
            OnClikedDuplicateAssetButton();
        });

    auto test = EditorUIUtility::CreateSmallTextButton(mFileOptionPanel, "AssetDuplicateButton");
    test->mTextComponent->SetText("Test 버튼");
    test->SetWidth(mFileOptionPanel->GetWidth());
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

        case CoreAsset::EAssetType::eMap:
        {
            CoreAsset::AssetPtr mapAsset =
                CoreAsset::AssetManager::GetInstance()->GetAsset<Map>(file->GetAssetInfo().mAssetID);
            Map *map = mapAsset.As<Map>();
            if (map)
            {
                // 저장 확인과 편집 맵 전환은 기존 프로젝트 관리 경로에 맡긴다.
                Quad::EditorProjectManager::GetInstance()->OpenMap(map);
            }
        }
        break;
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
        case CoreAsset::EAssetType::eAnimation:
        {
            CoreAsset::AssetPtr clipAsset = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(
                file->GetAssetInfo().mAssetID);
            Quad::EditorDirector::GetInstance()->ChangeToAnimationClipEditWorkSpace(
                clipAsset.As<CoreAsset::AnimationClip>());
        }
        break;
        case CoreAsset::EAssetType::eAnimationTransitionSet:
        {
            CoreAsset::AssetPtr transitionSetAsset =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationTransitionSet>(
                    file->GetAssetInfo().mAssetID);
            Quad::EditorDirector::GetInstance()->ChangeToAnimationTransitionSetEditWorkSpace(
                transitionSetAsset.As<CoreAsset::AnimationTransitionSet>());
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
    // 탭 전환의 편집 취소 뒤 포커스 해제 알림이 도착할 수도 있다.
    if (mCurrEditingFileItem == nullptr)
        return;
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

void UIAssetBrowser::OnRightClickedFileItem(float x, float y)
{
    auto canvas = mFileOptionPanel->GetDestCanvas();
    if (!canvas)
        return;

    mFileOptionPanel->SetActiveFlag(true);

    mFileOptionPanel->SetPositionLocal(x, y);

    const float windowHeight = canvas->GetWindowSize().Y;
    const float panelHeight = mFileOptionPanel->GetHeight();
    // 올바르게 렌더링하기위해서
    // 창의 크기(높이)가 height보다 작다 그러면 어쩔수없어 그냥 x,y 위치지정

    // 근데 창의 높이가 height보다 커 - > 그런경우에 그  y로 지정하면 y + height > 창의 height 보다 크다? -> 그러면
    // 벗어난만큼 y를 뺸 위치에 지정

    if (windowHeight < panelHeight)
    {
        return;
    }
    else if (windowHeight < panelHeight + y)
    {

        mFileOptionPanel->SetPositionLocal(x, y - (panelHeight + y - windowHeight));
    }
}

void UIAssetBrowser::ActiveOptionalPanel(UI::UIElement *panel, float x, float y) {}

void UIAssetBrowser::ReleaseCallbacks() {}

void UIAssetBrowser::OnClikedDuplicateAssetButton()
{

    // 현재 선택된 파일아이템 에셋에대해
    //
    // 복사수행 요청
    if (mCurrSelectedFileItem == nullptr)
        return;

    auto fileNode = mCurrSelectedFileItem->GetLogicalFileNode();

    if (fileNode && fileNode->GetNodeType() == QuadLF::ELogicalNodeType::eFile)
    {

        QuadLF::LogicalFile *file = static_cast<QuadLF::LogicalFile *>(fileNode);

        CoreAsset::AssetID assetID = file->GetAssetInfo().mAssetID;

        // 브라우저의 탐색 폴더는 파일시스템의 전역 현재 폴더와 다를 수 있다.
        if (!Quad::EditorAssetManager::GetInstance()->DuplicateAsset(assetID, mCurrFolder).Get())
        {
            GlobalOverlayManager::GetInstance()->ShowMessageBox("에셋 복제 또는 파일 등록에 실패했습니다.");
        }
    }
}

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

    auto item = EditorUIUtility::Create<UIFileItem>(canvas, "item");
    //   auto com = item->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    item->mOnFileOpendCallbackSystem.Register([this](QuadLF::LogicalNode *node) { OnSelectedFile(node); });

    // 눌렀을때 Global EditBox로 대체
    item->mFileTextButtonComponent->mButtonClickCallbackSystem.Register([item, this](float x, float y)
                                                                        { OnClickedFileItemText(item, x, y); });

    item->mFileRightButtonComponent->mButtonClickCallbackSystem.Register([this](float x, float y)
                                                                         { OnRightClickedFileItem(x, y); });

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
        mCurrSelectedFileItem = static_cast<UIFileItem *>(selectableCom->GetOwnerUIElement());
    }
    else
    {
        mCurrSelectedFileItem = nullptr;
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
    // 이미 Begin된 Canvas에서는 자식 생성 즉시 OnBegin이 호출되므로 뒤늦은 초기 폴더 지정도 반영한다.
    if (folder && mIsBegun && mDirectoryTreePanel && mFilePanel)
        SelectFolderProgrammtically(folder);
}

void UIAssetBrowser::ResizeBrowserHeight(float deltaY)
{
    if (!mBodyPanel || !std::isfinite(deltaY))
        return;
    float bodyHeight = std::clamp(mBodyPanel->GetHeight() - deltaY, mBodyMinHeight, mBodyMaxHeight);
    SetContentSize(GetWidth(), bodyHeight + mToolbar->GetHeight() + mNavigationBar->GetHeight());
}

void UIAssetBrowser::SetContentSize(float width, float height)
{
    if (!std::isfinite(width) || !std::isfinite(height))
        return;
    width = std::max(1.0f, width);
    height = std::max(1.0f, height);
    SetSize(width, height);
    if (!mIsBegun || !mToolbar || !mNavigationBar || !mBodyPanel)
        return;

    // 작은 높이에서도 본문이 남도록 각 헤더의 높이를 전체 영역의 1/4 이하로 제한한다.
    const float headerHeight = std::min(mToolbarMaxHeight, height * 0.25f);
    const float bodyHeight = std::max(1.0f, height - 2.0f * headerHeight);
    mToolbar->SetSize(width, headerHeight);
    mNavigationBar->SetSize(width, headerHeight);
    mBodyPanel->SetSize(width, bodyHeight);
    mDirectoryTreePanel->SetHeight(bodyHeight);
    mFilePanel->SetHeight(bodyHeight);
    mBodyPanel->UpdateLayout();

    // 기존 splitter가 결정한 파일 영역 시작 위치를 사용하여 양쪽 콘텐츠를 본문 너비에 맞춘다.
    const float filePanelX = mFilePanel->mTransform.GetLocalPosition().x;
    mDirectoryTreePanel->SetWidth(std::max(1.0f, filePanelX - 10.0f));
    mFilePanel->SetWidth(std::max(1.0f, width - filePanelX));
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

void UIAssetBrowser::CloseOptionsPanel()
{

    if (mFileOptionPanel)
    {
        mFileOptionPanel->SetActiveFlag(false);
    }
}

void UIAssetBrowser::CloseTransientPanels()
{
    CloseOptionsPanel();
    if (!mGlobalFileEditBox)
        return;
    if (mCurrEditingFileItem)
        mCurrEditingFileItem->mFileTextElement->SetActiveFlag(true);
    mCurrEditingFileItem = nullptr;
    mGlobalFileEditBox->SetActiveFlag(false);
    mGlobalFileEditBox->GetTextInputComponent()->ReleaseKeyboardFocus();
}

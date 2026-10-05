#include "BottomPanel.h"
#include <EditorDirector/EditorUIUtility.h>
#include <EditorDirector/LogPanel.h>
#include <EditorDirector/UIAssetBrowser.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIMouseDragComponent.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
#include <algorithm>
#include <cmath>

BottomPanel::BottomPanel()
{
    SetStyleRole(UI::EUIStyleRole::eNone);
    SetColor(0.2f, 0.2f, 0.2f);
    SetSize(3000.0f, mResizeHandleHeight + mTabHeight + mContentHeight);
    mVerticalLayoutComponent = CreateUIComponent<UI::UIVerticalLayoutComponent>("VerticalLayoutCom");
}

BottomPanel::~BottomPanel() {}

void BottomPanel::OnBegin()
{
    UI::UIImage::OnBegin();
    if (mContentPanel)
        return;

    // 콘텐츠 생성 중에도 부모 크기 알림이 오므로 초기화가 끝날 때까지 레이아웃 적용을 막는다.
    mUpdatingLayout = true;
    CreateResizeHeightHandle();
    mTabPanel = EditorUIUtility::Create<UI::UIImage>(this, "TabPanel");
    EditorUIUtility::ApplyPreset(mTabPanel, UI::EUIStyleRole::eNone);
    mTabPanel->SetColor(0.16f, 0.16f, 0.16f);
    mTabPanel->SetSize(GetWidth(), mTabHeight);

    mContentPanel = EditorUIUtility::Create<UI::UIElement>(this, "ContentPanel");
    mContentPanel->SetSize(GetWidth(), mContentHeight);
    mContentPanel->SetUseScissorRect(true);

    mAssetBrowser = EditorUIUtility::Create<UIAssetBrowser>(mContentPanel, "UIAssetBrowser");
    mAssetBrowser->SetInitFolder(mInitFolder);
    mLogPanel = EditorUIUtility::Create<LogPanel>(mContentPanel, "LogPanel");
    AddContent("Assets", mAssetBrowser);
    AddContent("Log", mLogPanel);
    SetActiveContent("Assets");
    mUpdatingLayout = false;
    UpdatePanelLayout();
}

void BottomPanel::Update(float deltaTime)
{
    if (!GetActiveFlag())
        return;

    if (auto canvas = GetDestCanvas())
    {
        const auto windowSize = canvas->GetWindowSize();
        if (windowSize.X > 0.0f && GetWidth() != static_cast<int>(windowSize.X))
            SetWidth(windowSize.X);
    }
    UI::UIImage::Update(deltaTime);
}

void BottomPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (type == UI::ETransformChangeType::eSize || type == UI::ETransformChangeType::eAll)
        UpdatePanelLayout();
}

void BottomPanel::SetInitFolder(QuadLF::LogicalFolder *folder)
{
    mInitFolder = folder;
    if (mAssetBrowser)
        mAssetBrowser->SetInitFolder(folder);
}

UIAssetBrowser *BottomPanel::GetAssetBrowser() const
{
    return mAssetBrowser;
}

LogPanel *BottomPanel::GetLogPanel() const
{
    return mLogPanel;
}

bool BottomPanel::AddContent(const std::string &name, UI::UIElement *panel)
{
    if (!mContentPanel || !mTabPanel || !panel || panel == this || panel == mContentPanel ||
        panel == mTabPanel || panel == mResizeHeightHandle || name.empty() ||
        panel->GetDestCanvas() != GetDestCanvas())
        return false;
    // 상위 패널을 콘텐츠로 옮기면 부모 연결이 순환하므로 등록하지 않는다.
    for (auto ancestor = GetParent(); ancestor; ancestor = ancestor->GetParent())
    {
        if (ancestor == panel)
            return false;
    }
    for (const auto &entry : mContents)
    {
        if (entry.mName == name || entry.mPanel == panel)
            return false;
    }

    panel->SetParent(mContentPanel);
    panel->SetUseScissorRect(true);
    panel->SetActiveFlag(false);

    auto button = EditorUIUtility::CreateMediumTextButton(mTabPanel, "ContentTabButton");
    button->SetHeight(mTabHeight);
    button->mTextComponent->SetPaddingLeft(8.0f);
    button->mTextComponent->SetPaddingTop(0.0f);
    button->SetWidth(140.0f);
    button->mTextComponent->SetText(name);
    button->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [this, name](float, float) { SetActiveContent(name); });

    mContents.push_back({name, panel, button});
    mPanelLayoutDirty = true;
    if (!mActiveContent)
        SetActiveContent(name);
    UpdatePanelLayout();
    return true;
}

bool BottomPanel::SetActiveContent(const std::string &name)
{
    auto it = std::find_if(mContents.begin(), mContents.end(),
                           [&name](const ContentEntry &entry) { return entry.mName == name; });
    if (it == mContents.end())
        return false;

    // 브라우저의 메뉴와 편집창은 Canvas 직속 요소이므로 콘텐츠를 숨기기 전에 별도로 닫는다.
    if (mAssetBrowser && it->mPanel != mAssetBrowser)
        mAssetBrowser->CloseTransientPanels();

    mActiveContent = it->mPanel;
    float buttonX = 0.0f;
    for (const auto &entry : mContents)
    {
        const bool active = entry.mPanel == mActiveContent;
        entry.mPanel->SetActiveFlag(active);
        // 탭 선택은 생성 규격이 아닌 실행 중 시각 상태이며, hover 색상은 공통 테마를 따른다.
        UI::UIControlStyleOverride visual;
        visual.mBackgroundColor = active ? UI::UIColor{0.3f, 0.3f, 0.3f} : UI::UIColor{0.16f, 0.16f, 0.16f};
        entry.mButton->SetStyleOverride(visual);
        // 시각 상태 변경과 무관하게 탭의 배치 위치는 이 레이아웃에서만 결정한다.
        entry.mButton->SetPositionLocal(buttonX, 0.0f);
        buttonX += entry.mButton->GetWidth();
    }
    return true;
}

void BottomPanel::ResizeContentHeight(float deltaY)
{
    if (!std::isfinite(deltaY))
        return;
    float maxHeight = mMaxContentHeight;
    if (auto canvas = GetDestCanvas())
    {
        const float windowHeight = canvas->GetWindowSize().Y;
        if (windowHeight > 0.0f)
            maxHeight = std::min(maxHeight, std::max(mMinContentHeight,
                                                    windowHeight - mResizeHandleHeight - mTabHeight));
    }

    // 하단 고정 패널에서는 위쪽으로 드래그할수록 높이가 증가한다.
    mContentHeight = std::clamp(mContentHeight - deltaY, mMinContentHeight, maxHeight);
    UpdatePanelLayout();
}

void BottomPanel::CreateResizeHeightHandle()
{
    mResizeHeightHandle = EditorUIUtility::Create<UI::UIImage>(this, "ResizeHeightHandle");
    EditorUIUtility::ApplyPreset(mResizeHeightHandle, UI::EUIStyleRole::eNone);
    mResizeHeightHandle->SetSize(GetWidth(), mResizeHandleHeight);
    mResizeHeightHandle->SetColor(0.3f, 0.3f, 0.3f);
    auto dragCom = mResizeHeightHandle->CreateUIComponent<UI::UIMouseDragComponent>("DragCom");
    auto handle = mResizeHeightHandle;
    dragCom->mOnHoverCallbackSystem.Register([handle]() { handle->SetColor(0.5f, 0.5f, 0.5f); });
    dragCom->mOnReleaseHoverCallbackSystem.Register([handle]() { handle->SetColor(0.3f, 0.3f, 0.3f); });
    dragCom->mOnDragStartedCallbackSystem.Register([handle]() { handle->SetColor(0.7f, 0.7f, 0.7f); });
    dragCom->mOnDraggedCallbackSystem.Register(
        [this](const UI::UIMouseDragContext &context) { ResizeContentHeight(context.mDeltaY); });
    dragCom->mOnDragEndededCallbackSystem.Register([handle]() { handle->SetColor(0.5f, 0.5f, 0.5f); });
}

void BottomPanel::UpdatePanelLayout()
{
    if (mUpdatingLayout || !mResizeHeightHandle || !mTabPanel || !mContentPanel)
        return;
    const float width = std::max(1, GetWidth());
    // 자식의 크기 알림으로 부모 높이가 다시 지정되어도 동일 영역을 반복해서 재배치하지 않는다.
    if (!mPanelLayoutDirty && width == mLayoutWidth && mContentHeight == mLayoutContentHeight)
        return;
    mUpdatingLayout = true;
    mResizeHeightHandle->SetSize(width, mResizeHandleHeight);
    mTabPanel->SetSize(width, mTabHeight);
    mContentPanel->SetSize(width, mContentHeight);

    float buttonX = 0.0f;
    for (const auto &entry : mContents)
    {
        entry.mButton->SetPositionLocal(buttonX, 0.0f);
        buttonX += entry.mButton->GetWidth();
        entry.mPanel->SetPositionLocal(0.0f, 0.0f);
        if (entry.mPanel == mAssetBrowser)
            mAssetBrowser->SetContentSize(width, mContentHeight);
        else
            entry.mPanel->SetSize(width, mContentHeight);
    }

    // 내부 콘텐츠는 중첩 영역에 겹쳐 놓고, 세로 레이아웃에는 공통 영역 세 개만 참여시킨다.
    mVerticalLayoutComponent->CalculateLayout();
    UpdatePosAnchor();
    mLayoutWidth = width;
    mLayoutContentHeight = mContentHeight;
    mPanelLayoutDirty = false;
    mUpdatingLayout = false;
}

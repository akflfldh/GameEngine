#include "UIScrollBox.h"
#include <EditorDirector/UIGridLayoutComponent.h>
#include <EditorDirector/UIScrollControlComponent.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIMovableComponent.h>
#include <UiSystem/UITextComponent.h>
#include <UiSystem/UIVerticalLayoutComponent.h>
UIScrollBox::UIScrollBox()
    : mItemHeight(20), mItemStartYOffsetLocal(0.0f), mItemFontSize(40.0f), mContentPanel(nullptr),
      mLayout(EUIScrollLayout::eVertical)
{

    mImageComponent = CreateUIComponent<UI::UIImageComponent>("ImageCom");

    //  mMovableComponent = CreateUIComponent<UI::UIMovableComponent>("movableCom");

    SetSize(100, 100);
    SetUseScissorRect(true);
}

UIScrollBox::~UIScrollBox() {}

void UIScrollBox::OnBegin()
{

    UI::UIElement::OnBegin();

    if (mImageComponent)
    {
        mImageComponent->NotUseTexture();
    }

    mContentPanel = GetDestCanvas()->CreateUIElement<UI::UIElement>("ContentPanel");
    // UI::UIImageComponent *contentPanelImageCom = mContentPanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");

    mContentPanel->SetParent(this);

    auto contentPanelButtonCom = mContentPanel->CreateUIComponent<UI::UIButtonComponent>("ButtonCom");
    contentPanelButtonCom->mButtonClickCallbackSystem.Register(
        [this](float, float) { mOnBackgroudClickedCallbackSystem.ExecuteCallbacks(); });

    //  레이아웃 컴포넌트 생성
    if (mLayout == EUIScrollLayout::eVertical)
    {
        mContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("LayoutCom");
    }
    else if (mLayout == EUIScrollLayout::eGrid)
    {
        mContentPanel->CreateUIComponent<UIGridLayoutComponent>("LayoutCom");
    }

    glm::vec2 scrollBoxSize = mTransform.GetSize();

    mContentPanel->SetSize(scrollBoxSize.r, scrollBoxSize.g);
    mContentPanel->SetPositionLocal(0, 0);

    // contentPanelImageCom->NotUseTexture();
    // contentPanelImageCom->SetColor(0.2f, 0.2, 0.2f);

    mScrollControlPanel = GetDestCanvas()->CreateUIElement<UI::UIElement>("mScrollControlPanel");
    UI::UIImageComponent *scrollboxImageCom = mScrollControlPanel->CreateUIComponent<UI::UIImageComponent>("ImageCom");
    UIScrollControlComponent *scrollControlCom =
        mScrollControlPanel->CreateUIComponent<UIScrollControlComponent>("ControlCom");
    scrollControlCom->SetDestElement(mContentPanel);

    mScrollControlPanel->SetParent(this);

    glm::vec2 scrollControlPanelSize = {50, scrollBoxSize.g / 6};
    mScrollControlPanel->SetSize(scrollControlPanelSize);

    float scrollLocalPosX = scrollBoxSize.r - scrollControlPanelSize.r / 2;
    mScrollControlPanel->SetPositionLocal(scrollLocalPosX, 0);
    scrollboxImageCom->NotUseTexture();
    scrollboxImageCom->SetColor(0.0F, 1.0F, 0.0F);

    mItemStartYOffsetLocal = 0;
    //    mTransform.GetSize().g / 2;

    SetUseScissorRect(true);
    mContentPanel->SetUseScissorRect(true);
    mScrollControlPanel->SetUseScissorRect(true);

    mContentPanel->SetDepthValue(2);
    mScrollControlPanel->SetDepthValue(1);
}

void UIScrollBox::SetBackgrounColor(float r, float g, float b)
{
    if (mImageComponent)
    {
        mImageComponent->SetColor(r, g, b);
    }
}

void UIScrollBox::AddItem(const std::string &str) {}

void UIScrollBox::AddItem(UI::UIElement *itemElement)
{

    if (itemElement == nullptr)
        return;

    itemElement->SetParent(mContentPanel);
    itemElement->SetUseScissorRect(true);
    itemElement->SetWidth(mTransform.GetSize().r);
    // itemElement->SetPositionLocal(0, GetNextItemY());

    // content 내부적으로 레이아웃 업데이트
    UpdateContentLayout();

    // 외적으로 레이아웃 업데이트
    UpdateLayout();
}

void UIScrollBox::AddItemList(const std::vector<UI::UIElement *> &itemElementList)
{

    float nextY = mItemStartYOffsetLocal;
    for (auto item : itemElementList)
    {
        if (item == nullptr)
            continue;

        item->SetParent(mContentPanel);
        item->SetUseScissorRect(true);

        item->SetWidth(mTransform.GetSize().r);
        //  item->SetPositionLocal(0, nextY);
        //   nextY += item->mTransform.GetSize().g;

        // mItemVec.push_back(item);
    }
    // content 내부적으로 레이아웃 업데이트
    UpdateContentLayout();
    // 외적으로 레이아웃 업데이트
    UpdateLayout();
}

void UIScrollBox::RemoveItem(const std::string &str)
{

    /*  int index = -1;
      for (int i = 0; i < mItemVec.size(); ++i)
      {

          UI::UITextComponent *texCom = nullptr;
          mItemVec[i]->GetComponents<UI::UITextComponent>(&texCom, 1);

          if (texCom)
          {
              if (texCom->GetText() == str)
              {
                  index = i;
                  break;
              }
          }
      }

      if (index != -1)
      {
          RemoveItem(index);
      }*/
}

UI::UIElement *UIScrollBox::RemoveItem(int index, bool bDestory)
{

    if (mContentPanel->GetChildVector().size() <= index || index < 0)
        return nullptr;

    auto item = mContentPanel->GetChildVector()[index];

    if (bDestory)
        item->Destroy();

    UpdateContentLayout();

    if (!bDestory)
    {
        item->SetParent(nullptr);
        return item;
    }

    return nullptr;
}
void UIScrollBox::RemoveItem(UI::UIElement *item)
{

    item->SetParent(nullptr);
}

std::vector<UI::UIElement *> UIScrollBox::RemoveItemAll(bool bDestory)
{
    std::vector<UI::UIElement *> itemList;

    const std::vector<UI::UIElement *> &itemVec = mContentPanel->GetChildVector();
    for (int i = mContentPanel->GetChildVector().size() - 1; i >= 0; --i)
    {
        UI::UIElement *item = RemoveItem(i, bDestory);
        if (!bDestory)
        {
            itemList.push_back(item);
        }
    }

    return itemList;
}

void UIScrollBox::SetItemTextSize(float size)
{

    mItemFontSize = size;
}

void UIScrollBox::SetItemHeight(float h)
{

    mItemHeight = h;

    // update
}

void UIScrollBox::UpdateLayout()
{

    if (!mIsBegun)
        return;

    // mTransform.SetSize(w, h);
    glm::vec2 scrollBoxSize = mTransform.GetSize();

    float currentContentHeight = mContentPanel->mTransform.GetSize().g;
    /* for (auto item : mItemVec)
     {
         totalHeight += item->mTransform.GetSize().y;
     }*/

    if (currentContentHeight > mTransform.GetSize().y)
    {
        //  mContentPanel->SetSize(mContentPanel->mTransform.GetSize().x, totalHeight);

        // 위치 조정이 필요해 현재 scrollcontrolpanel의 위치에맞추어서
        UIScrollControlComponent *scrollControlCom = nullptr;

        mScrollControlPanel->GetComponents<UIScrollControlComponent>(&scrollControlCom, 1);
        if (scrollControlCom)
        {
            scrollControlCom->UpdateDestPanelPos();
        }

        //// item들도 다시 조정
        // float nextY = 0;
        //// mContentPanel->mTransform.GetSize().y / 2;

        // for (int i = 0; i < mItemVec.size(); ++i)
        //{
        //     mItemVec[i]->SetPositionLocal(0, nextY);
        //     nextY += mItemVec[i]->mTransform.GetSize().y;
        // }
    }

    glm::vec2 scrollControlPanelSize = {25, scrollBoxSize.g / 6};
    mScrollControlPanel->SetSize(scrollControlPanelSize);

    float scrollLocalPosX = scrollBoxSize.r - scrollControlPanelSize.r;
    mScrollControlPanel->SetPositionLocal(scrollLocalPosX, 0);
}

void UIScrollBox::UpdateContentLayout()
{
    if (mContentPanel)
    {
        UI::UILayoutComponent *contentLayoutCom = nullptr;

        mContentPanel->GetComponents<UI::UILayoutComponent>(&contentLayoutCom, 1);
        if (contentLayoutCom)
        {
            contentLayoutCom->CalculateLayout();
        }
    }
}

void UIScrollBox::OnTransformChanged(UI::ETransformChangeType type)
{

    UIElement::OnTransformChanged(type);

    if (type == UI::ETransformChangeType::eSize || type == UI::ETransformChangeType::eAll)
    {
        if (mContentPanel)
        {
            float h = mContentPanel->mTransform.GetSize().g;
            mContentPanel->SetSize(mTransform.GetSize().r, h);
        }

        UpdateContentLayout();

        UpdateLayout();
    }
}

void UIScrollBox::SetLayout(EUIScrollLayout layout)
{

    if (mLayout == layout)
        return;

    if (mContentPanel)
    {
        // 기존 레이아웃 컴포넌트제거
        UI::UILayoutComponent *preLayoutCom = nullptr;
        mContentPanel->GetComponents<UI::UILayoutComponent>(&preLayoutCom, 1);
        mContentPanel->ReleaseUIComponent(preLayoutCom);

        // 새로운 레이아웃 컴포넌트 생성
        if (layout == EUIScrollLayout::eVertical)
        {
            mContentPanel->CreateUIComponent<UI::UIVerticalLayoutComponent>("LayoutCom");
        }
        else if (layout == EUIScrollLayout::eGrid)
        {
            mContentPanel->CreateUIComponent<UIGridLayoutComponent>("LayoutCom");
        }
    }

    mLayout = layout;
}

const std::vector<UI::UIElement *> &UIScrollBox::GetItemList() const
{

    return mContentPanel->GetChildVector();
    // TODO: 여기에 return 문을 삽입합니다.
}

void UIScrollBox::ForceUpdateLayout()
{

    UpdateContentLayout();
    UpdateLayout();
}

void UIScrollBox::SetItemSize(float w, float h)
{
    if (mContentPanel)
    {
        UI::UILayoutComponent *layoutCom = nullptr;
        mContentPanel->GetComponents<UI::UILayoutComponent>(&layoutCom, 1);
        layoutCom->SetItemSize(w, h);
        ForceUpdateLayout();
    }
}

float UIScrollBox::GetNextItemY() const
{

    return 0.0f;
}
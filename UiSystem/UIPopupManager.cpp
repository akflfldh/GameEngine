#include "UIPopupManager.h"
#include <UiSystem/UIElement.h>
#include <UiSystem/UIManager.h>

UI::UIPopupManager *UI::UIPopupManager::GetInstance()
{

    static UIPopupManager instance;
    return &instance;
}

void UI::UIPopupManager::RegisterPopup(UIElement *popup, UIPopupScopeID scopeID)
{

    if (popup == nullptr)
        return;

    popup->SetRenderLayer(EUIRenderLayer::ePopup);
    // PopupContext 생성 및 등록

    UIPopupContext popupContext;
    popupContext.mPopupElement = popup->GetID();
    popupContext.mScopeID = scopeID;

    mPopupContextTable[popup->GetID()] = popupContext;

    // scope 는 최상위
}

UI::UIPopupScopeID UI::UIPopupManager::CreatePopupScope(UIElement *ownerPopup)
{

    mScopeOwnerPopupUITable[mNextPopupScopeID] = ownerPopup->GetID();

    return mNextPopupScopeID++;
}

UI::UIPopupContext &UI::UIPopupManager::GetPopupContext(UIElementID popupID)
{

    auto it = mPopupContextTable.find(popupID);

    return it->second;

    if (it == mPopupContextTable.end())
    {
    }

    // TODO: 여기에 return 문을 삽입합니다.
}

UI::PopupStack &UI::UIPopupManager::GetPopupStack(UIPopupScopeID popupScopeID)
{
    // TODO: 여기에 return 문을 삽입합니다.

    auto it = mActivePopupStackTable.find(popupScopeID);

    return it->second;
}

void UI::UIPopupManager::OpenPopup(UIElementID popup)
{

    UIPopupContext &context = GetPopupContext(popup);
    PopupStack &stack = GetPopupStack(context.mScopeID);

    auto uiManager = UI::UIManager::GetInstance();
    auto parentPopup = uiManager->GetUIElement(context.mParentPopup);

    if (!parentPopup)
    {
        // 부모팝업없다 .- > 독립적인 팝업이다. // 기존 scope의 팝업들을 모두 닫는다.
        ClosePopupStack(context.mScopeID);
    }
    else
    {

        // 부모팝업이 있다 - > scope 스택에 계층적으로 열려있다. parentPopup전까지만 열어두고 나머지는 닫는다.
        ClosePopupsAfter(context.mParentPopup);
    }

    ActivatePopup(context);
    stack.push_back(popup);
}

void UI::UIPopupManager::ClosePopupStack(UIPopupScopeID popupScope)
{
    auto uiManager = UI::UIManager::GetInstance();

    PopupStack &stack = GetPopupStack(popupScope);

    while (!stack.empty())
    {

        UIElementID popupID = stack.back();
        stack.pop_back();
        UIElement *popup = uiManager->GetUIElement(popupID);
    }
}

void UI::UIPopupManager::ClosePopupsAfter(UIElementID popup)
{

    auto uiManager = UI::UIManager::GetInstance();
    auto &stack = GetPopupStack(GetPopupContext(popup).mScopeID);

    auto it = std::find(stack.begin(), stack.end(), popup);
    if (it == stack.end())
        return;

    it++;
    auto firstPopupToClose = it;

    for (auto closeIt = stack.end(); closeIt != firstPopupToClose;)
    {
        --closeIt;
        // stack.pop_back();
        DeactivatePopup(*closeIt);
    }

    stack.erase(firstPopupToClose, stack.end());
}

void UI::UIPopupManager::ActivatePopup(UIPopupContext &context)
{

    UIElement *popup = ResolveElement(context.mPopupElement);

    popup->SetActiveFlag(true);
}

void UI::UIPopupManager::DeactivatePopup(UIElementID id)
{

    UIElement *popup = ResolveElement(id);

    popup->SetActiveFlag(false);
}

UI::UIElement *UI::UIPopupManager::ResolveElement(UIElementID id) const
{
    auto uiManager = UI::UIManager::GetInstance();
    return uiManager->GetUIElement(id);
}

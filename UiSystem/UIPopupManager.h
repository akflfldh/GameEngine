#pragma once

#include <UIType.h>
#include <UiSystem/UISystemDllMacro.h>
#include <unordered_map>

namespace UI
{
struct UIPopupContext
{
    UIElementID mOwnerElement;
    UIElementID mPopupElement;
    UIElementID mOwnerWindow;

    UIElementID mParentPopup;
    UIPopupScopeID mScopeID;

    uint64_t mOpenSequence = 0;
    //  EUIPopupType mType = EUIPopupType::eTransient;
};
using PopupStack = std::vector<UIElementID>;

class UISYSTEM_API UIPopupManager
{
  public:
    static UIPopupManager *GetInstance();

    void RegisterPopup(UIElement *popup, UIPopupScopeID scopeID);

    UIPopupScopeID CreatePopupScope(UIElement *ownerPopup);

    UIPopupContext &GetPopupContext(UIElementID popupID);
    PopupStack &GetPopupStack(UIPopupScopeID popupScopeID);
    void OpenPopup(UIElementID popup);
    // scope 에 모든 열린 popup을 닫는다.
    void ClosePopupStack(UIPopupScopeID popupScope);

    // 해당 popup까지만 열어두고 나머지는 다 닫는다.
    void ClosePopupsAfter(UIElementID popup);
    void ActivatePopup(UIPopupContext &context);
    void DeactivatePopup(UIElementID id);

  private:
    UIElement *ResolveElement(UIElementID id) const;

  private:
    std::unordered_map<UIElementID, UIPopupContext> mPopupContextTable;

    std::unordered_map<UIPopupScopeID, PopupStack> mActivePopupStackTable;

    std::unordered_map<UIElementID, UIPopupScopeID> mScopeOwnerPopupUITable;

    UIPopupScopeID mNextPopupScopeID = 1;
};

} // namespace UI
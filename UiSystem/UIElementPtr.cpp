#include "UIElementPtr.h"
// #include <UiSystem/UIElement.h>
// #include <UiSystem/UIManager.h>
// UI::UIElementPtr::UIElementPtr(UIElement *uiElement) : mUIElement(uiElement), mUIElementID(InvaildUIElementID)
//{
//     if (mUIElement)
//     {
//         mUIElementID = mUIElement->GetID();
//     }
// }
//
// UI::UIElementPtr::~UIElementPtr() {}
//
// UI::UIElementPtr::UIElementPtr(UIElementPtr &&rhs) noexcept : mUIElement(rhs.mUIElement),
// mUIElementID(rhs.mUIElementID)
//{
//
//     rhs.mUIElement = nullptr;
//     rhs.mUIElementID = InvaildUIElementID;
// }
//
// UI::UIElementPtr &UI::UIElementPtr::operator=(UIElementPtr &&rhs) noexcept
//{
//
//     mUIElement = rhs.mUIElement;
//     mUIElementID = rhs.mUIElementID;
//
//     rhs.mUIElement = nullptr;
//     rhs.mUIElementID = InvaildUIElementID;
//
//     return *this;
// }
//
// UI::UIElementPtr &UI::UIElementPtr::operator=(UIElement *uiElement)
//{
//     mUIElement = uiElement;
//     if (mUIElement)
//     {
//         mUIElementID = mUIElement->GetID();
//     }
//     else
//     {
//         mUIElementID = InvaildUIElementID;
//     }
//
//     return *this;
//     // TODO: 여기에 return 문을 삽입합니다.
// }
//
// UI::UIElement *UI::UIElementPtr::Get() const
//{
//
//     // TO DO
//
//     if (mUIElementID == InvaildUIElementID)
//         return nullptr;
//
//     // UIManager로 부터 해당 ElementID로 UIElement를 검증하고 리턴
//
//     UIManager *uiManager = UIManager ::GetInstance();
//     if (mUIElement == uiManager->GetUIElement(mUIElementID))
//         return mUIElement;
//     else
//     {
//         mUIElement = nullptr;
//         mUIElementID = InvaildUIElementID;
//         return nullptr;
//     }
// }
#pragma once
#include <CoreBase/CallbackSystem.h>
#include <Logger/Logger.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UIType.h>

namespace UI
{

template <typename T> class UIElementPtr
{
  public:
    UIElementPtr(T *uiElement = nullptr);
    ~UIElementPtr();

    UIElementPtr(const UIElementPtr &) = default;
    UIElementPtr &operator=(const UIElementPtr &) = default;

    UIElementPtr(UIElementPtr &&) noexcept;
    UIElementPtr &operator=(UIElementPtr &&) noexcept;

    UIElementPtr &operator=(T *uiElement);

    T *Get() const;

  private:
    mutable T *mUIElement;
    mutable UIElementHandle mUIElementHandle;
};

template <typename T> inline UI::UIElementPtr<T>::UIElementPtr(T *uiElement)
{

    if constexpr (std::is_base_of_v<UI::UIElement, T> == false)
    {
        LOG_MESSAGE_ERROR("UIElementPtr", "UIElement타입이 아닙니다.");
        static_assert(0);
    }

    mUIElement = uiElement;
    if (mUIElement)
        mUIElementHandle = uiElement->GetHandle();
}

template <typename T> inline UIElementPtr<T>::~UIElementPtr() {}

template <typename T> inline UI::UIElementPtr<T>::UIElementPtr(UIElementPtr &&rhs) noexcept
{

    mUIElement = rhs.mUIElement;
    mUIElementHandle = rhs.mUIElementHandle;

    rhs.mUIElement = nullptr;
}

template <typename T> inline UIElementPtr<T> &UIElementPtr<T>::operator=(UIElementPtr &&rhs) noexcept
{

    mUIElement = rhs.mUIElement;
    mUIElementHandle = rhs.mUIElementHandle;

    rhs.mUIElement = nullptr;
    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

template <typename T> inline T *UIElementPtr<T>::Get() const
{

    UIManager *uiManager = UIManager::GetInstance();

    // TODO
    //  uiManager에서 handle을 넘겨서 uiElement를 가져오는거다.
    if (mUIElement == nullptr)
        return nullptr;

    UIElement *uiElement = uiManager->GetUIElement(mUIElementHandle);
    return static_cast<T *>(uiElement);

    /* if (mUIElement == uiManager->GetUIElement(mUIElementID))
     {
         return mUIElement;
     }
     else
     {
         mUIElement = nullptr;
         mUIElementID = InvaildUIElementID;
         return nullptr;
     }*/
}

} // namespace UI

template <typename T>
struct Core::CheckInstancePtr<T, typename std::enable_if<std::is_base_of<UI::UIElement, T>::value>::type>
{
    static UI::UIElementPtr<T> GetPtr(T *instance)
    {
        return UI::UIElementPtr(instance);
    }
};
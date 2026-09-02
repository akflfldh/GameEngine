#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>
#include <UiSystem/UIComponentFactory.h>
#include <UiSystem/UIElementTyprDef.h>

namespace UI
{
class IUIComponent;

template <typename T> class UIComponentPtr
{
  public:
    UIComponentPtr(T *uiComponent = nullptr);
    ~UIComponentPtr();

    UIComponentPtr(const UIComponentPtr &) = default;
    UIComponentPtr &operator=(const UIComponentPtr &) = default;

    UIComponentPtr(UIComponentPtr &&) noexcept;
    UIComponentPtr &operator=(UIComponentPtr &&) noexcept;

    T *Get();

  private:
    T *mComponent;
    UIComponentHandle mUIComponentHandle;
};
template <typename T> inline UIComponentPtr<T>::UIComponentPtr(T *uiComponent) : mComponent(uiComponent)
{
    if constexpr (std::is_base_of_v<IUIComponent, T> == false)
    {
        static_assert(0);
    }

    if (mComponent)
    {
        mUIComponentHandle = uiComponent->GetHandle();
    }
}
template <typename T> inline UIComponentPtr<T>::~UIComponentPtr() {}
template <typename T>
inline UIComponentPtr<T>::UIComponentPtr(UIComponentPtr &&rhs) noexcept
    : mComponent(rhs.mComponent), mUIComponentHandle(rhs.mUIComponentHandle)
{

    rhs.mComponent = nullptr;
}
template <typename T> inline UIComponentPtr<T> &UIComponentPtr<T>::operator=(UIComponentPtr &&rhs) noexcept
{
    // TODO: 여기에 return 문을 삽입합니다.
    mComponent = rhs.mComponent;
    mUIComponentHandle = rhs.mUIComponentHandle;
    rhs.mComponent = nullptr;

    return *this;
}

template <typename T> inline T *UIComponentPtr<T>::Get()
{
    if (mComponent == nullptr)
        return nullptr;

    UIComponentFactory *factory = UIComponentFactory::GetInstance();
    if (factory)
    {
        IUIComponent *component = factory->Get(mUIComponentHandle);

        return static_cast<T *>(component);
    }

    return nullptr;
}

} // namespace UI

template <typename T>
struct Core::CheckInstancePtr<T, typename std::enable_if<std::is_base_of<UI::IUIComponent, T>::value>::type>
{
    static UI::UIComponentPtr<T> GetPtr(T *instance)
    {
        return UI::UIComponentPtr<T>(instance);
    }
};
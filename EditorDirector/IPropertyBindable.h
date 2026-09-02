#pragma once

#include <functional>

namespace UI
{
class UIElement;
}

namespace Quad
{
struct PropertyInfo;
}

class IPropertyBindable
{
  public:
    IPropertyBindable();
    virtual ~IPropertyBindable();

    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) = 0;
    virtual void Release() = 0;

    //    virtual Quad::PropertyInfo *GetPropertyInfo() const;

    std::function<void(UI::UIElement *)> mReturnToPoolCallback;
};

#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreMath/CoreMath.h>
#include <UISystem/UIElementTyprDef.h>
#include <queue>
#include <vector>

namespace UI
{
class IUIComponent;
class UIElement;

struct UIComponentSlot
{
    uint64_t mGeneration;
    IUIComponent *mComponent;
};

class UISYSTEM_API UIComponentFactory
{
  public:
    static UIComponentFactory *GetInstance();
    UIComponentFactory();
    ~UIComponentFactory();

    IUIComponent *Get(const UIComponentHandle &handle) const;

    UI::IUIComponent *Create(UIElement *uiElement, const char *componentStaticName, const char *componentInstanceName);
    void Release(UIElement *uiElement, IUIComponent *component);

    bool IsAncestorClass(const char *baseClassName, const char *childClassName) const;

  private:
    void RegisterToSlot(IUIComponent *uiComponent);
    void UnRegisterFromSlot(uint64_t slotIndex);

  private:
    std::vector<UIComponentSlot> mComponentSlotList;

    std::queue<uint64_t> mComponentFreeIndexPool;
    uint64_t mNextPoolIndex;
};

} // namespace UI
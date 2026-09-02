#include "UiSystem/UIComponentFactory.h"
#include "UiSystem/IUIComponent.h"
#include "UiSystem/UIElement.h"
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
UI::UIComponentFactory *UI::UIComponentFactory::GetInstance()
{
    static UIComponentFactory instance;
    return &instance;
}

UI::UIComponentFactory::UIComponentFactory() {}

UI::UIComponentFactory::~UIComponentFactory() {}

UI::IUIComponent *UI::UIComponentFactory::Get(const UIComponentHandle &handle) const
{

    if (mComponentSlotList.size() <= handle.mPoolSlotIndex)
        return nullptr;

    if (mComponentSlotList[handle.mPoolSlotIndex].mGeneration != handle.mGeneration)
        return nullptr;

    return mComponentSlotList[handle.mPoolSlotIndex].mComponent;
}

UI::IUIComponent *UI::UIComponentFactory::Create(UIElement *uiElement, const char *componentStaticName,
                                                 const char *componentInstanceName)
{

    if (uiElement == nullptr)
        return nullptr;

    // std::unordered_map<const char *, IUIComponent *>::iterator it =
    //     uiElement->mComponentContainer.find(componentInstanceName);

    // if (it != uiElement->mComponentContainer.end())
    //     return nullptr;

    if (uiElement->GetComponent(componentInstanceName) != nullptr)
        return nullptr;

    // 리플렉션시스템에게 부탁하여 생성한다.
    // factory옆에는 커스텀 할당자필요

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    //  size_t classSize = reflectionSystem->GetClassSize(componentStaticName);

    // char *memBuffer = new char[classSize];

    void *component = reflectionSystem->CreateClassInstance(componentStaticName);

    if (component == nullptr)
    {
        return nullptr;
    }

    IUIComponent *uiComponent = (IUIComponent *)component;
    uiComponent->mName = componentInstanceName;
    // uiElement->mComponentContainer[componentInstanceName] = uiComponent;
    uiElement->RegisterComponent(componentInstanceName, uiComponent);
    uiComponent->SetOwnerUIElement(uiElement);
    if (uiElement->mIsBegun)
    {
        uiComponent->Begin();
    }

    return uiComponent;
}

void UI::UIComponentFactory::Release(UIElement *uiElement, IUIComponent *component)
{
    if (component == nullptr)
        return;

    component->OnRemoved();

    if (uiElement)
    {
        uiElement->UnRegisterComponent(component);
    }

    UnRegisterFromSlot(component->GetHandle().mPoolSlotIndex);

    const char *className = component->GetRunTimeClassName();
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    reflectionSystem->DestoryClassInstance(component);
}

bool UI::UIComponentFactory::IsAncestorClass(const char *baseClassName, const char *childClassName) const
{
    if (std::strcmp(baseClassName, childClassName) == 0)
        return true;

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    Quad::ClassInfo *classInfo = reflectionSystem->FindClassInfo(childClassName);

    return classInfo->IsAncestorClass(baseClassName);
}

void UI::UIComponentFactory::RegisterToSlot(IUIComponent *uiComponent)
{

    if (uiComponent == nullptr)
        return;

    uint64_t newSlotIndex = mNextPoolIndex;

    if (mComponentFreeIndexPool.empty())
    {
        mComponentSlotList.push_back({});
        mNextPoolIndex++;
    }
    else
    {
        newSlotIndex = mComponentFreeIndexPool.front();
        mComponentFreeIndexPool.pop();
    }

    mComponentSlotList[newSlotIndex].mComponent = uiComponent;
    mComponentSlotList[newSlotIndex].mGeneration++;

    uiComponent->SetUIComponentHandle({mComponentSlotList[newSlotIndex].mGeneration, newSlotIndex});
}

void UI::UIComponentFactory::UnRegisterFromSlot(uint64_t slotIndex)
{
    if (mComponentSlotList.size() <= slotIndex)
        return;

    if (mComponentSlotList[slotIndex].mComponent == nullptr)
        return;

    mComponentSlotList[slotIndex].mComponent = nullptr;
    mComponentFreeIndexPool.push(slotIndex);
}

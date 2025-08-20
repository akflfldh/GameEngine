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

UI::IUIComponent *UI::UIComponentFactory::Create(UIElement *uiElement, const char *componentStaticName,
                                                 const char *componentInstanceName)
{

    if (uiElement == nullptr)
        return nullptr;

    std::unordered_map<const char *, IUIComponent *>::iterator it =
        uiElement->mComponentContainer.find(componentInstanceName);

    if (it != uiElement->mComponentContainer.end())
        return nullptr;

    // 리플렉션시스템에게 부탁하여 생성한다.
    // factory옆에는 커스텀 할당자필요

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    size_t classSize = reflectionSystem->GetClassSize(componentStaticName);

    char *memBuffer = new char[classSize];

    void *component = reflectionSystem->CreateClassInstance(componentStaticName, memBuffer);

    if (component == nullptr)
    {
        delete[] memBuffer;
        return nullptr;
    }
    IUIComponent *uiComponent = (IUIComponent *)component;
    uiComponent->mName = componentInstanceName;
    uiElement->mComponentContainer[componentInstanceName] = uiComponent;
    uiComponent->SetOwnerUIElement(uiElement);

    return uiComponent;
}

void UI::UIComponentFactory::Release(IUIComponent *component)
{
    if (component == nullptr)
        return;

    const char *className = component->GetClassName();
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    reflectionSystem->DestoryClassInstance(className, component);
}

bool UI::UIComponentFactory::IsBaseClass(const char *baseClassName, const char *childClassName) const
{
    if (std::strcmp(baseClassName, childClassName) == 0)
        return true;

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    Quad::ClassInfo *classInfo = reflectionSystem->GetClassInfo(childClassName);

    return classInfo->IsBaseClass(baseClassName);
}

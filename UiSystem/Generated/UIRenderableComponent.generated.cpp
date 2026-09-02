#include "UIRenderableComponent.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct UIRenderableComponentRegistrar
{
    UIRenderableComponentRegistrar()
    {
        Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
        Quad::ClassInfo *cls = reflectionSystem->CreateClassInfo("UIRenderableComponent",
                                                                 sizeof(UI::UIRenderableComponent), nullptr, nullptr);
        cls->SetBaseClassList("IUIComponent ", 1);
        cls->SetParentClassList("IUIComponent", 1);

        reflectionSystem->AddProperty(
            cls, {"name", "type name", "offset", true, reflectionSystem->GetTypeDecriptor("class")});

        reflectionSystem->AddProperty(
            cls, {"name", "type name", "offset", false, reflectionSystem->GetTypeDecriptor("int")});
    }
};
static UIRenderableComponentRegistrar G_UIRenderableComponentRegistrar;

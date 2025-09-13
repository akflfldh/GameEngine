#include "Component.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct ComponentRegistrar {
    ComponentRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("Component" ,sizeof(Core::Component) , nullptr , nullptr); 
cls->SetBaseClassList("",0); 
    }
};
static ComponentRegistrar G_ComponentRegistrar;

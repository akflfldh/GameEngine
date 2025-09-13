#include "Object.h"
#include <ReflectSystem/ReflectionSystem.h>

#include <ReflectSystem/ReflectionClassInfo.h>
#include <cstddef>

struct ObjectRegistrar {
    ObjectRegistrar()    {
Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); 
Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo("Object" ,sizeof(Core::Object) , nullptr , nullptr); 
cls->SetBaseClassList("",0); 
    }
};
static ObjectRegistrar G_ObjectRegistrar;

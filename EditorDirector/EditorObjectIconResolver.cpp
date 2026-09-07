#include "EditorObjectIconResolver.h"
#include <Core/Object.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
const char *EditorObjectIconResolver::Resolve(const Object &object)
{

    const char *rumTimeClassName = object.GetRunTimeClassName();

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    auto classInfo = reflectionSystem->FindClassInfo(rumTimeClassName);

    if (classInfo->IsAncestorClass("PlayerStart"))
    {
        return "Engine/PlayerStartIcon";
    }
    else if (classInfo->IsAncestorClass("LightObject"))
    {
        return "Engine/LightIcon";
    }
    else if (classInfo->IsAncestorClass("Character"))
    {
        return "Engine/CharacterIcon";
    }
    else if (classInfo->IsAncestorClass("CameraObject"))
    {
        return "Engine/CameraIcon";
    }
    else if (classInfo->IsAncestorClass("Entity"))
    {
        return "Engine/EntityIcon";
    }

    return "Engine/ObjectIcon";
}

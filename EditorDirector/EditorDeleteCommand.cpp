#include "EditorDeleteCommand.h"
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/SceneComponent.h>
#include <EditorDirector/EditorSelectionManager.h>
EditorDeleteCommand::EditorDeleteCommand() {}

EditorDeleteCommand::~EditorDeleteCommand() {}

bool EditorDeleteCommand::Execute(BaseSelectionManager *selectionMangaer)
{

    if (selectionMangaer == nullptr)
        return false;

    if (selectionMangaer->GetSelectedComponent() != nullptr)
    {

        // 선택된 컴포넌트가있다면 컴포넌트 제거시도
        return DeleteSelectedComponent(selectionMangaer);
    }
    else if (selectionMangaer->GetSelectedObject() != nullptr)
    {

        // 선택된 컴포넌트가없고 오브젝트만있는경우에 제거 시도
        return DeleteSelectedObject(selectionMangaer);
    }

    return false;
}

bool EditorDeleteCommand::DeleteSelectedComponent(BaseSelectionManager *selectionManager)
{
    Component *component = selectionManager->GetSelectedComponent();

    if (!CanDeleteComponenet(component))
        return false;

    selectionManager->SetSelectedComponent(nullptr);
    component->Destory();

    return true;
}

bool EditorDeleteCommand::DeleteSelectedObject(BaseSelectionManager *selectionManager)
{

    Object *object = selectionManager->GetSelectedObject();

    if (!CanDeleteObject(object))
        return false;

    selectionManager->SetSelectedObject(nullptr);
    object->Destroy();

    return true;
}

bool EditorDeleteCommand::CanDeleteComponenet(Component *com)
{

    if (!CanDeleteSingleComponent(com))
        return false;

    if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
    {
        if (!CanDeleteSceneComponentSubtree(sceneCom))
            return false;
    }

    return true;
}

bool EditorDeleteCommand::CanDeleteObject(Object *object)
{

    if (object == nullptr)
        return false;

    if (object->HasObjectFlag(Core::EObjectFlag::eEngineEntity))
        return false;

    return true;
}

bool EditorDeleteCommand::CanDeleteSingleComponent(Component *com)
{
    if (com == nullptr)
        return false;

    Object *object = com->GetOwnerObject();

    if (object == nullptr)
        return false;

    if (!com->HasComponentFlag(Core::EComponentFlag::eEngineAdded))
        return false;

    if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
    {

        Entity *entity = dynamic_cast<Entity *>(object);
        if (entity)
        {
            if (entity->GetRootComponent() == sceneCom)
                return false;
        }
    }

    return true;
}

bool EditorDeleteCommand::CanDeleteSceneComponentSubtree(SceneComponent *com)
{

    if (!CanDeleteSingleComponent(com))
        return false;

    for (auto child : com->GetChildSceneComponentList())
    {
        if (child && !CanDeleteSceneComponentSubtree(child))
        {
            return false;
        }
    }

    return true;
}

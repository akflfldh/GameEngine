#include "EditorSelectionManager.h"
#include <Core/Component.h>
#include <Core/IDrawableOutline.h>
#include <Core/Object.h>

EditorSelectionManager *EditorSelectionManager::GetInstance()
{

    static EditorSelectionManager instance;
    return &instance;
}

EditorSelectionManager::EditorSelectionManager() {}

EditorSelectionManager::~EditorSelectionManager() {}

BaseSelectionManager::BaseSelectionManager() {}

BaseSelectionManager::~BaseSelectionManager() {}

void BaseSelectionManager::SetSelectedObject(Object *object)
{
    mSelectedComponent = nullptr;
    mOnSelectedComponentCallbackSystem.ExecuteCallbacks(nullptr);

    if (mSelectedObject)
    {
        for (auto com : mSelectedObject->GetComponentList())
        {
            if (auto *drawable = dynamic_cast<Core::IDrawableOutline *>(com))
                drawable->SetDrawOutline(false);
        }
    }

    // 새로운 오브젝트를 갱신하고 아웃라인을 켠다.
    mSelectedObject = object;
    if (mSelectedObject)
    {
        for (auto com : mSelectedObject->GetComponentList())
        {
            if (auto *drawable = dynamic_cast<Core::IDrawableOutline *>(com))
                drawable->SetDrawOutline(true);
        }
    }

    mOnSelectedObjectCallbackSystem.ExecuteCallbacks(object);
}

void BaseSelectionManager::SetSelectedComponent(Component *com)
{
    if (com == nullptr)
    {
        mSelectedComponent = nullptr;
        SetSelectedObject(nullptr);
        return;
    }

    Object *object = com->GetOwnerObject();

    if (mSelectedObject)
    {
        for (auto com : mSelectedObject->GetComponentList())
        {
            if (auto *drawable = dynamic_cast<Core::IDrawableOutline *>(com))
                drawable->SetDrawOutline(false);
        }
    }

    // 새로운 오브젝트를 갱신하고 아웃라인을 켠다.
    mSelectedObject = object;
    if (mSelectedObject)
    {

        if (auto *drawable = dynamic_cast<Core::IDrawableOutline *>(com))
            drawable->SetDrawOutline(true);
    }

    mSelectedComponent = com;

    mOnSelectedObjectCallbackSystem.ExecuteCallbacks(object);
    mOnSelectedComponentCallbackSystem.ExecuteCallbacks(com);
}

void BaseSelectionManager::OnMapObjectRemoved(Object *object)
{

    if (object == mSelectedObject)
    {

        if (mSelectedComponent != nullptr)
        {
            SetSelectedComponent(nullptr);
        }
        else
        {
            SetSelectedObject(nullptr);
        }
    }
}

Object *BaseSelectionManager::GetSelectedObject() const
{
    return mSelectedObject;
}

Component *BaseSelectionManager::GetSelectedComponent() const
{
    return mSelectedComponent;
}

void BaseSelectionManager::ClearSelection()
{
    if (mSelectedComponent)
    { // object처리도함으로 object에관해 두번처리방지
        SetSelectedComponent(nullptr);
        return;
    }

    if (mSelectedObject)
    {
        SetSelectedObject(nullptr);
    }
}

EditorPrefabSelectionManager *EditorPrefabSelectionManager::GetInstance()
{
    static EditorPrefabSelectionManager instance;
    return &instance;
}

EditorPrefabSelectionManager::EditorPrefabSelectionManager() {}

EditorPrefabSelectionManager::~EditorPrefabSelectionManager() {}

EditorMaterialSelectionManager *EditorMaterialSelectionManager::GetInstance()
{
    static EditorMaterialSelectionManager instance;
    return &instance;
}

EditorMaterialSelectionManager::EditorMaterialSelectionManager() {}

EditorMaterialSelectionManager::~EditorMaterialSelectionManager() {}

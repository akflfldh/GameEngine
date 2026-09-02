#include "EditorCameraVisualizerObject.h"
#include "EditorLightVisualizerObject.h"
#include <Core/CameraComponent.h>
#include <Core/StaticMeshComponent.h>

EditorCameraVisualizerObject::EditorCameraVisualizerObject()
{

    mIconMeshComponent = static_cast<StaticMeshComponent *>(CreateComponent<StaticMeshComponent>("IconMeshCom"));
    mIconMeshComponent->SetParent(mRootSceneComponent);
}

EditorCameraVisualizerObject ::~EditorCameraVisualizerObject() {}

void EditorCameraVisualizerObject::Bind(CameraComponent *sourceCamera)
{

    if (mSourceCamera)
    {
        if (mComDestroyCallbackID != Core::CallbackIDNone)
            mSourceCamera->mOnDestoryRequestedCallbackSystem.UnRegister(mComDestroyCallbackID);
    }

    mSourceCamera = sourceCamera;
    mComDestroyCallbackID = Core::CallbackIDNone;

    if (mSourceCamera)
    {
        // component destory 콜백등록
        mComDestroyCallbackID =
            mSourceCamera->mOnDestoryRequestedCallbackSystem.Register([this](Component *com) { UnBind(); });
        SetActive(true);
    }
    else
    {
        SetActive(false);
    }
}

CameraComponent *EditorCameraVisualizerObject::GetSourceCamera() const
{
    return mSourceCamera;
}

void EditorCameraVisualizerObject::OnBegin()
{
    Entity::OnBegin();

    if (mIconMeshComponent)
    {
        mIconMeshComponent->SetMesh("Engine/EditorCameraMesh");

    }
}

void EditorCameraVisualizerObject::Tick(float DeltaTime)
{
    Entity::Tick(DeltaTime);

    // SyncFromSource();
    SyncFromSource();
}

void EditorCameraVisualizerObject::SyncFromSource()
{

    if (mSourceCamera == nullptr)
        return;

    SetPositionWorld(mSourceCamera->GetPositionWorld());
    SetRotationWorld(mSourceCamera->GetRotationWorld());
}

void EditorCameraVisualizerObject::SelectSource(BaseSelectionManager *selectionManager) {}

void EditorCameraVisualizerObject::UnBind()
{
    mSourceCamera = nullptr;
    mComDestroyCallbackID = Core::CallbackIDNone;
    SetActive(false);
}

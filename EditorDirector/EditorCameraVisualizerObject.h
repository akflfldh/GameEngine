#pragma once

#include <Core/Entity.h>
#include <EditorDirector/IEditorVisualizerObject.h>

#include "EditorCameraVisualizerObject.generated.h"

class CameraComponent;
class StaticMeshComponent;

class REFLECT_CLASS(EngineClass) EditorCameraVisualizerObject : public Entity, public IEditorVisualizerObject
{
    GENERATED_BODY(EditorCameraVisualizerObject)
  public:
    EditorCameraVisualizerObject();
    virtual ~EditorCameraVisualizerObject();

    void Bind(CameraComponent *sourceCamera);
    CameraComponent *GetSourceCamera() const;

    void SyncFromSource();

    virtual void SelectSource(BaseSelectionManager *selectionManager) override;

    void UnBind();

  protected:
    virtual void OnBegin() override;
    virtual void Tick(float DeltaTime) override;

  private:
    CameraComponent *mSourceCamera = nullptr;
    StaticMeshComponent *mIconMeshComponent = nullptr;
    Core::CallbackID mComDestroyCallbackID = Core::CallbackIDNone;
};

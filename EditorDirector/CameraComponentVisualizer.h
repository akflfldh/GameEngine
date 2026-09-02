#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/IEditorVisualizer.h>
#include <vector>

class Map;
class CameraComponent;
class EditorCameraVisualizerObject;

class CameraComponentVisualizer : public IEditorVisualizer
{
  public:
    CameraComponentVisualizer();
    ~CameraComponentVisualizer();

    void Update(CameraComponent *editorCamera);
    void SetVisible(bool visible);

  protected:
    virtual void OnBeforeBindingMap() override;
    virtual void OnBindMap(Map *sourceMap) override;

  private:
    void RebuildVisualObjects();

    void OnCameraComponentAdded(CameraComponent *com);
    void OnCameraComponentRemoved(CameraComponent *com);

    void RemoveVisualObject(CameraComponent *com);
    void AddVisualObject(CameraComponent *com);

    void ClearVisualObjects();

  private:
    Core::CallbackID mCameraComponentAddedCallbackID = Core::CallbackIDNone;
    Core::CallbackID mCameraComponentRemovedCallbackID = Core::CallbackIDNone;

    std::vector<EditorCameraVisualizerObject *> mEntityList;
};

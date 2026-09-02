#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/IEditorVisualizer.h>
#include <vector>

class Map;
class CameraComponent;
class EditorLightVisualizerObject;
class LightComponent;

class LightComponentVisualizer : public IEditorVisualizer
{
  public:
    void Update(CameraComponent *editorCamera);
    void SetVisible(bool visible);

  protected:
    virtual void OnBeforeBindingMap() override;
    virtual void OnBindMap(Map *sourceMap) override;

  private:
    void UnBindMap();

    void RebuildVisualObject();

    void OnLightComponentAdded(LightComponent *com);
    void OnLightComponentRemoved(LightComponent *com);

    void AddVisualObject(LightComponent *com);
    void RemoveVisualObject(LightComponent *com);

    void ClearVisualObjects();

  private:
    Core::CallbackID mLightComponentAddedCallbackID = Core::CallbackIDNone;
    Core::CallbackID mLightComponentRemovedCallbackID = Core::CallbackIDNone;

    std::vector<EditorLightVisualizerObject *> mEntityList;
};

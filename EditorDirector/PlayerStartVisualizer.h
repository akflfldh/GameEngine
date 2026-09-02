#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/IEditorVisualizer.h>
#include <vector>

class EditorPlayerStartVisualizerObject;
class PlayerStart;
class PlayerStartVisualizer : public IEditorVisualizer
{
  public:
    PlayerStartVisualizer();
    virtual ~PlayerStartVisualizer();
    virtual void Update(CameraComponent *editorCamera) override;

  protected:
    virtual void OnBeforeBindingMap() override;
    virtual void OnBindMap(Map *sourceMap) override;

    void OnPlayerStartAdded(PlayerStart *playerStart);
    void OnPlayerStartRemoved(PlayerStart *playerStart);

    void RemoveVisualObject(PlayerStart *playerStart);
    void AddVisualObject(PlayerStart *playerStart);

    void ClearVisualObjects();
    void RebuildVisualObjects();

  private:
    std::vector<EditorPlayerStartVisualizerObject *> mEntityList;
    Core::CallbackID mPlayerStartAddedCallbackID = Core::CallbackIDNone;
    Core::CallbackID mPlayerStartRemovedCallbackID = Core::CallbackIDNone;
};

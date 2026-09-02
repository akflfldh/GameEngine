#pragma once

#include <Core/Entity.h>
#include <EditorDirector/IEditorVisualizerObject.h>

#include "EditorPlayerStartVisualizerObject.generated.h"

class PlayerStart;
class BillboardComponent;
class EditorHitProxyComponent;

class REFLECT_CLASS(EngineClass) EditorPlayerStartVisualizerObject : public Entity, public IEditorVisualizerObject
{
    GENERATED_BODY(EditorPlayerStartVisualizerObject)
  public:
    EditorPlayerStartVisualizerObject();
    virtual ~EditorPlayerStartVisualizerObject();

    void Bind(PlayerStart *source);
    PlayerStart *GetSourcePlayerStart() const;

    void SyncFromSource();

    virtual void SelectSource(BaseSelectionManager *selectionManager) override;

  protected:
    virtual void OnBegin() override;
    virtual void Tick(float DeltaTime) override;

  private:
    BillboardComponent *mBillboardCom = nullptr;
    EditorHitProxyComponent *mHitProxyCom = nullptr;
    PlayerStart *mSource = nullptr;
};

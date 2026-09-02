#pragma once

#include <Core/Entity.h>
#include <EditorDirector/IEditorVisualizerObject.h>

#include "EditorLightVisualizerObject.generated.h"

class LightComponent;
class BillboardComponent;
class EditorHitProxyComponent;

class REFLECT_CLASS(EngineClass) EditorLightVisualizerObject : public Entity, public IEditorVisualizerObject
{
    GENERATED_BODY(EditorLightVisualizerObject)

  public:
    EditorLightVisualizerObject();
    virtual ~EditorLightVisualizerObject();

    void Bind(LightComponent *source);
    LightComponent *GetSourceLight() const;

    void SyncFromSource();
    virtual void SelectSource(BaseSelectionManager *selectionManager) override;

  protected:
    virtual void OnBegin() override;
    virtual void Tick(float DeltaTime) override;
    //  void DrawDebugLightVisual();

  private:
    LightComponent *mSourceLight = nullptr;
    BillboardComponent *mBillboardCom = nullptr;
    EditorHitProxyComponent *mHitProxyCom = nullptr;
};

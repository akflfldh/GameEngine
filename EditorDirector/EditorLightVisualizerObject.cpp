#include "EditorLightVisualizerObject.h"
#include <Core/BillboardComponent.h>
#include <Core/DebugDraw.h>
#include <Core/LightComponent.h>
#include <EditorHitProxyComponent.h>
#include <EditorSelectionManager.h>

EditorLightVisualizerObject::EditorLightVisualizerObject()
{

    mBillboardCom = static_cast<BillboardComponent *>(CreateComponent<BillboardComponent>("BillboardCom"));
    mBillboardCom->SetParent(mRootSceneComponent);

    mHitProxyCom = static_cast<EditorHitProxyComponent *>(CreateComponent<EditorHitProxyComponent>("HitProxyCom"));
    mHitProxyCom->SetParent(mRootSceneComponent);
    mHitProxyCom->SetHalfExtent({5, 5, 5});
}

EditorLightVisualizerObject::~EditorLightVisualizerObject() {}

void EditorLightVisualizerObject::OnBegin()
{
    Entity::OnBegin();

    if (mBillboardCom)
    {
        mBillboardCom->SetTexture("Engine/LightBillboard");
        mBillboardCom->SetSize({10, 10});
    }
}
void EditorLightVisualizerObject::Tick(float DeltaTime)
{

    Entity::Tick(DeltaTime);
}

void EditorLightVisualizerObject::Bind(LightComponent *source)
{

    mSourceLight = source;
}

LightComponent *EditorLightVisualizerObject::GetSourceLight() const
{
    return mSourceLight;
}

void EditorLightVisualizerObject::SyncFromSource()
{

    if (mSourceLight == nullptr)
    {
        return;
    }

    SetPositionWorld(mSourceLight->GetPositionWorld());
    SetRotationWorld(mSourceLight->GetRotationWorld());
}

void EditorLightVisualizerObject::SelectSource(BaseSelectionManager *selectionManager)
{

    if (selectionManager == nullptr)
        return;

    if (mSourceLight == nullptr)
        return;

    selectionManager->SetSelectedObject(mSourceLight->GetOwnerObject());
}

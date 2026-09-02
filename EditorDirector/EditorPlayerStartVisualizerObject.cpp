#include "EditorPlayerStartVisualizerObject.h"
#include <Core/BillboardComponent.h>
#include <Core/PlayerStart.h>
#include <EditorHitProxyComponent.h>
#include <EditorSelectionManager.h>

EditorPlayerStartVisualizerObject::EditorPlayerStartVisualizerObject()
{

    mBillboardCom = static_cast<BillboardComponent *>(CreateComponent<BillboardComponent>("BillboardCom"));
    mBillboardCom->SetParent(mRootSceneComponent);

    mHitProxyCom = static_cast<EditorHitProxyComponent *>(CreateComponent<EditorHitProxyComponent>("HitProxyCom"));
    mHitProxyCom->SetParent(mRootSceneComponent);

    mHitProxyCom->SetHalfExtent({5, 5, 5});
}
EditorPlayerStartVisualizerObject ::~EditorPlayerStartVisualizerObject() {}

void EditorPlayerStartVisualizerObject::Bind(PlayerStart *source)
{

    mSource = source;
}

PlayerStart *EditorPlayerStartVisualizerObject::GetSourcePlayerStart() const
{
    return mSource;
}

void EditorPlayerStartVisualizerObject::SyncFromSource()
{

    if (mSource == nullptr)
        return;

    SetPositionWorld(mSource->GetPositionWorld());
    SetRotationWorld(mSource->GetRotationWorld());
}

void EditorPlayerStartVisualizerObject::OnBegin()
{
    Entity::OnBegin();

    if (mBillboardCom)
    {
        mBillboardCom->SetTexture("Engine/PlayerStart");
        mBillboardCom->SetSize({10, 10});
    }
}

void EditorPlayerStartVisualizerObject::Tick(float DeltaTime)
{

    Entity::Tick(DeltaTime);

    SyncFromSource();
}

void EditorPlayerStartVisualizerObject::SelectSource(BaseSelectionManager *selectionManager)
{

    if (selectionManager == nullptr)
        return;

    if (mSource == nullptr)
        return;

    selectionManager->SetSelectedObject(mSource);
}

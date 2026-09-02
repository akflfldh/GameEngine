#include "GizmoMeshComponent.h"
#include <Core/IRenderProxyManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>

Quad::GizmoMeshComponent::GizmoMeshComponent()
{

    if (mRenderProxy)
    {
        mRenderProxy->mIsEditorOverlay = true;
    }

    
}

Quad::GizmoMeshComponent::~GizmoMeshComponent() {}

void Quad::GizmoMeshComponent::OnBegin()
{
    StaticMeshComponent::OnBegin();
}

void Quad::GizmoMeshComponent::SetColor(const CoreMath::Vector4 &color)
{

    mColor = color;
    if (mRenderProxy)
    {
        mRenderProxy->mCustomShaderData = mColor;
    }
}

void Quad::GizmoMeshComponent::OnSettedMesh()
{

    if (mStaticMeshPtr.GetAssetID() != NoneAssetID)
    {

        if (mGizmoMaterial.GetAssetID() == NoneAssetID)
        {
            mGizmoMaterial =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Material>("Engine/GizmoMaterial");
        }

        for (int i = 0; i < mSubMeshMaterialList.size(); ++i)
        {
            mSubMeshMaterialList[i] = mGizmoMaterial;
        }
    }

    UpdateRenderProxy();
}

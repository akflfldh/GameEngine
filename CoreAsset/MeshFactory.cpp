#include "MeshFactory.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/SkinningMesh.h>
#include <CoreAsset/StaticMesh.h>

CoreAsset::MeshFactory *CoreAsset::MeshFactory::GetInstance()
{
    static MeshFactory instance;
    return &instance;
}

CoreAsset::MeshFactory::MeshFactory() {}

CoreAsset::MeshFactory::~MeshFactory() {}

CoreAsset::Asset *CoreAsset::MeshFactory::CreateEmptyAsset(EAssetType assetType)
{
    if (assetType == EAssetType::eStaticMesh)
    {
        return CreateMesh(assetType);
    }
    else if (assetType == EAssetType::eSkinningMesh)
    {
        return CreateMesh(assetType);
    }

    return nullptr;
}

CoreAsset::Asset *CoreAsset::MeshFactory::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{

    EAssetType assetType = intermediateAsset.mAssetType;
    if (assetType == EAssetType::eStaticMesh)
    {
        const IntermediateStaticMesh &intermediateStaticMesh =
            static_cast<const IntermediateStaticMesh &>(intermediateAsset);

        StaticMesh *staticMesh = static_cast<StaticMesh *>(CreateMesh(assetType));

        staticMesh->SetIndexVector(std::move(intermediateStaticMesh.mIndexVector));
        staticMesh->SetVertexVector(std::move(intermediateStaticMesh.mVertexVector),
                                    intermediateStaticMesh.bCaculateAABB);
        staticMesh->SetSubMeshVector(std::move(intermediateStaticMesh.mSubMeshVector));

        auto assetManager = AssetManager::GetInstance();

        for (auto &subMesh : staticMesh->GetSubMeshVector())
        {
            if (subMesh.mMaterialID == NoneAssetID)
            {
                subMesh.mMaterialID = assetManager->GetDefaultStaticMeshMaterial().GetAssetID();
            }
        }

        staticMesh->SetEmptyAssetFlag(false);

        return staticMesh;
    }
    else if (assetType == EAssetType::eSkinningMesh)
    {

        const IntermediateSkinningMesh &intermediateSkinningMesh =
            static_cast<const IntermediateSkinningMesh &>(intermediateAsset);

        SkinningMesh *skinningMesh = static_cast<SkinningMesh *>(CreateMesh(assetType));

        skinningMesh->SetIndexVector(std::move(intermediateSkinningMesh.mIndexVector));
        skinningMesh->SetVertexVector(std::move(intermediateSkinningMesh.mVertexVector));
        skinningMesh->SetSubMeshVector(std::move(intermediateSkinningMesh.mSubMeshVector));

        auto assetManager = AssetManager::GetInstance();

        for (auto &subMesh : skinningMesh->GetSubMeshVector())
        {
            if (subMesh.mMaterialID == NoneAssetID)
            {
                subMesh.mMaterialID = assetManager->GetDefaultStaticMeshMaterial().GetAssetID();
            }
        }

        skinningMesh->SetEmptyAssetFlag(false);

        SkinBinding skinBinding;
        skinBinding.mInverseBindMatrices = std::move(intermediateSkinningMesh.mInverseBindMatrices);
        skinBinding.mPaletteToSkeletonJoint = std::move(intermediateSkinningMesh.mPaletteToSkeletonJoint);
        skinBinding.mMeshToSkeleton = intermediateSkinningMesh.mMeshToSkeleton;



        skinningMesh->SetSkinBinding(std::move(skinBinding));

        return skinningMesh;
    }

    return nullptr;
}

CoreAsset::Mesh *CoreAsset::MeshFactory::CreateMesh(EAssetType type)
{

    switch (type)
    {
    case EAssetType::eStaticMesh:

        return new StaticMesh;

        break;
    case EAssetType::eSkinningMesh:
        return new SkinningMesh;
        break;
    }

    return nullptr;
}

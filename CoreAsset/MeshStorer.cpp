#include "MeshStorer.h"
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/StaticMesh.h>

CoreAsset::MeshStorer *CoreAsset::MeshStorer::GetInstance()
{

    static MeshStorer instance;

    return &instance;
}

CoreAsset::MeshStorer::MeshStorer() {}

CoreAsset::MeshStorer::~MeshStorer() {}

void CoreAsset::MeshStorer::StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData)
{

    MeshCommonHeader meshCommonHeader;

    MeshMetaData *meshMetaData = static_cast<MeshMetaData *>(assetMetaData);
    // meshCommonHeader.mRawFileName = meshMetaData->mRawFileName;
    meshCommonHeader.Serialize(arch);

    asset->Serialize(arch);
}

bool CoreAsset::MeshStorer::Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                                  const std::string &filePath)
{

    return false;
}

bool CoreAsset::MeshStorer::StoreAssetRawDataFile(Arch &arch, CoreAsset::Asset *asset,
                                                  CoreAsset::AssetMetaData *metaData)
{

    Mesh *mesh = static_cast<Mesh *>(asset);

    std::vector<MeshIndexType> &indexVector = mesh->GetMeshIndexVector();

    arch << indexVector;

    switch (asset->GetType())
    {
    case EAssetType::eStaticMesh:
    {
        StaticMesh *staticMesh = static_cast<StaticMesh *>(mesh);
        std::vector<StaticVertex> &vertexVector = staticMesh->GetVertexVector();
        arch << vertexVector;
    }
    break;

    case EAssetType::eSkinningMesh:

        break;
    }

    return true;
}

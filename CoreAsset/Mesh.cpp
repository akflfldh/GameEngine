#include "Mesh.h"
#include <CoreBase/Arch.h>
CoreAsset::Mesh::Mesh(EAssetType meshType) : Asset(meshType), mIndexNum(0) {}

CoreAsset::Mesh::~Mesh() {}

void CoreAsset::Mesh::Serialize(Arch &arch)
{

    Asset::Serialize(arch);
    arch << mIndexNum;
    arch << mSubMeshVector;
}

const std::vector<CoreAsset::SubMesh> &CoreAsset::Mesh::GetSubMeshVector() const
{
    return mSubMeshVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

std::vector<CoreAsset::SubMesh> &CoreAsset::Mesh::GetSubMeshVector()
{

    return const_cast<std::vector<CoreAsset::SubMesh> &>((static_cast<const Mesh *>(this)->GetSubMeshVector()));
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::Mesh::SetSubMeshVector(const std::vector<SubMesh> &vec)
{

    mSubMeshVector = vec;
}

void CoreAsset::Mesh::SetSubMeshVector(std::vector<SubMesh> &&vec)
{

    mSubMeshVector = std::move(vec);
}

void CoreAsset::Mesh::SetSubMeshMaterial(AssetID materialID, unsigned int subMeshIndex)
{

    if (mSubMeshVector.size() <= subMeshIndex)
        return;

    mSubMeshVector[subMeshIndex].mMaterialID = materialID;
}

uint64_t CoreAsset::Mesh::GetIndexNum() const
{
    return mIndexNum;
}

const std::vector<CoreAsset::MeshIndexType> &CoreAsset::Mesh::GetMeshIndexVector() const
{

    return mIndexVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

std::vector<CoreAsset::MeshIndexType> &CoreAsset::Mesh::GetMeshIndexVector()
{
    // TODO: 여기에 return 문을 삽입합니다.
    return mIndexVector;
}

void CoreAsset::Mesh::SetIndexVector(std::vector<MeshIndexType> &&vec)
{

    mIndexVector = std::move(vec);
    mIndexNum = mIndexVector.size();
}

void CoreAsset::Mesh::SetIndexVector(const std::vector<MeshIndexType> &vec)
{

    mIndexVector = vec;
    mIndexNum = mIndexVector.size();
}

const CoreMath::AABB &CoreAsset::Mesh::GetAABB() const
{
    return mAABB;
}

void CoreAsset::Mesh::SetAABB(const CoreMath::AABB &aabb)
{
    mAABB = aabb;
}

CORE_ASSET_API Arch &CoreAsset::operator<<(Arch &arch, SubMesh &subMesh)
{

    arch << subMesh.mName;
    arch << subMesh.mMaterialID;
    arch << subMesh.mVertexOffset;
    arch << subMesh.mIndexOffset;
    arch << subMesh.mIndexNum;

    // TODO: 여기에 return 문을 삽입합니다.
    return arch;
}

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

bool CoreAsset::Mesh::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const Mesh *sourceMesh = dynamic_cast<const Mesh *>(&source);
    if (!sourceMesh || GetType() != source.GetType())
    {
        if (failureReason)
            *failureReason = "같은 타입의 Mesh 에셋이 필요합니다.";
        return false;
    }
    // 헤더만 로드된 메시를 복제하면 새 ID에는 읽어 올 원본 raw 파일이 없어 데이터가 유실된다.
    if (sourceMesh->mIndexVector.size() != sourceMesh->mIndexNum)
    {
        if (failureReason)
            *failureReason = "Mesh의 index raw data를 먼저 로드해야 합니다.";
        return false;
    }
    mIndexVector = sourceMesh->mIndexVector;
    mIndexNum = sourceMesh->mIndexNum;
    // SubMesh의 MaterialID는 같은 외부 에셋 참조를 유지한다.
    mSubMeshVector = sourceMesh->mSubMeshVector;
    mAABB = sourceMesh->mAABB;
    return Asset::CopyDataFrom(source, failureReason);
}

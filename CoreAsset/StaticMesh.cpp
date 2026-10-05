#include "StaticMesh.h"
#include <CoreBase/Arch.h>
CoreAsset::StaticMesh::StaticMesh() : Mesh(EAssetType::eStaticMesh) {}

CoreAsset::StaticMesh::~StaticMesh() {}

bool CoreAsset::StaticMesh::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const StaticMesh *sourceMesh = dynamic_cast<const StaticMesh *>(&source);
    if (!sourceMesh)
    {
        if (failureReason)
            *failureReason = "StaticMesh 에셋이 필요합니다.";
        return false;
    }
    if (sourceMesh->mVertexVector.size() != sourceMesh->mVertexNum)
    {
        if (failureReason)
            *failureReason = "StaticMesh의 vertex raw data를 먼저 로드해야 합니다.";
        return false;
    }
    if (!Mesh::CopyDataFrom(source, failureReason))
        return false;

    // geometry와 충돌 설정은 독립 소유하며, AABB는 Mesh가 복사한 원본 값을 보존한다.
    mVertexVector = sourceMesh->mVertexVector;
    mVertexNum = sourceMesh->mVertexNum;
    mPhysicsCollisionPreset = sourceMesh->mPhysicsCollisionPreset;
    return true;
}

CoreAsset::EAssetType CoreAsset::StaticMesh::GetAssetType()
{

    return CoreAsset::EAssetType::eStaticMesh;
}

void CoreAsset::StaticMesh::Serialize(Arch &arch)
{

    Mesh::Serialize(arch);

    arch << mVertexNum;
    arch << mPhysicsCollisionPreset;
}

const std::vector<CoreAsset::StaticVertex> &CoreAsset::StaticMesh::GetVertexVector() const
{
    return mVertexVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

std::vector<CoreAsset::StaticVertex> &CoreAsset::StaticMesh::GetVertexVector()
{
    return mVertexVector;
    // TODO: 여기에 return 문을 삽입합니다.
}

void CoreAsset::StaticMesh::SetVertexVector(std::vector<StaticVertex> &&vec, bool bCaculateAABB)
{

    mVertexVector = std::move(vec);
    mVertexNum = mVertexVector.size();

    if (bCaculateAABB)
        CaculateAABB();
}

void CoreAsset::StaticMesh::SetVertexVector(const std::vector<StaticVertex> &vec, bool bCaculateAABB)
{
    mVertexVector = vec;
    mVertexNum = mVertexVector.size();

    if (bCaculateAABB)
        CaculateAABB();
}

void *CoreAsset::StaticMesh::GetVertexData()
{
    return mVertexVector.data();
}

uint64_t CoreAsset::StaticMesh::GetVertexNum() const
{

    return mVertexVector.size();
}
uint32_t CoreAsset::StaticMesh ::GetVertexStride() const
{

    return sizeof(StaticVertex);
}

void CoreAsset::StaticMesh::SetPhysicsCollisionPreset(const PhysicsCollisionPreset &preset)
{

    mPhysicsCollisionPreset = preset;
}

const PhysicsCollisionPreset &CoreAsset::StaticMesh::GetPhysicsCollisionPreset() const
{

    return mPhysicsCollisionPreset;
}

void CoreAsset::StaticMesh::CaculateAABB()
{
    if (mVertexVector.size() == 0)
        return;

    float minPos[3] = {mVertexVector[0].mPos.X, mVertexVector[0].mPos.Y, mVertexVector[0].mPos.Z};
    float maxPos[3] = {mVertexVector[0].mPos.X, mVertexVector[0].mPos.Y, mVertexVector[0].mPos.Z};

    for (size_t i = 1; i < mVertexVector.size(); ++i)
    {
        float pos[3] = {mVertexVector[i].mPos.X, mVertexVector[i].mPos.Y, mVertexVector[i].mPos.Z};

        for (int j = 0; j < 3; ++j)
        {
            if (minPos[j] > pos[j])
                minPos[j] = pos[j];

            if (maxPos[j] < pos[j])
                maxPos[j] = pos[j];
        }
    }

    CoreMath::AABB aabb;
    aabb.mMin = {minPos[0], minPos[1], minPos[2]};
    aabb.mMax = {maxPos[0], maxPos[1], maxPos[2]};
    SetAABB(aabb);
}

#include "StaticMesh.h"
#include <CoreBase/Arch.h>
CoreAsset::StaticMesh::StaticMesh() : Mesh(EAssetType::eStaticMesh) {}

CoreAsset::StaticMesh::~StaticMesh() {}

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
uint64_t CoreAsset::StaticMesh::GetVertexNum() const
{

    return mVertexVector.size();
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

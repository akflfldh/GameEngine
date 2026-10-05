#include "SkinningMesh.h"

namespace CoreAsset
{
namespace
{
void SerializeMatrix(Arch &arch, CoreMath::Matrix4X4 &matrix)
{
    // Matrix4X4의 compiler padding이나 raw memory ABI 대신 계약상의 column/성분 순서를 명시적으로 저장한다.
    for (int column = 0; column < 4; ++column)
    {
        arch << matrix.mat[column].X << matrix.mat[column].Y << matrix.mat[column].Z << matrix.mat[column].W;
    }
}
} // namespace

bool SkinBinding::Validate(const Skeleton &skeleton, std::string *failureReason) const
{
    // AssetID는 참조 대상을, signature는 같은 ID 아래에서 바뀐 joint 구조를 함께 검출한다.
    if (mSkeletonAssetID != skeleton.GetID() || mSkeletonSignature != skeleton.GetStructureSignature())
    {
        if (failureReason != nullptr)
            *failureReason = "SkinBinding과 Skeleton의 AssetID 또는 structure signature가 일치하지 않습니다.";
        return false;
    }

    // vertex의 palette index 하나가 remap과 inverse bind의 같은 위치를 가리키므로 두 배열 길이는 같아야 한다.
    if (mPaletteToSkeletonJoint.size() != mInverseBindMatrices.size())
    {
        if (failureReason != nullptr)
            *failureReason = "palette remap과 inverse bind matrix 개수가 다릅니다.";
        return false;
    }

    for (uint32_t jointIndex : mPaletteToSkeletonJoint)
    {
        if (jointIndex >= skeleton.GetJoints().size())
        {
            if (failureReason != nullptr)
                *failureReason = "palette remap이 Skeleton joint 범위를 벗어났습니다.";
            return false;
        }
    }

    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}

Arch &operator<<(Arch &arch, SkinBinding &binding)
{
    arch << binding.mSkeletonAssetID << binding.mSkeletonSignature << binding.mPaletteToSkeletonJoint;

    uint64_t matrixCount = static_cast<uint64_t>(binding.mInverseBindMatrices.size());
    arch << matrixCount;
    if (arch.GetLoadingFlag())
        binding.mInverseBindMatrices.resize(matrixCount);
    for (CoreMath::Matrix4X4 &matrix : binding.mInverseBindMatrices)
        SerializeMatrix(arch, matrix);

    SerializeMatrix(arch, binding.mMeshToSkeleton);
    return arch;
}

SkinningMesh::SkinningMesh() : Mesh(EAssetType::eSkinningMesh) {}

bool SkinningMesh::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const SkinningMesh *sourceMesh = dynamic_cast<const SkinningMesh *>(&source);
    if (!sourceMesh)
    {
        if (failureReason)
            *failureReason = "SkinningMesh 에셋이 필요합니다.";
        return false;
    }
    if (sourceMesh->mVertexVector.size() != sourceMesh->mVertexNum)
    {
        if (failureReason)
            *failureReason = "SkinningMesh의 vertex raw data를 먼저 로드해야 합니다.";
        return false;
    }
    if (!Mesh::CopyDataFrom(source, failureReason))
        return false;

    // palette index와 remap/inverse bind의 대응은 함께 보존한다.
    // Skeleton은 복제하지 않으므로 binding의 AssetID와 signature도 원본 참조를 유지한다.
    mFormatVersion = sourceMesh->mFormatVersion;
    mVertexVector = sourceMesh->mVertexVector;
    mVertexNum = sourceMesh->mVertexNum;
    mSkinBinding = sourceMesh->mSkinBinding;
    return true;
}

EAssetType SkinningMesh::GetAssetType()
{
    return EAssetType::eSkinningMesh;
}

void SkinningMesh::Serialize(Arch &arch)
{
    // vertex bulk data는 기존 Mesh raw-data 경로가 저장하므로 asset payload에는 binding과 검증용 개수만 둔다.
    Mesh::Serialize(arch);
    arch << mFormatVersion << mVertexNum << mSkinBinding;
}

const std::vector<SkinningVertex> &SkinningMesh::GetVertexVector() const
{
    return mVertexVector;
}

std::vector<SkinningVertex> &SkinningMesh::GetVertexVector()
{
    return mVertexVector;
}

void SkinningMesh::SetVertexVector(std::vector<SkinningVertex> vertices)
{
    mVertexVector = std::move(vertices);
    mVertexNum = mVertexVector.size();
}

const SkinBinding &SkinningMesh::GetSkinBinding() const
{
    return mSkinBinding;
}

SkinBinding &SkinningMesh::GetSkinBinding()
{
    return mSkinBinding;
}

void SkinningMesh::SetSkinBinding(SkinBinding &&binding)
{
    mSkinBinding = std::move(binding);
}

void *SkinningMesh::GetVertexData()
{

    return mVertexVector.data();
}
uint64_t SkinningMesh::GetVertexNum() const
{

    return mVertexVector.size();
}

uint32_t SkinningMesh::GetVertexStride() const
{

    return sizeof(SkinningVertex);
}

void SkinningMesh::SetSkeleton(AssetID id)
{

    mSkinBinding.mSkeletonAssetID = id;
}

void SkinningMesh::SetSkeletonSignature(uint64_t signature)
{

    mSkinBinding.mSkeletonSignature = signature;
}

} // namespace CoreAsset

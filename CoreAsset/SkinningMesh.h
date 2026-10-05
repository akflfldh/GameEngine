#pragma once

#include <CoreAsset/Mesh.h>
#include <CoreAsset/Skeleton.h>

namespace CoreAsset
{

/// 한 mesh의 vertex palette와 Skeleton 에셋 사이를 연결하는 영구 binding 데이터다.
/// Skeleton 객체의 수명이나 현재 pose를 소유하지 않으며 runtime/GPU palette도 저장하지 않는다.
struct CORE_ASSET_API SkinBinding
{
    AssetID mSkeletonAssetID = NoneAssetID; ///< 참조 대상 Skeleton 에셋의 비소유 identity다.
    uint64_t mSkeletonSignature = 0;        ///< 로드된 Skeleton의 joint 구조 호환성을 검사한다.
    /// SkinningVertex의 palette index를 Skeleton의 parent-first joint index로 변환한다.
    /*

    • mPaletteToSkeletonJoint는 메시의 palette 번호를 Skeleton의 joint 번호로 바꾸는 대응표입니다.

     예를 들어 Skeleton에서 Arm이 7번 joint인데, 이 메시에서는 Arm을 palette 0번에 저장했다면:
      mPaletteToSkeletonJoint[0] = 7;
      이 배열이 필요한 이유는 메시마다 사용하는 joint와 저장 순서가 다를 수 있기 때문입니다. 몸과 옷이
      같은 Skeleton 애니메이션을 공유해도 각자 필요한 joint만 자기 palette 순서로 담을 수 있습니다.

    */
    std::vector<uint32_t> mPaletteToSkeletonJoint;
    /// palette와 같은 순서이며 mesh bind object space를 각 joint bind space로 변환한다.
    std::vector<CoreMath::Matrix4X4> mInverseBindMatrices;
    /// import된 mesh bind space와 Skeleton object space 사이의 정적 정렬을 보존한다.
    CoreMath::Matrix4X4 mMeshToSkeleton = CoreMath::Matrix4X4::Identity;

    bool Validate(const Skeleton &skeleton, std::string *failureReason = nullptr) const;
};

CORE_ASSET_API Arch &operator<<(Arch &arch, SkinBinding &binding);

/// skinning vertex raw data와 그 vertex가 사용하는 SkinBinding을 함께 소유하는 mesh 에셋이다.
/// 참조 대상 Skeleton 객체, animation clip, runtime pose, GPU resource는 소유하지 않는다.
class CORE_ASSET_API SkinningMesh : public Mesh
{
  public:
    SkinningMesh();
    ~SkinningMesh() override = default;

    static EAssetType GetAssetType();
    void Serialize(Arch &arch) override;
    bool CopyDataFrom(const Asset &source, std::string *failureReason = nullptr) override;

    const std::vector<SkinningVertex> &GetVertexVector() const;
    std::vector<SkinningVertex> &GetVertexVector();
    void SetVertexVector(std::vector<SkinningVertex> vertices);

    const SkinBinding &GetSkinBinding() const;
    SkinBinding &GetSkinBinding();
    void SetSkinBinding(SkinBinding &&binding);

    virtual void *GetVertexData() override;
    virtual uint64_t GetVertexNum() const override;
    virtual uint32_t GetVertexStride() const override;

    void SetSkeleton(AssetID id);
    void SetSkeletonSignature(uint64_t signature);

  private:
    uint32_t mFormatVersion = 1;
    uint64_t mVertexNum = 0;
    std::vector<SkinningVertex> mVertexVector;
    SkinBinding mSkinBinding; ///< 이 mesh 전용 palette remap과 inverse bind 데이터다.
};
AssetClassName(SkinningMesh)

} // namespace CoreAsset

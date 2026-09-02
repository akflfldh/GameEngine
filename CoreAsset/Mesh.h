#pragma once

#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetType.h>
#include <CoreMath/Geometry.h>
#include <vector>

namespace CoreAsset
{

class CORE_ASSET_API Mesh : public Asset
{
  public:
    Mesh(EAssetType meshType);
    virtual ~Mesh() = 0;

    virtual void Serialize(Arch &arch) override;

    const std::vector<SubMesh> &GetSubMeshVector() const;
    std::vector<SubMesh> &GetSubMeshVector();
    void SetSubMeshVector(const std::vector<SubMesh> &vec);
    void SetSubMeshVector(std::vector<SubMesh> &&vec);

    void SetSubMeshMaterial(AssetID materialID, unsigned int subMeshIndex = 0);

    uint64_t GetIndexNum() const;

    const std::vector<MeshIndexType> &GetMeshIndexVector() const;
    std::vector<MeshIndexType> &GetMeshIndexVector();

    void SetIndexVector(std::vector<MeshIndexType> &&vec);
    void SetIndexVector(const std::vector<MeshIndexType> &vec);

    const CoreMath::AABB &GetAABB() const;

  protected:
    void SetAABB(const CoreMath::AABB &aabb);

  private:
    // 필요한것들중 공통은

    // Index buffer

    std::vector<MeshIndexType> mIndexVector;
    uint64_t mIndexNum;

    // 그리고 머터리얼(머터리얼은 하나 )
    // 근데 머터리얼도 MeshType에 맞는것을 사용해야함

    // subemsh도 submesh는 static, skinnig 이든 , 각 머터리얼 정보, index, vertex의 오프셋 , name 정도
    std::vector<SubMesh> mSubMeshVector;
    CoreMath::AABB mAABB;
};

} // namespace CoreAsset
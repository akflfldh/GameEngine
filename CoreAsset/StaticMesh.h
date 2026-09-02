#pragma once

#include <CoreAsset/Mesh.h>
#include <CoreBase/CollisionPhysicsType.h>

namespace CoreAsset
{

class CORE_ASSET_API StaticMesh : public Mesh
{
  public:
    StaticMesh();
    ~StaticMesh();

    static CoreAsset::EAssetType GetAssetType();

    virtual void Serialize(Arch &arch) override;

    const std::vector<StaticVertex> &GetVertexVector() const;
    std::vector<StaticVertex> &GetVertexVector();

    void SetVertexVector(std::vector<StaticVertex> &&vec, bool bCaculateAABB = true);
    void SetVertexVector(const std::vector<StaticVertex> &vec, bool bCaculateAABB = true);

    uint64_t GetVertexNum() const;

    void SetPhysicsCollisionPreset(const PhysicsCollisionPreset &preset);
    const PhysicsCollisionPreset &GetPhysicsCollisionPreset() const;

  private:
    void CaculateAABB();

  private:
    // vector <staticVertex> // 리스트 ,  gpu버퍼 리소스는 renderFrontend가 관리.
    std::vector<StaticVertex> mVertexVector; // raw data
    uint64_t mVertexNum;

    // 머터리얼

    // 충돌데이터
    PhysicsCollisionPreset mPhysicsCollisionPreset;
};
AssetClassName(StaticMesh)

} // namespace CoreAsset
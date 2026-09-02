#pragma once
#include <CoreAsset/AssetType.h>
#include <vector>
namespace CoreAsset
{

struct SubMeshData
{
    uint32_t mVertexOffset = 0;
    uint32_t mIndexOffset = 0;
    uint32_t mIndexNum = 0;
};

struct MeshData
{
    std::vector<StaticVertex> mVertices;
    std::vector<uint32_t> mIndices;

    std::vector<SubMeshData> mSubMeshDataList;
};

class GeometryGenerator
{
  public:
    ~GeometryGenerator();

    static MeshData CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount,
                                   uint32_t stackCount);

    static MeshData CreateArrow(float cylinderHeight);
    static MeshData CreateArrowCube(float cylinderHeight);
    static MeshData CreateQuad(float radius, float sliceCount);

    /*
    sliceCount min - 3,
    bodyStackCount min - 4
    */
    static MeshData CreateSphere(float radius, int sliceCount, int stackCount);

    static MeshData CreateEditorCamera();

    static MeshData CreateSkySphere(float radius, int sliceCount, int stackCount);

    static void CaculateVertexNoraml(std::vector<StaticVertex> &vertexVec, const std::vector<MeshIndexType> &indexVec);
    static void CaculateTangents(std::vector<StaticVertex> &vertices, const std::vector<uint32_t> &indices);

  private:
    GeometryGenerator();

    static void AddCubeMeshData(float width, float height, float depth, std::vector<StaticVertex> &oVertices,
                                std::vector<uint32_t> &oIndices, const CoreMath::Vector3 &offsetPos = {0, 0, 0});

    static void CaculateTangent(const CoreMath::Vector3 &v1Pos, const CoreMath::Vector3 &v2Pos,
                                const CoreMath::Vector3 &v3Pos, const CoreMath::Vector2 &v1Tex,
                                const CoreMath::Vector2 &v2Tex, const CoreMath::Vector2 &v3Tex,
                                CoreMath::Vector4 &outV1Tangent, CoreMath::Vector4 &outV2Tangent,
                                CoreMath::Vector4 &outV3Tangent, CoreMath::Vector3 &outV1BiTangent,
                                CoreMath::Vector3 &outV2BiTangent, CoreMath::Vector3 &outV3BiTangent);

    // 그람슈미트과정 수행, Tangent의 W성분에 핸드리스 부호값설정
    static void FinishCaculateTangent(const CoreMath::Vector3 &normal, const CoreMath::Vector3 &biTangent,
                                      CoreMath::Vector4 &outTangent);
};

} // namespace CoreAsset

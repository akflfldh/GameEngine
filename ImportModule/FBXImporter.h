#pragma once

#include <CoreAsset/IAssetImporter.h>
#include <CoreMath/CoreMath.h>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CoreAsset
{
struct IntermediateAsset;
class AssetImporterManager;
} // namespace CoreAsset

namespace fbxsdk
{
// template <typename T> class FbxVectorTemplate3;
// struct FbxDouble3;
class FbxManager;
class FbxScene;
class FbxNode;
class FbxMesh;
class FbxLayerElementMaterial;
class FbxSurfaceMaterial;
class FbxAMatrix;
class FbxTexture;
// class FbxVectorTemplate3<FbxDouble>;

// template <typename T> class FbxPropertyT;

} // namespace fbxsdk

namespace Import
{

struct FbxMeshNodeInfo
{
    fbxsdk::FbxNode *mNode = nullptr;
    fbxsdk::FbxMesh *mMesh = nullptr;

    std::vector<fbxsdk::FbxSurfaceMaterial *> mMaterials;
    //   FbxAMatrix localTransform;
    //   FbxAMatrix globalTransform;
};

struct FBXMaterial
{
    CoreMath::Vector3 mDiffuseColor;
    CoreMath::Vector3 mSpecular;
    float mRoughness;
};

struct FBXImportVertex
{
    CoreMath::Vector3 mPosition;
    CoreMath::Vector3 mNormal;
    CoreMath::Vector2 mUV;
    CoreMath::Vector4 mTangent;
    int mMaterialSlotIndex = 0;

    // skinningData
};
struct FBXMaterialKeyContext
{
    CoreAsset::ImportAssetKey mKey;
    bool mVaild = true;
    int mGlobalIndex = 0;
};
struct FBXImportSubMesh
{
    uint32_t mVertexOffset = 0;
    uint32_t mIndexOffset = 0;
    uint32_t mIndexNum = 0;
    uint32_t mMaterialSlotIndex = 0;
};

struct FBXImportMeshPart
{
    uint32_t mStartSubMeshIndex = 0;
    uint32_t mSubMeshCount = 0;
};

struct FBXImportMeshPartInstance
{
    uint32_t mMeshPartIndex = 0;
    CoreMath::Matrix4X4 mLocalTransform;

    std::vector<FBXMaterialKeyContext> mSubMeshMaterialKeyList;
};

struct FBXImportMesh
{

    std::string mName;
    std::vector<FBXImportVertex> mVertices;
    std::vector<uint32_t> mIndices;

    std::vector<FBXImportSubMesh> mSubMeshes;

    std::vector<FBXImportMeshPart> mMeshParts;
    std::vector<FBXImportMeshPartInstance> mMeshPartInstances;
    CoreAsset::ImportAssetKey mMeshKey;
};

struct FBXImportTempMeshPerMat
{
    std::vector<FBXImportVertex> mVertices;
    std::vector<uint32_t> mIndices;
};

struct FBXLoadResult
{
    bool mSuccess = false;
    std::string mFailReason;

    std::vector<std::unique_ptr<CoreAsset::IntermediateAsset>> mIntermediateAssetList;

    FBXLoadResult();
    ~FBXLoadResult();
    FBXLoadResult(const FBXLoadResult &) = delete;
    FBXLoadResult &operator=(const FBXLoadResult &) = delete;
    FBXLoadResult(FBXLoadResult &&) noexcept = default;
    FBXLoadResult &operator=(FBXLoadResult &&) noexcept = default;
};

struct FBXImportAssetContext
{
    std::unordered_map<fbxsdk::FbxMesh *, std::unique_ptr<FBXImportMesh>> mMeshTable;
    std::vector<fbxsdk::FbxNode *> mMeshNodes;
    std::vector<FbxMeshNodeInfo> mMeshNodeInfoList;

    FBXImportMesh mTotalMesh;
};

struct FBXImportContext
{
    fbxsdk::FbxScene *mFbxScene = nullptr;
    std::vector<fbxsdk::FbxNode *> mAllNodes;
    // std::vector<fbxsdk::FbxNode *> mMeshNodes;

    // 해당메시와 연결된 노드들
    std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> mMeshToNodes;

    // materials - key table
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> mMaterialKeyTable;
    std::vector<fbxsdk::FbxSurfaceMaterial *> mGlobalFbxMaterialList;
    std::vector<fbxsdk::FbxTexture *> mFbxTextureList;

    std::vector<FbxMeshNodeInfo> mMeshNodeInfoList;

    FBXImportAssetContext mAssetContext;

    std::string mFbxFileName;

    // vertex의 index순서를 뒤집어야하는지여부
    bool mWindingFlipFlag = false;
    bool mFlipZ = false;

    bool mNeedToCalculateNormals = false;
    bool mNeedToCalculateTangents = false;
};

class FBXImporter : public CoreAsset::IAssetImporter
{
  public:
    static FBXImporter *GetInstance();
    FBXImporter();
    ~FBXImporter();

    void Initialize();
    CoreAsset::ImportPackage Import(const std::filesystem::path &filePath,
                                    CoreAsset::AssetImporterManager *importerManager,
                                    const CoreAsset::ImportExecutionContext &executionContext) const override;

    CoreAsset::ImportPackage Load(const std::filesystem::path &filePath) const;

  private:
    void ConvertAxisSystem(FBXImportContext &importContext, fbxsdk::FbxScene *scene) const;
    void ConvertUnitSystem(fbxsdk::FbxScene *scene) const;
    void Triangulate(fbxsdk::FbxScene *scene) const;

    // Node, Attribute 수집
    void TraverseScene(fbxsdk::FbxScene *scene, FBXImportContext &importContext) const;
    void TraverseNode(fbxsdk::FbxNode *node, FBXImportContext &importContext) const;

#pragma region Extract

    void ExtractData(FBXImportContext &importContext) const;

    void ExtractMeshData(
        FBXImportAssetContext &importAssetContext, fbxsdk::FbxMesh *fbxMesh,
        const std::vector<fbxsdk::FbxNode *> &fbxNodes,
        const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
        bool bIndexFlip) const;

    void ExtractMaterialData(FbxMeshNodeInfo &fbxMeshNodeInfo) const;
    void ExtractMeshLayerData(fbxsdk::FbxMesh *fbxMesh) const;
    void ExtractMaterialLayerElement(fbxsdk::FbxLayerElementMaterial *layerElementMaterial) const;

    bool ExtractVertexNormal(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex, int controlPointIndex,
                             CoreMath::Vector3 &oNormal) const;
    bool ExtractVertexUV(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex, int controlPointIndex,
                         CoreMath::Vector2 &oUV) const;
    bool ExtractVertexTangent(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex, int controlPointIndex,
                              CoreMath::Vector4 &oTangent) const;

    void ExtractTextureList(const std::vector<fbxsdk::FbxSurfaceMaterial *> &fbxSurfaceMaterialList,
                            std::vector<fbxsdk::FbxTexture *> &oFbxTextureList) const;

#pragma endregion

    bool CheckFbxLayerTangentElement(fbxsdk::FbxMesh *fbxMesh) const;
    bool CheckFbxLayerNormalElement(fbxsdk::FbxMesh *fbxMesh) const;
    /*
        Mesh들의 정점,인덱스,subMesh를 머터리얼별로 분류하고 합친다.
    */
    void MergeMeshData(
        FBXImportAssetContext &importAssetContext,
        const std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> &meshToNodes,
        const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
        const std::string &totalMeshName, bool flipZ) const;

    void BakeVertex(std::vector<FBXImportVertex> &vertices, const fbxsdk::FbxAMatrix &matrix, bool bFlipZ) const;

#pragma region Build
    void BuildIntermediateAssets(const FBXImportContext &importContext, CoreAsset::ImportPackage &oImportPackage) const;

    CoreAsset::ImportedIntermediateAsset BuildIntermediateMeshAsset(
        FBXImportMesh *importMesh, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const;

    CoreAsset::ImportedIntermediateAsset BuildIntermediateMeshAsset(
        const FBXImportContext &importContext, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const;

    bool BuildIntermediateMaterialAssets(fbxsdk::FbxSurfaceMaterial *surfaceMaterial,

                                         std::vector<CoreAsset::ImportDependencyContext> &oDependencyList,
                                         CoreAsset::ImportedIntermediateAsset &oImportedIntermediateAsset) const;

    // 중복된 임시 vertex 구축
    void BuildTempVertices(
        fbxsdk::FbxMesh *fbxMesh, std::vector<FBXImportVertex> &oTempVertices,
        const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable) const;
    // vertex 의 중복처리 및 인덱스 배열 구축 ,subMeshList구축
    void BuildIndexedVertices(const std::vector<FBXImportVertex> &tempVertices,
                              std::vector<FBXImportVertex> &oFinalVertices, std::vector<uint32_t> &oFinalIndices,
                              std::vector<FBXImportSubMesh> &oSubMeshList, bool bIndexFlip) const;

    // 메시 part , partInstance를 구축 , fbxMaterial들을 테이블에 등록
    void BuildMeshParts(
        FBXImportMesh *pImportMesh, const std::vector<fbxsdk::FbxNode *> &fbxNodes,
        std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &oFbxMaterialKeyTable) const;

    void CollectTextureDependencyFromProperty(fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial, const char *propertyName,
                                              CoreAsset::EImportDependencySubInfo subInfo,
                                              std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const;

#pragma endregion

    void SetImportPackageOptions(const FBXImportContext &context, CoreAsset::ImportPackage &oImportPackage) const;

    int GetPolygonMaterialSlot(fbxsdk::FbxMesh *fbxMesh, int polygonIndex) const;

#pragma region Utility

    void GetGeometrix(fbxsdk::FbxNode *node, fbxsdk::FbxAMatrix &oMatrix) const;

#pragma endregion

  private:
    fbxsdk::FbxManager *mFbxManager = nullptr;
};
} // namespace Import
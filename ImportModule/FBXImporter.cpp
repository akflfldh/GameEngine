#include "FBXImporter.h"
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/CoreAssert.h>
#include <Logger/Logger.h>
#include <Utility/Utility.h>
#include <fbxsdk.h>
#include <map>
namespace Import
{

static void Copy(const fbxsdk::FbxPropertyT<FbxDouble3> &source, CoreMath::Vector3 &dest)
{

    const FbxDouble3 &fbxD3 = source.Get();
    dest.X = fbxD3[0];
    dest.Y = fbxD3[1];
    dest.Z = fbxD3[2];
}

static void Copy(const fbxsdk::FbxPropertyT<FbxDouble> &source, float &dest)
{
    dest = source.Get();
}

static void Copy(const fbxsdk::FbxDouble4 &source, CoreMath::Vector3 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
    dest.Z = source.mData[2];
}

static void Copy(const fbxsdk::FbxDouble4 &source, CoreMath::Vector4 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
    dest.Z = source.mData[2];
    dest.W = source.mData[3];
}

static void Copy(const fbxsdk::FbxVector2 &source, CoreMath::Vector2 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
}

static void Copy(const fbxsdk::FbxAMatrix &source, CoreMath::Matrix4X4 &dest)
{

    CoreMath::Vector4 col[4];

    for (int c = 0; c < 4; ++c)
    {
        Copy(source.GetColumn(c), col[c]);
    }

    dest = CoreMath::Matrix4X4(col[0], col[1], col[2], col[3]);
}

static void TryCopyVector3Property(fbxsdk::FbxObject *fbxObject, const char *propertyName,
                                   CoreMath::Vector3 &outVector3)
{
    FbxProperty fbxProperty = fbxObject->FindProperty(propertyName);
    if (fbxProperty.IsValid() == false)
        return;

    fbxsdk::FbxDouble3 srcVector3 = fbxProperty.Get<fbxsdk::FbxDouble3>();

    outVector3.X = srcVector3.mData[0];
    outVector3.Y = srcVector3.mData[1];
    outVector3.Z = srcVector3.mData[2];
}

static void TryCopyDoubleProperty(fbxsdk::FbxObject *fbxObject, const char *propertyName, float &oValue)
{
    FbxProperty fbxProperty = fbxObject->FindProperty(propertyName);

    if (fbxProperty.IsValid() == false)
        return;

    fbxsdk::FbxDouble srcValue = fbxProperty.Get<fbxsdk::FbxDouble>();

    oValue = srcValue;
}

FBXLoadResult::FBXLoadResult() {}

FBXLoadResult::~FBXLoadResult() {}

namespace
{
struct FBXDestroyDeleter
{

    template <typename T> void operator()(T *object) const
    {
        if (object)
            object->Destroy();
    }
};

template <typename T> using FbxUniquePtr = std::unique_ptr<T, FBXDestroyDeleter>;
} // namespace

FBXImporter *FBXImporter::GetInstance()
{
    static FBXImporter instance;
    return &instance;
}

FBXImporter::FBXImporter() {}

FBXImporter::~FBXImporter()
{

    if (mFbxManager)
    {
        mFbxManager->Destroy();
    }
}

void FBXImporter::Initialize()
{

    mFbxManager = fbxsdk::FbxManager::Create();

    fbxsdk::FbxIOSettings *ios = fbxsdk::FbxIOSettings::Create(mFbxManager, IOSROOT);
    mFbxManager->SetIOSettings(ios);
}

CoreAsset::ImportPackage FBXImporter::Import(const std::filesystem::path &filePath,
                                             CoreAsset::AssetImporterManager *importerManager,
                                             const CoreAsset::ImportExecutionContext &executionContext) const
{
    // 일단 임시로
    return Load(filePath);
}

CoreAsset::ImportPackage FBXImporter::Load(const std::filesystem::path &filePath) const
{
    CoreAsset::ImportPackage importPackage;

    if (filePath.empty())
    {
        importPackage.mFailReason = "파일 경로가 올바르지못합니다.";
        return importPackage;
    }

    FbxUniquePtr<fbxsdk::FbxImporter> lImporter(fbxsdk::FbxImporter::Create(mFbxManager, ""));
    if (!lImporter->Initialize(filePath.string().c_str(), -1, mFbxManager->GetIOSettings()))
    {

        fbxsdk::FbxString error = lImporter->GetStatus().GetErrorString();
        importPackage.mFailReason = error;
        return importPackage;
    }

    FbxUniquePtr<fbxsdk::FbxScene> lScene(fbxsdk::FbxScene::Create(mFbxManager, "scene"));
    bool bImportResult = lImporter->Import(lScene.get());
    if (bImportResult == false)
    {
        fbxsdk::FbxString error = lImporter->GetStatus().GetErrorString();
        importPackage.mFailReason = error;
        return importPackage;
    }

    FBXImportContext importContext;
    importContext.mFbxFileName = filePath.filename().string();
    // CoreUtility::Utility::GetFileNameFromPath(filePath);
    FbxAxisSystem sceneAxis = lScene->GetGlobalSettings().GetAxisSystem();

    int upAxis, sign;
    sceneAxis.GetUpVector(upAxis);

    ConvertAxisSystem(importContext, lScene.get());
    ConvertUnitSystem(lScene.get());
    Triangulate(lScene.get());

    TraverseScene(lScene.get(), importContext);

    ExtractData(importContext);

    BuildIntermediateAssets(importContext, importPackage);

    SetImportPackageOptions(importContext, importPackage);
    ;

    return importPackage;
}

void FBXImporter::ConvertAxisSystem(FBXImportContext &importContext, fbxsdk::FbxScene *scene) const
{
    fbxsdk::FbxAxisSystem engineAxis(fbxsdk::FbxAxisSystem::eYAxis, fbxsdk::FbxAxisSystem::eParityOdd,
                                     fbxsdk::FbxAxisSystem::eRightHanded);

    // 이후에 에니메이션에서 Z up 에서 yup으로 변환을 AxisSystem의 convert에 맡기지않고 직접 해야할수도있다 (향후
    // 문제점이 생기면)
    if (scene->GetGlobalSettings().GetAxisSystem().GetCoorSystem() == fbxsdk::FbxAxisSystem::eRightHanded)
    {

        //  importContext.mWindingFlipFlag = true;
        importContext.mFlipZ = true;
    }
    // fbxsdk::FbxAxisSystem::DirectX.ConvertScene(scene);
    engineAxis.ConvertScene(scene);
}

void FBXImporter::ConvertUnitSystem(fbxsdk::FbxScene *scene) const
{
    fbxsdk::FbxSystemUnit::cm.ConvertScene(scene);
}

void FBXImporter::Triangulate(fbxsdk::FbxScene *scene) const
{

    FbxGeometryConverter converter(mFbxManager);
    converter.Triangulate(scene, true);
}

void FBXImporter::TraverseScene(fbxsdk::FbxScene *scene, FBXImportContext &importContext) const
{
    fbxsdk::FbxNode *rootNode = scene->GetRootNode();
    TraverseNode(rootNode, importContext);
}

void FBXImporter::TraverseNode(fbxsdk::FbxNode *node, FBXImportContext &importContext) const
{
    importContext.mAllNodes.push_back(node);

    // 일단  attribute가 하나만 바운드 되었다고 전제하자
    fbxsdk::FbxNodeAttribute *nodeAttribute = node->GetNodeAttribute();

    if (nodeAttribute)
    {
        switch (nodeAttribute->GetAttributeType())
        {
        case fbxsdk::FbxNodeAttribute::EType::eMesh:
        {
            fbxsdk::FbxMesh *fbxMesh = static_cast<fbxsdk::FbxMesh *>(nodeAttribute);
            importContext.mMeshToNodes[fbxMesh].push_back(node);
        }
        break;
        }
    }

    for (size_t i = 0; i < node->GetChildCount(); ++i)
    {
        TraverseNode(node->GetChild(i), importContext);
    }
}

void FBXImporter::ExtractData(FBXImportContext &importContext) const
{

    std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> &meshToNodes = importContext.mMeshToNodes;
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> fbxMaterialKeyTable;

    // umap< mesh, globalIndexOffset int>
    std::vector<fbxsdk::FbxSurfaceMaterial *> &globalFbxMaterialList = importContext.mGlobalFbxMaterialList;
    std::vector<fbxsdk::FbxTexture *> fbxTextureList;
    //< fbx surface material - id > table 구축
    for (const auto &e : meshToNodes)
    {
        const std::vector<fbxsdk::FbxNode *> &fbxNodes = e.second;

        for (auto node : fbxNodes)
        {
            int materialCount = node->GetMaterialCount();

            if (materialCount == 0)
            {
                auto it = fbxMaterialKeyTable.find(nullptr);
                if (it == fbxMaterialKeyTable.end())
                {

                    globalFbxMaterialList.push_back(nullptr);
                    fbxMaterialKeyTable[nullptr].mGlobalIndex = globalFbxMaterialList.size() - 1;
                    fbxMaterialKeyTable[nullptr].mVaild = false;
                    fbxMaterialKeyTable[nullptr].mKey = "";
                }
            }
            else
            {

                for (int i = 0; i < materialCount; ++i)
                {
                    fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = node->GetMaterial(i);

                    auto it = fbxMaterialKeyTable.find(fbxSurfaceMaterial);
                    if (it == fbxMaterialKeyTable.end())
                    {

                        globalFbxMaterialList.push_back(fbxSurfaceMaterial);
                        int index = globalFbxMaterialList.size() - 1;
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mKey = fbxSurfaceMaterial->GetName();
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mVaild = true;
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mGlobalIndex = index;
                    }
                }
            }
        }
    }

    // fbx material에서 fbx texture list 구축

    ExtractTextureList(globalFbxMaterialList, fbxTextureList);

    // 순수하게 mesh 정보 추출
    for (const auto &e : meshToNodes)
    {
        fbxsdk::FbxMesh *fbxMesh = e.first;
        const std::vector<fbxsdk::FbxNode *> &fbxNodes = e.second;

        // 메시및 관련 데이터 추출 및 구축
        ExtractMeshData(importContext.mAssetContext, fbxMesh, fbxNodes, fbxMaterialKeyTable,
                        importContext.mWindingFlipFlag);

        // 하나의 메시라도 필요하다면 전체메시들에대해서 다 계산한다.

        importContext.mNeedToCalculateNormals =
            importContext.mNeedToCalculateNormals || CheckFbxLayerNormalElement(fbxMesh);

        importContext.mNeedToCalculateTangents =
            importContext.mNeedToCalculateTangents || CheckFbxLayerTangentElement(fbxMesh);
    }

    MergeMeshData(importContext.mAssetContext, importContext.mMeshToNodes, fbxMaterialKeyTable,
                  importContext.mFbxFileName, importContext.mFlipZ);

    importContext.mAssetContext.mMeshNodeInfoList = std::move(importContext.mMeshNodeInfoList);
    importContext.mMaterialKeyTable = std::move(fbxMaterialKeyTable);
    importContext.mFbxTextureList = std::move(fbxTextureList);
}

void FBXImporter::ExtractMeshData(
    FBXImportAssetContext &importAssetContext, fbxsdk::FbxMesh *fbxMesh, const std::vector<fbxsdk::FbxNode *> &fbxNodes,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    bool bIndexFlip) const
{

    importAssetContext.mMeshTable[fbxMesh] = std::make_unique<FBXImportMesh>();
    FBXImportMesh *pImportMesh = importAssetContext.mMeshTable[fbxMesh].get();
    pImportMesh->mName = fbxMesh->GetName();

    std::vector<FBXImportVertex> tempVertices;
    std::vector<uint32_t> templIndices;

    BuildTempVertices(fbxMesh, tempVertices, fbxMaterialKeyTable);

    std::vector<FBXImportVertex> finalVertices;
    std::vector<uint32_t> finalIndices;
    std::vector<FBXImportSubMesh> importSubMeshList;

    // 중복 처리 (중복 정점 제거, 인덱스값 조정 )
    // 위치, 노멀, uv ,material index 가 동일하면 동일한 vertex 중복 처
    BuildIndexedVertices(tempVertices, finalVertices, finalIndices, importSubMeshList, bIndexFlip);

    pImportMesh->mVertices = std::move(finalVertices);
    pImportMesh->mIndices = std::move(finalIndices);
    pImportMesh->mSubMeshes = std::move(importSubMeshList);

    // BuildMeshParts(pImportMesh, fbxNodes, fbxMaterialKeyTable);
}

void FBXImporter::ExtractMaterialData(FbxMeshNodeInfo &fbxMeshNodeInfo) const
{

    if (fbxMeshNodeInfo.mNode == nullptr)
        return;

    fbxsdk::FbxNode *node = fbxMeshNodeInfo.mNode;

    int materialCount = node->GetMaterialCount();

    for (int i = 0; i < materialCount; ++i)
    {
        fbxsdk::FbxSurfaceMaterial *surfaceMaterial = node->GetMaterial(i);
        fbxMeshNodeInfo.mMaterials.push_back(surfaceMaterial);

        if (surfaceMaterial->GetClassId().Is(fbxsdk::FbxSurfacePhong::ClassId))
        {

            fbxsdk::FbxSurfacePhong *surfacePhong = static_cast<fbxsdk::FbxSurfacePhong *>(surfaceMaterial);
        }
        else if (surfaceMaterial->GetClassId().Is(fbxsdk::FbxSurfaceLambert::ClassId))
        {
        }
    }
}

void FBXImporter::ExtractMeshLayerData(fbxsdk::FbxMesh *fbxMesh) const
{
    // 0번 layer만 처리
    fbxsdk::FbxLayer *layer = fbxMesh->GetLayer(0);

    if (layer == nullptr)
        return;

    fbxsdk::FbxLayerElementMaterial *layerElementMaterial = layer->GetMaterials();
    if (layerElementMaterial)
    {
        // ExtractMaterial
        ExtractMaterialLayerElement(layerElementMaterial);
    }
}

void FBXImporter::ExtractMaterialLayerElement(fbxsdk::FbxLayerElementMaterial *layerElementMaterial) const {}

bool FBXImporter::ExtractVertexNormal(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                      int controlPointIndex, CoreMath::Vector3 &oNormal) const
{

    fbxsdk::FbxLayerElementNormal *fbxLayerElementNormal = fbxMesh->GetElementNormal();
    if (fbxLayerElementNormal == nullptr)
        return false;

    fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementNormal->GetMappingMode();
    fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementNormal->GetReferenceMode();

    int normalIndex = 0;
    if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    {
        normalIndex = controlPointIndex;
    }
    else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    {
        normalIndex = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
    }

    if (referenceMode == fbxsdk::FbxLayerElement::eIndexToDirect)
    {
        normalIndex = fbxLayerElementNormal->GetIndexArray().GetAt(normalIndex);
    }

    fbxsdk::FbxVector4 fbxNormal = fbxLayerElementNormal->GetDirectArray().GetAt(normalIndex);
    Copy(fbxNormal, oNormal);

    return true;
}

bool FBXImporter::ExtractVertexUV(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                  int controlPointIndex, CoreMath::Vector2 &oUV) const
{

    fbxsdk::FbxLayerElementUV *fbxLayerElementUV = fbxMesh->GetElementUV();
    if (fbxLayerElementUV == nullptr)
        return false;

    fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementUV->GetMappingMode();
    fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementUV->GetReferenceMode();

    int index = 0;
    if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    {
        index = controlPointIndex;
    }
    else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    {
        //   index = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
        index = fbxMesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
    }

    if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
    {
        index = fbxLayerElementUV->GetIndexArray().GetAt(index);
    }

    fbxsdk::FbxVector2 fbxTex = fbxLayerElementUV->GetDirectArray().GetAt(index);
    Copy(fbxTex, oUV);
    oUV.Y = 1.0f - oUV.Y;

    return true;
}

bool FBXImporter::ExtractVertexTangent(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                       int controlPointIndex, CoreMath::Vector4 &oTangent) const
{

    fbxsdk::FbxLayerElementTangent *fbxLayerElementTangent = fbxMesh->GetElementTangent();

    if (fbxLayerElementTangent == nullptr)
    {
        return false;
    }

    fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementTangent->GetMappingMode();
    fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementTangent->GetReferenceMode();
    int index = 0;
    if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    {
        index = controlPointIndex;
    }
    else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    {
        //   index = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
        index = fbxMesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
    }

    if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
    {
        index = fbxLayerElementTangent->GetIndexArray().GetAt(index);
    }

    fbxsdk::FbxVector4 fbxTangent = fbxLayerElementTangent->GetDirectArray().GetAt(index);
    Copy(fbxTangent, oTangent);

    return true;
}

void FBXImporter::ExtractTextureList(const std::vector<fbxsdk::FbxSurfaceMaterial *> &fbxSurfaceMaterialList,
                                     std::vector<fbxsdk::FbxTexture *> &oFbxTextureList) const
{

    for (auto fbxSurfaceMaterial : fbxSurfaceMaterialList)
    {
        FbxProperty diffuseProperty = fbxSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
        if (diffuseProperty.IsValid())
        {
            fbxsdk::FbxTexture *fbxTexture = diffuseProperty.GetSrcObject<fbxsdk::FbxTexture>();
            if (fbxTexture)
            {
                auto it = std::find(oFbxTextureList.begin(), oFbxTextureList.end(), fbxTexture);
                if (it == oFbxTextureList.end())
                {
                    oFbxTextureList.push_back(fbxTexture);
                }
            }
        }
    }
}

bool FBXImporter::CheckFbxLayerTangentElement(fbxsdk::FbxMesh *fbxMesh) const
{
    return fbxMesh->GetElementTangent() != nullptr ? true : false;
}

bool FBXImporter::CheckFbxLayerNormalElement(fbxsdk::FbxMesh *fbxMesh) const
{

    return fbxMesh->GetElementNormal() != nullptr ? true : false;
}

void FBXImporter::MergeMeshData(
    FBXImportAssetContext &importAssetContext,
    const std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> &meshToNodes,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    const std::string &totalMeshName, bool flipZ) const
{

    std::unordered_map<int, FBXImportTempMeshPerMat> tempMeshPerMatTable;

    // 병합된 vertext list 에서의 Node별 mesh vertex offset
    std::unordered_map<fbxsdk::FbxNode *, uint32_t> meshOffsetTable;

    // mesh들의 vertex 병합

    std::vector<FBXImportVertex> &totalVertices = importAssetContext.mTotalMesh.mVertices;
    std::vector<uint32_t> &totalIndices = importAssetContext.mTotalMesh.mIndices;

    for (auto &e : meshToNodes)
    {
        fbxsdk::FbxMesh *fbxMesh = e.first;
        const std::vector<fbxsdk::FbxNode *> &nodes = e.second;

        std::unique_ptr<FBXImportMesh> &importMesh = importAssetContext.mMeshTable[fbxMesh];
        for (auto node : nodes)
        {

            uint32_t meshOffset = totalVertices.size();

            // vertex bake 적용로직이 향후 들어갈것이다.

            // BakeVertex(importMesh->mVertices, localMatrix );

            // CoreMath::Matrix4X4 globalMatrix;
            // Copy(node->EvaluateGlobalTransform(), globalMatrix);
            //  CoreMath::Matrix4X4 matrix = globalMatrix * GetGeometrix(node);
            fbxsdk::FbxAMatrix geoMat;
            GetGeometrix(node, geoMat);

            fbxsdk::FbxAMatrix globalMatrix = node->EvaluateGlobalTransform();
            BakeVertex(importMesh->mVertices, node->EvaluateGlobalTransform() * geoMat, flipZ);
            totalVertices.insert(totalVertices.end(), importMesh->mVertices.begin(), importMesh->mVertices.end());

            meshOffsetTable[node] = meshOffset;
        }
    }

    for (const auto &e : meshToNodes)
    {
        fbxsdk::FbxMesh *fbxMesh = e.first;
        const std::vector<fbxsdk::FbxNode *> &nodes = e.second;

        for (auto fbxNode : nodes)
        {
            std::unique_ptr<FBXImportMesh> &pFbxImportMesh = importAssetContext.mMeshTable[fbxMesh];

            for (const auto &subMesh : pFbxImportMesh->mSubMeshes)
            {

                // global mat index 구하기
                int localMaterialIndex = subMesh.mMaterialSlotIndex;
                auto fbxSurfaceMatIt = fbxMaterialKeyTable.find(fbxNode->GetMaterial(localMaterialIndex));

                if (fbxSurfaceMatIt == fbxMaterialKeyTable.end())
                {
                    continue;
                }

                const FBXMaterialKeyContext &FBXMaterialkeyContext = fbxSurfaceMatIt->second;
                int globalMaterialIndex = FBXMaterialkeyContext.mGlobalIndex;

                // index 복사
                const std::vector<uint32_t> &localIndices = pFbxImportMesh->mIndices;

                uint32_t meshVertexOffset = meshOffsetTable[fbxNode];
                for (size_t i = subMesh.mIndexOffset; i < subMesh.mIndexNum + subMesh.mIndexOffset; ++i)
                {
                    uint32_t globalIndex = localIndices[i] + meshVertexOffset;
                    tempMeshPerMatTable[globalMaterialIndex].mIndices.push_back(globalIndex);
                }
            }
        }
    }

    // 머터리얼별 index list 병합
    // 머터리얼별 서브메시 구축

    for (auto &e : tempMeshPerMatTable)
    {
        int matIndex = e.first;
        FBXImportTempMeshPerMat &tempMeshPerMat = e.second;

        size_t indexOffset = totalIndices.size();

        FBXImportSubMesh subMesh;
        subMesh.mIndexOffset = indexOffset;
        subMesh.mIndexNum = tempMeshPerMat.mIndices.size();
        subMesh.mVertexOffset = 0;
        subMesh.mMaterialSlotIndex = matIndex;

        importAssetContext.mTotalMesh.mSubMeshes.push_back(subMesh);

        // 인덱스 리스트 합병
        totalIndices.insert(totalIndices.end(), tempMeshPerMat.mIndices.begin(), tempMeshPerMat.mIndices.end());
    }

    importAssetContext.mTotalMesh.mName = totalMeshName;
}

void FBXImporter::BakeVertex(std::vector<FBXImportVertex> &vertices, const fbxsdk::FbxAMatrix &Amatrix,
                             bool bFlipZ) const
{

    fbxsdk::FbxAMatrix normalMatrix = Amatrix.Inverse().Transpose();

    for (auto &vertex : vertices)
    {
        fbxsdk::FbxVector4 p(vertex.mPosition.X, vertex.mPosition.Y, vertex.mPosition.Z, 1.0);
        fbxsdk::FbxVector4 n(vertex.mNormal.X, vertex.mNormal.Y, vertex.mNormal.Z, 0.0);
        fbxsdk::FbxVector4 t(vertex.mTangent.X, vertex.mTangent.Y, vertex.mTangent.Z, 0.0f);

        fbxsdk::FbxVector4 bp = Amatrix.MultT(p);
        fbxsdk::FbxVector4 bn = normalMatrix.MultT(n);
        fbxsdk::FbxVector4 bt = Amatrix.MultT(t);
        bt.Normalize();

        vertex.mPosition = {static_cast<float>(bp[0]), static_cast<float>(bp[1]), static_cast<float>(bp[2])};
        vertex.mNormal = {static_cast<float>(bn[0]), static_cast<float>(bn[1]), static_cast<float>(bn[2])};
        vertex.mTangent = {static_cast<float>(bt[0]), static_cast<float>(bt[1]), static_cast<float>(bt[2]),
                           vertex.mTangent.W};

        if (bFlipZ)
        {
            vertex.mPosition.Z *= -1.0f;
            vertex.mNormal.Z *= -1.0f;
            vertex.mTangent.Z *= -1.0f;
            vertex.mTangent.W *= -1.0f;
        }
        vertex.mNormal.Normalize();
    }
}

void FBXImporter::BuildIntermediateAssets(const FBXImportContext &importContext,
                                          CoreAsset::ImportPackage &oImportPackage) const
{

    std::unordered_map<fbxsdk::FbxMesh *, CoreAsset::IntermediateAsset *> mFbxMeshAssetTable;

    // intermediate mesh asset 생성
    // 하나의 mesh만 나옴(static combine mode라고 보면됨 )
    std::vector<CoreAsset::ImportDependencyContext> importDependencyContextList;

    CoreAsset::ImportedIntermediateAsset importAsset =
        BuildIntermediateMeshAsset(importContext, importDependencyContextList);

    oImportPackage.mInteremdiateAssets.push_back(std::move(importAsset));
    oImportPackage.mDependencyContexts.insert(oImportPackage.mDependencyContexts.begin(),
                                              importDependencyContextList.begin(), importDependencyContextList.end());

    // intermediate material asset 생성
    for (const auto &e : importContext.mMaterialKeyTable)
    {
        std::vector<CoreAsset::ImportDependencyContext> importDependencyContextList;
        fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = e.first;
        CoreAsset::ImportedIntermediateAsset importAsset;

        bool bKeyValid = e.second.mVaild;
        importAsset.mKey = e.second.mKey;

        // 동시에 텍스처 리소스 수집

        bool ret = BuildIntermediateMaterialAssets(fbxSurfaceMaterial, importDependencyContextList, importAsset);

        // 디폴트머터리얼사용시 경우도 해당된다.
        if (ret == false)
            importAsset.mValid = false;

        oImportPackage.mInteremdiateAssets.push_back(std::move(importAsset));
        oImportPackage.mDependencyContexts.insert(oImportPackage.mDependencyContexts.begin(),
                                                  importDependencyContextList.begin(),
                                                  importDependencyContextList.end());
    }

    // fbxTexture들에대해서 requestTextureImport 생성
    for (fbxsdk::FbxTexture *fbxTexture : importContext.mFbxTextureList)
    {
        fbxsdk::FbxClassId classID = fbxTexture->GetClassId();

        CoreAsset::ImportRequestTextureContext requestTextureContext;

        if (classID == fbxsdk::FbxFileTexture::ClassId)
        {
            fbxsdk::FbxFileTexture *fbxFileTexture = static_cast<fbxsdk::FbxFileTexture *>(fbxTexture);
            requestTextureContext.mKey = fbxFileTexture->GetName();
            requestTextureContext.mFilePath = fbxFileTexture->GetFileName();
            oImportPackage.mImportRequestTextureContexts.push_back(requestTextureContext);
        }
        else if (classID == fbxsdk::FbxLayeredTexture::ClassId)
        {
            LOG_MESSAGE_ERROR("FBX Importer", "FbxTexture가 FbxLayerdTexture 타입이라서 중지함");
            CHECK(0);

            //    oImportPackage.mImportRequestTextureContexts.push_back(requestTextureContext);
        }
    }
}

CoreAsset::ImportedIntermediateAsset FBXImporter::BuildIntermediateMeshAsset(
    FBXImportMesh *importMesh, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{

    CoreAsset::ImportedIntermediateAsset importedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateStaticMesh> staticMesh =
        std::make_unique<CoreAsset::IntermediateStaticMesh>();

    CoreAsset::ImportAssetKey meshAssetKey = importMesh->mName;

    // vertex
    for (size_t i = 0; i < importMesh->mVertices.size(); ++i)
    {
        CoreAsset::StaticVertex vertex;
        vertex.mPos = importMesh->mVertices[i].mPosition;
        vertex.mTex = importMesh->mVertices[i].mUV;
        vertex.mNormal = importMesh->mVertices[i].mNormal;
        vertex.mTangent = {0, 0, 1, 0};

        staticMesh->mVertexVector.push_back(vertex);
    }

    // index
    for (size_t i = 0; i < importMesh->mIndices.size(); ++i)
    {
        staticMesh->mIndexVector.push_back(importMesh->mIndices[i]);
    }

    // sub mesh
    for (size_t i = 0; i < importMesh->mSubMeshes.size(); ++i)
    {
        CoreAsset::SubMesh subMesh;

        subMesh.mIndexNum = importMesh->mSubMeshes[i].mIndexNum;
        subMesh.mIndexOffset = importMesh->mSubMeshes[i].mIndexOffset;
        subMesh.mVertexOffset = importMesh->mSubMeshes[i].mVertexOffset;
        subMesh.mMaterialID = NoneAssetID; // 설정할수없으니 NoneAssetID - > 의존성 CONTEXT에서 관련정보를 담음
        staticMesh->mSubMeshVector.push_back(subMesh);
    }

    // mesh part
    for (size_t i = 0; i < importMesh->mMeshParts.size(); ++i)
    {
        const FBXImportMeshPart &importMeshPart = importMesh->mMeshParts[i];
        CoreAsset::MeshPart meshPart;
        meshPart.mStartSubMeshIndex = importMeshPart.mStartSubMeshIndex;
        meshPart.mSubMeshCount = importMeshPart.mStartSubMeshIndex;

        staticMesh->mMeshPartVector.push_back(meshPart);
    }

    // mesh part instance;

    for (size_t i = 0; i < importMesh->mMeshPartInstances.size(); ++i)
    {
        FBXImportMeshPartInstance &importMeshPartInstance = importMesh->mMeshPartInstances[i];

        CoreAsset::MeshPartInstance meshPartInstance;
        meshPartInstance.mLocalTransform = importMeshPartInstance.mLocalTransform;
        meshPartInstance.mMeshPartIndex = importMeshPartInstance.mMeshPartIndex;

        // 의존성 context 구축
        //        meshPartInstance.mSubMeshMaterialIDList = importMeshPartInstance.mSubMeshMaterialKeyList;

        staticMesh->mMeshPartInstanceVector.push_back(meshPartInstance);

        for (int j = 0; j < importMeshPartInstance.mSubMeshMaterialKeyList.size(); ++j)
        {

            CoreAsset::ImportDependencyContext importDependencyContext;
            importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eMeshPartInstanceMaterial;
            importDependencyContext.mOwnerAssetKey = meshAssetKey;
            importDependencyContext.mDependencyAssetKey = importMeshPartInstance.mSubMeshMaterialKeyList[j].mKey;
            importDependencyContext.mSlotIndex = i;
            oDependencyList.push_back(importDependencyContext);
        }
    }

    // mesh 의 기본머터리얼 설정
    FBXImportMeshPartInstance &defaultMeshPartInstance = importMesh->mMeshPartInstances[0];

    for (int i = 0; i < defaultMeshPartInstance.mSubMeshMaterialKeyList.size(); ++i)
    {
        CoreAsset::ImportDependencyContext importDependencyContext;
        importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eSubMeshDefaultMaterial;
        importDependencyContext.mOwnerAssetKey = meshAssetKey;
        importDependencyContext.mDependencyAssetKey = defaultMeshPartInstance.mSubMeshMaterialKeyList[i].mKey;
        importDependencyContext.mSlotIndex = i;
        if (defaultMeshPartInstance.mSubMeshMaterialKeyList[i].mVaild == false)
        {
            importDependencyContext.mSubInfo = CoreAsset::EImportDependencySubInfo::eUseDefaultMaterial;
        }

        oDependencyList.push_back(importDependencyContext);
    }

    staticMesh->mAssetName = importMesh->mName.c_str();
    staticMesh->bCaculateAABB = true;

    importedIntermediateAsset.mIntermediateAsset = std::move(staticMesh);
    importedIntermediateAsset.mKey = meshAssetKey;

    return importedIntermediateAsset;
}

CoreAsset::ImportedIntermediateAsset FBXImporter::BuildIntermediateMeshAsset(
    const FBXImportContext &importContext, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{

    CoreAsset::ImportedIntermediateAsset importedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateStaticMesh> staticMesh =
        std::make_unique<CoreAsset::IntermediateStaticMesh>();

    const FBXImportMesh &importMesh = importContext.mAssetContext.mTotalMesh;

    CoreAsset::ImportAssetKey meshAssetKey = importMesh.mName;

    // vertex
    for (size_t i = 0; i < importMesh.mVertices.size(); ++i)
    {
        CoreAsset::StaticVertex vertex;
        vertex.mPos = importMesh.mVertices[i].mPosition;
        vertex.mTex = importMesh.mVertices[i].mUV;
        vertex.mNormal = importMesh.mVertices[i].mNormal;
        vertex.mTangent = importMesh.mVertices[i].mTangent;

        staticMesh->mVertexVector.push_back(vertex);
    }

    // index
    for (size_t i = 0; i < importMesh.mIndices.size(); ++i)
    {
        staticMesh->mIndexVector.push_back(importMesh.mIndices[i]);
    }

    // sub mesh
    for (size_t i = 0; i < importMesh.mSubMeshes.size(); ++i)
    {
        CoreAsset::SubMesh subMesh;

        subMesh.mIndexNum = importMesh.mSubMeshes[i].mIndexNum;
        subMesh.mIndexOffset = importMesh.mSubMeshes[i].mIndexOffset;
        subMesh.mVertexOffset = importMesh.mSubMeshes[i].mVertexOffset;
        subMesh.mMaterialID = NoneAssetID; // 설정할수없으니 NoneAssetID - > 의존성 CONTEXT에서 관련정보를 담음
        staticMesh->mSubMeshVector.push_back(subMesh);

        // dependency  구축

        fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial =
            importContext.mGlobalFbxMaterialList[importMesh.mSubMeshes[i].mMaterialSlotIndex];
        auto it = importContext.mMaterialKeyTable.find(fbxSurfaceMaterial);

        CoreAsset::ImportDependencyContext importDependencyContext;
        importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eSubMeshDefaultMaterial;
        importDependencyContext.mOwnerAssetKey = meshAssetKey;
        importDependencyContext.mDependencyAssetKey = it->second.mKey;
        if (it == importContext.mMaterialKeyTable.end() || it->second.mVaild == false)
            importDependencyContext.mSubInfo = CoreAsset::EImportDependencySubInfo::eUseDefaultMaterial;

        importDependencyContext.mSlotIndex = i;
        oDependencyList.push_back(importDependencyContext);
    }

    staticMesh->mAssetName = meshAssetKey.c_str();
    staticMesh->bCaculateAABB = true;

    importedIntermediateAsset.mIntermediateAsset = std::move(staticMesh);
    importedIntermediateAsset.mKey = meshAssetKey;
    return importedIntermediateAsset;
}

bool FBXImporter::BuildIntermediateMaterialAssets(
    fbxsdk::FbxSurfaceMaterial *surfaceMaterial, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList,
    CoreAsset::ImportedIntermediateAsset &oImportedIntermediateAsset) const
{

    if (surfaceMaterial == nullptr)
        return false;

    CoreAsset::ImportedIntermediateAsset &importedIntermediateAsset = oImportedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateMaterial> intermediateMaterial =
        std::make_unique<CoreAsset::IntermediateMaterial>();

    intermediateMaterial->mAssetName = surfaceMaterial->GetName();

    TryCopyVector3Property(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuse, intermediateMaterial->mDiffuseColor);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor,
                          intermediateMaterial->mDiffuseFactor);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sShininess, intermediateMaterial->mShininess);
    TryCopyVector3Property(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sSpecular, intermediateMaterial->mSpecular);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sSpecularFactor,
                          intermediateMaterial->mSpecularFactor);

    // collect texture dependency
    CollectTextureDependencyFromProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuse,
                                         CoreAsset::EImportDependencySubInfo::eDiffuseMap, oDependencyList);

    importedIntermediateAsset.mIntermediateAsset = std::move(intermediateMaterial);

    return true;
}

void FBXImporter::BuildTempVertices(
    fbxsdk::FbxMesh *fbxMesh, std::vector<FBXImportVertex> &oTempVertices,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable) const
{
    int controlPointCount = fbxMesh->GetControlPointsCount();
    fbxsdk::FbxVector4 *controlCountArray = fbxMesh->GetControlPoints();

    int polygonCount = fbxMesh->GetPolygonCount();

    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
    {

        // 폴리곤별로 material slot index 설정
        // 머터리얼이없다면 -1
        int materialSlotIndex = GetPolygonMaterialSlot(fbxMesh, polygonIndex);

        for (int i = 0; i < fbxMesh->GetPolygonSize(polygonIndex); ++i)
        {
            // 폴리곤마다 중복정점생성
            FBXImportVertex vertex;
            vertex.mMaterialSlotIndex = materialSlotIndex;

            int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, i);
            Copy(controlCountArray[controlPointIndex], vertex.mPosition);

            // normal
            ExtractVertexNormal(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mNormal);

            // uv
            ExtractVertexUV(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mUV);

            ExtractVertexTangent(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mTangent);

            oTempVertices.push_back(vertex);
        }
    }
}

void FBXImporter::BuildIndexedVertices(const std::vector<FBXImportVertex> &tempVertices,
                                       std::vector<FBXImportVertex> &oFinalVertices,
                                       std::vector<uint32_t> &oFinalIndices,
                                       std::vector<FBXImportSubMesh> &oSubMeshList, bool bIndexFlip) const
{

    // 1 temp vertices 순회하면서 final vertices에 중복되는 vertex가있는지 검사
    // 있다면 temp-final table에 각 vertex의 index를 기록
    // 없다면 해당 vertex를 finalvertices에 추가
    std::vector<FBXImportVertex> finalVertices;
    std::vector<uint32_t> tempIndices;
    std::vector<FBXImportSubMesh> subMeshList;

    //  std::unordered_map<uint32_t, uint32_t> tempFinalVertexIndexTable;

    // material slot - vertex index list table
    std::map<int, std::vector<uint32_t>> matIndexListTable;

    size_t tempVertexCount = tempVertices.size();
    // final vertex, temp-key table 구축
    for (uint32_t tempVertexIndex = 0; tempVertexIndex < tempVertexCount; ++tempVertexIndex)
    {

        const auto &tempVertex = tempVertices[tempVertexIndex];

        auto it = std::find_if(finalVertices.begin(), finalVertices.end(),
                               [&tempVertex](const FBXImportVertex &vertex)
                               {
                                   if (tempVertex.mPosition != vertex.mPosition)
                                       return false;
                                   if (tempVertex.mNormal != vertex.mNormal)
                                       return false;
                                   if (tempVertex.mUV != vertex.mUV)
                                       return false;

                                   return true;
                               });

        uint32_t Index = 0;

        if (it == finalVertices.end())
        {
            // final vertex 에 추가
            // temp-final table에 기록
            //   tempFinalVertexIndexTable[tempVertexIndex] = finalVertices.size();
            Index = finalVertices.size();
            finalVertices.push_back(tempVertex);
        }
        else
        {
            // temp-final table에 기록만
            Index = it - finalVertices.begin();
            //  tempFinalVertexIndexTable[tempVertexIndex] = it - finalVertices.begin();
        }

        tempIndices.push_back(Index);
        matIndexListTable[tempVertex.mMaterialSlotIndex].push_back(tempVertexIndex);
    }

    std::vector<uint32_t> finalIndices;
    // mat별 index 들을 정렬 (subMesh별)

    int indexOrder[3] = {0, 1, 2};
    if (bIndexFlip)
    {
        indexOrder[1] = 2;
        indexOrder[2] = 1;
    }

    for (const auto &e : matIndexListTable)
    {

        int matSlotIndex = e.first;
        const std::vector<uint32_t> &indexList = e.second;

        FBXImportSubMesh importSubMesh;
        importSubMesh.mIndexNum = indexList.size();
        importSubMesh.mIndexOffset = finalIndices.size();
        importSubMesh.mMaterialSlotIndex = matSlotIndex;
        importSubMesh.mVertexOffset = 0;

        for (size_t j = 0; j < indexList.size() / 3; ++j)
        {
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[0]]]);
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[1]]]);
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[2]]]);
        }

        subMeshList.push_back(importSubMesh);
    }

    oFinalVertices = std::move(finalVertices);
    oFinalIndices = std::move(finalIndices);
    oSubMeshList = std::move(subMeshList);
}

void FBXImporter::BuildMeshParts(
    FBXImportMesh *pImportMesh, const std::vector<fbxsdk::FbxNode *> &fbxNodes,
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &oFbxMaterialKeyTable) const
{

    // 현재 MeshPart는 subMesh전체영역이라 한개만 존재
    FBXImportMeshPart importMeshPart;
    importMeshPart.mStartSubMeshIndex = 0;
    importMeshPart.mSubMeshCount = pImportMesh->mSubMeshes.size();

    // FbxImportMeshPart Instance 구축
    for (auto pNode : fbxNodes)
    {

        FBXImportMeshPartInstance importMeshPartInstance;
        importMeshPartInstance.mMeshPartIndex = 0;
        // pImportMesh->mMeshParts.size();

        // int materialCount = pNode->GetMaterialCount();

        // 실제 subMesh가 사용하는 material 의 asset key를 meshPartInstance에 등록
        for (const auto &subMesh : pImportMesh->mSubMeshes)
        {
            int matLocalIndex = subMesh.mMaterialSlotIndex;
            if (matLocalIndex == -1)
            {
                oFbxMaterialKeyTable.try_emplace(nullptr, "", false);
                importMeshPartInstance.mSubMeshMaterialKeyList.push_back({"", false});
            }
            else
            {

                fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = pNode->GetMaterial(matLocalIndex);

                if (fbxSurfaceMaterial)
                {
                    CoreAsset::ImportAssetKey importAssetKey = fbxSurfaceMaterial->GetName();
                    oFbxMaterialKeyTable.try_emplace(fbxSurfaceMaterial, importAssetKey);
                    // 해당 material assetkey를  submesh material key list에 저장

                    // matSlotIndex가 -1이면 머터리얼이없는경우인데 이부분 처리
                    importMeshPartInstance.mSubMeshMaterialKeyList.push_back({importAssetKey, true});
                }
            }
        }
        pImportMesh->mMeshPartInstances.push_back(importMeshPartInstance);
    }

    pImportMesh->mMeshParts.push_back(importMeshPart);
}

void FBXImporter::CollectTextureDependencyFromProperty(
    fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial, const char *propertyName,
    CoreAsset::EImportDependencySubInfo subInfo, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{
    if (fbxSurfaceMaterial == nullptr)
        return;

    fbxsdk::FbxProperty fbxProperty = fbxSurfaceMaterial->FindProperty(propertyName);

    if (fbxProperty.IsValid() == false)
        return;

    CoreAsset::ImportDependencyContext importDependencyContext;

    importDependencyContext.mSubInfo = subInfo;
    importDependencyContext.mOwnerAssetKey = fbxSurfaceMaterial->GetName();
    importDependencyContext.mDependencyAssetKey = fbxProperty.GetName();
    importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eMaterialTexture;
    oDependencyList.push_back(importDependencyContext);
}

void FBXImporter::SetImportPackageOptions(const FBXImportContext &context,
                                          CoreAsset::ImportPackage &oImportPackage) const
{

    oImportPackage.mOption.mNeedToCalculateNormals = context.mNeedToCalculateNormals;
    oImportPackage.mOption.mNeedToCalculateTangents = context.mNeedToCalculateTangents;
}

int FBXImporter::GetPolygonMaterialSlot(fbxsdk::FbxMesh *fbxMesh, int polygonIndex) const
{
    fbxsdk::FbxLayer *layer = fbxMesh->GetLayer(0);
    fbxsdk::FbxLayerElementMaterial *layerElementMaterial = layer->GetMaterials();

    if (layerElementMaterial)
    {
        fbxsdk::FbxLayerElement::EMappingMode mappingMode = layerElementMaterial->GetMappingMode();
        fbxsdk::FbxLayerElement::EReferenceMode referenceMode = layerElementMaterial->GetReferenceMode();

        if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygon)
        {
            if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
            {

                return layerElementMaterial->GetIndexArray().GetAt(polygonIndex);
            }
            else if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eDirect)
            {
                // 사용한되는 Mode
                return 0;
            }
        }
        else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eAllSame)
        {
            if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
            {
                return layerElementMaterial->GetIndexArray().GetAt(0);
            }
            else if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eDirect)
            {
                return 0;
            }
        }
    }

    return -1;
}

void FBXImporter::GetGeometrix(fbxsdk::FbxNode *node, FbxAMatrix &oMatrix) const
{

    if (node == nullptr)
    {
        return;
        //   return CoreMath::Matrix4X4::Identity;
    }
    fbxsdk::FbxDouble3 geoTranslation = node->GeometricTranslation;
    fbxsdk::FbxDouble3 geoRotation = node->GeometricRotation;
    fbxsdk::FbxDouble3 geoScaling = node->GeometricScaling;

    fbxsdk::FbxAMatrix matrix(geoTranslation, geoRotation, geoScaling);

    oMatrix = matrix;

    //  return matrix;

    // CoreMath::Matrix4X4 ret;
    // Copy(matrix, ret);
    //  return ret;
}

} // namespace Import
#include "AssetManager.h"
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetIOManager.h>
#include <CoreAsset/AssetImporterManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/GeometryGenerator.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/Mesh.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreAsset/Texture.h>
// #include <CoreAsset/g_DefaultFontTexture.h>
#include <CoreBase/CoreAssert.h>
#include <CoreBase/FVector.h>
#include <CoreDevice/ImageLoader.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <IAssetImporter.h>
#include <Utility/Utility.h>

CoreAsset::AssetManager *CoreAsset::AssetManager::GetInstance()
{
    static AssetManager instance;
    return &instance;
}

CoreAsset::AssetManager::~AssetManager() {}

void CoreAsset::AssetManager::Initialize(AssetFactoryManager *assetFactoryManager, AssetIOManager *assetIOManager,
                                         AssetImporterManager *assetImporterManager,
                                         const std::filesystem::path &editorAssetPath)
{
    mAssetFactoryManager = assetFactoryManager;
    mAssetIOManager = assetIOManager;
    mAssetImporterManager = assetImporterManager;
    mEditorAssetPath = editorAssetPath;

    mGlobalAssetRegistrySystem = GlobalAssetRegistrySystem::GetInstance();
    mAssetMetaDataManager = AssetMetaDataManager::GetInstance();
}

void CoreAsset::AssetManager::SetAssetImporterManager(AssetImporterManager *assetImporterManager)
{
    mAssetImporterManager = assetImporterManager;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAsset(CoreAsset::EAssetType assetType,
                                                         const IntermediateAsset *intermediateAssetData,
                                                         const char *prefixAssetName, bool bEngine)
{

    // factory create
    Asset *asset = nullptr;

    asset = mAssetFactoryManager->CreateAssetFromData(*intermediateAssetData);

    if (asset == nullptr)
        return nullptr;

    // asset들의 unique한 name을 최종 설정 & 전역테이블에 등록 & 메타데이터 등록

    std::string displayName = asset->GetName().c_str();

    std::string uniqueName = std::string(prefixAssetName) + "/" + displayName;

    // uniqueName의 중복을 검사해야한다. , 전역테이블시스템으로부터
    size_t nameCount = 0;
    std::string displayNameTemp = displayName;
    while (mGlobalAssetRegistrySystem->GetAsset(uniqueName) != nullptr)
    {
        displayNameTemp = displayName + std::to_string(nameCount++);
        uniqueName = std::string(prefixAssetName) + "/" + displayNameTemp;
    }
    asset->SetName(displayNameTemp.c_str());
    mGlobalAssetRegistrySystem->RegisterAsset(asset, uniqueName, bEngine);

    if (bEngine == false)
    {
        asset->SetRawDataDirty(true);
    }

    mAssetMetaDataManager->Register(asset);

    return asset;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAsset(CoreAsset::EAssetType assetType,
                                                         const IntermediateAsset &intermediateAssetData,
                                                         const char *prefixAssetName, bool bEngine)
{
    // 레퍼런스 버전
    return CreateAsset(assetType, &intermediateAssetData, prefixAssetName, bEngine);
}

CoreAsset::AssetLoadResult CoreAsset::AssetManager::LoadAsset(const std::filesystem::path &assetPath,
                                                              const std::string &logicalFolderPath,
                                                              const AssetLoadExecutionContext &executionContext)
{

    // CHECK(filePath != nullptr, "파일경로가 nullptr입니다.");

    AssetLoadResult result;

    const std::string extension = assetPath.extension().string();
    //    CoreUtility::Utility::GetExtensionFromPath(filePath);

    bool ret = true;

    // 해당 asset이 이미 존재하는지확인 // asset파일을 로드하는거라면 filePath의 경로는 논리적경로와 동일한 물리적경로
    // 문자열
    std::string removedExtensionPath = assetPath.stem().string();
    // CoreUtility::Utility::RemoveExtension(filePath);
    AssetPtr assetPtr = GetAssetCommon(removedExtensionPath.c_str());
    if (assetPtr.Get() != nullptr)
    {
        result.pAsset = assetPtr.Get();
        result.mAssetType = assetPtr.Get()->GetAssetType();
        result.mResultFlag = EAssetLoadResultFlag::eAlready;

        return result;
    }

    Asset *asset = nullptr;
    std::unique_ptr<AssetMetaData> assetMetaDataPtr = nullptr;

    if (extension == ".asset" || extension == ".map")
    {
        result = mAssetIOManager->LoadAssetFromMetaData(assetPath, mAssetFactoryManager, asset, assetMetaDataPtr,
                                                        executionContext);
    }
    else
    {
        CHECK(false, "확장자가 올바르지않다  : %s", extension);

        result.pAsset = nullptr;
        result.mResultFlag = EAssetLoadResultFlag::eFail;

        return result;
    }

    if (result.mResultFlag == EAssetLoadResultFlag::eSuccess)
    {
        // 메타데이터 등록
        mAssetMetaDataManager->Register(*assetMetaDataPtr.get());
        // 전역테이블에 등록하는 메서드 호출
        std::string assetUniqueName = logicalFolderPath + "/" + asset->GetName().c_str();
        RegisterAsset(asset, assetUniqueName);
    }

    return result;
}

bool CoreAsset::AssetManager::LoadAssetRawData(CoreAsset::Asset *asset)
{

    if (asset == nullptr)
        return false;

    AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset);

    if (assetMetaData == nullptr)
        return false;

    std::filesystem::path path = mRawDataPath / assetMetaData->mRawFileName;

    bool ret = mAssetIOManager->LoadAssetRawData(asset, path);

    if (ret)
    {
        asset->SetLoadState(Asset::LoadState::Loaded);
    }
    else
    {
        asset->SetLoadState(Asset::LoadState::Failed);
    }

    return ret;
}

void CoreAsset::AssetManager::SetAssetRawDataPath(const std::filesystem::path &path)
{
    mRawDataPath = path;
}

std::vector<CoreAsset::Asset *> CoreAsset::AssetManager::ImportAsset(const std::filesystem::path &filePath,
                                                                     const char *prefixAssetName, bool bEngine)
{
    // 임포팅이라면 절대경로
    if (mAssetImporterManager == nullptr)
        return {};

    // import
    ImportExecutionContext importContext;
    importContext.bEngineAsset = bEngine;
    ImportPackage importPackage = mAssetImporterManager->Import(filePath, importContext);
    if (importPackage.mInteremdiateAssets.empty())
    {
        return {};
    }

    ProcessImportOptions(importPackage);

    if (importPackage.mImportRequestTextureContexts.empty() == false)
    {
        for (const ImportRequestTextureContext &e : importPackage.mImportRequestTextureContexts)
        {

            ImportPackage textureImportPackage = mAssetImporterManager->Import(e.mFilePath.c_str(), importContext);

            for (auto &e : textureImportPackage.mInteremdiateAssets)
            {
                importPackage.mInteremdiateAssets.push_back(std::move(e));
            }
        }
    }

    // factory create
    std::vector<Asset *> assetPtrVector;

    std::unordered_map<CoreAsset::ImportAssetKey, Asset *> importKeyAssetTable;
    for (size_t assetIndex = 0; assetIndex < importPackage.mInteremdiateAssets.size(); ++assetIndex)
    {
        ImportedIntermediateAsset &importedIntermediateAsset = importPackage.mInteremdiateAssets[assetIndex];

        if (importedIntermediateAsset.mValid == false)
            continue;

        const auto &intermediateAssetPtr = importedIntermediateAsset.mIntermediateAsset;

        Asset *asset =
            CreateAsset(intermediateAssetPtr->mAssetType, intermediateAssetPtr.get(), prefixAssetName, bEngine).Get();

        assetPtrVector.push_back(asset);

        importKeyAssetTable[importedIntermediateAsset.mKey] = asset;
    }

    // dependency
    for (const auto &dependencyContext : importPackage.mDependencyContexts)
    {

        int slotIndex = dependencyContext.mSlotIndex;
        switch (dependencyContext.mDependencyType)
        {
        case EImportDependencyType::eSubMeshDefaultMaterial:
        {
            Mesh *mesh = static_cast<CoreAsset::Mesh *>(importKeyAssetTable[dependencyContext.mOwnerAssetKey]);
            Asset *material = nullptr;
            if (dependencyContext.mSubInfo == EImportDependencySubInfo::eUseDefaultMaterial)
            {
                material = GetDefaultMaterial().Get();
            }
            else
            {

                material = importKeyAssetTable[dependencyContext.mDependencyAssetKey];
            }
            mesh->SetSubMeshMaterial(material->GetID(), slotIndex);
        }
        break;
        case EImportDependencyType::eMeshPartInstanceMaterial:
        {
            Mesh *mesh = static_cast<CoreAsset::Mesh *>(importKeyAssetTable[dependencyContext.mOwnerAssetKey]);
            Asset *material = importKeyAssetTable[dependencyContext.mDependencyAssetKey];

            int meshPartInstanceIndex = dependencyContext.mSlotIndex;
        }
        break;
        case EImportDependencyType::eMaterialTexture:
        {
        }
        break;
        }
    }

    return assetPtrVector;
}

bool CoreAsset::AssetManager::StoreAsset(Asset *asset, const std::filesystem::path &filePath,
                                         AssetMetaData *assetMetaData)
{

    // 파일명어디, 논리적 name .

    if (asset == nullptr)
        return false;

    bool ret = mAssetIOManager->StoreAsset(asset, filePath, assetMetaData);
    return ret;
}

bool CoreAsset::AssetManager::StoreAssetRawData(Asset *asset, const std::filesystem::path &filePath)
{

    if (asset == nullptr)
        return false;

    AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset->GetID());

    bool ret = mAssetIOManager->StoreAssetRawData(asset, filePath, assetMetaData);

    return ret;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAssetInner()
{
    return nullptr;
}

// CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAsset(SerializedAsset *serializedAsset,
//                                                          const AssetMetaData &assetMetaData)
//{
//     AssetPtr assetPtr;
//     // global registry check
//
//     Asset *pAsset = mGlobalAssetRegistrySystem->GetAsset(serializedAsset->mAssetName);
//     if (pAsset != nullptr)
//     {
//         // 이미존재
//         return nullptr;
//     }
//
//     // assetPtr = mAssetFactoryManager->CreateAssetFromData(*(serializedAsset), assetMetaData);
//     ////    mAssetFactory->CreateAsset(serializedAsset);
//     // if (assetPtr.Get() == nullptr)
//     //{
//     //     // 생성 실패
//     //     return nullptr;
//     // }
//
//     // mGlobalAssetRegistrySystem->RegisterAsset(assetPtr.Get(), serializedAsset->mAssetName);
//
//     return assetPtr;
// }

void CoreAsset::AssetManager::RegisterAsset(Asset *asset, const std::string &assetUniqueName)
{

    if (asset == nullptr)
        return;

    mGlobalAssetRegistrySystem->RegisterAsset(asset, assetUniqueName.c_str());
}

void CoreAsset::AssetManager::InitAssetSetting(Asset *asset)
{

    if (asset == nullptr)
        return;
}

void CoreAsset::AssetManager::SetAssetRawFileName(AssetMetaData *assetMetaData)
{

    CHECK(assetMetaData != nullptr);

    assetMetaData->mRawFileName = (assetMetaData->mAssetName + std::to_string(assetMetaData->mAssetID)).c_str();
}

void CoreAsset::AssetManager::ProcessImportOptions(ImportPackage &importPackage)
{

    if (importPackage.mOption.mNeedToCalculateNormals)
    {
        // tangent 을 직접 엔진에서 계산한다.
        for (auto &impportIntermediateAsset : importPackage.mInteremdiateAssets)
        {
            if (impportIntermediateAsset.mIntermediateAsset->mAssetType == EAssetType::eStaticMesh)
            {

                IntermediateStaticMesh *intermediateStaticMesh =
                    static_cast<IntermediateStaticMesh *>(impportIntermediateAsset.mIntermediateAsset.get());

                GeometryGenerator::CaculateVertexNoraml(intermediateStaticMesh->mVertexVector,
                                                        intermediateStaticMesh->mIndexVector);
            }
        }
    }

    if (importPackage.mOption.mNeedToCalculateTangents)
    {
        // tangent 을 직접 엔진에서 계산한다.
        for (auto &impportIntermediateAsset : importPackage.mInteremdiateAssets)
        {
            if (impportIntermediateAsset.mIntermediateAsset->mAssetType == EAssetType::eStaticMesh)
            {

                IntermediateStaticMesh *intermediateStaticMesh =
                    static_cast<IntermediateStaticMesh *>(impportIntermediateAsset.mIntermediateAsset.get());

                GeometryGenerator::CaculateTangents(intermediateStaticMesh->mVertexVector,
                                                    intermediateStaticMesh->mIndexVector);
            }
        }
    }
}

std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreateBuiltInAsset()
{

    std::vector<AssetPtr> vec;

    const auto &fontAssetVec = CreateBuiltInFont();
    vec.insert(vec.end(), fontAssetVec.begin(), fontAssetVec.end());

    auto textVec = CreateBuiltInTexture();
    vec.insert(vec.begin(), textVec.begin(), textVec.end());

    {
        auto staticMeshMatVec = CreateBuiltInStaticMeshMaterial();
        vec.insert(vec.end(), staticMeshMatVec.begin(), staticMeshMatVec.end());
    }

    {
        auto matList = CreatStaticMeshEditorUnlitMaterial();
        vec.insert(vec.end(), matList.begin(), matList.end());
    }

    auto meshVec = CreateBuiltInMesh();
    vec.insert(vec.end(), meshVec.begin(), meshVec.end());

    vec.push_back(CreateBuiltInDefaultNormalMap());

    return vec;
}

std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreateBuiltInMesh()
{
    std::vector<CoreAsset::AssetPtr> assetVec;

    MeshData cubeData = GeometryGenerator::CreateCube();

    CoreAsset::IntermediateStaticMesh IntermediateStaticMesh;
    IntermediateStaticMesh.mVertexVector = std::move(cubeData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(cubeData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    SubMesh subMesh;
    subMesh.mVertexOffset = 0;
    subMesh.mName = "Cube";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Cube";

    AssetPtr pCube = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    {
        PhysicsCollisionPreset cubePhysicsCollisionPreset;

        StaticMesh *pStaticMeshCube = pCube.As<CoreAsset::StaticMesh>();

        PhysicsCollisionShapeData data;
        data.mShapeType = EPhysicsCollisionShapeType::eBox;
        data.mBoxData.mLocalHalfExtents = {0.5f, 0.5f, 0.5f};
        cubePhysicsCollisionPreset.mShapeList.push_back(data);

        pStaticMeshCube->SetPhysicsCollisionPreset(cubePhysicsCollisionPreset);
    }

    assetVec.push_back(pCube);

    mDefaultCubeMesh = pCube;

    // Cylinder

    MeshData cylinderData = GeometryGenerator::CreateCylinder(1, 1, 1, 36, 6);

    IntermediateStaticMesh.mVertexVector = std::move(cylinderData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(cylinderData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "Cylinder";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Cylinder";

    AssetPtr pCylinder = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pCylinder);

    // Cone

    MeshData coneData = GeometryGenerator::CreateCylinder(1, 0, 1, 12, 6);

    IntermediateStaticMesh.mVertexVector = std::move(coneData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(coneData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "Cone";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Cone";

    AssetPtr pCone = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pCone);

    // Arrow

    MeshData arrowData = GeometryGenerator::CreateArrow(1);

    IntermediateStaticMesh.mVertexVector = std::move(arrowData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(arrowData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "Arrow";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Arrow";

    AssetPtr pArrow = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pArrow);

    // Cube Arrow
    MeshData arrowCubeData = GeometryGenerator::CreateArrowCube(1);

    IntermediateStaticMesh.mVertexVector = std::move(arrowCubeData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(arrowCubeData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "ArrowCube";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "ArrowCube";

    AssetPtr pArrowCube = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pArrowCube);

    // Quad
    MeshData qaudData = GeometryGenerator::CreateQuad(1, 6);

    IntermediateStaticMesh.mVertexVector = std::move(qaudData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(qaudData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "Quad";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Quad";

    AssetPtr pQuad = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pQuad);

    // Sphere

    MeshData SphereData = GeometryGenerator::CreateSphere(1.0f, 36, 24);
    IntermediateStaticMesh.mVertexVector = std::move(SphereData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(SphereData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "Sphere";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID();
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "Sphere";

    AssetPtr pSphere = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pSphere);

    {
        MeshData EditorCamMeshData = GeometryGenerator::CreateEditorCamera();
        IntermediateStaticMesh.mVertexVector = std::move(EditorCamMeshData.mVertices);
        IntermediateStaticMesh.mIndexVector = std::move(EditorCamMeshData.mIndices);
        IntermediateStaticMesh.bCaculateAABB = true;

        std::vector<SubMesh> subMeshList(EditorCamMeshData.mSubMeshDataList.size());

        for (int i = 0; i < subMeshList.size(); ++i)
        {
            subMeshList[i].mVertexOffset = EditorCamMeshData.mSubMeshDataList[i].mVertexOffset;
            subMeshList[i].mIndexOffset = EditorCamMeshData.mSubMeshDataList[i].mIndexOffset;
            subMeshList[i].mIndexNum = EditorCamMeshData.mSubMeshDataList[i].mIndexNum;
            subMeshList[i].mMaterialID = mEditorVisualizerMaterial[i].GetAssetID();
        }

        /*   subMesh.mVertexOffset = 0;
           subMesh.mName = "EditorCamera";
           subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
           subMesh.mIndexOffset = 0;
           subMesh.mMaterialID = mEditorVisualizerMaterial.GetAssetID();*/
        IntermediateStaticMesh.mSubMeshVector = std::move(subMeshList);
        IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
        IntermediateStaticMesh.mAssetName = "EditorCameraMesh";

        AssetPtr pEditorCameraMesh =
            CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

        assetVec.push_back(pEditorCameraMesh);
    }

    // Sky Sphere

    MeshData SkySphereData = GeometryGenerator::CreateSkySphere(1.0f, 36, 24);
    IntermediateStaticMesh.mVertexVector = std::move(SkySphereData.mVertices);
    IntermediateStaticMesh.mIndexVector = std::move(SkySphereData.mIndices);
    IntermediateStaticMesh.bCaculateAABB = true;

    subMesh.mVertexOffset = 0;
    subMesh.mName = "SkySphere";
    subMesh.mIndexNum = IntermediateStaticMesh.mIndexVector.size();
    subMesh.mIndexOffset = 0;
    subMesh.mMaterialID = mDefaultMaterial.GetAssetID(); // 머터리얼이 의미가있을수도있고 없을수도있고
    IntermediateStaticMesh.mSubMeshVector.clear();
    IntermediateStaticMesh.mSubMeshVector.push_back(subMesh);
    IntermediateStaticMesh.mAssetType = EAssetType::eStaticMesh;
    IntermediateStaticMesh.mAssetName = "SkySphere";

    AssetPtr pSkySphere = CreateAsset(CoreAsset::EAssetType::eStaticMesh, IntermediateStaticMesh, "Engine", true);

    assetVec.push_back(pSkySphere);

    return assetVec;
}
std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreateBuiltInStaticMeshMaterial()
{
    std::vector<CoreAsset::AssetPtr> vec;

    {
        CoreAsset::IntermediateMaterial intermediate;
        intermediate.mAssetType = EAssetType::eMaterial;
        intermediate.mAssetName = "DefaultMaterial";

        intermediate.mTexResourceList.push_back(
            {mDefaultGrayTexture.Get()->GetName().c_str(), mDefaultGrayTexture.GetAssetID()});
        intermediate.mGpuMaterialID = 5;
        intermediate.mSamplerResourceList.push_back(1);
        intermediate.mDiffuseFactor = 1.0f;
        intermediate.mDiffuseColor = {1.0f, 1.0f, 1.0f};

        mDefaultMaterial = CreateAsset(CoreAsset::EAssetType::eMaterial, intermediate, "Engine", true);

        vec.push_back(mDefaultMaterial);
    }

    return vec;
}

std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreateBuiltInTexture()
{

    // DefaultGray
    CoreAsset::IntermediateTexture intermediate;

    intermediate.mAssetName = "DefaultGray";
    intermediate.mTextureRawData;

    GRM::TextureDesc &textureDesc = intermediate.mTextureRawData;

    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;
    GRM::ScratchImage scratchImage;

    scratchImage.mimagesNum = 1;
    GRM::Image image;
    image.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;
    image.mWidth = 1;
    image.mHeight = 1;
    image.mRowPitch = 4;
    image.mSlicePitch = 4;
    std::vector<uint8_t> pixels = {200, 200, 200, 255};
    scratchImage.mMemory = std::move(pixels);

    image.mPixels = scratchImage.mMemory.data();

    scratchImage.mImages.push_back(image);
    scratchImage.mSize = 4;
    scratchImage.mMetadata.mWidth = 1;
    scratchImage.mMetadata.mHeight = 1;
    scratchImage.mMetadata.mDepth = 1;
    scratchImage.mMetadata.mArraySize = 1;
    scratchImage.mMetadata.mMipLevels = 1;
    scratchImage.mMetadata.mDimension = GRM::ETextureType::eTexture2D;
    scratchImage.mMetadata.mMiscFlags = 0;
    scratchImage.mMetadata.mMiscFlags2 = 0;
    scratchImage.mMetadata.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;

    textureDesc.mScratchImage = std::move(scratchImage);

    mDefaultGrayTexture = CreateAsset(EAssetType::eTexture, intermediate, "Engine", true);

    // RegisterDefaultAsset(EAssetType::eTexture, mDefaultGrayTexture.GetAssetID());

    // DefaultWhite

    intermediate.mAssetName = "DefaultWhite";
    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;

    for (int i = 0; i < 4; ++i)
        pixels.push_back(255);

    scratchImage.mMemory = std::move(pixels);
    image.mPixels = scratchImage.mMemory.data();
    scratchImage.mImages.push_back(image);

    textureDesc.mScratchImage = std::move(scratchImage);

    mDefaultWhiteTexture = CreateAsset(EAssetType::eTexture, intermediate, "Engine", true);

    RegisterDefaultAsset(EAssetType::eTexture, mDefaultWhiteTexture.GetAssetID());

    return {mDefaultGrayTexture, mDefaultWhiteTexture};
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateBuiltInDefaultNormalMap()
{
    // DefaultNormalMap
    CoreAsset::IntermediateTexture intermediate;
    GRM::TextureDesc &textureDesc = intermediate.mTextureRawData;

    intermediate.mAssetName = "DefaultNormalMap";
    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;
    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;
    GRM::ScratchImage scratchImage;
    std::vector<uint8_t> pixels = {127, 127, 255, 255}; // 0,0,1.0f,1.0f
    scratchImage.mMemory = std::move(pixels);

    scratchImage.mimagesNum = 1;
    GRM::Image image;
    image.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;
    image.mWidth = 1;
    image.mHeight = 1;
    image.mRowPitch = 4;
    image.mSlicePitch = 4;
    image.mPixels = scratchImage.mMemory.data();
    scratchImage.mImages.push_back(image);
    scratchImage.mSize = 4;
    scratchImage.mMetadata.mWidth = 1;
    scratchImage.mMetadata.mHeight = 1;
    scratchImage.mMetadata.mDepth = 1;
    scratchImage.mMetadata.mArraySize = 1;
    scratchImage.mMetadata.mMipLevels = 1;
    scratchImage.mMetadata.mDimension = GRM::ETextureType::eTexture2D;
    scratchImage.mMetadata.mMiscFlags = 0;
    scratchImage.mMetadata.mMiscFlags2 = 0;
    scratchImage.mMetadata.mFormat = GRM::ETextureFormat::eR8G8B8A8_UNORM;
    textureDesc.mScratchImage = std::move(scratchImage);

    mDefaultNormalMap = CreateAsset(EAssetType::eTexture, intermediate, "Engine", true);

    //   RegisterDefaultAsset(EAssetType::eTexture, mDefaultNormalMap.GetAssetID());

    return mDefaultNormalMap;
}

std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreatStaticMeshEditorUnlitMaterial()
{

    // EditorVisualizerMaterial -Unlit;

    {
        CoreAsset::IntermediateMaterial intermediate;
        intermediate.mAssetType = EAssetType::eMaterial;
        intermediate.mAssetName = "EditorVisualizerMaterial";

        intermediate.mTexResourceList.push_back(
            {mDefaultWhiteTexture.Get()->GetName().c_str(), mDefaultWhiteTexture.GetAssetID()});
        intermediate.mGpuMaterialID = 0;
        intermediate.mSamplerResourceList.push_back(1);
        intermediate.mDiffuseFactor = 1.0f;
        intermediate.mDiffuseColor = {1.0f, 1.0f, 1.0f};
        intermediate.mShadingModel = EShadingModel::eUnlit;

        mEditorVisualizerMaterial.push_back(
            CreateAsset(CoreAsset::EAssetType::eMaterial, intermediate, "Engine", true));
    }

    {
        CoreAsset::IntermediateMaterial intermediate;
        intermediate.mAssetType = EAssetType::eMaterial;
        intermediate.mAssetName = "EditorVisualizerMaterial2";

        intermediate.mTexResourceList.push_back(
            {mDefaultWhiteTexture.Get()->GetName().c_str(), mDefaultWhiteTexture.GetAssetID()});
        intermediate.mGpuMaterialID = 0;
        intermediate.mSamplerResourceList.push_back(1);
        intermediate.mDiffuseFactor = 1.0f;
        intermediate.mDiffuseColor = {0.4f, 0.4f, 0.4f};
        intermediate.mShadingModel = EShadingModel::eUnlit;

        mEditorVisualizerMaterial.push_back(
            CreateAsset(CoreAsset::EAssetType::eMaterial, intermediate, "Engine", true));
    }

    return mEditorVisualizerMaterial;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetDefaultStaticMeshMaterial() const
{

    return mDefaultMaterial;
}

CoreAsset::EAssetType CoreAsset::AssetManager::GetAssetTypeFromClassName(const char *className) const
{

    std::unordered_map<std::string, EAssetType>::const_iterator it = mAssetClassNameTable.find(className);

    if (it == mAssetClassNameTable.cend())
        return EAssetType::eUnknown;

    return it->second;
}

void CoreAsset::AssetManager::SetAssetTypeClassName()
{

    mAssetClassNameTable["Material"] = EAssetType::eMaterial;
    mAssetClassNameTable["Texture"] = EAssetType::eTexture;
    mAssetClassNameTable["StaticMesh"] = EAssetType::eStaticMesh;
    mAssetClassNameTable["Font"] = EAssetType::eFont;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetCommon(const char *assetName) const
{
    return mGlobalAssetRegistrySystem->GetAsset(assetName);
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetCommon(AssetID assetID) const
{
    return mGlobalAssetRegistrySystem->GetAsset(assetID);
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetFromAssetName(const char *assetClassName, AssetID id) const
{

    EAssetType type = GetAssetTypeFromClassName(assetClassName);

    if (type == EAssetType::eUnknown)
        return nullptr;

    AssetPtr pAsset = GetAssetCommon(id);

    if (pAsset.Get() == nullptr)
        return pAsset;

    if (pAsset.Get()->GetType() != type)
        return nullptr;

    return pAsset;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetFromAssetName(const char *assetClassName,
                                                                   const char *assetName) const
{
    AssetPtr pAsset = GetAssetCommon(assetName);
    if (pAsset.Get() == nullptr)
        return pAsset;

    return GetAssetFromAssetName(assetClassName, pAsset.Get()->GetID());
}

CoreAsset::AssetPtr CoreAsset::AssetManager::ResolveAsset(CoreAsset::EAssetType type, CoreAsset::AssetID id) const
{
    auto pAsset = GetAssetCommon(id);

    if (pAsset.Get() != nullptr)
    {
        return pAsset;
    }

    return GetDefaultAsset(type);
}

void CoreAsset::AssetManager::RegisterDefaultAsset(EAssetType assetType, AssetID id)
{

    Asset *asset = mGlobalAssetRegistrySystem->GetAsset(id);
    if (asset->GetType() != assetType)
        return;

    mDefaultAssetIDTable[assetType] = id;
}

CoreAsset::AssetID CoreAsset::AssetManager::GetDefaultAssetID(EAssetType assetType) const
{

    std::unordered_map<EAssetType, AssetID>::const_iterator it = mDefaultAssetIDTable.find(assetType);
    if (it == mDefaultAssetIDTable.cend())
        return NoneAssetID;

    return it->second;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetDefaultAsset(EAssetType assetType) const
{

    return GetDefaultAssetID(assetType);
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetDefaultNormalMap() const
{

    return mDefaultNormalMap;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetDefafultDiffuseWhiteMap() const
{

    return mDefaultWhiteTexture;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetDefaultMaterial() const
{

    return mDefaultMaterial;
}

CoreAsset::AssetID CoreAsset::AssetManager::GetDefaultEngineCubeMeshID() const
{

    return mDefaultCubeMesh.GetAssetID();
}

CoreAsset::AssetManager::AssetManager()
{

    mAssetMetaDataManager = AssetMetaDataManager::GetInstance();
    SetAssetTypeClassName();
}

std::vector<CoreAsset::AssetPtr> CoreAsset::AssetManager::CreateBuiltInFont()
{

    IntermediateTexture intermediateTex;

    intermediateTex.mAssetName = "DefaultFontGlyph";
    intermediateTex.mTextureRawData;

    GRM::TextureDesc &textureDesc = intermediateTex.mTextureRawData;

    textureDesc.mTextureUsage = GRM::ETextureUsage::eShaderResource;

    std::filesystem::path atlasPath = mEditorAssetPath / "Fonts" / "font_atlas.png";

    //  GRM::ScratchImage scratchImage = Core::ImageLoader::LoadFromMemory(g_DefaultFontTexture, 2690973);
    GRM::ScratchImage scratchImage = Core::ImageLoader::LoadFromFile(atlasPath);

    textureDesc.mScratchImage = std::move(scratchImage);

    AssetPtr pTex = CreateAsset(EAssetType::eTexture, intermediateTex, "Engine", true);

    //

    CoreAsset::IntermediateFont intermediate;

    intermediate.mAssetName = "DefaultFont";
    intermediate.mGlyphAltasID = pTex.GetAssetID();

    intermediate.mFontMatrix.mAscender = 1.08837890625;
    intermediate.mFontMatrix.mDescender = -0.24169921875;
    intermediate.mFontMatrix.mLineHeight = 1.330078125;
    intermediate.mFontMatrix.mUnderlineY = -0.22412109375;

    intermediate.mFontAltas.mDistanceRange = 3;
    intermediate.mFontAltas.mHeight = 2048;
    intermediate.mFontAltas.mWidth = 2048;
    intermediate.mFontAltas.mSize = 42.3125f;
    intermediate.mFontAltas.mPxRange = intermediate.mFontAltas.mDistanceRange / intermediate.mFontAltas.mSize;

    // push glyphs
    {
        intermediate.mGlyphVector.push_back({32, 0.3515625, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}});
        intermediate.mGlyphVector.push_back(
            {33,
             0.2890625,
             {0.049752397410450525, -0.05908419497784343, 0.2388218213395495, 0.7680945347119646},
             {2039.5, 2012.5, 2047.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {34,
             0.39501953125,
             {0.03207401968703838, 0.43722304283604135, 0.3629455115629616, 0.7680945347119645},
             {2033.5, 1714.5, 2047.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {35,
             0.6064453125,
             {-0.019005824755354464, 0.011816838995568686, 0.6191034810053546, 0.7680945347119646},
             {2020.5, 812.5, 2047.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {36,
             0.55078125,
             {0.036026068593057604, -0.1536189069423929, 0.5323333064069424, 0.862629246676514},
             {1372.5, 1550.5, 1393.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {37,
             0.83642578125,
             {-0.006216750715472674, -0.05908419497784343, 0.8445956569654727, 0.7680945347119646},
             {638.5, 72.5, 674.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {38,
             0.81787109375,
             {0.015365713280096012, -0.05908419497784343, 0.842544442969904, 0.7680945347119646},
             {675.5, 72.5, 710.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {39,
             0.23193359375,
             {0.03300478328101921, 0.43722304283604135, 0.1984405292189808, 0.7680945347119645},
             {2040.5, 1759.5, 2047.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {40,
             0.3046875,
             {0.028754139932607066, -0.20088626292466766, 0.3359919538173929, 0.7680945347119646},
             {2034.5, 1912.5, 2047.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {41,
             0.3046875,
             {-0.029107188192392934, -0.20088626292466766, 0.2781306256923929, 0.7680945347119646},
             {2033.5, 1597.5, 2046.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {42,
             0.4248046875,
             {0.00018752307976366116, 0.3190546528803545, 0.4255937269202364, 0.7680945347119645},
             {2024.5, 1954.5, 2042.5, 1973.5}});
        intermediate.mGlyphVector.push_back(
            {43,
             0.70068359375,
             {0.07928692185192027, 0.011816838995568686, 0.6228615156480798, 0.5790251107828657},
             {2024.5, 1974.5, 2047.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44,
             0.21875,
             {-0.01943748846011817, -0.1772525849335303, 0.1932656134601182, 0.15361890694239294},
             {2035.5, 862.5, 2044.5, 876.5}});
        intermediate.mGlyphVector.push_back(
            {45,
             0.41015625,
             {0.0317317900664697, 0.20088626292466766, 0.3862369599335303, 0.3663220088626293},
             {2027.5, 1865.5, 2042.5, 1872.5}});
        intermediate.mGlyphVector.push_back(
            {46,
             0.21875,
             {0.014840288035450525, -0.05908419497784343, 0.2039097119645495, 0.15361890694239294},
             {2037.5, 1821.5, 2045.5, 1830.5}});
        intermediate.mGlyphVector.push_back(
            {47,
             0.39599609375,
             {-0.052841118906942396, -0.1772525849335303, 0.44346611890694243, 0.7680945347119646},
             {1557.5, 109.5, 1578.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48,
             0.55078125,
             {0.004091609351920277, -0.05908419497784343, 0.5476662031480798, 0.7680945347119646},
             {752.5, 72.5, 775.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49,
             0.55078125,
             {0.039688177968057604, -0.03545051698670606, 0.5359954157819424, 0.791728212703102},
             {776.5, 72.5, 797.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {50,
             0.55078125,
             {0.006142823347488963, -0.03545051698670606, 0.5260837391525112, 0.7680945347119646},
             {2025.5, 546.5, 2047.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {51,
             0.55078125,
             {0.023330756093057604, -0.05908419497784343, 0.5196379939069424, 0.7680945347119646},
             {839.5, 72.5, 860.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {52,
             0.55078125,
             {-0.0349229280142171, -0.03545051698670606, 0.5559190217642173, 0.7680945347119646},
             {1385.5, 73.5, 1410.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {53,
             0.55078125,
             {0.04540150133862629, -0.05908419497784343, 0.5180750611613737, 0.7680945347119646},
             {2027.5, 1873.5, 2047.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54,
             0.55078125,
             {0.010683406226920277, -0.05908419497784343, 0.5542580000230798, 0.7680945347119646},
             {902.5, 72.5, 925.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {55,
             0.55078125,
             {0.0011619218519202772, -0.03545051698670606, 0.5447365156480798, 0.7680945347119646},
             {1411.5, 73.5, 1434.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {56,
             0.55078125,
             {0.003847468726920277, -0.05908419497784343, 0.5474220625230798, 0.7680945347119646},
             {926.5, 72.5, 949.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {57,
             0.55078125,
             {0.002138484351920277, -0.05908419497784343, 0.5457130781480798, 0.7680945347119646},
             {950.5, 72.5, 973.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {58,
             0.21875,
             {0.014840288035450525, -0.05908419497784343, 0.2039097119645495, 0.5790251107828657},
             {2033.5, 1686.5, 2041.5, 1713.5}});
        intermediate.mGlyphVector.push_back(
            {59,
             0.21875,
             {-0.026127374330686855, -0.1772525849335303, 0.21020940558068685, 0.5790251107828657},
             {2037.5, 1831.5, 2047.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {60,
             0.70068359375,
             {0.09110376084748895, 0.011816838995568686, 0.611044676652511, 0.602658788774003},
             {566.5, 6.5, 588.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {61,
             0.70068359375,
             {0.07928692185192027, 0.10635155096011817, 0.6228615156480798, 0.4844903988183161},
             {2018.5, 1023.5, 2041.5, 1039.5}});
        intermediate.mGlyphVector.push_back(
            {62,
             0.70068359375,
             {0.09110376084748895, 0.011816838995568686, 0.611044676652511, 0.602658788774003},
             {696.5, 6.5, 718.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {63,
             0.4599609375,
             {0.025089866829763663, -0.05908419497784343, 0.4504960706702364, 0.7680945347119646},
             {2029.5, 633.5, 2047.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {64,
             0.9794921875,
             {0.04241519629338995, -0.12998522895125555, 0.94049495995661, 0.7680945347119645},
             {205.5, 69.5, 243.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {65,
             0.658203125,
             {-0.025647747992060523, -0.03545051698670606, 0.6833625917420606, 0.7680945347119646},
             {1476.5, 73.5, 1506.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {66,
             0.58349609375,
             {0.05326196397248897, -0.03545051698670606, 0.5732028797775112, 0.7680945347119646},
             {1507.5, 73.5, 1529.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {67,
             0.634765625,
             {0.007703592365214222, -0.05908419497784343, 0.6221792201347859, 0.7680945347119646},
             {2021.5, 1381.5, 2047.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {68,
             0.71728515625,
             {0.05120858624907685, -0.03545051698670606, 0.7129515700009232, 0.7680945347119646},
             {2019.5, 1559.5, 2047.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {69,
             0.51708984375,
             {0.05941560595919498, -0.03545051698670606, 0.5084554877908051, 0.7680945347119646},
             {2028.5, 986.5, 2047.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {70,
             0.49853515625,
             {0.05135896533419498, -0.03545051698670606, 0.5003988471658051, 0.7680945347119646},
             {2028.5, 590.5, 2047.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {71,
             0.70166015625,
             {0.00726327374907685, -0.05908419497784343, 0.6690062575009232, 0.7680945347119646},
             {2019.5, 1513.5, 2047.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {72,
             0.724609375,
             {0.05506687361521422, -0.03545051698670606, 0.6695425013847859, 0.7680945347119646},
             {2021.5, 1206.5, 2047.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {73,
             0.27001953125,
             {-0.019097422567392934, -0.03545051698670606, 0.2881403913173929, 0.7680945347119646},
             {1735.5, 73.5, 1748.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {74,
             0.35986328125,
             {-0.036871725558530306, -0.05908419497784343, 0.3176334443085303, 0.7680945347119646},
             {2032.5, 1293.5, 2047.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {75,
             0.59033203125,
             {0.058436879731351594, -0.03545051698670606, 0.6256451515186485, 0.7680945347119646},
             {2023.5, 287.5, 2047.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {76,
             0.47998046875,
             {0.05526521533419498, -0.03545051698670606, 0.5043050971658051, 0.7680945347119646},
             {2028.5, 373.5, 2047.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {77,
             0.9169921875,
             {0.0567235679006647, -0.03545051698670606, 0.8602686195993354, 0.7680945347119646},
             {1831.5, 73.5, 1865.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {78,
             0.76513671875,
             {0.05169686749907685, -0.03545051698670606, 0.7134398512509232, 0.7680945347119646},
             {1866.5, 73.5, 1894.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {79,
             0.7734375,
             {0.008579902141802109, -0.05908419497784343, 0.7648575978581981, 0.7680945347119646},
             {1097.5, 72.5, 1129.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {80,
             0.57080078125,
             {0.05204126084748897, -0.03545051698670606, 0.5719821766525112, 0.7680945347119646},
             {1895.5, 73.5, 1917.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {81,
             0.7734375,
             {0.004087281896233381, -0.24815361890694237, 0.7839986556037667, 0.7680945347119645},
             {1985.5, 1550.5, 2018.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {82,
             0.60986328125,
             {0.0541884001107829, -0.03545051698670606, 0.6450303498892173, 0.7680945347119646},
             {1918.5, 73.5, 1943.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {83,
             0.54296875,
             {0.017373292097488963, -0.05908419497784343, 0.5373142079025112, 0.7680945347119646},
             {1130.5, 72.5, 1152.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {84,
             0.53369140625,
             {-0.01578187026864841, -0.03545051698670606, 0.5514264015186485, 0.7680945347119646},
             {1944.5, 73.5, 1968.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {85,
             0.70263671875,
             {0.04432468611521422, -0.05908419497784343, 0.6588003138847859, 0.7680945347119646},
             {1153.5, 72.5, 1179.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {86,
             0.63427734375,
             {-0.03736649799206052, -0.03545051698670606, 0.6716438417420606, 0.7680945347119646},
             {1969.5, 73.5, 1999.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {87,
             0.95361328125,
             {-0.030829154934453477, -0.03545051698670606, 0.9854189986844535, 0.7680945347119646},
             {2004.5, 678.5, 2047.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {88,
             0.60107421875,
             {-0.03155508562592315, -0.03545051698670606, 0.6301878981259232, 0.7680945347119646},
             {41.5, 32.5, 69.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {89,
             0.5634765625,
             {-0.035851527880354464, -0.03545051698670606, 0.6022577778803546, 0.7680945347119646},
             {2020.5, 766.5, 2047.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {90,
             0.58251953125,
             {-0.030236293505354464, -0.03545051698670606, 0.6078730122553546, 0.7680945347119646},
             {70.5, 32.5, 97.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {91,
             0.3046875,
             {0.06020031792374443, -0.20088626292466766, 0.3201707758262555, 0.7680945347119646},
             {2031.5, 151.5, 2042.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {92,
             0.763671875,
             {-0.03175342734490399, -0.03545051698670606, 0.795425302344904, 0.7680945347119646},
             {221.5, 32.5, 256.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {93,
             0.3046875,
             {-0.013774291451255541, -0.20088626292466766, 0.24619616645125555, 0.7680945347119646},
             {246.5, 108.5, 257.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {94,
             0.70068359375,
             {0.07928692185192027, 0.27178729689807973, 0.6228615156480798, 0.7680945347119645},
             {1393.5, 10.5, 1416.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {95,
             0.42578125,
             {-0.047079832902511075, -0.20088626292466766, 0.47286108290251105, -0.03545051698670604},
             {2019.5, 1551.5, 2041.5, 1558.5}});
        intermediate.mGlyphVector.push_back(
            {96,
             0.27197265625,
             {-0.004839177321824248, 0.5553914327917283, 0.2787649585718242, 0.8153618906942394},
             {2035.5, 877.5, 2047.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {97,
             0.52001953125,
             {-0.0013274470319423956, -0.05908419497784343, 0.49497979078194243, 0.5790251107828657},
             {116.5, 4.5, 137.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {98,
             0.6005859375,
             {0.046327937476920274, -0.05908419497784343, 0.5899025312730798, 0.8153618906942394},
             {326.5, 70.5, 349.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {99,
             0.47265625,
             {0.0043858763386262906, -0.05908419497784343, 0.47705943616137375, 0.5790251107828657},
             {231.5, 4.5, 251.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {100,
             0.60205078125,
             {0.011659968726920277, -0.05908419497784343, 0.5552345625230798, 0.8153618906942394},
             {350.5, 70.5, 373.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {101,
             0.53515625,
             {0.011269776472488962, -0.05908419497784343, 0.5312106922775112, 0.5790251107828657},
             {138.5, 4.5, 160.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {102,
             0.31640625,
             {-0.009381923929098989, -0.03545051698670606, 0.368756923929099, 0.8153618906942394},
             {2030.5, 113.5, 2046.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {103,
             0.60205078125,
             {0.011659968726920277, -0.29542097488921715, 0.5552345625230798, 0.5790251107828657},
             {374.5, 70.5, 397.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {104,
             0.57861328125,
             {0.046768256093057604, -0.03545051698670606, 0.5430754939069424, 0.8153618906942394},
             {521.5, 71.5, 542.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {105,
             0.24609375,
             {0.030221147410450525, -0.03545051698670606, 0.2192905713395495, 0.791728212703102},
             {2038.5, 415.5, 2046.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {106,
             0.24609375,
             {-0.1401432099335303, -0.29542097488921715, 0.2143619599335303, 0.791728212703102},
             {7.5, 2001.5, 22.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {107,
             0.505859375,
             {0.046035834218057604, -0.03545051698670606, 0.5423430720319424, 0.8153618906942394},
             {543.5, 71.5, 564.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {108,
             0.24609375,
             {0.04032900203101921, -0.03545051698670606, 0.2057647479689808, 0.8153618906942394},
             {2040.5, 328.5, 2047.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {109,
             0.8798828125,
             {0.04378411477566469, -0.03545051698670606, 0.8473291664743354, 0.5790251107828657},
             {441.5, 5.5, 475.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {110,
             0.578125,
             {0.046768256093057604, -0.03545051698670606, 0.5430754939069424, 0.5790251107828657},
             {333.5, 5.5, 354.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {111,
             0.59912109375,
             {0.004627853235782905, -0.05908419497784343, 0.5954698030142173, 0.5790251107828657},
             {271.5, 4.5, 296.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {112,
             0.6005859375,
             {0.046327937476920274, -0.2717872968980798, 0.5899025312730798, 0.5790251107828657},
             {565.5, 71.5, 588.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {113,
             0.60205078125,
             {0.011659968726920277, -0.2717872968980798, 0.5552345625230798, 0.5790251107828657},
             {589.5, 71.5, 612.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {114,
             0.353515625,
             {0.0385677275664697, -0.03545051698670606, 0.3930728974335303, 0.5790251107828657},
             {476.5, 5.5, 491.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {115,
             0.43310546875,
             {0.007511741829763663, -0.05908419497784343, 0.4329179456702364, 0.5790251107828657},
             {252.5, 4.5, 270.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {116,
             0.3447265625,
             {-0.019635830179098986, -0.05908419497784343, 0.358503017679099, 0.7208271787296899},
             {1159.5, 33.5, 1175.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {117,
             0.578125,
             {0.035781927968057604, -0.05908419497784343, 0.5320891657819424, 0.5553914327917283},
             {355.5, 5.5, 376.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {118,
             0.4873046875,
             {-0.038975229643648406, -0.03545051698670606, 0.5282330421436485, 0.5553914327917283},
             {634.5, 6.5, 658.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {119,
             0.736328125,
             {-0.03336432272433531, -0.03545051698670606, 0.7701807289743354, 0.5553914327917283},
             {753.5, 6.5, 787.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {120,
             0.46484375,
             {-0.027792723527511075, -0.03545051698670606, 0.49214819227751105, 0.5553914327917283},
             {589.5, 6.5, 611.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {121,
             0.49267578125,
             {-0.036289682768648406, -0.29542097488921715, 0.5309185890186485, 0.5553914327917282},
             {613.5, 71.5, 637.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {122,
             0.4619140625,
             {-0.020126275156942396, -0.03545051698670606, 0.47618096265694243, 0.5553914327917283},
             {612.5, 6.5, 633.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {123,
             0.3046875,
             {0.006049061807607066, -0.20088626292466766, 0.3132868756923929, 0.7680945347119646},
             {381.5, 108.5, 394.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {124,
             0.2392578125,
             {0.0494602941515879, -0.29542097488921715, 0.19126236209841213, 0.8389955686853767},
             {0.5, 1999.5, 6.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {125,
             0.3046875,
             {-0.007134531942392934, -0.20088626292466766, 0.3001032819423929, 0.7680945347119646},
             {518.5, 108.5, 531.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {126,
             0.70068359375,
             {0.06747008285635159, 0.17725258493353027, 0.6346783546436485, 0.413589364844904},
             {2020.5, 801.5, 2044.5, 811.5}});
        intermediate.mGlyphVector.push_back(
            {12593,
             1,
             {0.12742842388293946, 0.011816838995568686, 0.8364387636170606, 0.5790251107828657},
             {1053.5, 7.5, 1083.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12594,
             1,
             {0.09588415689623336, 0.011816838995568686, 0.8757955306037667, 0.5790251107828657},
             {2014.5, 254.5, 2047.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {12595,
             1,
             {0.10071684303452733, -0.011816838995568686, 0.9515292507154727, 0.602658788774003},
             {492.5, 5.5, 528.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12596,
             1,
             {0.16488002850350814, 0.011816838995568686, 0.8502566902464919, 0.5790251107828657},
             {2018.5, 1040.5, 2047.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {12597,
             1,
             {0.100570791405096, 0.011816838995568686, 0.927749521094904, 0.5790251107828657},
             {924.5, 7.5, 959.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12598,
             1,
             {0.08474961341395862, -0.011816838995568686, 0.9591956990860414, 0.6262924667651404},
             {161.5, 4.5, 198.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12599,
             1,
             {0.15316127850350814, 0.011816838995568686, 0.8385379402464919, 0.5790251107828657},
             {1023.5, 7.5, 1052.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12600,
             1,
             {0.11019036477566468, 0.011816838995568686, 0.9137354164743354, 0.5790251107828657},
             {1148.5, 7.5, 1182.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12601,
             1,
             {0.16585659100350814, 0.011816838995568686, 0.8512332527464919, 0.5790251107828657},
             {860.5, 7.5, 889.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12602,
             1,
             {0.09612829752123336, 0.011816838995568686, 0.8760396712287667, 0.5790251107828657},
             {890.5, 7.5, 923.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12603,
             1,
             {0.10320837564623336, 0.011816838995568686, 0.8831197493537667, 0.5790251107828657},
             {1183.5, 7.5, 1216.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12604,
             1,
             {0.10516150064623336, 0.011816838995568686, 0.8850728743537667, 0.5790251107828657},
             {1114.5, 7.5, 1147.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12605,
             1,
             {0.10413481178452733, 0.011816838995568686, 0.9549472194654727, 0.602658788774003},
             {529.5, 6.5, 565.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12608,
             1,
             {0.103012197655096, -0.011816838995568686, 0.930190927344904, 0.6262924667651404},
             {297.5, 4.5, 332.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12609,
             1,
             {0.15023159100350814, 0.011816838995568686, 0.8356082527464919, 0.5790251107828657},
             {1084.5, 7.5, 1113.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12610,
             1,
             {0.16571053937407684, 0.011816838995568686, 0.8274535231259232, 0.5790251107828657},
             {960.5, 7.5, 988.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12611,
             1,
             {0.11102087564623336, 0.011816838995568686, 0.8909322493537667, 0.5790251107828657},
             {989.5, 7.5, 1022.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12612,
             1,
             {0.10657621803452733, 0.011816838995568686, 0.9573886257154727, 0.602658788774003},
             {659.5, 6.5, 695.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12613,
             1,
             {0.10174353189623336, 0.011816838995568686, 0.8816549056037667, 0.602658788774003},
             {719.5, 6.5, 752.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12614,
             1,
             {0.06751371803452733, -0.011816838995568686, 0.9183261257154727, 0.602658788774003},
             {404.5, 5.5, 440.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12615,
             1,
             {0.1942270298652142, -0.011816838995568686, 0.8087026576347859, 0.602658788774003},
             {377.5, 5.5, 403.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12616,
             1,
             {0.10174353189623336, -0.011816838995568686, 0.8816549056037667, 0.5790251107828657},
             {826.5, 6.5, 859.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12617,
             1,
             {0.05569687903895864, -0.011816838995568686, 0.9301429647110414, 0.5790251107828657},
             {788.5, 6.5, 825.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12618,
             1,
             {0.11150915689623336, -0.011816838995568686, 0.8914205306037667, 0.6971935007385525},
             {82.5, 1.5, 115.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12619,
             1,
             {0.12742842388293946, 0.011816838995568686, 0.8364387636170606, 0.5790251107828657},
             {1217.5, 7.5, 1247.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12620,
             1,
             {0.15316127850350814, 0.011816838995568686, 0.8385379402464919, 0.5790251107828657},
             {1248.5, 7.5, 1277.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12621,
             1,
             {0.1189314646418021, 0.011816838995568686, 0.8752091603581981, 0.5790251107828657},
             {1278.5, 7.5, 1310.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12622,
             1,
             {0.13318970988737078, -0.011816838995568686, 0.8658337276126293, 0.6262924667651404},
             {199.5, 4.5, 230.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12623,
             1,
             {0.4252385149326071, -0.22451994091580502, 0.732476328817393, 0.8153618906942394},
             {1497.5, 2003.5, 1510.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12625,
             1,
             {0.4247502336826071, -0.22451994091580502, 0.731988047567393, 0.8153618906942394},
             {1511.5, 2003.5, 1524.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12626,
             1,
             {0.31498287770033234, -0.22451994091580502, 0.7167554035496677, 0.8153618906942394},
             {1525.5, 2003.5, 1542.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12627,
             1,
             {0.276506747945901, -0.22451994091580502, 0.654645595804099, 0.8153618906942394},
             {1618.5, 2003.5, 1634.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12629,
             1,
             {0.279924716695901, -0.22451994091580502, 0.658063564554099, 0.8153618906942394},
             {1635.5, 2003.5, 1651.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12630,
             1,
             {0.20208965622692027, -0.22451994091580502, 0.7456642500230798, 0.8153618906942394},
             {1652.5, 2003.5, 1675.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {12631,
             1,
             {0.02732644017725258, 0.05908419497784343, 0.9726735598227475, 0.5790251107828656},
             {1311.5, 9.5, 1351.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12635,
             1,
             {0.02732644017725258, 0.05908419497784343, 0.9726735598227475, 0.5553914327917282},
             {1352.5, 10.5, 1392.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12636,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.5081240768094535},
             {41.5, 0.5, 81.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12640,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.5081240768094535},
             {0.5, 0.5, 40.5, 31.5}});
        intermediate.mGlyphVector.push_back(
            {12641,
             1,
             {0.02732644017725258, 0.12998522895125553, 0.9726735598227475, 0.29542097488921715},
             {2004.5, 670.5, 2044.5, 677.5}});
        intermediate.mGlyphVector.push_back(
            {12643,
             1,
             {0.41679384578101925, -0.22451994091580502, 0.5822295917189808, 0.8153618906942394},
             {1790.5, 2003.5, 1797.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {44032,
             1,
             {0.0399236636816839, -0.22451994091580502, 1.0089044613183162, 0.8153618906942394},
             {1798.5, 2003.5, 1839.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {44033,
             1,
             {0.03465065892725258, -0.22451994091580502, 0.9799977785727475, 0.8153618906942394},
             {1840.5, 2003.5, 1880.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {44036,
             1,
             {0.03538308080225258, -0.1772525849335303, 0.9807302004477475, 0.8153618906942394},
             {1306.5, 365.5, 1346.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {44039,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {768.5, 1505.5, 808.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {44040,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1201.5, 1505.5, 1241.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {44041,
             1,
             {0.03416237767725258, -0.22451994091580502, 0.9795094973227475, 0.8153618906942394},
             {1919.5, 2003.5, 1959.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {44042,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1363.5, 1505.5, 1403.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {44048,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {1684.5, 1505.5, 1724.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {44049,
             1,
             {0.0274724918066839, -0.20088626292466766, 0.9964532894433162, 0.8153618906942393},
             {573.5, 1461.5, 614.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44050,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {692.5, 1461.5, 732.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44051,
             1,
             {0.0274724918066839, -0.20088626292466766, 0.9964532894433162, 0.8153618906942393},
             {733.5, 1461.5, 774.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44052,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {775.5, 1461.5, 815.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44053,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {816.5, 1461.5, 856.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44054,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {857.5, 1461.5, 897.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44055,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {898.5, 1461.5, 938.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44057,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {980.5, 1461.5, 1020.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44058,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {1021.5, 1461.5, 1061.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44059,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1062.5, 1461.5, 1102.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44060,
             1,
             {0.02713242591395864, -0.22451994091580502, 0.9015785115860414, 0.8153618906942394},
             {1960.5, 2003.5, 1997.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {44061,
             1,
             {0.048470749284527326, -0.22451994091580502, 0.8992831569654727, 0.8153618906942394},
             {79.5, 1954.5, 115.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44064,
             1,
             {0.03802066504338995, -0.1772525849335303, 0.93610042870661, 0.8153618906942394},
             {1807.5, 322.5, 1845.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {44068,
             1,
             {0.04983750403895864, -0.20088626292466766, 0.9242835897110414, 0.8153618906942393},
             {1452.5, 1461.5, 1489.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44076,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {1565.5, 1461.5, 1601.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44077,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {1602.5, 1461.5, 1638.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44079,
             1,
             {0.04256124792282127, -0.20088626292466766, 0.9642746895771788, 0.8153618906942393},
             {1639.5, 1461.5, 1678.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44080,
             1,
             {0.04964132604782127, -0.20088626292466766, 0.9713547677021788, 0.8153618906942393},
             {1679.5, 1461.5, 1718.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44081,
             1,
             {0.04471055091395864, -0.20088626292466766, 0.9191566365860414, 0.8153618906942393},
             {1719.5, 1461.5, 1756.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44088,
             1,
             {0.04441628392725259, -0.22451994091580502, 0.9897634035727475, 0.8153618906942394},
             {157.5, 1954.5, 197.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44089,
             1,
             {0.03465065892725258, -0.22451994091580502, 0.9799977785727475, 0.8153618906942394},
             {198.5, 1954.5, 238.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44092,
             1,
             {0.03465065892725258, -0.1772525849335303, 0.9799977785727475, 0.8153618906942394},
             {1859.5, 279.5, 1899.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44096,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {1962.5, 1461.5, 2002.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44107,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {2003.5, 1461.5, 2043.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {44109,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {41.5, 1417.5, 81.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44116,
             1,
             {0.02664414466395864, -0.22451994091580502, 0.9010902303360414, 0.8153618906942394},
             {277.5, 1954.5, 314.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44120,
             1,
             {0.03802066504338995, -0.1772525849335303, 0.93610042870661, 0.8153618906942394},
             {350.5, 236.5, 388.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {44124,
             1,
             {0.04788437903895864, -0.1772525849335303, 0.9223304647110414, 0.8389955686853766},
             {193.5, 1417.5, 230.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44144,
             1,
             {0.031136764909527326, -0.22451994091580502, 0.8819491725904727, 0.8153618906942394},
             {315.5, 1954.5, 351.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44145,
             1,
             {0.039681686784527326, -0.22451994091580502, 0.8904940944654727, 0.8153618906942394},
             {429.5, 1954.5, 465.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44148,
             1,
             {0.04178086341395864, -0.1772525849335303, 0.9162269490860414, 0.8153618906942394},
             {1814.5, 236.5, 1851.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {44151,
             1,
             {0.03836289466395864, -0.20088626292466766, 0.9128089803360414, 0.8153618906942393},
             {746.5, 1417.5, 783.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44152,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {784.5, 1417.5, 821.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44154,
             1,
             {0.040658249284527326, -0.20088626292466766, 0.8914706569654727, 0.8153618906942393},
             {822.5, 1417.5, 858.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44160,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {859.5, 1417.5, 895.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44161,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {896.5, 1417.5, 932.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44163,
             1,
             {0.04256124792282127, -0.20088626292466766, 0.9642746895771788, 0.8153618906942393},
             {933.5, 1417.5, 972.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44164,
             1,
             {0.03831276830225258, -0.20088626292466766, 0.9836598879477475, 0.8153618906942393},
             {973.5, 1417.5, 1013.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44165,
             1,
             {0.043587936784527326, -0.20088626292466766, 0.8944003444654727, 0.8153618906942393},
             {1014.5, 1417.5, 1050.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44166,
             1,
             {0.04705386816838995, -0.20088626292466766, 0.94513363183161, 0.8153618906942393},
             {1051.5, 1417.5, 1089.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44169,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {1131.5, 1417.5, 1168.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44170,
             1,
             {0.03982773841395864, -0.20088626292466766, 0.9142738240860414, 0.8153618906942393},
             {1169.5, 1417.5, 1206.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44171,
             1,
             {0.03836289466395864, -0.20088626292466766, 0.9128089803360414, 0.8153618906942393},
             {1398.5, 1417.5, 1435.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44172,
             1,
             {0.02762070716395864, -0.22451994091580502, 0.9020667928360414, 0.8153618906942394},
             {466.5, 1954.5, 503.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44176,
             1,
             {0.03899722754338995, -0.1772525849335303, 0.93707699120661, 0.8153618906942394},
             {1768.5, 193.5, 1806.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44180,
             1,
             {0.05081406653895864, -0.20088626292466766, 0.9252601522110414, 0.8153618906942393},
             {1705.5, 1417.5, 1742.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44188,
             1,
             {0.049935593034527326, -0.20088626292466766, 0.9007480007154727, 0.8153618906942393},
             {1743.5, 1417.5, 1779.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {44189,
             1,
             {0.049935593034527326, -0.20088626292466766, 0.9007480007154727, 0.8153618906942393},
             {159.5, 1373.5, 195.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {44191,
             1,
             {0.04158468542282127, -0.20088626292466766, 0.9632981270771788, 0.8153618906942393},
             {405.5, 1373.5, 444.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {44192,
             1,
             {0.04539284642725259, -0.20088626292466766, 0.9907399660727475, 0.8153618906942393},
             {763.5, 1373.5, 803.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {44193,
             1,
             {0.04471055091395864, -0.20088626292466766, 0.9191566365860414, 0.8153618906942393},
             {1310.5, 1373.5, 1347.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {44200,
             1,
             {0.031136764909527326, -0.22451994091580502, 0.8819491725904727, 0.8153618906942394},
             {504.5, 1954.5, 540.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44201,
             1,
             {0.039681686784527326, -0.22451994091580502, 0.8904940944654727, 0.8153618906942394},
             {695.5, 1954.5, 731.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44202,
             1,
             {0.039681686784527326, -0.22451994091580502, 0.8904940944654727, 0.8153618906942394},
             {809.5, 1954.5, 845.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44204,
             1,
             {0.03933945716395864, -0.1772525849335303, 0.9137855428360414, 0.8153618906942394},
             {1429.5, 150.5, 1466.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44207,
             1,
             {0.03836289466395864, -0.20088626292466766, 0.9128089803360414, 0.8153618906942393},
             {357.5, 1329.5, 394.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44208,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {559.5, 1329.5, 596.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44216,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {638.5, 1329.5, 674.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44217,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {916.5, 1329.5, 952.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44219,
             1,
             {0.061458165043389947, -0.20088626292466766, 0.95953792870661, 0.8153618906942393},
             {953.5, 1329.5, 991.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44220,
             1,
             {0.04173073705225258, -0.20088626292466766, 0.9870778566977475, 0.8153618906942393},
             {1614.5, 1329.5, 1654.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {44221,
             1,
             {0.044076218034527326, -0.20088626292466766, 0.8948886257154727, 0.8153618906942393},
             {159.5, 1285.5, 195.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44225,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {483.5, 1285.5, 520.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44228,
             1,
             {0.02762070716395864, -0.22451994091580502, 0.9020667928360414, 0.8153618906942394},
             {930.5, 1954.5, 967.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44232,
             1,
             {0.03899722754338995, -0.1772525849335303, 0.93707699120661, 0.8153618906942394},
             {1545.5, 150.5, 1583.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44236,
             1,
             {0.05081406653895864, -0.20088626292466766, 0.9252601522110414, 0.8153618906942393},
             {824.5, 1285.5, 861.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44245,
             1,
             {0.049935593034527326, -0.20088626292466766, 0.9007480007154727, 0.8153618906942393},
             {862.5, 1285.5, 898.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44247,
             1,
             {0.04158468542282127, -0.20088626292466766, 0.9632981270771788, 0.8153618906942393},
             {1471.5, 1285.5, 1510.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44256,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {667.5, 33.5, 707.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {44257,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1917.5, 1285.5, 1957.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {44260,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1579.5, 109.5, 1619.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44263,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1274.5, 150.5, 1314.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44264,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1233.5, 150.5, 1273.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44266,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1192.5, 150.5, 1232.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44268,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1151.5, 150.5, 1191.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44271,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1110.5, 150.5, 1150.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44272,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1069.5, 150.5, 1109.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44273,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1028.5, 150.5, 1068.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44275,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {123.5, 108.5, 163.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44277,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {945.5, 150.5, 985.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44278,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {904.5, 150.5, 944.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44284,
             1,
             {-0.006170951809453474, -0.22451994091580502, 1.0100772018094535, 0.8153618906942394},
             {1053.5, 1954.5, 1096.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44285,
             1,
             {0.0032044809361152123, -0.22451994091580502, 0.9958189565638849, 0.8153618906942394},
             {1097.5, 1954.5, 1139.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44288,
             1,
             {0.0201482730566839, -0.1772525849335303, 0.9891290706933162, 0.8153618906942394},
             {783.5, 150.5, 824.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44292,
             1,
             {0.0201482730566839, -0.20088626292466766, 0.9891290706933162, 0.8153618906942393},
             {1037.5, 1241.5, 1078.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {44294,
             1,
             {0.0201482730566839, -0.20088626292466766, 0.9891290706933162, 0.8153618906942393},
             {1606.5, 1241.5, 1647.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {44300,
             1,
             {0.0201482730566839, -0.1772525849335303, 0.9891290706933162, 0.8153618906942394},
             {668.5, 150.5, 709.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44301,
             1,
             {0.0201482730566839, -0.20088626292466766, 0.9891290706933162, 0.8153618906942393},
             {1648.5, 1241.5, 1689.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {44303,
             1,
             {0.0201482730566839, -0.20088626292466766, 0.9891290706933162, 0.8153618906942393},
             {1731.5, 1241.5, 1772.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {44305,
             1,
             {0.0206365543066839, -0.20088626292466766, 0.9896173519433162, 0.8153618906942393},
             {1848.5, 1241.5, 1889.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {44312,
             1,
             {0.011897618168389953, -0.22451994091580502, 0.90997738183161, 0.8153618906942394},
             {1140.5, 1954.5, 1178.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44316,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {474.5, 150.5, 512.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44320,
             1,
             {0.021419102543389953, -0.20088626292466766, 0.91949886620661, 0.8153618906942393},
             {1138.5, 1197.5, 1176.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44329,
             1,
             {0.01687851966395864, -0.20088626292466766, 0.8913246053360414, 0.8153618906942393},
             {1177.5, 1197.5, 1214.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44332,
             1,
             {0.0177068668066839, -0.22451994091580502, 0.9866876644433162, 0.8153618906942394},
             {1179.5, 1954.5, 1220.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44333,
             1,
             {0.008967930668389953, -0.20088626292466766, 0.90704769433161, 0.8153618906942393},
             {1599.5, 1197.5, 1637.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44340,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {1330.5, 1954.5, 1367.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44341,
             1,
             {0.032601608659527326, -0.22451994091580502, 0.8834140163404727, 0.8153618906942394},
             {1368.5, 1954.5, 1404.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44344,
             1,
             {0.03323594153895864, -0.1772525849335303, 0.9076820272110414, 0.8153618906942394},
             {201.5, 150.5, 238.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44348,
             1,
             {0.050423874284527326, -0.20088626292466766, 0.9012362819654727, 0.8153618906942393},
             {1790.5, 1197.5, 1826.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44356,
             1,
             {0.05076610390509601, -0.1772525849335303, 0.877944833594904, 0.8153618906942394},
             {124.5, 150.5, 159.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {44357,
             1,
             {0.04978954140509601, -0.20088626292466766, 0.876968271094904, 0.8153618906942393},
             {1827.5, 1197.5, 1862.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44359,
             1,
             {0.04851871191838995, -0.20088626292466766, 0.94659847558161, 0.8153618906942393},
             {1863.5, 1197.5, 1901.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44361,
             1,
             {0.035043014909527326, -0.20088626292466766, 0.8858554225904727, 0.8153618906942393},
             {1902.5, 1197.5, 1938.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44368,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {1118.5, 33.5, 1158.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {44372,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1743.5, 109.5, 1783.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44376,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1930.5, 193.5, 1970.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44385,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1889.5, 193.5, 1929.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44387,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {258.5, 108.5, 298.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44396,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {1807.5, 193.5, 1847.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44397,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1939.5, 1197.5, 1979.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {44400,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1907.5, 109.5, 1947.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44403,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1688.5, 193.5, 1728.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44404,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1647.5, 193.5, 1687.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44405,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 1153.5, 40.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44406,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1567.5, 193.5, 1607.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44411,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1526.5, 193.5, 1566.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44412,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1485.5, 193.5, 1525.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44413,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1444.5, 193.5, 1484.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44415,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1403.5, 193.5, 1443.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44417,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1362.5, 193.5, 1402.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44418,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1321.5, 193.5, 1361.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44424,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {1405.5, 1954.5, 1442.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44425,
             1,
             {0.043587936784527326, -0.22451994091580502, 0.8944003444654727, 0.8153618906942394},
             {1443.5, 1954.5, 1479.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44428,
             1,
             {0.04324570716395864, -0.20088626292466766, 0.9176917928360414, 0.8153618906942393},
             {272.5, 1153.5, 309.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44432,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {310.5, 1153.5, 347.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44444,
             1,
             {0.04197487767725258, -0.22451994091580502, 0.9873219973227475, 0.8153618906942394},
             {1480.5, 1954.5, 1520.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44445,
             1,
             {0.03640976966395864, -0.20088626292466766, 0.9108558553360414, 0.8153618906942393},
             {387.5, 1153.5, 424.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44452,
             1,
             {0.008967930668389953, -0.22451994091580502, 0.90704769433161, 0.8153618906942394},
             {1521.5, 1954.5, 1559.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44471,
             1,
             {0.006672576047821267, -0.20088626292466766, 0.9283860177021788, 0.8153618906942393},
             {580.5, 1153.5, 619.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44480,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1599.5, 1954.5, 1636.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44481,
             1,
             {0.041146530534527326, -0.22451994091580502, 0.8919589382154727, 0.8153618906942394},
             {1678.5, 1954.5, 1714.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44484,
             1,
             {0.04080430091395864, -0.1772525849335303, 0.9152503865860414, 0.8153618906942394},
             {889.5, 193.5, 926.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44488,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {886.5, 1153.5, 923.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44496,
             1,
             {0.041146530534527326, -0.1772525849335303, 0.8919589382154727, 0.8153618906942394},
             {811.5, 193.5, 847.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44497,
             1,
             {0.041146530534527326, -0.1772525849335303, 0.8919589382154727, 0.8153618906942394},
             {774.5, 193.5, 810.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44499,
             1,
             {0.04011984167282127, -0.20088626292466766, 0.9618332833271788, 0.8153618906942393},
             {962.5, 1153.5, 1001.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44508,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {692.5, 193.5, 732.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44512,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1989.5, 109.5, 2029.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44516,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {610.5, 193.5, 650.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44536,
             1,
             {0.02732644017725258, -0.03545051698670606, 0.9726735598227475, 0.7208271787296899},
             {1258.5, 34.5, 1298.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {44537,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1002.5, 1153.5, 1042.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44540,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {0.5, 67.5, 40.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {44543,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {446.5, 193.5, 486.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44544,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {405.5, 193.5, 445.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44545,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1043.5, 1153.5, 1083.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44552,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {323.5, 193.5, 363.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44553,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {282.5, 193.5, 322.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44555,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {655.5, 108.5, 695.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44557,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {696.5, 108.5, 736.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44564,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {1715.5, 1954.5, 1752.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44592,
             1,
             {0.05149852578009601, -0.22451994091580502, 0.878677255469904, 0.8153618906942394},
             {1753.5, 1954.5, 1788.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44593,
             1,
             {0.062973135155096, -0.22451994091580502, 0.890151864844904, 0.8153618906942394},
             {1789.5, 1954.5, 1824.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44596,
             1,
             {0.06311918678452733, -0.1772525849335303, 0.9139315944654727, 0.8153618906942394},
             {40.5, 193.5, 76.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {44599,
             1,
             {0.06165434303452732, -0.20088626292466766, 0.9124667507154727, 0.8153618906942393},
             {1197.5, 1153.5, 1233.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44600,
             1,
             {0.06507231178452733, -0.20088626292466766, 0.9158847194654727, 0.8153618906942393},
             {1275.5, 1153.5, 1311.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44602,
             1,
             {0.064437978905096, -0.20088626292466766, 0.891616708594904, 0.8153618906942393},
             {1312.5, 1153.5, 1347.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44608,
             1,
             {0.062973135155096, -0.20088626292466766, 0.890151864844904, 0.8153618906942393},
             {1388.5, 1153.5, 1423.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44609,
             1,
             {0.063461416405096, -0.20088626292466766, 0.890640146094904, 0.8153618906942393},
             {1694.5, 1153.5, 1729.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44611,
             1,
             {0.06585269629338994, -0.20088626292466766, 0.96393245995661, 0.8153618906942393},
             {370.5, 1109.5, 408.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44613,
             1,
             {0.069809072655096, -0.20088626292466766, 0.896987802344904, 0.8153618906942393},
             {409.5, 1109.5, 444.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44614,
             1,
             {0.07034531653895863, -0.20088626292466766, 0.9447914022110414, 0.8153618906942393},
             {486.5, 1109.5, 523.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44618,
             1,
             {0.06556059303452733, -0.20088626292466766, 0.9163730007154727, 0.8153618906942393},
             {827.5, 1109.5, 863.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44620,
             1,
             {0.0230779605566839, -0.22451994091580502, 0.9920587581933162, 0.8153618906942394},
             {1825.5, 1954.5, 1866.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44621,
             1,
             {0.04588112767725259, -0.22451994091580502, 0.9912282473227475, 0.8153618906942394},
             {1942.5, 1954.5, 1982.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44622,
             1,
             {0.04588112767725259, -0.22451994091580502, 0.9912282473227475, 0.8153618906942394},
             {1983.5, 1954.5, 2023.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {44624,
             1,
             {0.04588112767725259, -0.1772525849335303, 0.9912282473227475, 0.8153618906942394},
             {1571.5, 236.5, 1611.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {44628,
             1,
             {0.04514870580225259, -0.20088626292466766, 0.9904958254477475, 0.8153618906942393},
             {2007.5, 1153.5, 2047.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {44630,
             1,
             {0.04514870580225259, -0.20088626292466766, 0.9904958254477475, 0.8153618906942393},
             {980.5, 1109.5, 1020.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44636,
             1,
             {0.04685769017725259, -0.20088626292466766, 0.9922048098227475, 0.8153618906942393},
             {1021.5, 1109.5, 1061.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44637,
             1,
             {0.04612526830225259, -0.20088626292466766, 0.9914723879477475, 0.8153618906942393},
             {1062.5, 1109.5, 1102.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44639,
             1,
             {0.04514870580225259, -0.20088626292466766, 0.9904958254477475, 0.8153618906942393},
             {1144.5, 1109.5, 1184.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44640,
             1,
             {0.04612526830225259, -0.20088626292466766, 0.9914723879477475, 0.8153618906942393},
             {1185.5, 1109.5, 1225.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44641,
             1,
             {0.04734597142725259, -0.20088626292466766, 0.9926930910727475, 0.8153618906942393},
             {1267.5, 1109.5, 1307.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44645,
             1,
             {0.04514870580225259, -0.20088626292466766, 0.9904958254477475, 0.8153618906942393},
             {1308.5, 1109.5, 1348.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44648,
             1,
             {0.016536290043389953, -0.22451994091580502, 0.91461605370661, 0.8153618906942394},
             {0.5, 1909.5, 38.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44649,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {1388.5, 1109.5, 1425.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44652,
             1,
             {0.03362613379338995, -0.20088626292466766, 0.93170589745661, 0.8153618906942393},
             {1426.5, 1109.5, 1464.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44656,
             1,
             {0.03704410254338995, -0.20088626292466766, 0.93512386620661, 0.8153618906942393},
             {1775.5, 1109.5, 1813.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44664,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {1814.5, 1109.5, 1851.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44665,
             1,
             {0.03372422278895864, -0.20088626292466766, 0.9081703084610414, 0.8153618906942393},
             {0.5, 1065.5, 37.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44667,
             1,
             {0.03294167455225258, -0.20088626292466766, 0.9782887941977475, 0.8153618906942393},
             {80.5, 1065.5, 120.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44668,
             1,
             {0.03587136205225258, -0.20088626292466766, 0.9812184816977475, 0.8153618906942393},
             {121.5, 1065.5, 161.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44669,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {162.5, 1065.5, 199.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44676,
             1,
             {0.0199041324316839, -0.22451994091580502, 0.9888849300683162, 0.8153618906942394},
             {39.5, 1909.5, 80.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44677,
             1,
             {0.04270729955225258, -0.22451994091580502, 0.9880544191977475, 0.8153618906942394},
             {158.5, 1909.5, 198.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44684,
             1,
             {0.05208273229782127, -0.20088626292466766, 0.9737961739521788, 0.8153618906942393},
             {277.5, 1065.5, 316.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44732,
             1,
             {0.01126328528895864, -0.22451994091580502, 0.8857093709610414, 0.8153618906942394},
             {199.5, 1909.5, 236.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44733,
             1,
             {0.042123093034527326, -0.22451994091580502, 0.8929355007154727, 0.8153618906942394},
             {237.5, 1909.5, 273.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44734,
             1,
             {0.042123093034527326, -0.22451994091580502, 0.8929355007154727, 0.8153618906942394},
             {274.5, 1909.5, 310.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44736,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {428.5, 1065.5, 465.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44740,
             1,
             {0.04275742591395864, -0.20088626292466766, 0.9172035115860414, 0.8153618906942393},
             {466.5, 1065.5, 503.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44748,
             1,
             {0.042123093034527326, -0.20088626292466766, 0.8929355007154727, 0.8153618906942393},
             {586.5, 1065.5, 622.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44749,
             1,
             {0.042123093034527326, -0.20088626292466766, 0.8929355007154727, 0.8153618906942393},
             {623.5, 1065.5, 659.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44751,
             1,
             {0.04719991979782127, -0.20088626292466766, 0.9689133614521788, 0.8153618906942393},
             {660.5, 1065.5, 699.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44752,
             1,
             {0.04319558080225259, -0.20088626292466766, 0.9885427004477475, 0.8153618906942393},
             {700.5, 1065.5, 740.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44753,
             1,
             {0.052132858659527326, -0.22451994091580502, 0.9029452663404727, 0.8153618906942394},
             {311.5, 1909.5, 347.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44760,
             1,
             {0.0010573416728212673, -0.22451994091580502, 0.9227707833271788, 0.8153618906942394},
             {348.5, 1909.5, 387.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44761,
             1,
             {0.02908555091395864, -0.22451994091580502, 0.9035316365860414, 0.8153618906942394},
             {388.5, 1909.5, 425.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44764,
             1,
             {0.03240543066838995, -0.20088626292466766, 0.93048519433161, 0.8153618906942393},
             {2009.5, 1109.5, 2047.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {44776,
             1,
             {0.02908555091395864, -0.20088626292466766, 0.9035316365860414, 0.8153618906942393},
             {1053.5, 1065.5, 1090.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44779,
             1,
             {0.023176049552252585, -0.20088626292466766, 0.9685231691977475, 0.8153618906942393},
             {1211.5, 1065.5, 1251.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44781,
             1,
             {0.024348790043389953, -0.20088626292466766, 0.92242855370661, 0.8153618906942393},
             {1291.5, 1065.5, 1329.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44788,
             1,
             {0.01028672278895864, -0.22451994091580502, 0.8847328084610414, 0.8153618906942394},
             {500.5, 1909.5, 537.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44792,
             1,
             {0.03933945716395864, -0.20088626292466766, 0.9137855428360414, 0.8153618906942393},
             {1531.5, 1065.5, 1568.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44796,
             1,
             {0.056527389909527326, -0.20088626292466766, 0.9073397975904727, 0.8153618906942393},
             {1569.5, 1065.5, 1605.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44807,
             1,
             {0.03963156042282127, -0.20088626292466766, 0.9613450020771788, 0.8153618906942393},
             {1606.5, 1065.5, 1645.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44808,
             1,
             {0.03880104955225258, -0.20088626292466766, 0.9841481691977475, 0.8153618906942393},
             {1646.5, 1065.5, 1686.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44813,
             1,
             {0.04275742591395864, -0.20088626292466766, 0.9172035115860414, 0.8153618906942393},
             {1687.5, 1065.5, 1724.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44816,
             1,
             {-0.0028489083271787327, -0.22451994091580502, 0.9188645333271788, 0.8153618906942394},
             {580.5, 1909.5, 619.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44844,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {503.5, 33.5, 543.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {44845,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1765.5, 1065.5, 1805.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44848,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1866.5, 109.5, 1906.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44850,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1736.5, 279.5, 1776.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44852,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1695.5, 279.5, 1735.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44860,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1654.5, 279.5, 1694.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44861,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1613.5, 279.5, 1653.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44863,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1572.5, 279.5, 1612.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44865,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1531.5, 279.5, 1571.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44866,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1490.5, 279.5, 1530.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44867,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1449.5, 279.5, 1489.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44872,
             1,
             {0.0269842105566839, -0.22451994091580502, 0.9959650081933162, 0.8153618906942394},
             {661.5, 1909.5, 702.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44873,
             1,
             {0.0223455386816839, -0.22451994091580502, 0.9913263363183162, 0.8153618906942394},
             {703.5, 1909.5, 744.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44880,
             1,
             {0.0223455386816839, -0.20088626292466766, 0.9913263363183162, 0.8153618906942393},
             {1964.5, 1065.5, 2005.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44892,
             1,
             {0.0223455386816839, -0.22451994091580502, 0.9913263363183162, 0.8153618906942394},
             {745.5, 1909.5, 786.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44893,
             1,
             {0.0230779605566839, -0.20088626292466766, 0.9920587581933162, 0.8153618906942393},
             {2006.5, 1065.5, 2047.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {44900,
             1,
             {0.015071446293389953, -0.22451994091580502, 0.91315120995661, 0.8153618906942394},
             {787.5, 1909.5, 825.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44901,
             1,
             {0.02176133216395864, -0.22451994091580502, 0.8962074178360414, 0.8153618906942394},
             {826.5, 1909.5, 863.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44921,
             1,
             {0.02615586341395864, -0.20088626292466766, 0.9006019490860414, 0.8153618906942393},
             {698.5, 1021.5, 735.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44928,
             1,
             {0.039193405534527326, -0.22451994091580502, 0.8900058132154727, 0.8153618906942394},
             {941.5, 1909.5, 977.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {44932,
             1,
             {0.03518906653895864, -0.1772525849335303, 0.9096351522110414, 0.8153618906942394},
             {1049.5, 279.5, 1086.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44936,
             1,
             {0.049935593034527326, -0.20088626292466766, 0.9007480007154727, 0.8153618906942393},
             {777.5, 1021.5, 813.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44944,
             1,
             {0.04881297890509601, -0.20088626292466766, 0.875991708594904, 0.8153618906942393},
             {1194.5, 1021.5, 1229.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44945,
             1,
             {0.04930126015509601, -0.20088626292466766, 0.876479989844904, 0.8153618906942393},
             {1230.5, 1021.5, 1265.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44949,
             1,
             {0.04978954140509601, -0.20088626292466766, 0.876968271094904, 0.8153618906942393},
             {1266.5, 1021.5, 1301.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44956,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {462.5, 33.5, 502.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {44984,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {808.5, 279.5, 848.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44985,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1492.5, 1021.5, 1532.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {44988,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1475.5, 109.5, 1515.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {44992,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {684.5, 279.5, 724.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {44999,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {643.5, 279.5, 683.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45000,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {602.5, 279.5, 642.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45001,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {561.5, 279.5, 601.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45003,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {520.5, 279.5, 560.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45005,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {479.5, 279.5, 519.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45006,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {438.5, 279.5, 478.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45012,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {978.5, 1909.5, 1015.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45020,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {1823.5, 1021.5, 1860.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {45032,
             1,
             {0.04158468542282127, -0.22451994091580502, 0.9632981270771788, 0.8153618906942394},
             {1016.5, 1909.5, 1055.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45033,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {574.5, 977.5, 610.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45040,
             1,
             {0.012874180668389953, -0.22451994091580502, 0.91095394433161, 0.8153618906942394},
             {1097.5, 1909.5, 1135.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45041,
             1,
             {0.01736680091395864, -0.22451994091580502, 0.8918128865860414, 0.8153618906942394},
             {1136.5, 1909.5, 1173.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45044,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {164.5, 279.5, 202.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {45048,
             1,
             {0.025325352543389953, -0.20088626292466766, 0.92340511620661, 0.8153618906942393},
             {913.5, 977.5, 951.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45056,
             1,
             {0.02469101966395864, -0.20088626292466766, 0.8991371053360414, 0.8153618906942393},
             {952.5, 977.5, 989.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45057,
             1,
             {0.02469101966395864, -0.20088626292466766, 0.8991371053360414, 0.8153618906942393},
             {990.5, 977.5, 1027.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45060,
             1,
             {0.028303002677252585, -0.22451994091580502, 0.9736501223227475, 0.8153618906942394},
             {1215.5, 1909.5, 1255.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45068,
             1,
             {0.039681686784527326, -0.22451994091580502, 0.8904940944654727, 0.8153618906942394},
             {1339.5, 1909.5, 1375.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45072,
             1,
             {0.04568711341395864, -0.1772525849335303, 0.9201331990860414, 0.8153618906942394},
             {2002.5, 322.5, 2039.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45076,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {1104.5, 977.5, 1141.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45084,
             1,
             {0.046029343034527326, -0.1772525849335303, 0.8968417507154727, 0.8153618906942394},
             {1924.5, 322.5, 1960.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45085,
             1,
             {0.046029343034527326, -0.1772525849335303, 0.8968417507154727, 0.8153618906942394},
             {1887.5, 322.5, 1923.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45096,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {1846.5, 322.5, 1886.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45124,
             1,
             {0.02732644017725258, -0.03545051698670606, 0.9726735598227475, 0.7208271787296899},
             {1217.5, 34.5, 1257.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {45125,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1142.5, 977.5, 1182.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45128,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1702.5, 109.5, 1742.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45130,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1687.5, 322.5, 1727.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45132,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1646.5, 322.5, 1686.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45134,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1605.5, 322.5, 1645.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45139,
             1,
             {0.02830300267725258, -0.20088626292466766, 0.9736501223227475, 0.791728212703102},
             {1564.5, 322.5, 1604.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45140,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1523.5, 322.5, 1563.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45141,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1482.5, 322.5, 1522.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45143,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {1270.5, 108.5, 1310.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45145,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {1188.5, 108.5, 1228.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45149,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1359.5, 322.5, 1399.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45180,
             1,
             {0.036996139909527326, -0.22451994091580502, 0.8878085475904727, 0.8153618906942394},
             {1376.5, 1909.5, 1412.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45181,
             1,
             {0.063949697655096, -0.22451994091580502, 0.891128427344904, 0.8153618906942394},
             {1413.5, 1909.5, 1448.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45184,
             1,
             {0.04422226966395864, -0.1772525849335303, 0.9186683553360414, 0.8153618906942394},
             {1239.5, 322.5, 1276.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45188,
             1,
             {0.06214262428452732, -0.20088626292466766, 0.9129550319654727, 0.8153618906942393},
             {1296.5, 977.5, 1332.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45196,
             1,
             {0.068832510155096, -0.20088626292466766, 0.896011239844904, 0.8153618906942393},
             {1374.5, 977.5, 1409.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45197,
             1,
             {0.066391103905096, -0.20088626292466766, 0.893569833594904, 0.8153618906942393},
             {1451.5, 977.5, 1486.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45199,
             1,
             {0.04964132604782127, -0.20088626292466766, 0.9713547677021788, 0.8153618906942393},
             {1569.5, 977.5, 1608.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45201,
             1,
             {0.05872465553452732, -0.20088626292466766, 0.9095370632154727, 0.8153618906942393},
             {1609.5, 977.5, 1645.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45208,
             1,
             {0.09637027441838994, -0.22451994091580502, 0.99445003808161, 0.8153618906942394},
             {1449.5, 1909.5, 1487.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45209,
             1,
             {0.10574570716395862, -0.22451994091580502, 0.9801917928360414, 0.8153618906942394},
             {1488.5, 1909.5, 1525.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45210,
             1,
             {0.10574570716395862, -0.22451994091580502, 0.9801917928360414, 0.8153618906942394},
             {1526.5, 1909.5, 1563.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45212,
             1,
             {0.10052066504338994, -0.1772525849335303, 0.99860042870661, 0.8153618906942394},
             {876.5, 322.5, 914.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45215,
             1,
             {0.09905582129338994, -0.20088626292466766, 0.99713558495661, 0.8153618906942393},
             {1683.5, 977.5, 1721.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45216,
             1,
             {0.09148746191838994, -0.20088626292466766, 0.98956722558161, 0.8153618906942393},
             {1722.5, 977.5, 1760.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45217,
             1,
             {0.10574570716395862, -0.22451994091580502, 0.9801917928360414, 0.8153618906942394},
             {1638.5, 1909.5, 1675.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45218,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {1761.5, 977.5, 1798.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45224,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {1799.5, 977.5, 1836.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45225,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {1837.5, 977.5, 1874.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45227,
             1,
             {0.10428086341395862, -0.20088626292466766, 0.9787269490860414, 0.8153618906942393},
             {1916.5, 977.5, 1953.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {45228,
             1,
             {0.08440738379338994, -0.20088626292466766, 0.98248714745661, 0.8153618906942393},
             {189.5, 933.5, 227.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45229,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {452.5, 933.5, 489.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45230,
             1,
             {0.08098941504338994, -0.20088626292466766, 0.97906917870661, 0.8153618906942393},
             {490.5, 933.5, 528.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45231,
             1,
             {0.09197574316838994, -0.20088626292466766, 0.99005550683161, 0.8153618906942393},
             {529.5, 933.5, 567.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45233,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {609.5, 933.5, 646.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45235,
             1,
             {0.10623398841395862, -0.22451994091580502, 0.9806800740860414, 0.8153618906942394},
             {1676.5, 1909.5, 1713.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45236,
             1,
             {0.09773919290066468, -0.22451994091580502, 0.9012842445993354, 0.8153618906942394},
             {1714.5, 1909.5, 1748.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45237,
             1,
             {0.10857946939623336, -0.22451994091580502, 0.8884908431037667, 0.8153618906942394},
             {1749.5, 1909.5, 1782.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45240,
             1,
             {0.098861807030096, -0.1772525849335303, 0.926040536719904, 0.8153618906942394},
             {237.5, 322.5, 272.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {45244,
             1,
             {0.097396963280096, -0.20088626292466766, 0.924575692969904, 0.8153618906942393},
             {647.5, 933.5, 682.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45252,
             1,
             {0.10857946939623336, -0.20088626292466766, 0.8884908431037667, 0.8153618906942393},
             {910.5, 933.5, 943.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45253,
             1,
             {0.10857946939623336, -0.20088626292466766, 0.8884908431037667, 0.8153618906942393},
             {1740.5, 933.5, 1773.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45255,
             1,
             {0.10169340553452733, -0.20088626292466766, 0.9525058132154727, 0.8153618906942393},
             {1849.5, 933.5, 1885.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45256,
             1,
             {0.09807925879338994, -0.20088626292466766, 0.99615902245661, 0.8153618906942393},
             {1927.5, 933.5, 1965.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {45257,
             1,
             {0.09969231790066468, -0.20088626292466766, 0.9032373695993354, 0.8153618906942393},
             {0.5, 889.5, 34.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45264,
             1,
             {0.09905582129338994, -0.22451994091580502, 0.99713558495661, 0.8153618906942394},
             {1783.5, 1909.5, 1821.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45265,
             1,
             {0.10574570716395862, -0.22451994091580502, 0.9801917928360414, 0.8153618906942394},
             {1822.5, 1909.5, 1859.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45268,
             1,
             {0.10721055091395862, -0.1772525849335303, 0.9816566365860414, 0.8153618906942394},
             {1949.5, 365.5, 1986.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45272,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {35.5, 889.5, 72.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45280,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {114.5, 889.5, 151.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45285,
             1,
             {0.10574570716395862, -0.20088626292466766, 0.9801917928360414, 0.8153618906942393},
             {152.5, 889.5, 189.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45320,
             1,
             {0.09954626627123336, -0.22451994091580502, 0.8794576399787667, 0.8153618906942394},
             {1860.5, 1909.5, 1893.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45321,
             1,
             {0.1238142771418021, -0.22451994091580502, 0.8800919728581981, 0.8153618906942394},
             {1894.5, 1909.5, 1926.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45323,
             1,
             {0.12806059303452733, -0.22451994091580502, 0.9788730007154727, 0.8153618906942394},
             {1927.5, 1909.5, 1963.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45324,
             1,
             {0.13201696939623336, -0.1772525849335303, 0.9119283431037667, 0.8153618906942394},
             {1669.5, 365.5, 1702.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45328,
             1,
             {0.12737829752123336, -0.20088626292466766, 0.9072896712287667, 0.8153618906942393},
             {231.5, 889.5, 264.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45330,
             1,
             {0.1262556833918021, -0.20088626292466766, 0.8825333791081981, 0.8153618906942393},
             {857.5, 889.5, 889.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45331,
             1,
             {0.1262556833918021, -0.20088626292466766, 0.8825333791081981, 0.8153618906942393},
             {931.5, 889.5, 963.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45336,
             1,
             {0.1262556833918021, -0.20088626292466766, 0.8825333791081981, 0.8153618906942393},
             {1005.5, 889.5, 1037.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45337,
             1,
             {0.1262556833918021, -0.20088626292466766, 0.8825333791081981, 0.8153618906942393},
             {1038.5, 889.5, 1070.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45339,
             1,
             {0.132797353905096, -0.20088626292466766, 0.959976083594904, 0.8153618906942393},
             {1153.5, 889.5, 1188.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45340,
             1,
             {0.11184922278895862, -0.20088626292466766, 0.9862953084610414, 0.8153618906942393},
             {1189.5, 889.5, 1226.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45341,
             1,
             {0.1328474802668021, -0.20088626292466766, 0.8891251759831981, 0.8153618906942393},
             {1675.5, 889.5, 1707.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45347,
             1,
             {0.12737829752123336, -0.20088626292466766, 0.9072896712287667, 0.8153618906942393},
             {1749.5, 889.5, 1782.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45348,
             1,
             {0.085434072655096, -0.22451994091580502, 0.912612802344904, 0.8153618906942394},
             {1964.5, 1909.5, 1999.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45349,
             1,
             {0.10955603189623336, -0.22451994091580502, 0.8894674056037667, 0.8153618906942394},
             {2000.5, 1909.5, 2033.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {45352,
             1,
             {0.099838369530096, -0.1772525849335303, 0.927017099219904, 0.8153618906942394},
             {1188.5, 365.5, 1223.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45356,
             1,
             {0.11019036477566468, -0.20088626292466766, 0.9137354164743354, 0.8153618906942393},
             {1940.5, 889.5, 1974.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45364,
             1,
             {0.10955603189623336, -0.20088626292466766, 0.8894674056037667, 0.8153618906942393},
             {1975.5, 889.5, 2008.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45365,
             1,
             {0.10955603189623336, -0.20088626292466766, 0.8894674056037667, 0.8153618906942393},
             {570.5, 845.5, 603.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {45367,
             1,
             {0.10999418678452733, -0.20088626292466766, 0.9608065944654727, 0.8153618906942393},
             {954.5, 845.5, 990.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {45368,
             1,
             {0.09905582129338994, -0.20088626292466766, 0.99713558495661, 0.8153618906942393},
             {2009.5, 889.5, 2047.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {45369,
             1,
             {0.10066888040066468, -0.20088626292466766, 0.9042139320993354, 0.8153618906942393},
             {1467.5, 845.5, 1501.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {45376,
             1,
             {0.09954626627123336, -0.22451994091580502, 0.8794576399787667, 0.8153618906942394},
             {41.5, 1864.5, 74.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45377,
             1,
             {0.1238142771418021, -0.22451994091580502, 0.8800919728581981, 0.8153618906942394},
             {75.5, 1864.5, 107.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45380,
             1,
             {0.12957556314623336, -0.1772525849335303, 0.9094869368537667, 0.8153618906942394},
             {856.5, 365.5, 889.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45384,
             1,
             {0.12493689127123336, -0.20088626292466766, 0.9048482649787667, 0.8153618906942393},
             {428.5, 801.5, 461.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45392,
             1,
             {0.1238142771418021, -0.20088626292466766, 0.8800919728581981, 0.8153618906942393},
             {462.5, 801.5, 494.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45393,
             1,
             {0.1238142771418021, -0.20088626292466766, 0.8800919728581981, 0.8153618906942393},
             {495.5, 801.5, 527.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45396,
             1,
             {0.11184922278895862, -0.20088626292466766, 0.9862953084610414, 0.8153618906942393},
             {528.5, 801.5, 565.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45397,
             1,
             {0.1304060740168021, -0.20088626292466766, 0.8866837697331981, 0.8153618906942393},
             {566.5, 801.5, 598.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45400,
             1,
             {0.1238142771418021, -0.22451994091580502, 0.8800919728581981, 0.8153618906942394},
             {108.5, 1864.5, 140.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45404,
             1,
             {0.085434072655096, -0.22451994091580502, 0.912612802344904, 0.8153618906942394},
             {179.5, 1864.5, 214.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45408,
             1,
             {0.099838369530096, -0.1772525849335303, 0.927017099219904, 0.8153618906942394},
             {533.5, 365.5, 568.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45432,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {98.5, 32.5, 138.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {45433,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {637.5, 801.5, 677.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45436,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {299.5, 108.5, 339.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45440,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {369.5, 365.5, 409.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45442,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {328.5, 365.5, 368.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45448,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {287.5, 365.5, 327.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45449,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {678.5, 801.5, 718.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45451,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {205.5, 365.5, 245.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45453,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {164.5, 365.5, 204.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45458,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 365.5, 163.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {45459,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {719.5, 801.5, 759.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45460,
             1,
             {0.0343084293066839, -0.22451994091580502, 1.0032892269433162, 0.8153618906942394},
             {253.5, 1864.5, 294.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45464,
             1,
             {0.0284490543066839, -0.20088626292466766, 0.9974298519433162, 0.8153618906942393},
             {801.5, 801.5, 842.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45468,
             1,
             {0.0284490543066839, -0.20088626292466766, 0.9974298519433162, 0.8153618906942393},
             {960.5, 801.5, 1001.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45480,
             1,
             {0.0284490543066839, -0.22451994091580502, 0.9974298519433162, 0.8153618906942394},
             {295.5, 1864.5, 336.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45516,
             1,
             {0.02176133216395864, -0.22451994091580502, 0.8962074178360414, 0.8153618906942394},
             {337.5, 1864.5, 374.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45520,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {1002.5, 801.5, 1038.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45524,
             1,
             {0.053597702409527326, -0.20088626292466766, 0.9044101100904727, 0.8153618906942393},
             {1192.5, 801.5, 1228.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45532,
             1,
             {0.05223094765509601, -0.20088626292466766, 0.879409677344904, 0.8153618906942393},
             {1270.5, 801.5, 1305.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45533,
             1,
             {0.05223094765509601, -0.20088626292466766, 0.879409677344904, 0.8153618906942393},
             {1306.5, 801.5, 1341.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45535,
             1,
             {0.03191714941838995, -0.20088626292466766, 0.92999691308161, 0.8153618906942393},
             {1342.5, 801.5, 1380.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45544,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {0.5, 32.5, 40.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {45545,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1381.5, 801.5, 1421.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45548,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {614.5, 108.5, 654.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45552,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1587.5, 408.5, 1627.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45561,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1546.5, 408.5, 1586.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45563,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1505.5, 408.5, 1545.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45565,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1464.5, 408.5, 1504.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45572,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1422.5, 801.5, 1462.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45573,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1463.5, 801.5, 1503.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45576,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {573.5, 108.5, 613.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45579,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1299.5, 408.5, 1339.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45580,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1258.5, 408.5, 1298.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45588,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1217.5, 408.5, 1257.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45589,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1176.5, 408.5, 1216.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45591,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1135.5, 408.5, 1175.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45593,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1094.5, 408.5, 1134.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45600,
             1,
             {0.01834336341395864, -0.22451994091580502, 0.8927894490860414, 0.8153618906942394},
             {375.5, 1864.5, 412.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45620,
             1,
             {0.04417214330225259, -0.22451994091580502, 0.9895192629477475, 0.8153618906942394},
             {413.5, 1864.5, 453.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45628,
             1,
             {0.0043292587933899535, -0.22451994091580502, 0.90240902245661, 0.8153618906942394},
             {454.5, 1864.5, 492.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45656,
             1,
             {0.01565781653895864, -0.22451994091580502, 0.8901039022110414, 0.8153618906942394},
             {493.5, 1864.5, 530.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45660,
             1,
             {0.023860508793389953, -0.20088626292466766, 0.92194027245661, 0.8153618906942393},
             {1545.5, 801.5, 1583.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45664,
             1,
             {0.04715195716395864, -0.20088626292466766, 0.9215980428360414, 0.8153618906942393},
             {1584.5, 801.5, 1621.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45672,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1622.5, 801.5, 1658.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45673,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1782.5, 801.5, 1818.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45684,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1819.5, 801.5, 1859.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45685,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1860.5, 801.5, 1900.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45692,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {649.5, 408.5, 689.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45700,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {608.5, 408.5, 648.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45701,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {567.5, 408.5, 607.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45705,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {526.5, 408.5, 566.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45712,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {257.5, 32.5, 297.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {45713,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1901.5, 801.5, 1941.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45716,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {205.5, 108.5, 245.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {45720,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {369.5, 408.5, 409.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45721,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1942.5, 801.5, 1982.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {45722,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {287.5, 408.5, 327.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45728,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {246.5, 408.5, 286.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45729,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {205.5, 408.5, 245.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45731,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {164.5, 408.5, 204.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45733,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 408.5, 163.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45734,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 408.5, 122.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45738,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 408.5, 81.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {45740,
             1,
             {0.042611374284527326, -0.22451994091580502, 0.8934237819654727, 0.8153618906942394},
             {531.5, 1864.5, 567.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45744,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {319.5, 757.5, 355.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45748,
             1,
             {0.053597702409527326, -0.20088626292466766, 0.9044101100904727, 0.8153618906942393},
             {356.5, 757.5, 392.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45768,
             1,
             {0.1274763865168021, -0.22451994091580502, 0.8837540822331981, 0.8153618906942394},
             {568.5, 1864.5, 600.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45769,
             1,
             {0.1338240427668021, -0.22451994091580502, 0.8901017384831981, 0.8153618906942394},
             {601.5, 1864.5, 633.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45772,
             1,
             {0.13665564127123336, -0.1772525849335303, 0.9165670149787667, 0.8153618906942394},
             {1850.5, 451.5, 1883.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {45776,
             1,
             {0.13494665689623336, -0.20088626292466766, 0.9148580306037667, 0.8153618906942393},
             {434.5, 757.5, 467.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45778,
             1,
             {0.1369978708918021, -0.20088626292466766, 0.8932755666081981, 0.8153618906942393},
             {468.5, 757.5, 500.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45784,
             1,
             {0.1343123240168021, -0.20088626292466766, 0.8905900197331981, 0.8153618906942393},
             {501.5, 757.5, 533.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45785,
             1,
             {0.1343123240168021, -0.20088626292466766, 0.8905900197331981, 0.8153618906942393},
             {616.5, 757.5, 648.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45787,
             1,
             {0.135727041405096, -0.20088626292466766, 0.962905771094904, 0.8153618906942393},
             {905.5, 757.5, 940.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45789,
             1,
             {0.13030798502123336, -0.20088626292466766, 0.9102193587287667, 0.8153618906942393},
             {1204.5, 757.5, 1237.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45794,
             1,
             {0.13592321939623336, -0.20088626292466766, 0.9158345931037667, 0.8153618906942393},
             {1279.5, 757.5, 1312.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45796,
             1,
             {0.08552999792282126, -0.22451994091580502, 1.0072434395771788, 0.8153618906942394},
             {634.5, 1864.5, 673.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45797,
             1,
             {0.08733707129338994, -0.22451994091580502, 0.98541683495661, 0.8153618906942394},
             {674.5, 1864.5, 712.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45798,
             1,
             {0.08733707129338994, -0.22451994091580502, 0.98541683495661, 0.8153618906942394},
             {713.5, 1864.5, 751.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45800,
             1,
             {0.08880191504338994, -0.20088626292466766, 0.98688167870661, 0.8153618906942393},
             {1594.5, 757.5, 1632.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45803,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1633.5, 757.5, 1671.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45804,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1672.5, 757.5, 1710.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45805,
             1,
             {0.08684879004338994, -0.22451994091580502, 0.98492855370661, 0.8153618906942394},
             {752.5, 1864.5, 790.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45806,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1789.5, 757.5, 1827.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {45807,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {655.5, 713.5, 693.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45811,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {731.5, 713.5, 769.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45812,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {974.5, 713.5, 1012.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45813,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1013.5, 713.5, 1051.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45815,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1052.5, 713.5, 1090.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45816,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1091.5, 713.5, 1129.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45817,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1130.5, 713.5, 1168.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45818,
             1,
             {0.08098941504338994, -0.20088626292466766, 0.97906917870661, 0.8153618906942393},
             {1169.5, 713.5, 1207.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45819,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1208.5, 713.5, 1246.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45823,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1247.5, 713.5, 1285.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45824,
             1,
             {0.083480947655096, -0.22451994091580502, 0.910659677344904, 0.8153618906942394},
             {791.5, 1864.5, 826.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45825,
             1,
             {0.09700677102566468, -0.22451994091580502, 0.9005518227243354, 0.8153618906942394},
             {827.5, 1864.5, 861.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45828,
             1,
             {0.08753324928452733, -0.1772525849335303, 0.9383456569654727, 0.8153618906942394},
             {730.5, 451.5, 766.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {45832,
             1,
             {0.097885244530096, -0.20088626292466766, 0.925063974219904, 0.8153618906942393},
             {1367.5, 713.5, 1402.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45840,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1444.5, 713.5, 1478.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45841,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1520.5, 713.5, 1554.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45843,
             1,
             {0.09705473365952733, -0.20088626292466766, 0.9478671413404727, 0.8153618906942393},
             {1555.5, 713.5, 1591.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45844,
             1,
             {0.09344058691838994, -0.20088626292466766, 0.99152035058161, 0.8153618906942393},
             {1592.5, 713.5, 1630.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45845,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1631.5, 713.5, 1665.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45852,
             1,
             {0.08636050879338994, -0.22451994091580502, 0.98444027245661, 0.8153618906942394},
             {900.5, 1864.5, 938.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45908,
             1,
             {0.10101111002123336, -0.22451994091580502, 0.8809224837287667, 0.8153618906942394},
             {982.5, 1864.5, 1015.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45909,
             1,
             {0.1308943552668021, -0.22451994091580502, 0.8871720509831981, 0.8153618906942394},
             {1016.5, 1864.5, 1048.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45910,
             1,
             {0.1308943552668021, -0.22451994091580502, 0.8871720509831981, 0.8153618906942394},
             {1049.5, 1864.5, 1081.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45912,
             1,
             {0.13104040689623336, -0.20088626292466766, 0.9109517806037667, 0.8153618906942393},
             {1666.5, 713.5, 1699.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45915,
             1,
             {0.12957556314623336, -0.20088626292466766, 0.9094869368537667, 0.8153618906942393},
             {1976.5, 713.5, 2009.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45916,
             1,
             {0.13152868814623336, -0.20088626292466766, 0.9114400618537667, 0.8153618906942393},
             {41.5, 669.5, 74.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45918,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {75.5, 669.5, 107.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45919,
             1,
             {0.1313826365168021, -0.20088626292466766, 0.8876603322331981, 0.8153618906942393},
             {190.5, 669.5, 222.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45924,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {223.5, 669.5, 255.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45925,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {256.5, 669.5, 288.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45927,
             1,
             {0.12659574928452733, -0.20088626292466766, 0.9774081569654727, 0.8153618906942393},
             {289.5, 669.5, 325.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45929,
             1,
             {0.12542517252123336, -0.20088626292466766, 0.9053365462287667, 0.8153618906942393},
             {449.5, 669.5, 482.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45931,
             1,
             {0.134262197655096, -0.20088626292466766, 0.961440927344904, 0.8153618906942393},
             {483.5, 669.5, 518.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45934,
             1,
             {0.12654778665066468, -0.20088626292466766, 0.9300928383493354, 0.8153618906942393},
             {519.5, 669.5, 553.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45936,
             1,
             {0.09969231790066468, -0.22451994091580502, 0.9032373695993354, 0.8153618906942394},
             {1082.5, 1864.5, 1116.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45937,
             1,
             {0.10555169290066468, -0.22451994091580502, 0.9090967445993354, 0.8153618906942394},
             {1117.5, 1864.5, 1151.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45940,
             1,
             {0.107406728905096, -0.1772525849335303, 0.934585458594904, 0.8153618906942394},
             {1803.5, 494.5, 1838.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {45944,
             1,
             {0.106430166405096, -0.20088626292466766, 0.933608896094904, 0.8153618906942393},
             {554.5, 669.5, 589.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45952,
             1,
             {0.10555169290066468, -0.20088626292466766, 0.9090967445993354, 0.8153618906942393},
             {590.5, 669.5, 624.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45953,
             1,
             {0.10555169290066468, -0.20088626292466766, 0.9090967445993354, 0.8153618906942393},
             {625.5, 669.5, 659.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45955,
             1,
             {0.11145903053452733, -0.20088626292466766, 0.9622714382154727, 0.8153618906942393},
             {850.5, 669.5, 886.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45956,
             1,
             {0.11184922278895862, -0.20088626292466766, 0.9862953084610414, 0.8153618906942393},
             {2010.5, 713.5, 2047.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {45957,
             1,
             {0.11116692727566468, -0.20088626292466766, 0.9147119789743354, 0.8153618906942393},
             {887.5, 669.5, 921.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45964,
             1,
             {0.10296423502123336, -0.22451994091580502, 0.8828756087287667, 0.8153618906942394},
             {1152.5, 1864.5, 1185.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45968,
             1,
             {0.13055212564623336, -0.20088626292466766, 0.9104634993537667, 0.8153618906942393},
             {1124.5, 669.5, 1157.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45972,
             1,
             {0.13152868814623336, -0.20088626292466766, 0.9114400618537667, 0.8153618906942393},
             {1158.5, 669.5, 1191.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45984,
             1,
             {0.10672226966395862, -0.20088626292466766, 0.9811683553360414, 0.8153618906942393},
             {1192.5, 669.5, 1229.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45985,
             1,
             {0.12493689127123336, -0.20088626292466766, 0.9048482649787667, 0.8153618906942393},
             {1230.5, 669.5, 1263.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {45992,
             1,
             {0.09822747415066468, -0.22451994091580502, 0.9017725258493354, 0.8153618906942394},
             {1186.5, 1864.5, 1220.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {45996,
             1,
             {0.105941885155096, -0.1772525849335303, 0.933120614844904, 0.8153618906942394},
             {1278.5, 494.5, 1313.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46020,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1303.5, 73.5, 1343.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {46021,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1305.5, 669.5, 1345.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {46024,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1949.5, 151.5, 1989.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {46027,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1118.5, 494.5, 1158.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46028,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1077.5, 494.5, 1117.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46030,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1036.5, 494.5, 1076.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46032,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {995.5, 494.5, 1035.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46036,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {954.5, 494.5, 994.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46037,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {913.5, 494.5, 953.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46039,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {872.5, 494.5, 912.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46041,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {831.5, 494.5, 871.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46043,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {790.5, 494.5, 830.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46045,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {749.5, 494.5, 789.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46048,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {1221.5, 1864.5, 1263.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46052,
             1,
             {0.0211248355566839, -0.20088626292466766, 0.9901056331933162, 0.8153618906942393},
             {1710.5, 669.5, 1751.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {46056,
             1,
             {0.0184392886816839, -0.20088626292466766, 0.9874200863183162, 0.8153618906942393},
             {532.5, 625.5, 573.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46076,
             1,
             {0.009456211918389953, -0.22451994091580502, 0.90753597558161, 0.8153618906942394},
             {1264.5, 1864.5, 1302.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46096,
             1,
             {0.0201482730566839, -0.22451994091580502, 0.9891290706933162, 0.8153618906942394},
             {1303.5, 1864.5, 1344.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46104,
             1,
             {0.043587936784527326, -0.22451994091580502, 0.8944003444654727, 0.8153618906942394},
             {1345.5, 1864.5, 1381.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46108,
             1,
             {0.03885117591395864, -0.20088626292466766, 0.9132972615860414, 0.8153618906942393},
             {1881.5, 625.5, 1918.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46112,
             1,
             {0.03567734778895864, -0.20088626292466766, 0.9101234334610414, 0.8153618906942393},
             {1919.5, 625.5, 1956.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46120,
             1,
             {0.04881297890509601, -0.20088626292466766, 0.875991708594904, 0.8153618906942393},
             {1957.5, 625.5, 1992.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46121,
             1,
             {0.04637157265509601, -0.20088626292466766, 0.873550302344904, 0.8153618906942393},
             {1993.5, 625.5, 2028.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46123,
             1,
             {0.04534488379338995, -0.20088626292466766, 0.94342464745661, 0.8153618906942393},
             {41.5, 581.5, 79.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46124,
             1,
             {0.03474874792282127, -0.22451994091580502, 0.9564621895771788, 0.8153618906942394},
             {1458.5, 1864.5, 1497.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46132,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1262.5, 73.5, 1302.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {46160,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {346.5, 581.5, 386.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46161,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {387.5, 581.5, 427.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46164,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1867.5, 151.5, 1907.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {46168,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 494.5, 122.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46176,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 494.5, 81.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46177,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1314.5, 494.5, 1354.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46179,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 494.5, 40.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46181,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1984.5, 538.5, 2024.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46188,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {1498.5, 1864.5, 1535.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46208,
             1,
             {0.05354757604782127, -0.22451994091580502, 0.9752610177021788, 0.8153618906942394},
             {1536.5, 1864.5, 1575.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46216,
             1,
             {0.0043292587933899535, -0.22451994091580502, 0.90240902245661, 0.8153618906942394},
             {1576.5, 1864.5, 1614.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46237,
             1,
             {0.01687851966395864, -0.20088626292466766, 0.8913246053360414, 0.8153618906942393},
             {501.5, 581.5, 538.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46244,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {1615.5, 1864.5, 1652.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46248,
             1,
             {0.04568711341395864, -0.20088626292466766, 0.9201331990860414, 0.8153618906942393},
             {855.5, 581.5, 892.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46252,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {893.5, 581.5, 930.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46261,
             1,
             {0.041146530534527326, -0.20088626292466766, 0.8919589382154727, 0.8153618906942393},
             {972.5, 581.5, 1008.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46263,
             1,
             {0.05452413854782127, -0.20088626292466766, 0.9762375802021788, 0.8153618906942393},
             {1315.5, 581.5, 1354.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46265,
             1,
             {0.055062546159527326, -0.20088626292466766, 0.9058749538404727, 0.8153618906942393},
             {1435.5, 581.5, 1471.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46272,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {431.5, 537.5, 471.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46276,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1506.5, 538.5, 1546.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46280,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1465.5, 538.5, 1505.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46288,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1424.5, 538.5, 1464.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46293,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1383.5, 538.5, 1423.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46300,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {380.5, 33.5, 420.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {46301,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {472.5, 537.5, 512.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46304,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1703.5, 151.5, 1743.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {46307,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1226.5, 538.5, 1266.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46308,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1185.5, 538.5, 1225.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46310,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1144.5, 538.5, 1184.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46316,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1103.5, 538.5, 1143.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46317,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1062.5, 538.5, 1102.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46319,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1021.5, 538.5, 1061.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46321,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1305.5, 538.5, 1345.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46328,
             1,
             {0.040169968034527326, -0.22451994091580502, 0.8909823757154727, 0.8153618906942394},
             {1691.5, 1864.5, 1727.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46356,
             1,
             {0.11224157877123336, -0.22451994091580502, 0.8921529524787667, 0.8153618906942394},
             {1728.5, 1864.5, 1761.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46357,
             1,
             {0.1318709177668021, -0.22451994091580502, 0.8881486134831981, 0.8153618906942394},
             {1762.5, 1864.5, 1794.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46360,
             1,
             {0.13201696939623336, -0.20088626292466766, 0.9119283431037667, 0.8153618906942393},
             {829.5, 537.5, 862.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46363,
             1,
             {0.13055212564623336, -0.20088626292466766, 0.9104634993537667, 0.8153618906942393},
             {795.5, 537.5, 828.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46364,
             1,
             {0.13348181314623336, -0.20088626292466766, 0.9133931868537667, 0.8153618906942393},
             {761.5, 537.5, 794.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46372,
             1,
             {0.1318709177668021, -0.20088626292466766, 0.8881486134831981, 0.8153618906942393},
             {728.5, 537.5, 760.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46373,
             1,
             {0.1318709177668021, -0.20088626292466766, 0.8881486134831981, 0.8153618906942393},
             {695.5, 537.5, 727.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46375,
             1,
             {0.131820791405096, -0.20088626292466766, 0.958999521094904, 0.8153618906942393},
             {659.5, 537.5, 694.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46376,
             1,
             {0.10672226966395862, -0.20088626292466766, 0.9811683553360414, 0.8153618906942393},
             {621.5, 537.5, 658.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46377,
             1,
             {0.1382185740168021, -0.20088626292466766, 0.8944962697331981, 0.8153618906942393},
             {588.5, 537.5, 620.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46378,
             1,
             {0.118393057030096, -0.20088626292466766, 0.945571786719904, 0.8153618906942393},
             {552.5, 537.5, 587.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46384,
             1,
             {0.06687722142725258, -0.22451994091580502, 1.0122243410727476, 0.8153618906942394},
             {1795.5, 1864.5, 1835.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46385,
             1,
             {0.06428976354782126, -0.22451994091580502, 0.9860032052021788, 0.8153618906942394},
             {1836.5, 1864.5, 1875.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46388,
             1,
             {0.06428976354782126, -0.1772525849335303, 0.9860032052021788, 0.8153618906942394},
             {1586.5, 538.5, 1625.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46392,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {391.5, 537.5, 430.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46400,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {351.5, 537.5, 390.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46401,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {311.5, 537.5, 350.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46403,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {271.5, 537.5, 310.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46404,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {231.5, 537.5, 270.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46405,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {191.5, 537.5, 230.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46411,
             1,
             {0.06428976354782126, -0.20088626292466766, 0.9860032052021788, 0.8153618906942393},
             {151.5, 537.5, 190.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46412,
             1,
             {0.06922270240952733, -0.22451994091580502, 0.9200351100904727, 0.8153618906942394},
             {1876.5, 1864.5, 1912.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46413,
             1,
             {0.06604887428452733, -0.22451994091580502, 0.9168612819654727, 0.8153618906942394},
             {1990.5, 1864.5, 2026.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {46416,
             1,
             {0.06668320716395863, -0.1772525849335303, 0.9411292928360414, 0.8153618906942394},
             {692.5, 451.5, 729.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {46420,
             1,
             {0.06619492591395863, -0.20088626292466766, 0.9406410115860414, 0.8153618906942393},
             {0.5, 537.5, 37.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46428,
             1,
             {0.06556059303452733, -0.20088626292466766, 0.9163730007154727, 0.8153618906942393},
             {38.5, 537.5, 74.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46429,
             1,
             {0.06556059303452733, -0.20088626292466766, 0.9163730007154727, 0.8153618906942393},
             {1991.5, 581.5, 2027.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46431,
             1,
             {0.06438785254338994, -0.20088626292466766, 0.96246761620661, 0.8153618906942393},
             {1952.5, 581.5, 1990.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46432,
             1,
             {0.07342105566838994, -0.20088626292466766, 0.97150081933161, 0.8153618906942393},
             {1913.5, 581.5, 1951.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46433,
             1,
             {0.06849028053452733, -0.20088626292466766, 0.9193026882154727, 0.8153618906942393},
             {1876.5, 581.5, 1912.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46496,
             1,
             {0.061508291405096005, -0.22451994091580502, 0.888687021094904, 0.8153618906942394},
             {0.5, 1819.5, 35.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46497,
             1,
             {0.069564932030096, -0.22451994091580502, 0.896743661719904, 0.8153618906942394},
             {36.5, 1819.5, 71.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46500,
             1,
             {0.06971098365952733, -0.20088626292466766, 0.9205233913404727, 0.8153618906942393},
             {1766.5, 581.5, 1802.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46504,
             1,
             {0.07019926490952733, -0.20088626292466766, 0.9210116725904727, 0.8153618906942393},
             {1729.5, 581.5, 1765.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46506,
             1,
             {0.069564932030096, -0.20088626292466766, 0.896743661719904, 0.8153618906942393},
             {1693.5, 581.5, 1728.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46507,
             1,
             {0.069564932030096, -0.20088626292466766, 0.896743661719904, 0.8153618906942393},
             {1657.5, 581.5, 1692.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46512,
             1,
             {0.069564932030096, -0.20088626292466766, 0.896743661719904, 0.8153618906942393},
             {1621.5, 581.5, 1656.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46513,
             1,
             {0.069564932030096, -0.20088626292466766, 0.896743661719904, 0.8153618906942393},
             {1585.5, 581.5, 1620.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46515,
             1,
             {0.06888047278895863, -0.20088626292466766, 0.9433265584610414, 0.8153618906942393},
             {1547.5, 581.5, 1584.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46516,
             1,
             {0.07610660254338994, -0.20088626292466766, 0.97418636620661, 0.8153618906942393},
             {1508.5, 581.5, 1546.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46517,
             1,
             {0.075424307030096, -0.20088626292466766, 0.902603036719904, 0.8153618906942393},
             {1472.5, 581.5, 1507.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46523,
             1,
             {0.074936025780096, -0.22451994091580502, 0.902114755469904, 0.8153618906942394},
             {72.5, 1819.5, 107.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46524,
             1,
             {0.069320791405096, -0.22451994091580502, 0.896499521094904, 0.8153618906942394},
             {108.5, 1819.5, 143.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46525,
             1,
             {0.06604887428452733, -0.22451994091580502, 0.9168612819654727, 0.8153618906942394},
             {302.5, 1819.5, 338.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46528,
             1,
             {0.06668320716395863, -0.1772525849335303, 0.9411292928360414, 0.8153618906942394},
             {1663.5, 538.5, 1700.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46532,
             1,
             {0.06619492591395863, -0.20088626292466766, 0.9406410115860414, 0.8153618906942393},
             {1277.5, 581.5, 1314.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46540,
             1,
             {0.06556059303452733, -0.20088626292466766, 0.9163730007154727, 0.8153618906942393},
             {1240.5, 581.5, 1276.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46541,
             1,
             {0.06556059303452733, -0.20088626292466766, 0.9163730007154727, 0.8153618906942393},
             {1203.5, 581.5, 1239.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46543,
             1,
             {0.06863633216395863, -0.20088626292466766, 0.9430824178360414, 0.8153618906942393},
             {1165.5, 581.5, 1202.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46544,
             1,
             {0.07342105566838994, -0.20088626292466766, 0.97150081933161, 0.8153618906942393},
             {1126.5, 581.5, 1164.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46545,
             1,
             {0.06849028053452733, -0.20088626292466766, 0.9193026882154727, 0.8153618906942393},
             {1089.5, 581.5, 1125.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46552,
             1,
             {0.061508291405096005, -0.22451994091580502, 0.888687021094904, 0.8153618906942394},
             {339.5, 1819.5, 374.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46572,
             1,
             {0.07366519629338994, -0.20088626292466766, 0.97174495995661, 0.8153618906942393},
             {1009.5, 581.5, 1047.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46608,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {421.5, 33.5, 461.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {46609,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {931.5, 581.5, 971.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46612,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1434.5, 109.5, 1474.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {46616,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1783.5, 538.5, 1823.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46629,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1824.5, 538.5, 1864.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46636,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {375.5, 1819.5, 417.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46644,
             1,
             {0.0235662418066839, -0.20088626292466766, 0.9925470394433162, 0.8153618906942393},
             {736.5, 581.5, 777.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46664,
             1,
             {0.025569493168389953, -0.22451994091580502, 0.92364925683161, 0.8153618906942394},
             {418.5, 1819.5, 456.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46692,
             1,
             {0.02420273841395864, -0.22451994091580502, 0.8986488240860414, 0.8153618906942394},
             {457.5, 1819.5, 494.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46696,
             1,
             {0.04129258216395864, -0.20088626292466766, 0.9157386678360414, 0.8153618906942393},
             {621.5, 581.5, 658.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46748,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {580.5, 581.5, 620.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46749,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {539.5, 581.5, 579.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46752,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {1826.5, 151.5, 1866.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {46756,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1943.5, 538.5, 1983.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {46763,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {161.5, 494.5, 201.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46764,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {202.5, 494.5, 242.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46769,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {284.5, 494.5, 324.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46804,
             1,
             {0.011897618168389953, -0.22451994091580502, 0.90997738183161, 0.8153618906942394},
             {495.5, 1819.5, 533.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46832,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {534.5, 1819.5, 571.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46836,
             1,
             {0.030940586918389953, -0.20088626292466766, 0.92902035058161, 0.8153618906942393},
             {230.5, 581.5, 268.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46840,
             1,
             {0.04617539466395864, -0.20088626292466766, 0.9206214803360414, 0.8153618906942393},
             {192.5, 581.5, 229.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46848,
             1,
             {0.045541061784527326, -0.20088626292466766, 0.8963534694654727, 0.8153618906942393},
             {155.5, 581.5, 191.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46849,
             1,
             {0.045541061784527326, -0.20088626292466766, 0.8963534694654727, 0.8153618906942393},
             {118.5, 581.5, 154.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46853,
             1,
             {0.03640976966395864, -0.20088626292466766, 0.9108558553360414, 0.8153618906942393},
             {80.5, 581.5, 117.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46888,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {954.5, 33.5, 994.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {46889,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 581.5, 40.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {46892,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1516.5, 109.5, 1556.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {46895,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {398.5, 494.5, 438.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46896,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {439.5, 494.5, 479.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46904,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {480.5, 494.5, 520.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46905,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {521.5, 494.5, 561.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46907,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {562.5, 494.5, 602.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46916,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {649.5, 1819.5, 686.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46920,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {1763.5, 625.5, 1800.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46924,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {1725.5, 625.5, 1762.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46932,
             1,
             {0.043587936784527326, -0.20088626292466766, 0.8944003444654727, 0.8153618906942393},
             {1688.5, 625.5, 1724.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46933,
             1,
             {0.045541061784527326, -0.20088626292466766, 0.8963534694654727, 0.8153618906942393},
             {1651.5, 625.5, 1687.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46944,
             1,
             {0.062484853905096005, -0.22451994091580502, 0.889663583594904, 0.8153618906942394},
             {687.5, 1819.5, 722.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46948,
             1,
             {0.06873442115952733, -0.1772525849335303, 0.9195468288404727, 0.8153618906942394},
             {603.5, 494.5, 639.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {46952,
             1,
             {0.06873442115952733, -0.20088626292466766, 0.9195468288404727, 0.8153618906942393},
             {1534.5, 625.5, 1570.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46960,
             1,
             {0.07942864602566468, -0.20088626292466766, 0.8829736977243354, 0.8153618906942393},
             {1499.5, 625.5, 1533.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46961,
             1,
             {0.068100088280096, -0.20088626292466766, 0.895278817969904, 0.8153618906942393},
             {1463.5, 625.5, 1498.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46963,
             1,
             {0.06180039466395863, -0.20088626292466766, 0.9362464803360414, 0.8153618906942393},
             {1425.5, 625.5, 1462.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46965,
             1,
             {0.072982900780096, -0.20088626292466766, 0.900161630469904, 0.8153618906942393},
             {1389.5, 625.5, 1424.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46972,
             1,
             {0.08162374792282126, -0.22451994091580502, 1.0033371895771788, 0.8153618906942394},
             {723.5, 1819.5, 762.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46973,
             1,
             {0.08684879004338994, -0.22451994091580502, 0.98492855370661, 0.8153618906942394},
             {763.5, 1819.5, 801.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {46976,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1268.5, 625.5, 1306.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46980,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1229.5, 625.5, 1267.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46988,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1190.5, 625.5, 1228.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46989,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1151.5, 625.5, 1189.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46991,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1112.5, 625.5, 1150.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46992,
             1,
             {0.08196597754338994, -0.20088626292466766, 0.98004574120661, 0.8153618906942393},
             {1073.5, 625.5, 1111.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46993,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {1034.5, 625.5, 1072.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46994,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {995.5, 625.5, 1033.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46998,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {956.5, 625.5, 994.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {46999,
             1,
             {0.08684879004338994, -0.22451994091580502, 0.98492855370661, 0.8153618906942394},
             {802.5, 1819.5, 840.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47000,
             1,
             {0.080062978905096, -0.22451994091580502, 0.907241708594904, 0.8153618906942394},
             {841.5, 1819.5, 876.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47001,
             1,
             {0.082260244530096, -0.22451994091580502, 0.909438974219904, 0.8153618906942394},
             {877.5, 1819.5, 912.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47004,
             1,
             {0.08460356178452733, -0.1772525849335303, 0.9354159694654727, 0.8153618906942394},
             {640.5, 494.5, 676.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47008,
             1,
             {0.08313871803452733, -0.20088626292466766, 0.9339511257154727, 0.8153618906942393},
             {758.5, 625.5, 794.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47016,
             1,
             {0.082260244530096, -0.20088626292466766, 0.909438974219904, 0.8153618906942393},
             {722.5, 625.5, 757.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47017,
             1,
             {0.082260244530096, -0.20088626292466766, 0.909438974219904, 0.8153618906942393},
             {686.5, 625.5, 721.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47019,
             1,
             {0.08816758216395862, -0.20088626292466766, 0.9626136678360414, 0.8153618906942393},
             {648.5, 625.5, 685.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47020,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {610.5, 625.5, 647.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47021,
             1,
             {0.086410635155096, -0.20088626292466766, 0.913589364844904, 0.8153618906942393},
             {574.5, 625.5, 609.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47028,
             1,
             {0.07967062292282126, -0.22451994091580502, 1.0013840645771788, 0.8153618906942394},
             {913.5, 1819.5, 952.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47029,
             1,
             {0.08684879004338994, -0.22451994091580502, 0.98492855370661, 0.8153618906942394},
             {953.5, 1819.5, 991.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47032,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {459.5, 625.5, 497.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47047,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {420.5, 625.5, 458.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47049,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {381.5, 625.5, 419.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47084,
             1,
             {0.10613806314623336, -0.22451994091580502, 0.8860494368537667, 0.8153618906942394},
             {1117.5, 1819.5, 1150.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47085,
             1,
             {0.11053259439623336, -0.22451994091580502, 0.8904439681037667, 0.8153618906942394},
             {1151.5, 1819.5, 1184.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47088,
             1,
             {0.11116692727566468, -0.1772525849335303, 0.9147119789743354, 0.8153618906942394},
             {1774.5, 451.5, 1808.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47092,
             1,
             {0.11165520852566468, -0.20088626292466766, 0.9152002602243354, 0.8153618906942393},
             {242.5, 625.5, 276.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47100,
             1,
             {0.1274763865168021, -0.20088626292466766, 0.8837540822331981, 0.8153618906942393},
             {209.5, 625.5, 241.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47101,
             1,
             {0.11321814127123336, -0.20088626292466766, 0.8931295149787667, 0.8153618906942393},
             {175.5, 625.5, 208.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47103,
             1,
             {0.117416494530096, -0.20088626292466766, 0.944595224219904, 0.8153618906942393},
             {139.5, 625.5, 174.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47104,
             1,
             {0.09182969153895862, -0.20088626292466766, 0.9662757772110414, 0.8153618906942393},
             {101.5, 625.5, 138.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47105,
             1,
             {0.11492712564623336, -0.20088626292466766, 0.8948384993537667, 0.8153618906942393},
             {67.5, 625.5, 100.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {47111,
             1,
             {0.10823723977566468, -0.22451994091580502, 0.9117822914743354, 0.8153618906942394},
             {1185.5, 1819.5, 1219.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47112,
             1,
             {0.081039541405096, -0.22451994091580502, 0.908218271094904, 0.8153618906942394},
             {1220.5, 1819.5, 1255.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47113,
             1,
             {0.09651848977566468, -0.22451994091580502, 0.9000635414743354, 0.8153618906942394},
             {1256.5, 1819.5, 1290.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47116,
             1,
             {0.08704496803452733, -0.1772525849335303, 0.9378573757154727, 0.8153618906942394},
             {677.5, 494.5, 713.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47120,
             1,
             {0.097396963280096, -0.20088626292466766, 0.924575692969904, 0.8153618906942393},
             {1968.5, 669.5, 2003.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47128,
             1,
             {0.09651848977566468, -0.20088626292466766, 0.9000635414743354, 0.8153618906942393},
             {1933.5, 669.5, 1967.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47129,
             1,
             {0.09651848977566468, -0.20088626292466766, 0.9000635414743354, 0.8153618906942393},
             {1898.5, 669.5, 1932.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47131,
             1,
             {0.09241606178452733, -0.20088626292466766, 0.9432284694654727, 0.8153618906942393},
             {1861.5, 669.5, 1897.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47133,
             1,
             {0.089340322655096, -0.20088626292466766, 0.916519052344904, 0.8153618906942393},
             {1825.5, 669.5, 1860.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47140,
             1,
             {0.10638220377123336, -0.22451994091580502, 0.8862935774787667, 0.8153618906942394},
             {1291.5, 1819.5, 1324.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47141,
             1,
             {0.11053259439623336, -0.22451994091580502, 0.8904439681037667, 0.8153618906942394},
             {1392.5, 1819.5, 1425.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47144,
             1,
             {0.11116692727566468, -0.1772525849335303, 0.9147119789743354, 0.8153618906942394},
             {714.5, 494.5, 748.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47148,
             1,
             {0.11165520852566468, -0.20088626292466766, 0.9152002602243354, 0.8153618906942393},
             {1675.5, 669.5, 1709.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47156,
             1,
             {0.11053259439623336, -0.20088626292466766, 0.8904439681037667, 0.8153618906942393},
             {1641.5, 669.5, 1674.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47157,
             1,
             {0.11053259439623336, -0.20088626292466766, 0.8904439681037667, 0.8153618906942393},
             {1607.5, 669.5, 1640.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47159,
             1,
             {0.10623398841395862, -0.20088626292466766, 0.9806800740860414, 0.8153618906942393},
             {1569.5, 669.5, 1606.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47160,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {1531.5, 669.5, 1568.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47161,
             1,
             {0.11492712564623336, -0.20088626292466766, 0.8948384993537667, 0.8153618906942393},
             {1497.5, 669.5, 1530.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47168,
             1,
             {0.081039541405096, -0.22451994091580502, 0.908218271094904, 0.8153618906942394},
             {1426.5, 1819.5, 1461.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47172,
             1,
             {0.08704496803452733, -0.1772525849335303, 0.9378573757154727, 0.8153618906942394},
             {1200.5, 494.5, 1236.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47185,
             1,
             {0.09651848977566468, -0.20088626292466766, 0.9000635414743354, 0.8153618906942393},
             {1383.5, 669.5, 1417.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47187,
             1,
             {0.09510160865952733, -0.20088626292466766, 0.9459140163404727, 0.8153618906942393},
             {1346.5, 669.5, 1382.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47196,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {798.5, 72.5, 838.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {47197,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1264.5, 669.5, 1304.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47200,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1396.5, 494.5, 1436.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47204,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1437.5, 494.5, 1477.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47212,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1478.5, 494.5, 1518.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47213,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1519.5, 494.5, 1559.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47215,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1560.5, 494.5, 1600.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47217,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1042.5, 669.5, 1082.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47224,
             1,
             {0.0245428043066839, -0.22451994091580502, 0.9935236019433162, 0.8153618906942394},
             {1462.5, 1819.5, 1503.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47228,
             1,
             {0.03513894017725258, -0.20088626292466766, 0.9804860598227475, 0.8153618906942393},
             {963.5, 669.5, 1003.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47245,
             1,
             {0.03513894017725258, -0.22451994091580502, 0.9804860598227475, 0.8153618906942394},
             {1504.5, 1819.5, 1544.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47272,
             1,
             {0.009748315177252585, -0.22451994091580502, 0.9550954348227475, 0.8153618906942394},
             {1582.5, 1819.5, 1622.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47280,
             1,
             {-6.527245661004655e-05, -0.22451994091580502, 0.89801449120661, 0.8153618906942394},
             {1623.5, 1819.5, 1661.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47284,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {812.5, 669.5, 849.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47288,
             1,
             {0.03933945716395864, -0.20088626292466766, 0.9137855428360414, 0.8153618906942393},
             {774.5, 669.5, 811.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47296,
             1,
             {0.033578171159527326, -0.20088626292466766, 0.8843905788404727, 0.8153618906942393},
             {737.5, 669.5, 773.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47297,
             1,
             {0.033578171159527326, -0.20088626292466766, 0.8843905788404727, 0.8153618906942393},
             {700.5, 669.5, 736.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47299,
             1,
             {0.025715544797821267, -0.20088626292466766, 0.9474289864521788, 0.8153618906942393},
             {660.5, 669.5, 699.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47301,
             1,
             {0.029671921159527326, -0.22451994091580502, 0.8804843288404727, 0.8153618906942394},
             {1662.5, 1819.5, 1698.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47308,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {861.5, 72.5, 901.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {47312,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1919.5, 494.5, 1959.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47316,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1960.5, 494.5, 2000.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47325,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {2001.5, 494.5, 2041.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {47327,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 451.5, 40.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47329,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {408.5, 669.5, 448.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47336,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {367.5, 669.5, 407.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47337,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {326.5, 669.5, 366.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47340,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 451.5, 81.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47344,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 451.5, 122.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47352,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 451.5, 163.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47353,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {164.5, 451.5, 204.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47355,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {149.5, 669.5, 189.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47357,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {108.5, 669.5, 148.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {47364,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1699.5, 1819.5, 1736.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47384,
             1,
             {0.04695577917282127, -0.22451994091580502, 0.9686692208271788, 0.8153618906942394},
             {1737.5, 1819.5, 1776.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47392,
             1,
             {0.0057941025433899535, -0.22451994091580502, 0.90387386620661, 0.8153618906942394},
             {1777.5, 1819.5, 1815.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47420,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {1816.5, 1819.5, 1853.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47421,
             1,
             {0.02420273841395864, -0.22451994091580502, 0.8986488240860414, 0.8153618906942394},
             {1854.5, 1819.5, 1891.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47424,
             1,
             {0.04519883216395864, -0.20088626292466766, 0.9196449178360414, 0.8153618906942393},
             {1938.5, 713.5, 1975.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47428,
             1,
             {0.04617539466395864, -0.20088626292466766, 0.9206214803360414, 0.8153618906942393},
             {1900.5, 713.5, 1937.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47436,
             1,
             {0.045541061784527326, -0.20088626292466766, 0.8963534694654727, 0.8153618906942393},
             {1863.5, 713.5, 1899.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47439,
             1,
             {0.04524679479782127, -0.20088626292466766, 0.9669602364521788, 0.8153618906942393},
             {1823.5, 713.5, 1862.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47441,
             1,
             {0.03518906653895864, -0.22451994091580502, 0.9096351522110414, 0.8153618906942394},
             {1892.5, 1819.5, 1929.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47448,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1741.5, 713.5, 1781.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47449,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1700.5, 713.5, 1740.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47452,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {487.5, 451.5, 527.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47456,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {528.5, 451.5, 568.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47464,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {569.5, 451.5, 609.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47465,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {610.5, 451.5, 650.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47467,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {651.5, 451.5, 691.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47469,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1479.5, 713.5, 1519.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47476,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {1056.5, 72.5, 1096.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {47477,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1403.5, 713.5, 1443.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47480,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {767.5, 451.5, 807.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47484,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {808.5, 451.5, 848.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47492,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {849.5, 451.5, 889.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47493,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {890.5, 451.5, 930.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47495,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {931.5, 451.5, 971.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47497,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {972.5, 451.5, 1012.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47498,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1013.5, 451.5, 1053.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47501,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1054.5, 451.5, 1094.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47502,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1095.5, 451.5, 1135.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47532,
             1,
             {0.09490759439623336, -0.22451994091580502, 0.8748189681037667, 0.8153618906942394},
             {1930.5, 1819.5, 1963.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47533,
             1,
             {0.1269881052668021, -0.22451994091580502, 0.8832658009831981, 0.8153618906942394},
             {1964.5, 1819.5, 1996.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47536,
             1,
             {0.12713415689623336, -0.20088626292466766, 0.9070455306037667, 0.8153618906942393},
             {940.5, 713.5, 973.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47540,
             1,
             {0.12762243814623336, -0.20088626292466766, 0.9075338118537667, 0.8153618906942393},
             {906.5, 713.5, 939.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47548,
             1,
             {0.1264998240168021, -0.20088626292466766, 0.8827775197331981, 0.8153618906942393},
             {873.5, 713.5, 905.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47549,
             1,
             {0.1264998240168021, -0.20088626292466766, 0.8827775197331981, 0.8153618906942393},
             {840.5, 713.5, 872.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47551,
             1,
             {0.121566885155096, -0.20088626292466766, 0.948745614844904, 0.8153618906942393},
             {804.5, 713.5, 839.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47553,
             1,
             {0.11858923502123336, -0.20088626292466766, 0.8985006087287667, 0.8153618906942393},
             {770.5, 713.5, 803.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47560,
             1,
             {0.08113546667282126, -0.22451994091580502, 1.0028489083271788, 0.8153618906942394},
             {1997.5, 1819.5, 2036.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {47561,
             1,
             {0.08196597754338994, -0.22451994091580502, 0.98004574120661, 0.8153618906942394},
             {37.5, 1774.5, 75.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47564,
             1,
             {0.08196597754338994, -0.1772525849335303, 0.98004574120661, 0.8153618906942394},
             {1259.5, 451.5, 1297.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47566,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {616.5, 713.5, 654.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47567,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {577.5, 713.5, 615.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47568,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {538.5, 713.5, 576.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47569,
             1,
             {0.08147769629338994, -0.22451994091580502, 0.97955745995661, 0.8153618906942394},
             {76.5, 1774.5, 114.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47570,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {461.5, 713.5, 499.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47576,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {422.5, 713.5, 460.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47577,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {383.5, 713.5, 421.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47579,
             1,
             {0.08098941504338994, -0.20088626292466766, 0.97906917870661, 0.8153618906942393},
             {344.5, 713.5, 382.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47581,
             1,
             {0.08196597754338994, -0.20088626292466766, 0.98004574120661, 0.8153618906942393},
             {305.5, 713.5, 343.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47582,
             1,
             {0.08050113379338994, -0.20088626292466766, 0.97858089745661, 0.8153618906942393},
             {266.5, 713.5, 304.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47585,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {227.5, 713.5, 265.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47587,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {188.5, 713.5, 226.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47588,
             1,
             {0.078598135155096, -0.22451994091580502, 0.905776864844904, 0.8153618906942394},
             {115.5, 1774.5, 150.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47589,
             1,
             {0.077377432030096, -0.22451994091580502, 0.904556161719904, 0.8153618906942394},
             {151.5, 1774.5, 186.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47592,
             1,
             {0.07972074928452733, -0.1772525849335303, 0.9305331569654727, 0.8153618906942394},
             {410.5, 408.5, 446.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47596,
             1,
             {0.07825590553452733, -0.20088626292466766, 0.9290683132154727, 0.8153618906942393},
             {36.5, 713.5, 72.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47604,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {0.5, 713.5, 35.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47605,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {73.5, 713.5, 108.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {47607,
             1,
             {0.08328476966395862, -0.20088626292466766, 0.9577308553360414, 0.8153618906942393},
             {1982.5, 757.5, 2019.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47608,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {1944.5, 757.5, 1981.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47609,
             1,
             {0.081039541405096, -0.20088626292466766, 0.908218271094904, 0.8153618906942393},
             {1908.5, 757.5, 1943.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47610,
             1,
             {0.07195621191838994, -0.20088626292466766, 0.97003597558161, 0.8153618906942393},
             {1869.5, 757.5, 1907.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47616,
             1,
             {0.08015890417282126, -0.22451994091580502, 1.0018723458271788, 0.8153618906942394},
             {379.5, 1774.5, 418.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47617,
             1,
             {0.08196597754338994, -0.22451994091580502, 0.98004574120661, 0.8153618906942394},
             {419.5, 1774.5, 457.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47624,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1750.5, 757.5, 1788.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47637,
             1,
             {0.08196597754338994, -0.20088626292466766, 0.98004574120661, 0.8153618906942393},
             {1711.5, 757.5, 1749.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47672,
             1,
             {0.08113763040066468, -0.22451994091580502, 0.8846826820993354, 0.8153618906942394},
             {458.5, 1774.5, 492.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47673,
             1,
             {0.09759314127123336, -0.22451994091580502, 0.8775045149787667, 0.8153618906942394},
             {493.5, 1774.5, 526.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47676,
             1,
             {0.09822747415066468, -0.1772525849335303, 0.9017725258493354, 0.8153618906942394},
             {1586.5, 451.5, 1620.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47680,
             1,
             {0.09773919290066468, -0.20088626292466766, 0.9012842445993354, 0.8153618906942393},
             {1559.5, 757.5, 1593.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47682,
             1,
             {0.09710486002123336, -0.20088626292466766, 0.8770162337287667, 0.8153618906942393},
             {1525.5, 757.5, 1558.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47688,
             1,
             {0.09515173502123336, -0.20088626292466766, 0.8750631087287667, 0.8153618906942393},
             {1491.5, 757.5, 1524.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47689,
             1,
             {0.09759314127123336, -0.20088626292466766, 0.8775045149787667, 0.8153618906942393},
             {1457.5, 757.5, 1490.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47691,
             1,
             {0.09705473365952733, -0.20088626292466766, 0.9478671413404727, 0.8153618906942393},
             {1420.5, 757.5, 1456.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47693,
             1,
             {0.09261223977566468, -0.20088626292466766, 0.8961572914743354, 0.8153618906942393},
             {1385.5, 757.5, 1419.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47694,
             1,
             {0.09705473365952733, -0.20088626292466766, 0.9478671413404727, 0.8153618906942393},
             {1348.5, 757.5, 1384.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47699,
             1,
             {0.10335442727566468, -0.20088626292466766, 0.9068994789743354, 0.8153618906942393},
             {1313.5, 757.5, 1347.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47700,
             1,
             {0.079574697655096, -0.22451994091580502, 0.906753427344904, 0.8153618906942394},
             {527.5, 1774.5, 562.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47701,
             1,
             {0.077377432030096, -0.22451994091580502, 0.904556161719904, 0.8153618906942394},
             {563.5, 1774.5, 598.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47704,
             1,
             {0.07972074928452733, -0.1772525849335303, 0.9305331569654727, 0.8153618906942394},
             {1662.5, 451.5, 1698.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47708,
             1,
             {0.07923246803452733, -0.20088626292466766, 0.9300448757154727, 0.8153618906942393},
             {1167.5, 757.5, 1203.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47716,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {1131.5, 757.5, 1166.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47717,
             1,
             {0.078353994530096, -0.20088626292466766, 0.905532724219904, 0.8153618906942393},
             {1095.5, 757.5, 1130.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47719,
             1,
             {0.07244449316838994, -0.20088626292466766, 0.97052425683161, 0.8153618906942393},
             {1056.5, 757.5, 1094.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47720,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {1018.5, 757.5, 1055.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47721,
             1,
             {0.082016103905096, -0.20088626292466766, 0.909194833594904, 0.8153618906942393},
             {982.5, 757.5, 1017.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47728,
             1,
             {0.07869622415066468, -0.22451994091580502, 0.8822412758493354, 0.8153618906942394},
             {599.5, 1774.5, 633.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47729,
             1,
             {0.11004431314623336, -0.22451994091580502, 0.8899556868537667, 0.8153618906942394},
             {634.5, 1774.5, 667.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47732,
             1,
             {0.11312005227566468, -0.20088626292466766, 0.9166651039743354, 0.8153618906942393},
             {870.5, 757.5, 904.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47736,
             1,
             {0.11629388040066468, -0.20088626292466766, 0.9198389320993354, 0.8153618906942393},
             {835.5, 757.5, 869.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47747,
             1,
             {0.11243559303452733, -0.20088626292466766, 0.9632480007154727, 0.8153618906942393},
             {798.5, 757.5, 834.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47748,
             1,
             {0.08719101966395862, -0.20088626292466766, 0.9616371053360414, 0.8153618906942393},
             {760.5, 757.5, 797.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47749,
             1,
             {0.11443884439623336, -0.20088626292466766, 0.8943502181037667, 0.8153618906942393},
             {726.5, 757.5, 759.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47751,
             1,
             {0.11341215553452733, -0.20088626292466766, 0.9642245632154727, 0.8153618906942393},
             {689.5, 757.5, 725.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47756,
             1,
             {0.081039541405096, -0.22451994091580502, 0.908218271094904, 0.8153618906942394},
             {668.5, 1774.5, 703.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47784,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1749.5, 73.5, 1789.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {47785,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {575.5, 757.5, 615.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47787,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {534.5, 757.5, 574.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47788,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {164.5, 108.5, 204.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {47792,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1809.5, 451.5, 1849.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47794,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1925.5, 451.5, 1965.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47800,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1966.5, 451.5, 2006.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47801,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {2007.5, 451.5, 2047.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {47803,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 408.5, 40.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47805,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {328.5, 408.5, 368.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47812,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {741.5, 1774.5, 783.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47816,
             1,
             {0.011016980936115212, -0.20088626292466766, 1.0036314565638849, 0.8153618906942393},
             {197.5, 757.5, 239.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47832,
             1,
             {0.0235662418066839, -0.22451994091580502, 0.9925470394433162, 0.8153618906942394},
             {784.5, 1774.5, 825.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47833,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {112.5, 757.5, 153.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47868,
             1,
             {0.040658249284527326, -0.22451994091580502, 0.8914706569654727, 0.8153618906942394},
             {903.5, 1774.5, 939.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47872,
             1,
             {0.053109421159527326, -0.1772525849335303, 0.9039218288404727, 0.8153618906942394},
             {1346.5, 538.5, 1382.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {47876,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {36.5, 757.5, 72.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47885,
             1,
             {0.05027782265509601, -0.20088626292466766, 0.877456552344904, 0.8153618906942393},
             {0.5, 757.5, 35.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47887,
             1,
             {0.04436832129338995, -0.20088626292466766, 0.94244808495661, 0.8153618906942393},
             {73.5, 757.5, 111.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {47889,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {1983.5, 801.5, 2019.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47896,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {790.5, 33.5, 830.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {47900,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {340.5, 108.5, 380.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {47904,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {729.5, 408.5, 769.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47913,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {770.5, 408.5, 810.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47915,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {811.5, 408.5, 851.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47924,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1741.5, 801.5, 1781.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47925,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1700.5, 801.5, 1740.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47926,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1659.5, 801.5, 1699.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47928,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {436.5, 108.5, 476.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {47931,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {893.5, 408.5, 933.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47932,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1382.5, 408.5, 1422.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47933,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1504.5, 801.5, 1544.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47934,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1423.5, 408.5, 1463.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47940,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1669.5, 408.5, 1709.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47941,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1751.5, 408.5, 1791.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47943,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1792.5, 408.5, 1832.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47945,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1833.5, 408.5, 1873.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47949,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1874.5, 408.5, 1914.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {47951,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {983.5, 1774.5, 1023.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47952,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1024.5, 1774.5, 1061.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {47956,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {1154.5, 801.5, 1191.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47960,
             1,
             {0.04715195716395864, -0.20088626292466766, 0.9215980428360414, 0.8153618906942393},
             {1116.5, 801.5, 1153.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47969,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1079.5, 801.5, 1115.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47971,
             1,
             {0.04695577917282127, -0.20088626292466766, 0.9686692208271788, 0.8153618906942393},
             {1039.5, 801.5, 1078.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {47980,
             1,
             {0.0043292587933899535, -0.22451994091580502, 0.90240902245661, 0.8153618906942394},
             {1062.5, 1774.5, 1100.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48008,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {1101.5, 1774.5, 1138.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48012,
             1,
             {0.04519883216395864, -0.20088626292466766, 0.9196449178360414, 0.8153618906942393},
             {922.5, 801.5, 959.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48016,
             1,
             {0.04617539466395864, -0.20088626292466766, 0.9206214803360414, 0.8153618906942393},
             {884.5, 801.5, 921.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48036,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {843.5, 801.5, 883.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48040,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 365.5, 81.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48044,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 365.5, 122.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48052,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {246.5, 365.5, 286.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48055,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {451.5, 365.5, 491.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48064,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {708.5, 33.5, 748.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {48068,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {778.5, 108.5, 818.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48072,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {692.5, 365.5, 732.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48080,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {733.5, 365.5, 773.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48083,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {774.5, 365.5, 814.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48120,
             1,
             {0.09197790689623336, -0.22451994091580502, 0.8718892806037667, 0.8153618906942394},
             {1139.5, 1774.5, 1172.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48121,
             1,
             {0.1260115427668021, -0.22451994091580502, 0.8822892384831981, 0.8153618906942394},
             {1173.5, 1774.5, 1205.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48124,
             1,
             {0.12713415689623336, -0.1772525849335303, 0.9070455306037667, 0.8153618906942394},
             {364.5, 494.5, 397.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {48127,
             1,
             {0.12469275064623336, -0.20088626292466766, 0.9046041243537667, 0.8153618906942393},
             {356.5, 801.5, 389.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48128,
             1,
             {0.11531731790066468, -0.20088626292466766, 0.9188623695993354, 0.8153618906942393},
             {321.5, 801.5, 355.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48130,
             1,
             {0.11517126627123336, -0.20088626292466766, 0.8950826399787667, 0.8153618906942393},
             {287.5, 801.5, 320.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48136,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {254.5, 801.5, 286.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48137,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {221.5, 801.5, 253.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48139,
             1,
             {0.11902738990952733, -0.20088626292466766, 0.9698397975904727, 0.8153618906942393},
             {184.5, 801.5, 220.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48140,
             1,
             {0.12625351966395862, -0.20088626292466766, 1.0006996053360413, 0.8153618906942393},
             {146.5, 801.5, 183.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48141,
             1,
             {0.12469275064623336, -0.20088626292466766, 0.9046041243537667, 0.8153618906942393},
             {112.5, 801.5, 145.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48143,
             1,
             {0.11805082740952733, -0.20088626292466766, 0.9688632350904727, 0.8153618906942393},
             {75.5, 801.5, 111.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48145,
             1,
             {0.12469275064623336, -0.20088626292466766, 0.9046041243537667, 0.8153618906942393},
             {41.5, 801.5, 74.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {48148,
             1,
             {0.08015890417282126, -0.22451994091580502, 1.0018723458271788, 0.8153618906942394},
             {1206.5, 1774.5, 1245.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48149,
             1,
             {0.08098941504338994, -0.22451994091580502, 0.97906917870661, 0.8153618906942394},
             {1246.5, 1774.5, 1284.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48150,
             1,
             {0.08147769629338994, -0.22451994091580502, 0.97955745995661, 0.8153618906942394},
             {1285.5, 1774.5, 1323.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48151,
             1,
             {0.08147769629338994, -0.22451994091580502, 0.97955745995661, 0.8153618906942394},
             {1324.5, 1774.5, 1362.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48152,
             1,
             {0.08147769629338994, -0.1772525849335303, 0.97955745995661, 0.8153618906942394},
             {967.5, 365.5, 1005.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48155,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1920.5, 845.5, 1958.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48156,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1881.5, 845.5, 1919.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48157,
             1,
             {0.08147769629338994, -0.22451994091580502, 0.97955745995661, 0.8153618906942394},
             {1398.5, 1774.5, 1436.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48158,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1808.5, 845.5, 1846.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48159,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1769.5, 845.5, 1807.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48164,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1730.5, 845.5, 1768.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48165,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1691.5, 845.5, 1729.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48167,
             1,
             {0.08098941504338994, -0.20088626292466766, 0.97906917870661, 0.8153618906942393},
             {1652.5, 845.5, 1690.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48169,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1613.5, 845.5, 1651.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48173,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1574.5, 845.5, 1612.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48176,
             1,
             {0.078598135155096, -0.22451994091580502, 0.905776864844904, 0.8153618906942394},
             {1437.5, 1774.5, 1472.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48177,
             1,
             {0.077377432030096, -0.22451994091580502, 0.904556161719904, 0.8153618906942394},
             {1507.5, 1774.5, 1542.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48180,
             1,
             {0.07972074928452733, -0.1772525849335303, 0.9305331569654727, 0.8153618906942394},
             {1006.5, 365.5, 1042.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48184,
             1,
             {0.07825590553452733, -0.20088626292466766, 0.9290683132154727, 0.8153618906942393},
             {1430.5, 845.5, 1466.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48192,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {1394.5, 845.5, 1429.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48193,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {1358.5, 845.5, 1393.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48195,
             1,
             {0.08328476966395862, -0.20088626292466766, 0.9577308553360414, 0.8153618906942393},
             {1320.5, 845.5, 1357.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48196,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {1282.5, 845.5, 1319.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48197,
             1,
             {0.080062978905096, -0.20088626292466766, 0.907241708594904, 0.8153618906942393},
             {1246.5, 845.5, 1281.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48201,
             1,
             {0.07605863990952733, -0.20088626292466766, 0.9268710475904727, 0.8153618906942393},
             {1209.5, 845.5, 1245.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48204,
             1,
             {0.08806949316838994, -0.22451994091580502, 0.98614925683161, 0.8153618906942394},
             {1543.5, 1774.5, 1581.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48205,
             1,
             {0.08147769629338994, -0.22451994091580502, 0.97955745995661, 0.8153618906942394},
             {1582.5, 1774.5, 1620.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48208,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1100.5, 845.5, 1138.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48221,
             1,
             {0.08147769629338994, -0.20088626292466766, 0.97955745995661, 0.8153618906942393},
             {1061.5, 845.5, 1099.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48260,
             1,
             {0.08016106790066468, -0.22451994091580502, 0.8837061195993354, 0.8153618906942394},
             {1621.5, 1774.5, 1655.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48261,
             1,
             {0.1260115427668021, -0.22451994091580502, 0.8822892384831981, 0.8153618906942394},
             {1656.5, 1774.5, 1688.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48264,
             1,
             {0.11971184915066468, -0.1772525849335303, 0.9232569008493354, 0.8153618906942394},
             {1079.5, 365.5, 1113.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48267,
             1,
             {0.12469275064623336, -0.20088626292466766, 0.9046041243537667, 0.8153618906942393},
             {920.5, 845.5, 953.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48268,
             1,
             {0.11531731790066468, -0.20088626292466766, 0.9188623695993354, 0.8153618906942393},
             {885.5, 845.5, 919.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48270,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {852.5, 845.5, 884.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48276,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {819.5, 845.5, 851.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48277,
             1,
             {0.1264998240168021, -0.20088626292466766, 0.8827775197331981, 0.8153618906942393},
             {786.5, 845.5, 818.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48279,
             1,
             {0.12171293678452733, -0.20088626292466766, 0.9725253444654727, 0.8153618906942393},
             {749.5, 845.5, 785.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48281,
             1,
             {0.12298376627123336, -0.22451994091580502, 0.9028951399787667, 0.8153618906942394},
             {1689.5, 1774.5, 1722.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48282,
             1,
             {0.117416494530096, -0.20088626292466766, 0.944595224219904, 0.8153618906942393},
             {679.5, 845.5, 714.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48288,
             1,
             {0.079574697655096, -0.22451994091580502, 0.906753427344904, 0.8153618906942394},
             {1793.5, 1774.5, 1828.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48289,
             1,
             {0.077377432030096, -0.22451994091580502, 0.904556161719904, 0.8153618906942394},
             {1829.5, 1774.5, 1864.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48292,
             1,
             {0.07972074928452733, -0.1772525849335303, 0.9305331569654727, 0.8153618906942394},
             {1114.5, 365.5, 1150.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48295,
             1,
             {0.07605863990952733, -0.20088626292466766, 0.9268710475904727, 0.8153618906942393},
             {533.5, 845.5, 569.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48296,
             1,
             {0.07825590553452733, -0.20088626292466766, 0.9290683132154727, 0.8153618906942393},
             {496.5, 845.5, 532.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48304,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {460.5, 845.5, 495.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48305,
             1,
             {0.077377432030096, -0.20088626292466766, 0.904556161719904, 0.8153618906942393},
             {424.5, 845.5, 459.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48307,
             1,
             {0.07390933691838994, -0.20088626292466766, 0.97198910058161, 0.8153618906942393},
             {385.5, 845.5, 423.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48308,
             1,
             {0.08230820716395862, -0.20088626292466766, 0.9567542928360414, 0.8153618906942393},
             {347.5, 845.5, 384.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48309,
             1,
             {0.081527822655096, -0.20088626292466766, 0.908706552344904, 0.8153618906942393},
             {311.5, 845.5, 346.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48316,
             1,
             {0.08016106790066468, -0.22451994091580502, 0.8837061195993354, 0.8153618906942394},
             {1865.5, 1774.5, 1899.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48317,
             1,
             {0.1260115427668021, -0.22451994091580502, 0.8822892384831981, 0.8153618906942394},
             {1900.5, 1774.5, 1932.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48320,
             1,
             {0.11727044290066468, -0.20088626292466766, 0.9208154945993354, 0.8153618906942393},
             {210.5, 845.5, 244.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48324,
             1,
             {0.11531731790066468, -0.20088626292466766, 0.9188623695993354, 0.8153618906942393},
             {175.5, 845.5, 209.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48333,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {142.5, 845.5, 174.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48335,
             1,
             {0.11658598365952733, -0.20088626292466766, 0.9673983913404727, 0.8153618906942393},
             {105.5, 845.5, 141.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48336,
             1,
             {0.10183945716395862, -0.20088626292466766, 0.9762855428360414, 0.8153618906942393},
             {67.5, 845.5, 104.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48337,
             1,
             {0.12371618814623336, -0.22451994091580502, 0.9036275618537667, 0.8153618906942394},
             {1933.5, 1774.5, 1966.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48341,
             1,
             {0.12566931314623336, -0.20088626292466766, 0.9055806868537667, 0.8153618906942393},
             {991.5, 845.5, 1024.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {48344,
             1,
             {0.079086416405096, -0.22451994091580502, 0.906265146094904, 0.8153618906942394},
             {1967.5, 1774.5, 2002.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48348,
             1,
             {0.07972074928452733, -0.1772525849335303, 0.9305331569654727, 0.8153618906942394},
             {1151.5, 365.5, 1187.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48372,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {1015.5, 72.5, 1055.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {48373,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1899.5, 889.5, 1939.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48374,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {2003.5, 1774.5, 2043.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {48376,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {901.5, 108.5, 941.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48380,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1783.5, 889.5, 1823.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48388,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1347.5, 365.5, 1387.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48389,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1708.5, 889.5, 1748.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48391,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1388.5, 365.5, 1428.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48393,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {0.5, 1729.5, 40.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48400,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {108.5, 1729.5, 150.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48404,
             1,
             {0.013702527811115212, -0.20088626292466766, 1.0063170034388849, 0.8153618906942393},
             {1558.5, 889.5, 1600.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48420,
             1,
             {0.013702527811115212, -0.22451994091580502, 1.0063170034388849, 0.8153618906942394},
             {190.5, 1729.5, 232.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48428,
             1,
             {0.0033526962933899535, -0.22451994091580502, 0.90143245995661, 0.8153618906942394},
             {233.5, 1729.5, 271.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48448,
             1,
             {0.019123747922821267, -0.22451994091580502, 0.9408371895771788, 0.8153618906942394},
             {272.5, 1729.5, 311.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48456,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {312.5, 1729.5, 349.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48457,
             1,
             {0.031136764909527326, -0.22451994091580502, 0.8819491725904727, 0.8153618906942394},
             {350.5, 1729.5, 386.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48460,
             1,
             {0.03396836341395864, -0.20088626292466766, 0.9084144490860414, 0.8153618906942393},
             {1339.5, 889.5, 1376.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48464,
             1,
             {0.03225937903895864, -0.20088626292466766, 0.9067054647110414, 0.8153618906942393},
             {1301.5, 889.5, 1338.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48472,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1264.5, 889.5, 1300.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48473,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1227.5, 889.5, 1263.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48484,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {974.5, 72.5, 1014.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {48488,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {983.5, 108.5, 1023.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48512,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1112.5, 889.5, 1152.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48513,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1071.5, 889.5, 1111.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48516,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1509.5, 365.5, 1549.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48519,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1550.5, 365.5, 1590.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48520,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {964.5, 889.5, 1004.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48521,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {387.5, 1729.5, 427.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48522,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {890.5, 889.5, 930.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48528,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1628.5, 365.5, 1668.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48529,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {816.5, 889.5, 856.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48531,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {775.5, 889.5, 815.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48533,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {428.5, 1729.5, 468.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48537,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {695.5, 889.5, 735.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48538,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {654.5, 889.5, 694.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48540,
             1,
             {0.01834336341395864, -0.22451994091580502, 0.8927894490860414, 0.8153618906942394},
             {540.5, 1729.5, 577.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48548,
             1,
             {0.04226914466395864, -0.20088626292466766, 0.9167152303360414, 0.8153618906942393},
             {576.5, 889.5, 613.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48560,
             1,
             {0.03914327917282127, -0.22451994091580502, 0.9608567208271788, 0.8153618906942394},
             {578.5, 1729.5, 617.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48568,
             1,
             {0.0033526962933899535, -0.22451994091580502, 0.90143245995661, 0.8153618906942394},
             {618.5, 1729.5, 656.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48596,
             1,
             {0.02127305091395864, -0.22451994091580502, 0.8957191365860414, 0.8153618906942394},
             {657.5, 1729.5, 694.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48597,
             1,
             {0.041146530534527326, -0.22451994091580502, 0.8919589382154727, 0.8153618906942394},
             {695.5, 1729.5, 731.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48600,
             1,
             {0.04568711341395864, -0.20088626292466766, 0.9201331990860414, 0.8153618906942393},
             {382.5, 889.5, 419.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48604,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {344.5, 889.5, 381.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48617,
             1,
             {0.03543320716395864, -0.20088626292466766, 0.9098792928360414, 0.8153618906942393},
             {306.5, 889.5, 343.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48624,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {265.5, 889.5, 305.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48628,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1826.5, 365.5, 1866.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48632,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {190.5, 889.5, 230.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48640,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1867.5, 365.5, 1907.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48643,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1908.5, 365.5, 1948.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {48645,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {73.5, 889.5, 113.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48652,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1790.5, 73.5, 1830.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {48653,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1824.5, 889.5, 1864.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {48656,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1024.5, 108.5, 1064.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48660,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1966.5, 933.5, 2006.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48668,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 322.5, 122.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48669,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1886.5, 933.5, 1926.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48671,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 322.5, 163.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48708,
             1,
             {0.09246618814623336, -0.22451994091580502, 0.8723775618537667, 0.8153618906942394},
             {771.5, 1729.5, 804.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48709,
             1,
             {0.1260115427668021, -0.22451994091580502, 0.8822892384831981, 0.8153618906942394},
             {805.5, 1729.5, 837.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48712,
             1,
             {0.12664587564623336, -0.1772525849335303, 0.9065572493537667, 0.8153618906942394},
             {164.5, 322.5, 197.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48716,
             1,
             {0.11531731790066468, -0.20088626292466766, 0.9188623695993354, 0.8153618906942393},
             {1705.5, 933.5, 1739.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48718,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {1672.5, 933.5, 1704.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48724,
             1,
             {0.1260115427668021, -0.20088626292466766, 0.8822892384831981, 0.8153618906942393},
             {1639.5, 933.5, 1671.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48725,
             1,
             {0.1264998240168021, -0.20088626292466766, 0.8827775197331981, 0.8153618906942393},
             {1606.5, 933.5, 1638.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48727,
             1,
             {0.11951567115952733, -0.20088626292466766, 0.9703280788404727, 0.8153618906942393},
             {1569.5, 933.5, 1605.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48729,
             1,
             {0.12200720377123336, -0.20088626292466766, 0.9019185774787667, 0.8153618906942393},
             {1535.5, 933.5, 1568.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48730,
             1,
             {0.11658598365952733, -0.20088626292466766, 0.9673983913404727, 0.8153618906942393},
             {1498.5, 933.5, 1534.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48731,
             1,
             {0.12024809303452733, -0.20088626292466766, 0.9710605007154727, 0.8153618906942393},
             {1461.5, 933.5, 1497.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48736,
             1,
             {0.049445148056683895, -0.22451994091580502, 1.0184259456933162, 0.8153618906942394},
             {916.5, 1729.5, 957.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48737,
             1,
             {0.05393776830225259, -0.22451994091580502, 0.9992848879477475, 0.8153618906942394},
             {958.5, 1729.5, 998.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48740,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1345.5, 933.5, 1385.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48744,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1304.5, 933.5, 1344.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48746,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1263.5, 933.5, 1303.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48752,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1222.5, 933.5, 1262.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48753,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1181.5, 933.5, 1221.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48755,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1140.5, 933.5, 1180.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48756,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1099.5, 933.5, 1139.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48757,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1058.5, 933.5, 1098.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48763,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {1017.5, 933.5, 1057.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48764,
             1,
             {0.04471055091395864, -0.22451994091580502, 0.9191566365860414, 0.8153618906942394},
             {999.5, 1729.5, 1036.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48765,
             1,
             {0.06311918678452733, -0.22451994091580502, 0.9139315944654727, 0.8153618906942394},
             {1037.5, 1729.5, 1073.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48768,
             1,
             {0.053889805668389947, -0.1772525849335303, 0.95196956933161, 0.8153618906942394},
             {198.5, 322.5, 236.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48772,
             1,
             {0.06424180091395863, -0.20088626292466766, 0.9386878865860414, 0.8153618906942393},
             {872.5, 933.5, 909.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48780,
             1,
             {0.06311918678452733, -0.20088626292466766, 0.9139315944654727, 0.8153618906942393},
             {835.5, 933.5, 871.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48781,
             1,
             {0.06311918678452733, -0.20088626292466766, 0.9139315944654727, 0.8153618906942393},
             {798.5, 933.5, 834.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48783,
             1,
             {0.061946446293389947, -0.20088626292466766, 0.96002620995661, 0.8153618906942393},
             {759.5, 933.5, 797.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48784,
             1,
             {0.060969883793389947, -0.20088626292466766, 0.95904964745661, 0.8153618906942393},
             {720.5, 933.5, 758.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48785,
             1,
             {0.06311918678452733, -0.20088626292466766, 0.9139315944654727, 0.8153618906942393},
             {683.5, 933.5, 719.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48792,
             1,
             {0.046515460556683895, -0.22451994091580502, 1.0154962581933162, 0.8153618906942394},
             {1074.5, 1729.5, 1115.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48793,
             1,
             {0.05393776830225259, -0.22451994091580502, 0.9992848879477475, 0.8153618906942394},
             {1116.5, 1729.5, 1156.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48808,
             1,
             {0.05393776830225259, -0.20088626292466766, 0.9992848879477475, 0.8153618906942393},
             {568.5, 933.5, 608.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48848,
             1,
             {0.066879385155096, -0.22451994091580502, 0.894058114844904, 0.8153618906942394},
             {1229.5, 1729.5, 1264.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48849,
             1,
             {0.072982900780096, -0.22451994091580502, 0.900161630469904, 0.8153618906942394},
             {1265.5, 1729.5, 1300.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48852,
             1,
             {0.07874418678452733, -0.1772525849335303, 0.9295565944654727, 0.8153618906942394},
             {552.5, 322.5, 588.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48855,
             1,
             {0.07166410865952733, -0.20088626292466766, 0.9224765163404727, 0.8153618906942393},
             {415.5, 933.5, 451.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48856,
             1,
             {0.07410551490952733, -0.20088626292466766, 0.9249179225904727, 0.8153618906942393},
             {378.5, 933.5, 414.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48864,
             1,
             {0.072982900780096, -0.20088626292466766, 0.900161630469904, 0.8153618906942393},
             {342.5, 933.5, 377.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48867,
             1,
             {0.07561832129338994, -0.20088626292466766, 0.97369808495661, 0.8153618906942393},
             {303.5, 933.5, 341.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48868,
             1,
             {0.07195621191838994, -0.20088626292466766, 0.97003597558161, 0.8153618906942393},
             {264.5, 933.5, 302.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48869,
             1,
             {0.079086416405096, -0.20088626292466766, 0.906265146094904, 0.8153618906942393},
             {228.5, 933.5, 263.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48876,
             1,
             {0.04666367591395864, -0.22451994091580502, 0.9211097615860414, 0.8153618906942394},
             {1301.5, 1729.5, 1338.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48897,
             1,
             {0.06311918678452733, -0.20088626292466766, 0.9139315944654727, 0.8153618906942393},
             {152.5, 933.5, 188.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48904,
             1,
             {0.066879385155096, -0.22451994091580502, 0.894058114844904, 0.8153618906942394},
             {1339.5, 1729.5, 1374.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48905,
             1,
             {0.072982900780096, -0.22451994091580502, 0.900161630469904, 0.8153618906942394},
             {1375.5, 1729.5, 1410.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {48920,
             1,
             {0.072982900780096, -0.20088626292466766, 0.900161630469904, 0.8153618906942393},
             {36.5, 933.5, 71.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48921,
             1,
             {0.072982900780096, -0.20088626292466766, 0.900161630469904, 0.8153618906942393},
             {0.5, 933.5, 35.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48923,
             1,
             {0.07293277441838994, -0.20088626292466766, 0.97101253808161, 0.8153618906942393},
             {113.5, 933.5, 151.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {48924,
             1,
             {0.08133164466395862, -0.20088626292466766, 0.9557777303360414, 0.8153618906942393},
             {1990.5, 977.5, 2027.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {48925,
             1,
             {0.079086416405096, -0.20088626292466766, 0.906265146094904, 0.8153618906942393},
             {1954.5, 977.5, 1989.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {48960,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1612.5, 73.5, 1652.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {48961,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1875.5, 977.5, 1915.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {48964,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1147.5, 108.5, 1187.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {48968,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {712.5, 322.5, 752.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48976,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {794.5, 322.5, 834.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48977,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {835.5, 322.5, 875.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {48981,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {993.5, 322.5, 1033.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49044,
             1,
             {0.02859726966395864, -0.22451994091580502, 0.9030433553360414, 0.8153618906942394},
             {1411.5, 1729.5, 1448.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49072,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {139.5, 32.5, 179.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {49093,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1116.5, 322.5, 1156.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49100,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1528.5, 977.5, 1568.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49101,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1487.5, 977.5, 1527.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49104,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1157.5, 322.5, 1197.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49108,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1410.5, 977.5, 1450.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49116,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1198.5, 322.5, 1238.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49119,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1333.5, 977.5, 1373.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49121,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1277.5, 322.5, 1317.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49212,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1255.5, 977.5, 1295.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49233,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1318.5, 322.5, 1358.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {49240,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {831.5, 33.5, 871.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {49244,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1393.5, 108.5, 1433.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {49248,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {239.5, 279.5, 279.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49256,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 279.5, 40.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49257,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 279.5, 81.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49296,
             1,
             {0.070541494530096, -0.22451994091580502, 0.897720224219904, 0.8153618906942394},
             {1449.5, 1729.5, 1484.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49297,
             1,
             {0.08089348977566468, -0.22451994091580502, 0.8844385414743354, 0.8153618906942394},
             {1485.5, 1729.5, 1519.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49300,
             1,
             {0.080551260155096, -0.1772525849335303, 0.907729989844904, 0.8153618906942394},
             {203.5, 279.5, 238.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49304,
             1,
             {0.07019926490952733, -0.20088626292466766, 0.9210116725904727, 0.8153618906942393},
             {876.5, 977.5, 912.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49312,
             1,
             {0.07991692727566468, -0.20088626292466766, 0.8834619789743354, 0.8153618906942393},
             {841.5, 977.5, 875.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49313,
             1,
             {0.08089348977566468, -0.20088626292466766, 0.8844385414743354, 0.8153618906942393},
             {806.5, 977.5, 840.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49315,
             1,
             {0.07049136816838994, -0.20088626292466766, 0.96857113183161, 0.8153618906942393},
             {767.5, 977.5, 805.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49317,
             1,
             {0.072982900780096, -0.20088626292466766, 0.900161630469904, 0.8153618906942393},
             {731.5, 977.5, 766.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49324,
             1,
             {0.012725965311115212, -0.22451994091580502, 1.0053404409388849, 0.8153618906942394},
             {1520.5, 1729.5, 1562.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49325,
             1,
             {0.0189275699316839, -0.22451994091580502, 0.9879083675683162, 0.8153618906942394},
             {1563.5, 1729.5, 1604.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49327,
             1,
             {0.0189275699316839, -0.22451994091580502, 0.9879083675683162, 0.8153618906942394},
             {1605.5, 1729.5, 1646.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49328,
             1,
             {0.0189275699316839, -0.1772525849335303, 0.9879083675683162, 0.8153618906942394},
             {321.5, 279.5, 362.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49331,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {532.5, 977.5, 573.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49332,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {490.5, 977.5, 531.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49333,
             1,
             {0.0199041324316839, -0.22451994091580502, 0.9888849300683162, 0.8153618906942394},
             {1647.5, 1729.5, 1688.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49334,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {408.5, 977.5, 449.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49340,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {366.5, 977.5, 407.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49341,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {324.5, 977.5, 365.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49343,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {282.5, 977.5, 323.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49344,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {240.5, 977.5, 281.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49345,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {198.5, 977.5, 239.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49349,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {156.5, 977.5, 197.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49352,
             1,
             {0.03250351966395864, -0.22451994091580502, 0.9069496053360414, 0.8153618906942394},
             {1689.5, 1729.5, 1726.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49353,
             1,
             {0.02908555091395864, -0.22451994091580502, 0.9035316365860414, 0.8153618906942394},
             {1727.5, 1729.5, 1764.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49356,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {1839.5, 494.5, 1877.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {49360,
             1,
             {0.024837071293389953, -0.20088626292466766, 0.92291683495661, 0.8153618906942393},
             {0.5, 977.5, 38.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49368,
             1,
             {0.02908555091395864, -0.20088626292466766, 0.9035316365860414, 0.8153618906942393},
             {39.5, 977.5, 76.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49369,
             1,
             {0.02908555091395864, -0.20088626292466766, 0.9035316365860414, 0.8153618906942393},
             {77.5, 977.5, 114.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {49371,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {1978.5, 1021.5, 2017.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49372,
             1,
             {0.025129174552252585, -0.20088626292466766, 0.9704762941977475, 0.8153618906942393},
             {1937.5, 1021.5, 1977.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49373,
             1,
             {0.02273789466395864, -0.20088626292466766, 0.8971839803360414, 0.8153618906942393},
             {1899.5, 1021.5, 1936.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49380,
             1,
             {0.012725965311115212, -0.22451994091580502, 1.0053404409388849, 0.8153618906942394},
             {1765.5, 1729.5, 1807.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49381,
             1,
             {0.0189275699316839, -0.22451994091580502, 0.9879083675683162, 0.8153618906942394},
             {1808.5, 1729.5, 1849.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49384,
             1,
             {0.0189275699316839, -0.1772525849335303, 0.9879083675683162, 0.8153618906942394},
             {766.5, 279.5, 807.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49388,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {1740.5, 1021.5, 1781.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49396,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1698.5, 1021.5, 1739.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49397,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1656.5, 1021.5, 1697.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49399,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1614.5, 1021.5, 1655.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49401,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1572.5, 1021.5, 1613.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49408,
             1,
             {0.02835312903895864, -0.22451994091580502, 0.9027992147110414, 0.8153618906942394},
             {1850.5, 1729.5, 1887.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49412,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {890.5, 279.5, 928.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49416,
             1,
             {0.019954258793389953, -0.20088626292466766, 0.91803402245661, 0.8153618906942393},
             {1453.5, 1021.5, 1491.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49424,
             1,
             {0.01736680091395864, -0.20088626292466766, 0.8918128865860414, 0.8153618906942393},
             {1415.5, 1021.5, 1452.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49429,
             1,
             {0.02273789466395864, -0.20088626292466766, 0.8971839803360414, 0.8153618906942393},
             {1377.5, 1021.5, 1414.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49436,
             1,
             {0.01736680091395864, -0.22451994091580502, 0.8918128865860414, 0.8153618906942394},
             {1963.5, 1729.5, 2000.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {49437,
             1,
             {0.030648483659527326, -0.22451994091580502, 0.8814608913404727, 0.8153618906942394},
             {0.5, 1684.5, 36.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49438,
             1,
             {0.030648483659527326, -0.22451994091580502, 0.8814608913404727, 0.8153618906942394},
             {37.5, 1684.5, 73.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49439,
             1,
             {0.03606754004338995, -0.22451994091580502, 0.93414730370661, 0.8153618906942394},
             {112.5, 1684.5, 150.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49440,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {1011.5, 279.5, 1048.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49443,
             1,
             {0.040169968034527326, -0.20088626292466766, 0.8909823757154727, 0.8153618906942393},
             {1157.5, 1021.5, 1193.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49444,
             1,
             {0.02981797278895864, -0.20088626292466766, 0.9042640584610414, 0.8153618906942393},
             {1119.5, 1021.5, 1156.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49446,
             1,
             {0.028451218034527326, -0.20088626292466766, 0.8792636257154727, 0.8153618906942393},
             {1082.5, 1021.5, 1118.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49447,
             1,
             {0.028451218034527326, -0.20088626292466766, 0.8792636257154727, 0.8153618906942393},
             {1045.5, 1021.5, 1081.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49452,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {1008.5, 1021.5, 1044.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49453,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {971.5, 1021.5, 1007.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49455,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {931.5, 1021.5, 970.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49456,
             1,
             {0.03294167455225258, -0.20088626292466766, 0.9782887941977475, 0.8153618906942393},
             {890.5, 1021.5, 930.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49457,
             1,
             {0.03348008216395864, -0.20088626292466766, 0.9079261678360414, 0.8153618906942393},
             {852.5, 1021.5, 889.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49462,
             1,
             {0.03079453528895864, -0.20088626292466766, 0.9052406209610414, 0.8153618906942393},
             {814.5, 1021.5, 851.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49464,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {151.5, 1684.5, 188.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49465,
             1,
             {0.02957383216395864, -0.22451994091580502, 0.9040199178360414, 0.8153618906942394},
             {189.5, 1684.5, 226.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49468,
             1,
             {0.030940586918389953, -0.1772525849335303, 0.92902035058161, 0.8153618906942394},
             {1169.5, 279.5, 1207.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49472,
             1,
             {0.029964024418389953, -0.20088626292466766, 0.92804378808161, 0.8153618906942393},
             {659.5, 1021.5, 697.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49480,
             1,
             {0.02957383216395864, -0.20088626292466766, 0.9040199178360414, 0.8153618906942393},
             {621.5, 1021.5, 658.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49481,
             1,
             {0.02957383216395864, -0.20088626292466766, 0.9040199178360414, 0.8153618906942393},
             {583.5, 1021.5, 620.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49483,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {543.5, 1021.5, 582.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49484,
             1,
             {0.030256127677252585, -0.20088626292466766, 0.9756032473227475, 0.8153618906942393},
             {502.5, 1021.5, 542.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49485,
             1,
             {0.03323594153895864, -0.20088626292466766, 0.9076820272110414, 0.8153618906942393},
             {464.5, 1021.5, 501.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49492,
             1,
             {0.01736680091395864, -0.22451994091580502, 0.8918128865860414, 0.8153618906942394},
             {227.5, 1684.5, 264.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49493,
             1,
             {0.030648483659527326, -0.22451994091580502, 0.8814608913404727, 0.8153618906942394},
             {265.5, 1684.5, 301.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49496,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {1290.5, 279.5, 1327.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49500,
             1,
             {0.02981797278895864, -0.20088626292466766, 0.9042640584610414, 0.8153618906942393},
             {311.5, 1021.5, 348.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49508,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {274.5, 1021.5, 310.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49509,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {237.5, 1021.5, 273.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49511,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {197.5, 1021.5, 236.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49512,
             1,
             {0.03294167455225258, -0.20088626292466766, 0.9782887941977475, 0.8153618906942393},
             {156.5, 1021.5, 196.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49513,
             1,
             {0.03348008216395864, -0.20088626292466766, 0.9079261678360414, 0.8153618906942393},
             {118.5, 1021.5, 155.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49520,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {302.5, 1684.5, 339.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49524,
             1,
             {0.030940586918389953, -0.1772525849335303, 0.92902035058161, 0.8153618906942394},
             {1328.5, 279.5, 1366.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49528,
             1,
             {0.029964024418389953, -0.20088626292466766, 0.92804378808161, 0.8153618906942393},
             {349.5, 1021.5, 387.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49541,
             1,
             {0.03323594153895864, -0.20088626292466766, 0.9076820272110414, 0.8153618906942393},
             {0.5, 1021.5, 37.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {49548,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {2000.5, 73.5, 2040.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49549,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1923.5, 1065.5, 1963.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49550,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1882.5, 1065.5, 1922.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49552,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.7680945347119646},
             {1311.5, 108.5, 1351.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {49556,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1818.5, 279.5, 1858.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49558,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1900.5, 279.5, 1940.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49564,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1941.5, 279.5, 1981.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49565,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1982.5, 279.5, 2022.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {49567,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 236.5, 163.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49569,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 236.5, 40.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49573,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {41.5, 236.5, 81.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49576,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {340.5, 1684.5, 382.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49577,
             1,
             {0.0260076480566839, -0.22451994091580502, 0.9949884456933162, 0.8153618906942394},
             {383.5, 1684.5, 424.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49580,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {1451.5, 1065.5, 1492.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49584,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {1409.5, 1065.5, 1450.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49597,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {1367.5, 1065.5, 1408.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49604,
             1,
             {0.019954258793389953, -0.22451994091580502, 0.91803402245661, 0.8153618906942394},
             {425.5, 1684.5, 463.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49608,
             1,
             {0.026790196293389953, -0.1772525849335303, 0.92486995995661, 0.8153618906942394},
             {164.5, 236.5, 202.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49612,
             1,
             {0.024348790043389953, -0.20088626292466766, 0.92242855370661, 0.8153618906942393},
             {1252.5, 1065.5, 1290.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49620,
             1,
             {0.01419297278895864, -0.1772525849335303, 0.8886390584610414, 0.8153618906942394},
             {203.5, 236.5, 240.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49623,
             1,
             {0.021076872922821267, -0.20088626292466766, 0.9427903145771788, 0.8153618906942393},
             {1171.5, 1065.5, 1210.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49624,
             1,
             {0.015021319931683898, -0.22451994091580502, 0.9840021175683162, 0.8153618906942394},
             {464.5, 1684.5, 505.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49632,
             1,
             {0.040658249284527326, -0.22451994091580502, 0.8914706569654727, 0.8153618906942394},
             {543.5, 1684.5, 579.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49636,
             1,
             {0.053109421159527326, -0.1772525849335303, 0.9039218288404727, 0.8153618906942394},
             {241.5, 236.5, 277.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49640,
             1,
             {0.051400436784527326, -0.20088626292466766, 0.9022128444654727, 0.8153618906942393},
             {1016.5, 1065.5, 1052.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49648,
             1,
             {0.05027782265509601, -0.1772525849335303, 0.877456552344904, 0.8153618906942394},
             {278.5, 236.5, 313.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49649,
             1,
             {0.05027782265509601, -0.20088626292466766, 0.877456552344904, 0.8153618906942393},
             {938.5, 1065.5, 973.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49651,
             1,
             {0.04436832129338995, -0.20088626292466766, 0.94244808495661, 0.8153618906942393},
             {899.5, 1065.5, 937.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49660,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {298.5, 32.5, 338.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {49661,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {817.5, 1065.5, 857.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49664,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {123.5, 67.5, 163.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49668,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {389.5, 236.5, 429.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49676,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {430.5, 236.5, 470.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49677,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {471.5, 236.5, 511.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49679,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {512.5, 236.5, 552.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49681,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {553.5, 236.5, 593.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49688,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {545.5, 1065.5, 585.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49689,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {504.5, 1065.5, 544.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {49692,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {82.5, 67.5, 122.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49695,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {631.5, 236.5, 671.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49696,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {672.5, 236.5, 712.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49704,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {713.5, 236.5, 753.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49705,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {754.5, 236.5, 794.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49707,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {795.5, 236.5, 835.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49709,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {836.5, 236.5, 876.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49711,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {877.5, 236.5, 917.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49713,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {918.5, 236.5, 958.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49714,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {959.5, 236.5, 999.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49716,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {619.5, 1684.5, 656.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49736,
             1,
             {0.05110616979782127, -0.22451994091580502, 0.9728196114521788, 0.8153618906942394},
             {657.5, 1684.5, 696.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49744,
             1,
             {0.0075030869183899535, -0.22451994091580502, 0.90558285058161, 0.8153618906942394},
             {697.5, 1684.5, 735.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49745,
             1,
             {0.0038409775433899535, -0.22451994091580502, 0.90192074120661, 0.8153618906942394},
             {736.5, 1684.5, 774.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49748,
             1,
             {0.010921055668389953, -0.20088626292466766, 0.90900081933161, 0.8153618906942393},
             {1970.5, 1109.5, 2008.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49752,
             1,
             {0.004963591672821267, -0.20088626292466766, 0.9266770333271788, 0.8153618906942393},
             {1930.5, 1109.5, 1969.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49760,
             1,
             {0.0038409775433899535, -0.20088626292466766, 0.90192074120661, 0.8153618906942393},
             {1891.5, 1109.5, 1929.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49765,
             1,
             {0.007991368168389953, -0.20088626292466766, 0.90607113183161, 0.8153618906942393},
             {1852.5, 1109.5, 1890.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49772,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {860.5, 1684.5, 897.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49773,
             1,
             {0.031136764909527326, -0.22451994091580502, 0.8819491725904727, 0.8153618906942394},
             {898.5, 1684.5, 934.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49776,
             1,
             {0.03396836341395864, -0.20088626292466766, 0.9084144490860414, 0.8153618906942393},
             {1737.5, 1109.5, 1774.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49780,
             1,
             {0.03225937903895864, -0.20088626292466766, 0.9067054647110414, 0.8153618906942393},
             {1699.5, 1109.5, 1736.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49788,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1662.5, 1109.5, 1698.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49789,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1625.5, 1109.5, 1661.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49791,
             1,
             {0.03914327917282127, -0.20088626292466766, 0.9608567208271788, 0.8153618906942393},
             {1585.5, 1109.5, 1624.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49793,
             1,
             {0.03494492591395864, -0.20088626292466766, 0.9093910115860414, 0.8153618906942393},
             {1547.5, 1109.5, 1584.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49800,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1506.5, 1109.5, 1546.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49801,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1465.5, 1109.5, 1505.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49808,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1165.5, 236.5, 1205.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49816,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1206.5, 236.5, 1246.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49819,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1247.5, 236.5, 1287.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49821,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1288.5, 236.5, 1328.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49828,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1571.5, 73.5, 1611.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49829,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1226.5, 1109.5, 1266.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49832,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {41.5, 67.5, 81.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {49836,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1408.5, 236.5, 1448.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49837,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1103.5, 1109.5, 1143.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49844,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1449.5, 236.5, 1489.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49845,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1490.5, 236.5, 1530.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49847,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1612.5, 236.5, 1652.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49849,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1653.5, 236.5, 1693.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49884,
             1,
             {0.01687851966395864, -0.22451994091580502, 0.8913246053360414, 0.8153618906942394},
             {935.5, 1684.5, 972.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49885,
             1,
             {0.030648483659527326, -0.22451994091580502, 0.8814608913404727, 0.8153618906942394},
             {973.5, 1684.5, 1009.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49888,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {1776.5, 236.5, 1813.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49891,
             1,
             {0.040169968034527326, -0.20088626292466766, 0.8909823757154727, 0.8153618906942393},
             {790.5, 1109.5, 826.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49892,
             1,
             {0.02981797278895864, -0.20088626292466766, 0.9042640584610414, 0.8153618906942393},
             {752.5, 1109.5, 789.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49899,
             1,
             {0.03445664466395864, -0.20088626292466766, 0.9089027303360414, 0.8153618906942393},
             {714.5, 1109.5, 751.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49900,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {677.5, 1109.5, 713.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49901,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {640.5, 1109.5, 676.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49903,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {600.5, 1109.5, 639.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49905,
             1,
             {0.03348008216395864, -0.20088626292466766, 0.9079261678360414, 0.8153618906942393},
             {562.5, 1109.5, 599.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49910,
             1,
             {0.03079453528895864, -0.20088626292466766, 0.9052406209610414, 0.8153618906942393},
             {524.5, 1109.5, 561.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49912,
             1,
             {0.014923230936115212, -0.22451994091580502, 1.0075377065638849, 0.8153618906942394},
             {1010.5, 1684.5, 1052.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49913,
             1,
             {0.012824054306683898, -0.22451994091580502, 0.9818048519433162, 0.8153618906942394},
             {1053.5, 1684.5, 1094.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49915,
             1,
             {0.012824054306683898, -0.22451994091580502, 0.9818048519433162, 0.8153618906942394},
             {1095.5, 1684.5, 1136.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49916,
             1,
             {0.012824054306683898, -0.1772525849335303, 0.9818048519433162, 0.8153618906942394},
             {1893.5, 236.5, 1934.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49920,
             1,
             {0.013556476181683898, -0.20088626292466766, 0.9825372738183162, 0.8153618906942393},
             {328.5, 1109.5, 369.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49928,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {286.5, 1109.5, 327.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49929,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {244.5, 1109.5, 285.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49932,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {202.5, 1109.5, 243.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49933,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {160.5, 1109.5, 201.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49939,
             1,
             {0.013556476181683898, -0.20088626292466766, 0.9825372738183162, 0.8153618906942393},
             {118.5, 1109.5, 159.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49940,
             1,
             {0.008967930668389953, -0.22451994091580502, 0.90704769433161, 0.8153618906942394},
             {1137.5, 1684.5, 1175.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49941,
             1,
             {0.009944493168389953, -0.22451994091580502, 0.90802425683161, 0.8153618906942394},
             {1214.5, 1684.5, 1252.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {49944,
             1,
             {0.012287810422821267, -0.1772525849335303, 0.9340012520771788, 0.8153618906942394},
             {1531.5, 236.5, 1570.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {49948,
             1,
             {0.011799529172821267, -0.20088626292466766, 0.9335129708271788, 0.8153618906942393},
             {0.5, 1109.5, 39.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {49956,
             1,
             {0.009944493168389953, -0.20088626292466766, 0.90802425683161, 0.8153618906942393},
             {1968.5, 1153.5, 2006.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {49957,
             1,
             {0.009456211918389953, -0.20088626292466766, 0.90753597558161, 0.8153618906942393},
             {1929.5, 1153.5, 1967.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {49960,
             1,
             {0.014875268302252585, -0.20088626292466766, 0.9602223879477475, 0.8153618906942393},
             {1888.5, 1153.5, 1928.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {49961,
             1,
             {0.013606602543389953, -0.20088626292466766, 0.91168636620661, 0.8153618906942393},
             {1849.5, 1153.5, 1887.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {49989,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {1807.5, 1153.5, 1848.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50024,
             1,
             {0.022591843034527326, -0.22451994091580502, 0.8734042507154727, 0.8153618906942394},
             {1253.5, 1684.5, 1289.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50025,
             1,
             {0.00564805091395864, -0.22451994091580502, 0.8800941365860414, 0.8153618906942394},
             {1290.5, 1684.5, 1327.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50028,
             1,
             {0.0062823837933899535, -0.1772525849335303, 0.90436214745661, 0.8153618906942394},
             {1935.5, 236.5, 1973.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {50032,
             1,
             {0.008479649418389953, -0.20088626292466766, 0.90655941308161, 0.8153618906942393},
             {1655.5, 1153.5, 1693.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50034,
             1,
             {0.00833359778895864, -0.20088626292466766, 0.8827796834610414, 0.8153618906942393},
             {1617.5, 1153.5, 1654.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50040,
             1,
             {0.00467148841395864, -0.20088626292466766, 0.8791175740860414, 0.8153618906942393},
             {1579.5, 1153.5, 1616.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50041,
             1,
             {0.00662461341395864, -0.20088626292466766, 0.8810706990860414, 0.8153618906942393},
             {1541.5, 1153.5, 1578.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50044,
             1,
             {0.008771752677252585, -0.20088626292466766, 0.9541188723227475, 0.8153618906942393},
             {1500.5, 1153.5, 1540.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50045,
             1,
             {0.0033526962933899535, -0.20088626292466766, 0.90143245995661, 0.8153618906942393},
             {1461.5, 1153.5, 1499.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50052,
             1,
             {0.0070148056683899535, -0.22451994091580502, 0.90509456933161, 0.8153618906942394},
             {1328.5, 1684.5, 1366.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50056,
             1,
             {0.012287810422821267, -0.1772525849335303, 0.9340012520771788, 0.8153618906942394},
             {1974.5, 236.5, 2013.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {50060,
             1,
             {0.011799529172821267, -0.20088626292466766, 0.9335129708271788, 0.8153618906942393},
             {1348.5, 1153.5, 1387.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50112,
             1,
             {0.012287810422821267, -0.1772525849335303, 0.9340012520771788, 0.8153618906942394},
             {0.5, 193.5, 39.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50136,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {1180.5, 72.5, 1220.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {50137,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1234.5, 1153.5, 1274.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50140,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {737.5, 108.5, 777.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {50143,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {118.5, 193.5, 158.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50144,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {159.5, 193.5, 199.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50146,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {364.5, 193.5, 404.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50152,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {528.5, 193.5, 568.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50153,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {733.5, 193.5, 773.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50157,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {848.5, 193.5, 888.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50164,
             1,
             {0.0009091263155465261, -0.22451994091580502, 1.0171572799344535, 0.8153618906942394},
             {1367.5, 1684.5, 1410.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50165,
             1,
             {0.0184392886816839, -0.22451994091580502, 0.9874200863183162, 0.8153618906942394},
             {1411.5, 1684.5, 1452.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50168,
             1,
             {0.0184392886816839, -0.1772525849335303, 0.9874200863183162, 0.8153618906942394},
             {965.5, 193.5, 1006.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50184,
             1,
             {0.0184392886816839, -0.22451994091580502, 0.9874200863183162, 0.8153618906942394},
             {1453.5, 1684.5, 1494.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50192,
             1,
             {0.018001133793389953, -0.22451994091580502, 0.91608089745661, 0.8153618906942394},
             {1532.5, 1684.5, 1570.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50212,
             1,
             {0.019758080802252585, -0.22451994091580502, 0.9651052004477475, 0.8153618906942394},
             {1571.5, 1684.5, 1611.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50220,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {1612.5, 1684.5, 1649.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50224,
             1,
             {0.053109421159527326, -0.1772525849335303, 0.9039218288404727, 0.8153618906942394},
             {1007.5, 193.5, 1043.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50228,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {620.5, 1153.5, 656.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50236,
             1,
             {0.05027782265509601, -0.1772525849335303, 0.877456552344904, 0.8153618906942394},
             {1044.5, 193.5, 1079.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50237,
             1,
             {0.05027782265509601, -0.20088626292466766, 0.877456552344904, 0.8153618906942393},
             {544.5, 1153.5, 579.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50248,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1653.5, 73.5, 1693.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {50276,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {466.5, 1153.5, 506.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50277,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {425.5, 1153.5, 465.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50280,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {395.5, 108.5, 435.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {50284,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1157.5, 193.5, 1197.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50292,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1198.5, 193.5, 1238.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50293,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1239.5, 193.5, 1279.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50297,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1280.5, 193.5, 1320.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {50304,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {1650.5, 1684.5, 1687.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50324,
             1,
             {0.031086638547821267, -0.22451994091580502, 0.9528000802021788, 0.8153618906942394},
             {1688.5, 1684.5, 1727.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50332,
             1,
             {0.008967930668389953, -0.22451994091580502, 0.90704769433161, 0.8153618906942394},
             {1728.5, 1684.5, 1766.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50360,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {1842.5, 1684.5, 1879.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50364,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {41.5, 1153.5, 78.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {50409,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1463.5, 451.5, 1503.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {50416,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {180.5, 32.5, 220.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {50417,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1980.5, 1197.5, 2020.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50420,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1825.5, 109.5, 1865.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {50424,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {42.5, 150.5, 82.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50426,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {83.5, 150.5, 123.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50431,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {160.5, 150.5, 200.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50432,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {278.5, 150.5, 318.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50433,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {319.5, 150.5, 359.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50444,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {1880.5, 1684.5, 1917.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50448,
             1,
             {0.04275742591395864, -0.20088626292466766, 0.9172035115860414, 0.8153618906942393},
             {1675.5, 1197.5, 1712.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50452,
             1,
             {0.051400436784527326, -0.20088626292466766, 0.9022128444654727, 0.8153618906942393},
             {1638.5, 1197.5, 1674.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50460,
             1,
             {0.05027782265509601, -0.1772525849335303, 0.877456552344904, 0.8153618906942394},
             {360.5, 150.5, 395.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50472,
             1,
             {0.025521530534527326, -0.22451994091580502, 0.8763339382154727, 0.8153618906942394},
             {1918.5, 1684.5, 1954.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50473,
             1,
             {0.025033249284527326, -0.22451994091580502, 0.8758456569654727, 0.8153618906942394},
             {1955.5, 1684.5, 1991.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50476,
             1,
             {0.02322617591395864, -0.1772525849335303, 0.8976722615860414, 0.8153618906942394},
             {396.5, 150.5, 433.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50480,
             1,
             {0.02517930091395864, -0.20088626292466766, 0.8996253865860414, 0.8153618906942393},
             {1446.5, 1197.5, 1483.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50488,
             1,
             {0.021615280534527326, -0.20088626292466766, 0.8724276882154727, 0.8153618906942393},
             {1409.5, 1197.5, 1445.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50489,
             1,
             {0.023568405534527326, -0.20088626292466766, 0.8743808132154727, 0.8153618906942393},
             {1372.5, 1197.5, 1408.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50491,
             1,
             {0.024738982297821267, -0.20088626292466766, 0.9464524239521788, 0.8153618906942393},
             {1332.5, 1197.5, 1371.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50493,
             1,
             {0.02029648841395864, -0.20088626292466766, 0.8947425740860414, 0.8153618906942393},
             {1294.5, 1197.5, 1331.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50500,
             1,
             {0.056867455802252574, -0.22451994091580502, 1.0022145754477476, 0.8153618906942394},
             {1992.5, 1684.5, 2032.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {50501,
             1,
             {0.05916281042282127, -0.22451994091580502, 0.9808762520771788, 0.8153618906942394},
             {0.5, 1639.5, 39.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50504,
             1,
             {0.05965109167282127, -0.1772525849335303, 0.9813645333271788, 0.8153618906942394},
             {434.5, 150.5, 473.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50505,
             1,
             {0.06013937292282126, -0.1772525849335303, 0.9818528145771788, 0.8153618906942394},
             {513.5, 150.5, 552.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50506,
             1,
             {0.06013937292282126, -0.20088626292466766, 0.9818528145771788, 0.8153618906942393},
             {1098.5, 1197.5, 1137.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50508,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {1058.5, 1197.5, 1097.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50509,
             1,
             {0.05965109167282126, -0.22451994091580502, 0.9813645333271788, 0.8153618906942394},
             {114.5, 1639.5, 153.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50510,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {980.5, 1197.5, 1019.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50515,
             1,
             {0.06062765417282126, -0.20088626292466766, 0.9823410958271788, 0.8153618906942393},
             {940.5, 1197.5, 979.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50516,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {900.5, 1197.5, 939.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50517,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {860.5, 1197.5, 899.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50519,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {820.5, 1197.5, 859.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50520,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {780.5, 1197.5, 819.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50521,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {740.5, 1197.5, 779.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50525,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {700.5, 1197.5, 739.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50526,
             1,
             {0.05867452917282127, -0.20088626292466766, 0.9803879708271788, 0.8153618906942393},
             {660.5, 1197.5, 699.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50528,
             1,
             {0.05872465553452732, -0.22451994091580502, 0.9095370632154727, 0.8153618906942394},
             {236.5, 1639.5, 272.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50529,
             1,
             {0.053109421159527326, -0.22451994091580502, 0.9039218288404727, 0.8153618906942394},
             {273.5, 1639.5, 309.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50532,
             1,
             {0.05374375403895864, -0.1772525849335303, 0.9281898397110414, 0.8153618906942394},
             {553.5, 150.5, 590.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50536,
             1,
             {0.05398789466395864, -0.20088626292466766, 0.9284339803360414, 0.8153618906942393},
             {506.5, 1197.5, 543.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50544,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {469.5, 1197.5, 505.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50545,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {432.5, 1197.5, 468.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50547,
             1,
             {0.04402609167282127, -0.20088626292466766, 0.9657395333271788, 0.8153618906942393},
             {392.5, 1197.5, 431.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50548,
             1,
             {0.05198464330225259, -0.20088626292466766, 0.9973317629477475, 0.8153618906942393},
             {351.5, 1197.5, 391.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50549,
             1,
             {0.055062546159527326, -0.20088626292466766, 0.9058749538404727, 0.8153618906942393},
             {314.5, 1197.5, 350.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50556,
             1,
             {0.053937768302252574, -0.22451994091580502, 0.9992848879477475, 0.8153618906942394},
             {310.5, 1639.5, 350.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50557,
             1,
             {0.05916281042282127, -0.22451994091580502, 0.9808762520771788, 0.8153618906942394},
             {351.5, 1639.5, 390.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50560,
             1,
             {0.05818624792282127, -0.20088626292466766, 0.9798996895771788, 0.8153618906942393},
             {200.5, 1197.5, 239.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50564,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {160.5, 1197.5, 199.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50567,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {120.5, 1197.5, 159.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50572,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {80.5, 1197.5, 119.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50573,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {40.5, 1197.5, 79.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50575,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {0.5, 1197.5, 39.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {50577,
             1,
             {0.05916281042282127, -0.20088626292466766, 0.9808762520771788, 0.8153618906942393},
             {2008.5, 1241.5, 2047.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50581,
             1,
             {0.05965109167282126, -0.20088626292466766, 0.9813645333271788, 0.8153618906942393},
             {1968.5, 1241.5, 2007.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50583,
             1,
             {0.06062765417282126, -0.20088626292466766, 0.9823410958271788, 0.8153618906942393},
             {1928.5, 1241.5, 1967.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50584,
             1,
             {0.05725981178452732, -0.22451994091580502, 0.9080722194654727, 0.8153618906942394},
             {391.5, 1639.5, 427.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50588,
             1,
             {0.05374375403895864, -0.1772525849335303, 0.9281898397110414, 0.8153618906942394},
             {591.5, 150.5, 628.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50592,
             1,
             {0.05398789466395864, -0.20088626292466766, 0.9284339803360414, 0.8153618906942393},
             {1810.5, 1241.5, 1847.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50601,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {1773.5, 1241.5, 1809.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50612,
             1,
             {0.07356927102566468, -0.22451994091580502, 0.8771143227243354, 0.8153618906942394},
             {428.5, 1639.5, 462.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50613,
             1,
             {0.08724114602566468, -0.22451994091580502, 0.8907861977243354, 0.8153618906942394},
             {463.5, 1639.5, 497.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50616,
             1,
             {0.086898916405096, -0.1772525849335303, 0.914077646094904, 0.8153618906942394},
             {710.5, 150.5, 745.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50617,
             1,
             {0.09314848365952733, -0.1772525849335303, 0.9439608913404727, 0.8153618906942394},
             {746.5, 150.5, 782.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50619,
             1,
             {0.086898916405096, -0.20088626292466766, 0.914077646094904, 0.8153618906942393},
             {1570.5, 1241.5, 1605.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50620,
             1,
             {0.091049307030096, -0.20088626292466766, 0.918228036719904, 0.8153618906942393},
             {1534.5, 1241.5, 1569.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50621,
             1,
             {0.09090325540066468, -0.22451994091580502, 0.8944483070993354, 0.8153618906942394},
             {573.5, 1639.5, 607.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50622,
             1,
             {0.09187981790066468, -0.20088626292466766, 0.8954248695993354, 0.8153618906942393},
             {1458.5, 1241.5, 1492.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50628,
             1,
             {0.08724114602566468, -0.20088626292466766, 0.8907861977243354, 0.8153618906942393},
             {1423.5, 1241.5, 1457.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50629,
             1,
             {0.09090325540066468, -0.20088626292466766, 0.8944483070993354, 0.8153618906942393},
             {1388.5, 1241.5, 1422.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50630,
             1,
             {0.08977847754338994, -0.22451994091580502, 0.98785824120661, 0.8153618906942394},
             {608.5, 1639.5, 646.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50631,
             1,
             {0.08719101966395862, -0.20088626292466766, 0.9616371053360414, 0.8153618906942393},
             {1309.5, 1241.5, 1346.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50632,
             1,
             {0.08098941504338994, -0.20088626292466766, 0.97906917870661, 0.8153618906942393},
             {1270.5, 1241.5, 1308.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50633,
             1,
             {0.087387197655096, -0.20088626292466766, 0.914565927344904, 0.8153618906942393},
             {1234.5, 1241.5, 1269.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50634,
             1,
             {0.09461332740952733, -0.20088626292466766, 0.9454257350904727, 0.8153618906942393},
             {1197.5, 1241.5, 1233.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50636,
             1,
             {0.08870598977566468, -0.22451994091580502, 0.8922510414743354, 0.8153618906942394},
             {647.5, 1639.5, 681.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50638,
             1,
             {0.092758291405096, -0.20088626292466766, 0.919937021094904, 0.8153618906942393},
             {1120.5, 1241.5, 1155.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50640,
             1,
             {0.05823637428452732, -0.22451994091580502, 0.9090487819654727, 0.8153618906942394},
             {682.5, 1639.5, 718.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50641,
             1,
             {0.053597702409527326, -0.22451994091580502, 0.9044101100904727, 0.8153618906942394},
             {719.5, 1639.5, 755.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50644,
             1,
             {0.05423203528895864, -0.1772525849335303, 0.9286781209610414, 0.8153618906942394},
             {866.5, 150.5, 903.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50648,
             1,
             {0.05447617591395864, -0.20088626292466766, 0.9289222615860414, 0.8153618906942393},
             {963.5, 1241.5, 1000.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50656,
             1,
             {0.053597702409527326, -0.20088626292466766, 0.9044101100904727, 0.8153618906942393},
             {926.5, 1241.5, 962.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50657,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {889.5, 1241.5, 925.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50659,
             1,
             {0.05208273229782127, -0.20088626292466766, 0.9737961739521788, 0.8153618906942393},
             {849.5, 1241.5, 888.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50661,
             1,
             {0.057259811784527326, -0.20088626292466766, 0.9080722194654727, 0.8153618906942393},
             {812.5, 1241.5, 848.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50668,
             1,
             {0.07869622415066468, -0.22451994091580502, 0.8822412758493354, 0.8153618906942394},
             {756.5, 1639.5, 790.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50669,
             1,
             {0.08724114602566468, -0.20088626292466766, 0.8907861977243354, 0.8153618906942393},
             {740.5, 1241.5, 774.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50670,
             1,
             {0.08675286477566468, -0.22451994091580502, 0.8902979164743354, 0.8153618906942394},
             {874.5, 1639.5, 908.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50672,
             1,
             {0.086898916405096, -0.20088626292466766, 0.914077646094904, 0.8153618906942393},
             {664.5, 1241.5, 699.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50676,
             1,
             {0.090561025780096, -0.20088626292466766, 0.917739755469904, 0.8153618906942393},
             {628.5, 1241.5, 663.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50678,
             1,
             {0.08992669290066468, -0.20088626292466766, 0.8934717445993354, 0.8153618906942393},
             {593.5, 1241.5, 627.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50679,
             1,
             {0.08943841165066468, -0.20088626292466766, 0.8929834633493354, 0.8153618906942393},
             {558.5, 1241.5, 592.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50684,
             1,
             {0.08772942727566468, -0.20088626292466766, 0.8912744789743354, 0.8153618906942393},
             {523.5, 1241.5, 557.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50685,
             1,
             {0.08943841165066468, -0.20088626292466766, 0.8929834633493354, 0.8153618906942393},
             {488.5, 1241.5, 522.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50686,
             1,
             {0.08953433691838994, -0.20088626292466766, 0.98761410058161, 0.8153618906942393},
             {449.5, 1241.5, 487.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50687,
             1,
             {0.09231797278895862, -0.20088626292466766, 0.9667640584610414, 0.8153618906942393},
             {411.5, 1241.5, 448.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50688,
             1,
             {0.08587222754338994, -0.20088626292466766, 0.98395199120661, 0.8153618906942393},
             {372.5, 1241.5, 410.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50689,
             1,
             {0.088363760155096, -0.20088626292466766, 0.915542489844904, 0.8153618906942393},
             {336.5, 1241.5, 371.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50693,
             1,
             {0.089096182030096, -0.20088626292466766, 0.916274911719904, 0.8153618906942393},
             {300.5, 1241.5, 335.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50694,
             1,
             {0.096176260155096, -0.20088626292466766, 0.923354989844904, 0.8153618906942393},
             {264.5, 1241.5, 299.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50695,
             1,
             {0.095443838280096, -0.22451994091580502, 0.922622567969904, 0.8153618906942394},
             {909.5, 1639.5, 944.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50696,
             1,
             {0.05774809303452732, -0.22451994091580502, 0.9085605007154727, 0.8153618906942394},
             {945.5, 1639.5, 981.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50700,
             1,
             {0.05423203528895864, -0.1772525849335303, 0.9286781209610414, 0.8153618906942394},
             {1699.5, 451.5, 1736.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {50704,
             1,
             {0.05447617591395864, -0.20088626292466766, 0.9289222615860414, 0.8153618906942393},
             {113.5, 1241.5, 150.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50712,
             1,
             {0.053597702409527326, -0.20088626292466766, 0.9044101100904727, 0.8153618906942393},
             {76.5, 1241.5, 112.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50713,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {39.5, 1241.5, 75.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50715,
             1,
             {0.05633121191838995, -0.20088626292466766, 0.95441097558161, 0.8153618906942393},
             {0.5, 1241.5, 38.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50716,
             1,
             {0.05686745580225258, -0.20088626292466766, 1.0022145754477476, 0.8153618906942393},
             {151.5, 1241.5, 191.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {50724,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1435.5, 73.5, 1475.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {50725,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {982.5, 1639.5, 1022.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50728,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1990.5, 151.5, 2030.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50732,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1876.5, 1285.5, 1916.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50733,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1059.5, 1639.5, 1099.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50734,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1798.5, 1285.5, 1838.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50736,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1757.5, 1285.5, 1797.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50739,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1716.5, 1285.5, 1756.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50740,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1675.5, 1285.5, 1715.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50741,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1634.5, 1285.5, 1674.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50743,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1593.5, 1285.5, 1633.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50745,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1552.5, 1285.5, 1592.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50747,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1511.5, 1285.5, 1551.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50752,
             1,
             {0.012481824686115212, -0.22451994091580502, 1.0050963003138849, 0.8153618906942394},
             {1100.5, 1639.5, 1142.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50753,
             1,
             {0.0284490543066839, -0.22451994091580502, 0.9974298519433162, 0.8153618906942394},
             {1143.5, 1639.5, 1184.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50756,
             1,
             {0.0304021793066839, -0.20088626292466766, 0.9993829769433162, 0.8153618906942393},
             {1388.5, 1285.5, 1429.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50760,
             1,
             {0.0284490543066839, -0.20088626292466766, 0.9974298519433162, 0.8153618906942393},
             {1346.5, 1285.5, 1387.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50768,
             1,
             {0.0235662418066839, -0.20088626292466766, 0.9925470394433162, 0.8153618906942393},
             {1304.5, 1285.5, 1345.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50769,
             1,
             {0.0284490543066839, -0.20088626292466766, 0.9974298519433162, 0.8153618906942393},
             {1262.5, 1285.5, 1303.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50771,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {1220.5, 1285.5, 1261.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50772,
             1,
             {0.0235662418066839, -0.22451994091580502, 0.9925470394433162, 0.8153618906942394},
             {1258.5, 1639.5, 1299.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50773,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {1137.5, 1285.5, 1178.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50780,
             1,
             {0.0057941025433899535, -0.22451994091580502, 0.90387386620661, 0.8153618906942394},
             {1300.5, 1639.5, 1338.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50781,
             1,
             {0.01687851966395864, -0.22451994091580502, 0.8913246053360414, 0.8153618906942394},
             {1339.5, 1639.5, 1376.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50784,
             1,
             {0.013362461918389953, -0.20088626292466766, 0.91144222558161, 0.8153618906942393},
             {1016.5, 1285.5, 1054.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50796,
             1,
             {0.02176133216395864, -0.20088626292466766, 0.8962074178360414, 0.8153618906942393},
             {978.5, 1285.5, 1015.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50799,
             1,
             {0.013020232297821269, -0.20088626292466766, 0.9347336739521788, 0.8153618906942393},
             {938.5, 1285.5, 977.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50801,
             1,
             {0.010432774418389953, -0.20088626292466766, 0.90851253808161, 0.8153618906942393},
             {899.5, 1285.5, 937.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50808,
             1,
             {0.038705124284527326, -0.22451994091580502, 0.8895175319654727, 0.8153618906942394},
             {1377.5, 1639.5, 1413.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50809,
             1,
             {0.05027782265509601, -0.22451994091580502, 0.877456552344904, 0.8153618906942394},
             {1455.5, 1639.5, 1490.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50812,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {786.5, 1285.5, 823.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50816,
             1,
             {0.03421250403895864, -0.20088626292466766, 0.9086585897110414, 0.8153618906942393},
             {748.5, 1285.5, 785.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50824,
             1,
             {0.04734813515509601, -0.20088626292466766, 0.874526864844904, 0.8153618906942393},
             {712.5, 1285.5, 747.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50825,
             1,
             {0.05296336953009601, -0.20088626292466766, 0.880142099219904, 0.8153618906942393},
             {676.5, 1285.5, 711.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50827,
             1,
             {0.04680972754338995, -0.20088626292466766, 0.94488949120661, 0.8153618906942393},
             {637.5, 1285.5, 675.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50829,
             1,
             {0.040169968034527326, -0.20088626292466766, 0.8909823757154727, 0.8153618906942393},
             {600.5, 1285.5, 636.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50836,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7444608567208273},
             {1221.5, 73.5, 1261.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {50837,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1532.5, 1639.5, 1572.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50840,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1785.5, 151.5, 1825.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50844,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {442.5, 1285.5, 482.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50852,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {401.5, 1285.5, 441.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50853,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {360.5, 1285.5, 400.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50855,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {319.5, 1285.5, 359.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50857,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1573.5, 1639.5, 1613.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50864,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {237.5, 1285.5, 277.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50865,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1614.5, 1639.5, 1654.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50868,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1744.5, 151.5, 1784.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50872,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {118.5, 1285.5, 158.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50873,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1696.5, 1639.5, 1736.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50874,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {41.5, 1285.5, 81.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50880,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {0.5, 1285.5, 40.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {50881,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {2000.5, 1329.5, 2040.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50883,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1959.5, 1329.5, 1999.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50885,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1918.5, 1329.5, 1958.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50892,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1737.5, 1639.5, 1774.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50893,
             1,
             {0.046029343034527326, -0.22451994091580502, 0.8968417507154727, 0.8153618906942394},
             {1775.5, 1639.5, 1811.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50896,
             1,
             {0.04519883216395864, -0.20088626292466766, 0.9196449178360414, 0.8153618906942393},
             {1804.5, 1329.5, 1841.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50900,
             1,
             {0.04715195716395864, -0.20088626292466766, 0.9215980428360414, 0.8153618906942393},
             {1766.5, 1329.5, 1803.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50908,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1729.5, 1329.5, 1765.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50909,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1692.5, 1329.5, 1728.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50912,
             1,
             {0.05159445104782127, -0.22451994091580502, 0.9733078927021788, 0.8153618906942394},
             {1812.5, 1639.5, 1851.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50913,
             1,
             {0.051888718034527326, -0.22451994091580502, 0.9027011257154727, 0.8153618906942394},
             {1852.5, 1639.5, 1888.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50920,
             1,
             {0.0043292587933899535, -0.22451994091580502, 0.90240902245661, 0.8153618906942394},
             {1889.5, 1639.5, 1927.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50921,
             1,
             {0.02029648841395864, -0.22451994091580502, 0.8947425740860414, 0.8153618906942394},
             {1928.5, 1639.5, 1965.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50924,
             1,
             {0.019465977543389953, -0.20088626292466766, 0.91754574120661, 0.8153618906942393},
             {1499.5, 1329.5, 1537.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50928,
             1,
             {0.020930821293389953, -0.20088626292466766, 0.91901058495661, 0.8153618906942393},
             {1460.5, 1329.5, 1498.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50936,
             1,
             {0.02029648841395864, -0.20088626292466766, 0.8947425740860414, 0.8153618906942393},
             {1422.5, 1329.5, 1459.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50937,
             1,
             {0.02029648841395864, -0.20088626292466766, 0.8947425740860414, 0.8153618906942393},
             {1384.5, 1329.5, 1421.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50941,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {1966.5, 1639.5, 2003.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {50948,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {156.5, 1594.5, 193.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {50949,
             1,
             {0.041146530534527326, -0.22451994091580502, 0.8919589382154727, 0.8153618906942394},
             {194.5, 1594.5, 230.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {50952,
             1,
             {0.04275742591395864, -0.20088626292466766, 0.9172035115860414, 0.8153618906942393},
             {1223.5, 1329.5, 1260.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50956,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {1185.5, 1329.5, 1222.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50964,
             1,
             {0.041146530534527326, -0.20088626292466766, 0.8919589382154727, 0.8153618906942393},
             {1148.5, 1329.5, 1184.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50965,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1111.5, 1329.5, 1147.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50967,
             1,
             {0.04319558080225259, -0.20088626292466766, 0.9885427004477475, 0.8153618906942393},
             {1070.5, 1329.5, 1110.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50969,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {1033.5, 1329.5, 1069.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50976,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {231.5, 1594.5, 271.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {50977,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {272.5, 1594.5, 312.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {50980,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1584.5, 150.5, 1624.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {50984,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {875.5, 1329.5, 915.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50992,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {834.5, 1329.5, 874.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50993,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {793.5, 1329.5, 833.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50995,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {752.5, 1329.5, 792.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {50997,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {354.5, 1594.5, 394.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {50999,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {675.5, 1329.5, 715.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51004,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7444608567208273},
             {711.5, 72.5, 751.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {51005,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {436.5, 1594.5, 476.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51008,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1908.5, 151.5, 1948.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51012,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {518.5, 1329.5, 558.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51018,
             1,
             {0.02439675267725258, -0.20088626292466766, 0.9697438723227475, 0.8153618906942393},
             {477.5, 1329.5, 517.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51020,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {436.5, 1329.5, 476.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51021,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {395.5, 1329.5, 435.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51023,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1388.5, 150.5, 1428.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51025,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {316.5, 1329.5, 356.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51026,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {275.5, 1329.5, 315.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51027,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {234.5, 1329.5, 274.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51028,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {477.5, 1594.5, 517.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51029,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {156.5, 1329.5, 196.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51030,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {115.5, 1329.5, 155.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51031,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {74.5, 1329.5, 114.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51032,
             1,
             {0.02322617591395864, -0.22451994091580502, 0.8976722615860414, 0.8153618906942394},
             {518.5, 1594.5, 555.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51036,
             1,
             {0.052621139909527326, -0.20088626292466766, 0.9034335475904727, 0.8153618906942393},
             {0.5, 1329.5, 36.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51040,
             1,
             {0.056283249284527326, -0.20088626292466766, 0.9070956569654727, 0.8153618906942393},
             {197.5, 1329.5, 233.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {51048,
             1,
             {0.05223094765509601, -0.20088626292466766, 0.879409677344904, 0.8153618906942393},
             {1985.5, 1373.5, 2020.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51051,
             1,
             {0.04388004004338995, -0.20088626292466766, 0.94195980370661, 0.8153618906942393},
             {1946.5, 1373.5, 1984.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51060,
             1,
             {0.07991692727566468, -0.22451994091580502, 0.8834619789743354, 0.8153618906942394},
             {556.5, 1594.5, 590.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51061,
             1,
             {0.09041497415066468, -0.22451994091580502, 0.8939600258493354, 0.8153618906942394},
             {591.5, 1594.5, 625.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51064,
             1,
             {0.088852041405096, -0.1772525849335303, 0.916030771094904, 0.8153618906942394},
             {1352.5, 150.5, 1387.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51068,
             1,
             {0.092514150780096, -0.20088626292466766, 0.919692880469904, 0.8153618906942393},
             {1792.5, 1373.5, 1827.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51069,
             1,
             {0.09041497415066468, -0.22451994091580502, 0.8939600258493354, 0.8153618906942394},
             {626.5, 1594.5, 660.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51070,
             1,
             {0.09041497415066468, -0.20088626292466766, 0.8939600258493354, 0.8153618906942393},
             {1715.5, 1373.5, 1749.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51075,
             1,
             {0.09427109778895862, -0.20088626292466766, 0.9687171834610414, 0.8153618906942393},
             {1677.5, 1373.5, 1714.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51076,
             1,
             {0.10174353189623336, -0.20088626292466766, 0.8816549056037667, 0.8153618906942393},
             {1643.5, 1373.5, 1676.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51077,
             1,
             {0.10174353189623336, -0.20088626292466766, 0.8816549056037667, 0.8153618906942393},
             {1609.5, 1373.5, 1642.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51079,
             1,
             {0.08426133216395862, -0.20088626292466766, 0.9587074178360414, 0.8153618906942393},
             {1571.5, 1373.5, 1608.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51080,
             1,
             {0.09490543066838994, -0.20088626292466766, 0.99298519433161, 0.8153618906942393},
             {1532.5, 1373.5, 1570.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51081,
             1,
             {0.09993645852566468, -0.20088626292466766, 0.9034815102243354, 0.8153618906942393},
             {1497.5, 1373.5, 1531.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51082,
             1,
             {0.09925199928452733, -0.20088626292466766, 0.9500644069654727, 0.8153618906942393},
             {1460.5, 1373.5, 1496.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51086,
             1,
             {0.100570791405096, -0.20088626292466766, 0.927749521094904, 0.8153618906942393},
             {1424.5, 1373.5, 1459.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51088,
             1,
             {0.0294256168066839, -0.22451994091580502, 0.9984064144433162, 0.8153618906942394},
             {661.5, 1594.5, 702.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51089,
             1,
             {0.0194158511816839, -0.22451994091580502, 0.9883966488183162, 0.8153618906942394},
             {703.5, 1594.5, 744.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51092,
             1,
             {0.0189275699316839, -0.1772525849335303, 0.9879083675683162, 0.8153618906942394},
             {986.5, 150.5, 1027.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51094,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {1268.5, 1373.5, 1309.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51095,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1226.5, 1373.5, 1267.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51096,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {1184.5, 1373.5, 1225.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51098,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {1142.5, 1373.5, 1183.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51104,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1100.5, 1373.5, 1141.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51105,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1058.5, 1373.5, 1099.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51107,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {1016.5, 1373.5, 1057.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51108,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {974.5, 1373.5, 1015.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51109,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {932.5, 1373.5, 973.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51110,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {890.5, 1373.5, 931.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51116,
             1,
             {0.03250351966395864, -0.22451994091580502, 0.9069496053360414, 0.8153618906942394},
             {745.5, 1594.5, 782.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51117,
             1,
             {0.02908555091395864, -0.22451994091580502, 0.9035316365860414, 0.8153618906942394},
             {821.5, 1594.5, 858.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51120,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {239.5, 150.5, 277.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51124,
             1,
             {0.024837071293389953, -0.20088626292466766, 0.92291683495661, 0.8153618906942393},
             {724.5, 1373.5, 762.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51132,
             1,
             {0.02908555091395864, -0.20088626292466766, 0.9035316365860414, 0.8153618906942393},
             {686.5, 1373.5, 723.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51133,
             1,
             {0.02908555091395864, -0.20088626292466766, 0.9035316365860414, 0.8153618906942393},
             {648.5, 1373.5, 685.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51135,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {608.5, 1373.5, 647.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51136,
             1,
             {0.029279565177252585, -0.20088626292466766, 0.9746266848227475, 0.8153618906942393},
             {567.5, 1373.5, 607.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51137,
             1,
             {0.02273789466395864, -0.20088626292466766, 0.8971839803360414, 0.8153618906942393},
             {529.5, 1373.5, 566.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51144,
             1,
             {0.0294256168066839, -0.22451994091580502, 0.9984064144433162, 0.8153618906942394},
             {859.5, 1594.5, 900.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51145,
             1,
             {0.0194158511816839, -0.22451994091580502, 0.9883966488183162, 0.8153618906942394},
             {901.5, 1594.5, 942.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51148,
             1,
             {0.0189275699316839, -0.1772525849335303, 0.9879083675683162, 0.8153618906942394},
             {0.5, 150.5, 41.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {51150,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {363.5, 1373.5, 404.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51152,
             1,
             {0.0199041324316839, -0.20088626292466766, 0.9888849300683162, 0.8153618906942393},
             {321.5, 1373.5, 362.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51160,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {279.5, 1373.5, 320.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51165,
             1,
             {0.0189275699316839, -0.20088626292466766, 0.9879083675683162, 0.8153618906942393},
             {237.5, 1373.5, 278.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51172,
             1,
             {0.03348008216395864, -0.22451994091580502, 0.9079261678360414, 0.8153618906942394},
             {986.5, 1594.5, 1023.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51176,
             1,
             {0.021419102543389953, -0.1772525849335303, 0.91949886620661, 0.8153618906942394},
             {1971.5, 193.5, 2009.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51180,
             1,
             {0.024837071293389953, -0.20088626292466766, 0.92291683495661, 0.8153618906942393},
             {120.5, 1373.5, 158.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51200,
             1,
             {0.036263718034527326, -0.22451994091580502, 0.8870761257154727, 0.8153618906942394},
             {1024.5, 1594.5, 1060.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51201,
             1,
             {0.030648483659527326, -0.22451994091580502, 0.8814608913404727, 0.8153618906942394},
             {1061.5, 1594.5, 1097.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51204,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {2010.5, 193.5, 2047.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51208,
             1,
             {0.03225937903895864, -0.20088626292466766, 0.9067054647110414, 0.8153618906942393},
             {0.5, 1373.5, 37.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {51210,
             1,
             {0.028451218034527326, -0.20088626292466766, 0.8792636257154727, 0.8153618906942393},
             {1972.5, 1417.5, 2008.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51216,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {1935.5, 1417.5, 1971.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51217,
             1,
             {0.030160202409527326, -0.20088626292466766, 0.8809726100904727, 0.8153618906942393},
             {1898.5, 1417.5, 1934.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51219,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {1858.5, 1417.5, 1897.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51221,
             1,
             {0.03348008216395864, -0.20088626292466766, 0.9079261678360414, 0.8153618906942393},
             {1820.5, 1417.5, 1857.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51222,
             1,
             {0.03328390417282127, -0.20088626292466766, 0.9549973458271788, 0.8153618906942393},
             {1780.5, 1417.5, 1819.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51228,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {1098.5, 1594.5, 1135.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51229,
             1,
             {0.02957383216395864, -0.22451994091580502, 0.9040199178360414, 0.8153618906942394},
             {1177.5, 1594.5, 1214.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51232,
             1,
             {0.030940586918389953, -0.1772525849335303, 0.92902035058161, 0.8153618906942394},
             {1729.5, 193.5, 1767.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51236,
             1,
             {0.029964024418389953, -0.20088626292466766, 0.92804378808161, 0.8153618906942393},
             {1628.5, 1417.5, 1666.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51244,
             1,
             {0.02957383216395864, -0.20088626292466766, 0.9040199178360414, 0.8153618906942393},
             {1590.5, 1417.5, 1627.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51245,
             1,
             {0.02957383216395864, -0.20088626292466766, 0.9040199178360414, 0.8153618906942393},
             {1552.5, 1417.5, 1589.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51247,
             1,
             {0.029133513547821267, -0.20088626292466766, 0.9508469552021788, 0.8153618906942393},
             {1512.5, 1417.5, 1551.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51249,
             1,
             {0.03323594153895864, -0.20088626292466766, 0.9076820272110414, 0.8153618906942393},
             {1474.5, 1417.5, 1511.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51256,
             1,
             {0.032601608659527326, -0.22451994091580502, 0.8834140163404727, 0.8153618906942394},
             {1256.5, 1594.5, 1292.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51260,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {1119.5, 193.5, 1156.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51264,
             1,
             {0.02981797278895864, -0.20088626292466766, 0.9042640584610414, 0.8153618906942393},
             {1360.5, 1417.5, 1397.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51272,
             1,
             {0.030648483659527326, -0.20088626292466766, 0.8814608913404727, 0.8153618906942393},
             {1323.5, 1417.5, 1359.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51273,
             1,
             {0.030160202409527326, -0.20088626292466766, 0.8809726100904727, 0.8153618906942393},
             {1286.5, 1417.5, 1322.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51276,
             1,
             {0.03196511205225258, -0.20088626292466766, 0.9773122316977475, 0.8153618906942393},
             {1245.5, 1417.5, 1285.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51277,
             1,
             {0.03348008216395864, -0.20088626292466766, 0.9079261678360414, 0.8153618906942393},
             {1207.5, 1417.5, 1244.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51284,
             1,
             {0.02469101966395864, -0.22451994091580502, 0.8991371053360414, 0.8153618906942394},
             {1293.5, 1594.5, 1330.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51312,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {1036.5, 33.5, 1076.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {51313,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1090.5, 1417.5, 1130.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51316,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {477.5, 108.5, 517.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51320,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {651.5, 193.5, 691.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51322,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {569.5, 193.5, 609.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51328,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {487.5, 193.5, 527.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51329,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {241.5, 193.5, 281.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51331,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {200.5, 193.5, 240.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51333,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {77.5, 193.5, 117.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {51334,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1852.5, 236.5, 1892.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {51335,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1367.5, 236.5, 1407.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {51339,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {705.5, 1417.5, 745.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51340,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {1331.5, 1594.5, 1373.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51341,
             1,
             {0.0245428043066839, -0.22451994091580502, 0.9935236019433162, 0.8153618906942394},
             {1412.5, 1594.5, 1453.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51348,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {585.5, 1417.5, 626.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51357,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {543.5, 1417.5, 584.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51359,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {501.5, 1417.5, 542.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51361,
             1,
             {0.0260076480566839, -0.20088626292466766, 0.9949884456933162, 0.8153618906942393},
             {459.5, 1417.5, 500.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51368,
             1,
             {0.015559727543389953, -0.22451994091580502, 0.91363949120661, 0.8153618906942394},
             {1454.5, 1594.5, 1492.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51388,
             1,
             {0.015021319931683898, -0.22451994091580502, 0.9840021175683162, 0.8153618906942394},
             {1530.5, 1594.5, 1571.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51389,
             1,
             {0.014094883793389953, -0.20088626292466766, 0.91217464745661, 0.8153618906942393},
             {342.5, 1417.5, 380.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51396,
             1,
             {0.040658249284527326, -0.22451994091580502, 0.8914706569654727, 0.8153618906942394},
             {1572.5, 1594.5, 1608.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51400,
             1,
             {0.053109421159527326, -0.1772525849335303, 0.9039218288404727, 0.8153618906942394},
             {594.5, 236.5, 630.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {51404,
             1,
             {0.051400436784527326, -0.20088626292466766, 0.9022128444654727, 0.8153618906942393},
             {231.5, 1417.5, 267.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51412,
             1,
             {0.05027782265509601, -0.1772525849335303, 0.877456552344904, 0.8153618906942394},
             {314.5, 236.5, 349.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {51413,
             1,
             {0.05027782265509601, -0.20088626292466766, 0.877456552344904, 0.8153618906942393},
             {157.5, 1417.5, 192.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51415,
             1,
             {0.04436832129338995, -0.20088626292466766, 0.94244808495661, 0.8153618906942393},
             {118.5, 1417.5, 156.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51417,
             1,
             {0.05393993203009601, -0.20088626292466766, 0.881118661719904, 0.8153618906942393},
             {82.5, 1417.5, 117.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51424,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {749.5, 33.5, 789.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {51425,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 1417.5, 40.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {51428,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1106.5, 108.5, 1146.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51445,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1777.5, 279.5, 1817.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51452,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1921.5, 1461.5, 1961.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51453,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1880.5, 1461.5, 1920.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51456,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1229.5, 108.5, 1269.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51460,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1408.5, 279.5, 1448.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51461,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1757.5, 1461.5, 1797.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51462,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1367.5, 279.5, 1407.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51468,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1249.5, 279.5, 1289.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51469,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {849.5, 279.5, 889.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51471,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {725.5, 279.5, 765.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {51473,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1961.5, 322.5, 2001.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51480,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1609.5, 1594.5, 1646.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51500,
             1,
             {0.05291324316838995, -0.22451994091580502, 0.95099300683161, 0.8153618906942394},
             {1647.5, 1594.5, 1685.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51508,
             1,
             {0.0075030869183899535, -0.22451994091580502, 0.90558285058161, 0.8153618906942394},
             {1686.5, 1594.5, 1724.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51536,
             1,
             {0.01834336341395864, -0.22451994091580502, 0.8927894490860414, 0.8153618906942394},
             {1725.5, 1594.5, 1762.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51537,
             1,
             {0.031136764909527326, -0.22451994091580502, 0.8819491725904727, 0.8153618906942394},
             {1838.5, 1594.5, 1874.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51540,
             1,
             {0.03396836341395864, -0.20088626292466766, 0.9084144490860414, 0.8153618906942393},
             {1337.5, 1461.5, 1374.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51544,
             1,
             {0.03225937903895864, -0.20088626292466766, 0.9067054647110414, 0.8153618906942393},
             {1299.5, 1461.5, 1336.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51552,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1262.5, 1461.5, 1298.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51553,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {1225.5, 1461.5, 1261.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51555,
             1,
             {0.03914327917282127, -0.20088626292466766, 0.9608567208271788, 0.8153618906942393},
             {1185.5, 1461.5, 1224.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51564,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1144.5, 1461.5, 1184.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51568,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1352.5, 108.5, 1392.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51572,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1728.5, 322.5, 1768.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51580,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1441.5, 322.5, 1481.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51592,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {995.5, 33.5, 1035.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {51593,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {939.5, 1461.5, 979.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51596,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1784.5, 109.5, 1824.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51600,
             1,
             {0.02927956517725258, -0.20088626292466766, 0.9746266848227475, 0.791728212703102},
             {1400.5, 322.5, 1440.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51608,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1075.5, 322.5, 1115.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51609,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1034.5, 322.5, 1074.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51611,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {671.5, 322.5, 711.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51613,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {630.5, 322.5, 670.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51648,
             1,
             {0.029671921159527326, -0.22451994091580502, 0.8804843288404727, 0.8153618906942394},
             {1875.5, 1594.5, 1911.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51649,
             1,
             {0.059066885155096005, -0.22451994091580502, 0.886245614844904, 0.8153618906942394},
             {1912.5, 1594.5, 1947.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51652,
             1,
             {0.054818405534527326, -0.1772525849335303, 0.9056308132154727, 0.8153618906942394},
             {273.5, 322.5, 309.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51655,
             1,
             {0.055794968034527326, -0.20088626292466766, 0.9066073757154727, 0.8153618906942393},
             {536.5, 1461.5, 572.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51656,
             1,
             {0.06165434303452732, -0.20088626292466766, 0.9124667507154727, 0.8153618906942393},
             {499.5, 1461.5, 535.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51658,
             1,
             {0.05809032265509601, -0.20088626292466766, 0.885269052344904, 0.8153618906942393},
             {463.5, 1461.5, 498.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51664,
             1,
             {0.05491649453009601, -0.20088626292466766, 0.882095224219904, 0.8153618906942393},
             {427.5, 1461.5, 462.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51665,
             1,
             {0.059066885155096005, -0.20088626292466766, 0.886245614844904, 0.8153618906942393},
             {391.5, 1461.5, 426.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51667,
             1,
             {0.05315738379338995, -0.20088626292466766, 0.95123714745661, 0.8153618906942393},
             {352.5, 1461.5, 390.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51669,
             1,
             {0.05725981178452732, -0.20088626292466766, 0.9080722194654727, 0.8153618906942393},
             {315.5, 1461.5, 351.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51670,
             1,
             {0.06292300879338994, -0.20088626292466766, 0.96100277245661, 0.8153618906942393},
             {276.5, 1461.5, 314.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51673,
             1,
             {0.05823637428452732, -0.20088626292466766, 0.9090487819654727, 0.8153618906942393},
             {239.5, 1461.5, 275.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51674,
             1,
             {0.05374375403895863, -0.20088626292466766, 0.9281898397110414, 0.8153618906942393},
             {201.5, 1461.5, 238.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51676,
             1,
             {0.013214246561115212, -0.22451994091580502, 1.0058287221888849, 0.8153618906942394},
             {1948.5, 1594.5, 1990.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51677,
             1,
             {0.015509601181683898, -0.22451994091580502, 0.9844903988183162, 0.8153618906942394},
             {1991.5, 1594.5, 2032.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {51680,
             1,
             {0.012824054306683898, -0.1772525849335303, 0.9818048519433162, 0.8153618906942394},
             {1298.5, 451.5, 1339.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {51682,
             1,
             {0.0164861636816839, -0.20088626292466766, 0.9854669613183162, 0.8153618906942393},
             {42.5, 1461.5, 83.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51684,
             1,
             {0.0159978824316839, -0.20088626292466766, 0.9849786800683162, 0.8153618906942393},
             {0.5, 1461.5, 41.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51687,
             1,
             {0.0159978824316839, -0.20088626292466766, 0.9849786800683162, 0.8153618906942393},
             {84.5, 1461.5, 125.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {51692,
             1,
             {0.015509601181683898, -0.20088626292466766, 0.9844903988183162, 0.8153618906942393},
             {1977.5, 1505.5, 2018.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51693,
             1,
             {0.015509601181683898, -0.20088626292466766, 0.9844903988183162, 0.8153618906942393},
             {1935.5, 1505.5, 1976.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51695,
             1,
             {0.012824054306683898, -0.20088626292466766, 0.9818048519433162, 0.8153618906942393},
             {1893.5, 1505.5, 1934.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51696,
             1,
             {0.015509601181683898, -0.20088626292466766, 0.9844903988183162, 0.8153618906942393},
             {1851.5, 1505.5, 1892.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51697,
             1,
             {0.015509601181683898, -0.20088626292466766, 0.9844903988183162, 0.8153618906942393},
             {1809.5, 1505.5, 1850.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51704,
             1,
             {0.011653477543389953, -0.22451994091580502, 0.90973324120661, 0.8153618906942394},
             {0.5, 1549.5, 38.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51705,
             1,
             {0.011409336918389953, -0.22451994091580502, 0.90948910058161, 0.8153618906942394},
             {113.5, 1549.5, 151.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51708,
             1,
             {0.012287810422821267, -0.1772525849335303, 0.9340012520771788, 0.8153618906942394},
             {42.5, 322.5, 81.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51712,
             1,
             {0.011799529172821267, -0.20088626292466766, 0.9335129708271788, 0.8153618906942393},
             {1644.5, 1505.5, 1683.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51720,
             1,
             {0.011409336918389953, -0.20088626292466766, 0.90948910058161, 0.8153618906942393},
             {1605.5, 1505.5, 1643.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51721,
             1,
             {0.011409336918389953, -0.20088626292466766, 0.90948910058161, 0.8153618906942393},
             {1566.5, 1505.5, 1604.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51723,
             1,
             {0.012776091672821267, -0.20088626292466766, 0.9344895333271788, 0.8153618906942393},
             {1526.5, 1505.5, 1565.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51724,
             1,
             {0.014386987052252585, -0.20088626292466766, 0.9597341066977475, 0.8153618906942393},
             {1485.5, 1505.5, 1525.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51725,
             1,
             {0.015071446293389953, -0.20088626292466766, 0.91315120995661, 0.8153618906942393},
             {1446.5, 1505.5, 1484.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51732,
             1,
             {0.011749402811115212, -0.22451994091580502, 1.0043638784388849, 0.8153618906942394},
             {152.5, 1549.5, 194.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51736,
             1,
             {0.012824054306683898, -0.1772525849335303, 0.9818048519433162, 0.8153618906942394},
             {0.5, 322.5, 41.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {51753,
             1,
             {0.015509601181683898, -0.20088626292466766, 0.9844903988183162, 0.8153618906942393},
             {1321.5, 1505.5, 1362.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51788,
             1,
             {0.022591843034527326, -0.22451994091580502, 0.8734042507154727, 0.8153618906942394},
             {195.5, 1549.5, 231.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51789,
             1,
             {0.00564805091395864, -0.22451994091580502, 0.8800941365860414, 0.8153618906942394},
             {232.5, 1549.5, 269.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51792,
             1,
             {0.0062823837933899535, -0.1772525849335303, 0.90436214745661, 0.8153618906942394},
             {1429.5, 365.5, 1467.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {51796,
             1,
             {0.008479649418389953, -0.20088626292466766, 0.90655941308161, 0.8153618906942393},
             {1162.5, 1505.5, 1200.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51804,
             1,
             {0.00467148841395864, -0.20088626292466766, 0.8791175740860414, 0.8153618906942393},
             {1124.5, 1505.5, 1161.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51805,
             1,
             {0.00613633216395864, -0.20088626292466766, 0.8805824178360414, 0.8153618906942393},
             {1086.5, 1505.5, 1123.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51807,
             1,
             {0.011555388547821269, -0.20088626292466766, 0.9332688302021788, 0.8153618906942393},
             {1046.5, 1505.5, 1085.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51808,
             1,
             {0.008771752677252585, -0.20088626292466766, 0.9541188723227475, 0.8153618906942393},
             {1005.5, 1505.5, 1045.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51809,
             1,
             {0.01614609778895864, -0.20088626292466766, 0.8905921834610414, 0.8153618906942393},
             {967.5, 1505.5, 1004.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51816,
             1,
             {0.008967930668389953, -0.22451994091580502, 0.90704769433161, 0.8153618906942394},
             {270.5, 1549.5, 308.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51837,
             1,
             {0.015071446293389953, -0.20088626292466766, 0.91315120995661, 0.8153618906942393},
             {890.5, 1505.5, 928.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51844,
             1,
             {0.021615280534527326, -0.22451994091580502, 0.8724276882154727, 0.8153618906942394},
             {351.5, 1549.5, 387.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51864,
             1,
             {0.011457299552252585, -0.20088626292466766, 0.9568044191977475, 0.8153618906942393},
             {809.5, 1505.5, 849.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51900,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {585.5, 33.5, 625.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {51901,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {727.5, 1505.5, 767.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {51904,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {942.5, 108.5, 982.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {51908,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1265.5, 365.5, 1305.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {51916,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {651.5, 365.5, 691.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {51917,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {610.5, 365.5, 650.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {51919,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {492.5, 365.5, 532.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {51921,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1710.5, 408.5, 1750.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {51923,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1628.5, 408.5, 1668.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {51928,
             1,
             {0.0009091263155465261, -0.22451994091580502, 1.0171572799344535, 0.8153618906942394},
             {2004.5, 1639.5, 2047.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {51929,
             1,
             {0.0184392886816839, -0.22451994091580502, 0.9874200863183162, 0.8153618906942394},
             {388.5, 1549.5, 429.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51936,
             1,
             {0.0184392886816839, -0.1772525849335303, 0.9874200863183162, 0.8153618906942394},
             {1340.5, 408.5, 1381.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {51948,
             1,
             {0.0184392886816839, -0.22451994091580502, 0.9874200863183162, 0.8153618906942394},
             {430.5, 1549.5, 471.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51956,
             1,
             {0.018001133793389953, -0.22451994091580502, 0.91608089745661, 0.8153618906942394},
             {472.5, 1549.5, 510.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51976,
             1,
             {0.03245339330225258, -0.22451994091580502, 0.9778005129477475, 0.8153618906942394},
             {511.5, 1549.5, 551.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51984,
             1,
             {0.03421250403895864, -0.22451994091580502, 0.9086585897110414, 0.8153618906942394},
             {552.5, 1549.5, 589.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {51988,
             1,
             {0.03030625403895864, -0.1772525849335303, 0.9047523397110414, 0.8153618906942394},
             {488.5, 408.5, 525.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {51992,
             1,
             {0.054574264909527326, -0.1772525849335303, 0.9053866725904727, 0.8153618906942394},
             {1737.5, 451.5, 1773.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {52000,
             1,
             {0.05076610390509601, -0.1772525849335303, 0.877944833594904, 0.8153618906942394},
             {287.5, 451.5, 322.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {52001,
             1,
             {0.05076610390509601, -0.20088626292466766, 0.877944833594904, 0.8153618906942393},
             {41.5, 1505.5, 76.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52033,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {447.5, 408.5, 487.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {52040,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 1505.5, 40.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52041,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {77.5, 1505.5, 117.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52044,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {41.5, 108.5, 81.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {52048,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1355.5, 494.5, 1395.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52056,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1237.5, 494.5, 1277.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52057,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1159.5, 494.5, 1199.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52061,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {243.5, 494.5, 283.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52068,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {590.5, 1549.5, 627.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52088,
             1,
             {0.04866476354782127, -0.22451994091580502, 0.9703782052021788, 0.8153618906942394},
             {703.5, 1549.5, 742.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52089,
             1,
             {0.045541061784527326, -0.20088626292466766, 0.8963534694654727, 0.8153618906942393},
             {1712.5, 1550.5, 1748.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52124,
             1,
             {0.02078476966395864, -0.22451994091580502, 0.8952308553360414, 0.8153618906942394},
             {743.5, 1549.5, 780.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52152,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1629.5, 1550.5, 1669.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52180,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {544.5, 33.5, 584.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {52196,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1902.5, 538.5, 1942.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {52199,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1742.5, 538.5, 1782.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {52201,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1701.5, 538.5, 1741.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {52236,
             1,
             {0.024544968034527326, -0.22451994091580502, 0.8753573757154727, 0.8153618906942394},
             {781.5, 1549.5, 817.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52237,
             1,
             {0.025033249284527326, -0.22451994091580502, 0.8758456569654727, 0.8153618906942394},
             {818.5, 1549.5, 854.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52240,
             1,
             {0.02322617591395864, -0.1772525849335303, 0.8976722615860414, 0.8153618906942394},
             {1267.5, 538.5, 1304.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {52244,
             1,
             {0.03079453528895864, -0.20088626292466766, 0.9052406209610414, 0.8153618906942393},
             {1334.5, 1550.5, 1371.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52252,
             1,
             {0.029916061784527326, -0.20088626292466766, 0.8807284694654727, 0.8153618906942393},
             {1297.5, 1550.5, 1333.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52253,
             1,
             {0.031625046159527326, -0.20088626292466766, 0.8824374538404727, 0.8153618906942393},
             {1260.5, 1550.5, 1296.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52257,
             1,
             {0.03543320716395864, -0.20088626292466766, 0.9098792928360414, 0.8153618906942393},
             {1222.5, 1550.5, 1259.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52258,
             1,
             {0.028156951047821267, -0.20088626292466766, 0.9498703927021788, 0.8153618906942393},
             {1182.5, 1550.5, 1221.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52263,
             1,
             {0.03177109778895864, -0.20088626292466766, 0.9062171834610414, 0.8153618906942393},
             {1144.5, 1550.5, 1181.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52264,
             1,
             {0.0240545230566839, -0.22451994091580502, 0.9930353206933162, 0.8153618906942394},
             {855.5, 1549.5, 896.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52265,
             1,
             {0.0208806949316839, -0.22451994091580502, 0.9898614925683162, 0.8153618906942394},
             {897.5, 1549.5, 938.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52268,
             1,
             {0.03318581517725258, -0.1772525849335303, 0.9785329348227475, 0.8153618906942394},
             {980.5, 538.5, 1020.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {52270,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {980.5, 1550.5, 1021.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52272,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1060.5, 1550.5, 1101.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52280,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1102.5, 1550.5, 1143.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52281,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1394.5, 1550.5, 1435.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52283,
             1,
             {0.03220925267725258, -0.20088626292466766, 0.9775563723227475, 0.8153618906942393},
             {1436.5, 1550.5, 1476.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52284,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1670.5, 1550.5, 1711.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52285,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1749.5, 1550.5, 1790.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52286,
             1,
             {0.03220925267725258, -0.20088626292466766, 0.9775563723227475, 0.8153618906942393},
             {1791.5, 1550.5, 1831.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52292,
             1,
             {0.02640000403895864, -0.22451994091580502, 0.9008460897110414, 0.8153618906942394},
             {665.5, 1549.5, 702.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52293,
             1,
             {0.049447311784527326, -0.22451994091580502, 0.9002597194654727, 0.8153618906942394},
             {628.5, 1549.5, 664.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52296,
             1,
             {0.03997379004338995, -0.1772525849335303, 0.93805355370661, 0.8153618906942394},
             {690.5, 408.5, 728.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {52300,
             1,
             {0.05032578528895864, -0.20088626292466766, 0.9247718709610414, 0.8153618906942393},
             {233.5, 1505.5, 270.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52308,
             1,
             {0.049447311784527326, -0.20088626292466766, 0.9002597194654727, 0.8153618906942393},
             {271.5, 1505.5, 307.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52309,
             1,
             {0.049447311784527326, -0.20088626292466766, 0.9002597194654727, 0.8153618906942393},
             {308.5, 1505.5, 344.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52311,
             1,
             {0.04353781042282127, -0.20088626292466766, 0.9652512520771788, 0.8153618906942393},
             {383.5, 1505.5, 422.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52312,
             1,
             {0.04256124792282127, -0.20088626292466766, 0.9642746895771788, 0.8153618906942393},
             {850.5, 1505.5, 889.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52313,
             1,
             {0.04129258216395864, -0.20088626292466766, 0.9157386678360414, 0.8153618906942393},
             {929.5, 1505.5, 966.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52320,
             1,
             {0.0274724918066839, -0.22451994091580502, 0.9964532894433162, 0.8153618906942394},
             {309.5, 1549.5, 350.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52324,
             1,
             {0.03318581517725258, -0.1772525849335303, 0.9785329348227475, 0.8153618906942394},
             {1468.5, 365.5, 1508.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {52326,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1279.5, 1505.5, 1320.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52328,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1404.5, 1505.5, 1445.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52336,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1725.5, 1505.5, 1766.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52341,
             1,
             {0.0208806949316839, -0.20088626292466766, 0.9898614925683162, 0.8153618906942393},
             {1767.5, 1505.5, 1808.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52376,
             1,
             {0.034310593034527326, -0.22451994091580502, 0.8851230007154727, 0.8153618906942394},
             {76.5, 1549.5, 112.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52377,
             1,
             {0.030160202409527326, -0.22451994091580502, 0.8809726100904727, 0.8153618906942394},
             {39.5, 1549.5, 75.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52380,
             1,
             {0.03128281653895864, -0.1772525849335303, 0.9057289022110414, 0.8153618906942394},
             {123.5, 494.5, 160.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52384,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {126.5, 1461.5, 163.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52392,
             1,
             {0.030160202409527326, -0.20088626292466766, 0.8809726100904727, 0.8153618906942393},
             {164.5, 1461.5, 200.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52393,
             1,
             {0.031136764909527326, -0.20088626292466766, 0.8819491725904727, 0.8153618906942393},
             {615.5, 1461.5, 651.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52395,
             1,
             {0.03328390417282127, -0.20088626292466766, 0.9549973458271788, 0.8153618906942393},
             {652.5, 1461.5, 691.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52396,
             1,
             {0.03767843542282127, -0.20088626292466766, 0.9593918770771788, 0.8153618906942393},
             {1375.5, 1461.5, 1414.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52397,
             1,
             {0.040658249284527326, -0.20088626292466766, 0.8914706569654727, 0.8153618906942393},
             {1415.5, 1461.5, 1451.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52404,
             1,
             {0.02932969153895864, -0.22451994091580502, 0.9037757772110414, 0.8153618906942394},
             {1800.5, 1594.5, 1837.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52405,
             1,
             {0.051888718034527326, -0.22451994091580502, 0.9027011257154727, 0.8153618906942394},
             {1763.5, 1594.5, 1799.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52408,
             1,
             {0.05423203528895864, -0.1772525849335303, 0.9286781209610414, 0.8153618906942394},
             {1769.5, 322.5, 1806.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {52412,
             1,
             {0.05081406653895864, -0.20088626292466766, 0.9252601522110414, 0.8153618906942393},
             {1490.5, 1461.5, 1527.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52420,
             1,
             {0.051888718034527326, -0.20088626292466766, 0.9027011257154727, 0.8153618906942393},
             {1528.5, 1461.5, 1564.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {52421,
             1,
             {0.051888718034527326, -0.20088626292466766, 0.9027011257154727, 0.8153618906942393},
             {305.5, 1417.5, 341.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {52423,
             1,
             {0.04695577917282127, -0.20088626292466766, 0.9686692208271788, 0.8153618906942393},
             {381.5, 1417.5, 420.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {52425,
             1,
             {0.04226914466395864, -0.20088626292466766, 0.9167152303360414, 0.8153618906942393},
             {421.5, 1417.5, 458.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {52432,
             1,
             {0.034310593034527326, -0.22451994091580502, 0.8851230007154727, 0.8153618906942394},
             {1493.5, 1594.5, 1529.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52436,
             1,
             {0.03128281653895864, -0.1772525849335303, 0.9057289022110414, 0.8153618906942394},
             {1329.5, 236.5, 1366.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {52452,
             1,
             {0.03719015417282127, -0.20088626292466766, 0.9589035958271788, 0.8153618906942393},
             {665.5, 1417.5, 704.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {52460,
             1,
             {0.02932969153895864, -0.22451994091580502, 0.9037757772110414, 0.8153618906942394},
             {1374.5, 1594.5, 1411.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52464,
             1,
             {0.03997379004338995, -0.1772525849335303, 0.93805355370661, 0.8153618906942394},
             {1080.5, 193.5, 1118.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {52481,
             1,
             {0.04226914466395864, -0.20088626292466766, 0.9167152303360414, 0.8153618906942393},
             {1436.5, 1417.5, 1473.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {52488,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.791728212703102},
             {480.5, 71.5, 520.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {52489,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1215.5, 1594.5, 1255.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52492,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.8153618906942394},
             {1848.5, 193.5, 1888.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {52496,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8389955686853767},
             {1136.5, 1594.5, 1176.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52504,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {38.5, 1373.5, 78.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52505,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {79.5, 1373.5, 119.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52507,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {196.5, 1373.5, 236.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52509,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {445.5, 1373.5, 485.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52516,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {943.5, 1594.5, 985.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52520,
             1,
             {0.006622449686115212, -0.20088626292466766, 0.9992369253138849, 0.8153618906942393},
             {486.5, 1373.5, 528.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52524,
             1,
             {0.005157605936115212, -0.20088626292466766, 0.9977720815638849, 0.8153618906942393},
             {804.5, 1373.5, 846.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52537,
             1,
             {0.005157605936115212, -0.20088626292466766, 0.9977720815638849, 0.8153618906942393},
             {847.5, 1373.5, 889.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52572,
             1,
             {0.02664414466395864, -0.22451994091580502, 0.9010902303360414, 0.8153618906942394},
             {783.5, 1594.5, 820.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52576,
             1,
             {0.03640976966395864, -0.20088626292466766, 0.9108558553360414, 0.8153618906942393},
             {1348.5, 1373.5, 1385.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52580,
             1,
             {0.03738633216395864, -0.20088626292466766, 0.9118324178360414, 0.8153618906942393},
             {1386.5, 1373.5, 1423.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52588,
             1,
             {0.044564499284527326, -0.1772525849335303, 0.8953769069654727, 0.8153618906942394},
             {1315.5, 150.5, 1351.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52589,
             1,
             {0.044564499284527326, -0.20088626292466766, 0.8953769069654727, 0.8153618906942393},
             {1870.5, 1373.5, 1906.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52591,
             1,
             {0.04680972754338995, -0.20088626292466766, 0.94488949120661, 0.8153618906942393},
             {1907.5, 1373.5, 1945.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {52593,
             1,
             {0.046517624284527326, -0.20088626292466766, 0.8973300319654727, 0.8153618906942393},
             {37.5, 1329.5, 73.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52600,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.791728212703102},
             {439.5, 71.5, 479.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {52616,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {597.5, 1329.5, 637.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52628,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {62.5, 2002.5, 102.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {52629,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {395.5, 1594.5, 435.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52632,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.8153618906942394},
             {1625.5, 150.5, 1665.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52636,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8389955686853767},
             {313.5, 1594.5, 353.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52644,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {992.5, 1329.5, 1032.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52645,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1261.5, 1329.5, 1301.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52647,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1302.5, 1329.5, 1342.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52649,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1343.5, 1329.5, 1383.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52656,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {118.5, 1594.5, 155.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52676,
             1,
             {0.03880104955225258, -0.22451994091580502, 0.9841481691977475, 0.8153618906942394},
             {77.5, 1594.5, 117.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52684,
             1,
             {0.0043292587933899535, -0.22451994091580502, 0.90240902245661, 0.8153618906942394},
             {38.5, 1594.5, 76.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52688,
             1,
             {0.03640976966395864, -0.20088626292466766, 0.9108558553360414, 0.8153618906942393},
             {423.5, 1505.5, 460.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {52712,
             1,
             {0.02176133216395864, -0.22451994091580502, 0.8962074178360414, 0.8153618906942394},
             {0.5, 1594.5, 37.5, 1638.5}});
        intermediate.mGlyphVector.push_back(
            {52716,
             1,
             {0.04519883216395864, -0.20088626292466766, 0.9196449178360414, 0.8153618906942393},
             {1538.5, 1329.5, 1575.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52720,
             1,
             {0.04617539466395864, -0.20088626292466766, 0.9206214803360414, 0.8153618906942393},
             {1576.5, 1329.5, 1613.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52728,
             1,
             {0.044564499284527326, -0.1772525849335303, 0.8953769069654727, 0.8153618906942394},
             {1666.5, 150.5, 1702.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52729,
             1,
             {0.044564499284527326, -0.20088626292466766, 0.8953769069654727, 0.8153618906942393},
             {1655.5, 1329.5, 1691.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52731,
             1,
             {0.04680972754338995, -0.20088626292466766, 0.94488949120661, 0.8153618906942393},
             {1842.5, 1329.5, 1880.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52733,
             1,
             {0.046517624284527326, -0.20088626292466766, 0.8973300319654727, 0.8153618906942393},
             {1881.5, 1329.5, 1917.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {52740,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {103.5, 2002.5, 143.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {52744,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {196.5, 1285.5, 236.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52748,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8389955686853767},
             {1655.5, 1639.5, 1695.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52756,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {278.5, 1285.5, 318.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52761,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {521.5, 1285.5, 561.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52768,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.791728212703102},
             {398.5, 71.5, 438.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {52769,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {1491.5, 1639.5, 1531.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52772,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.8153618906942394},
             {1504.5, 150.5, 1544.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52776,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8389955686853767},
             {1414.5, 1639.5, 1454.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52784,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1055.5, 1285.5, 1095.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52785,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1096.5, 1285.5, 1136.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52787,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1179.5, 1285.5, 1219.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52789,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1430.5, 1285.5, 1470.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52824,
             1,
             {0.038705124284527326, -0.22451994091580502, 0.8895175319654727, 0.8153618906942394},
             {1221.5, 1639.5, 1257.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52825,
             1,
             {0.04783641640509601, -0.22451994091580502, 0.875015146094904, 0.8153618906942394},
             {1185.5, 1639.5, 1220.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52828,
             1,
             {0.046517624284527326, -0.1772525849335303, 0.8973300319654727, 0.8153618906942394},
             {1467.5, 150.5, 1503.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52831,
             1,
             {0.047005905534527326, -0.20088626292466766, 0.8978183132154727, 0.8153618906942393},
             {1839.5, 1285.5, 1875.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52832,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {1958.5, 1285.5, 1994.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {52833,
             1,
             {0.04783641640509601, -0.22451994091580502, 0.875015146094904, 0.8153618906942394},
             {1023.5, 1639.5, 1058.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52840,
             1,
             {0.04539501015509601, -0.20088626292466766, 0.872573739844904, 0.8153618906942393},
             {192.5, 1241.5, 227.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52841,
             1,
             {0.04637157265509601, -0.20088626292466766, 0.873550302344904, 0.8153618906942393},
             {228.5, 1241.5, 263.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52843,
             1,
             {0.03670187292282127, -0.20088626292466766, 0.9584153145771788, 0.8153618906942393},
             {700.5, 1241.5, 739.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52845,
             1,
             {0.044076218034527326, -0.20088626292466766, 0.8948886257154727, 0.8153618906942393},
             {775.5, 1241.5, 811.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52852,
             1,
             {0.0384588199316839, -0.22451994091580502, 1.0074396175683162, 0.8153618906942394},
             {832.5, 1639.5, 873.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52853,
             1,
             {0.03465065892725258, -0.22451994091580502, 0.9799977785727475, 0.8153618906942394},
             {791.5, 1639.5, 831.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52856,
             1,
             {0.03538308080225258, -0.1772525849335303, 0.9807302004477475, 0.8153618906942394},
             {825.5, 150.5, 865.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52860,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1079.5, 1241.5, 1119.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52868,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {1156.5, 1241.5, 1196.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52869,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1347.5, 1241.5, 1387.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52871,
             1,
             {0.03416237767725258, -0.20088626292466766, 0.9795094973227475, 0.8153618906942393},
             {1493.5, 1241.5, 1533.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52873,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {1690.5, 1241.5, 1730.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52880,
             1,
             {0.02713242591395864, -0.22451994091580502, 0.9015785115860414, 0.8153618906942394},
             {535.5, 1639.5, 572.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52881,
             1,
             {0.049447311784527326, -0.22451994091580502, 0.9002597194654727, 0.8153618906942394},
             {498.5, 1639.5, 534.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52884,
             1,
             {0.03802066504338995, -0.1772525849335303, 0.93610042870661, 0.8153618906942394},
             {629.5, 150.5, 667.5, 192.5}});
        intermediate.mGlyphVector.push_back(
            {52888,
             1,
             {0.04983750403895864, -0.20088626292466766, 0.9242835897110414, 0.8153618906942393},
             {1890.5, 1241.5, 1927.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {52896,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {240.5, 1197.5, 276.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52897,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {277.5, 1197.5, 313.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52899,
             1,
             {0.04256124792282127, -0.20088626292466766, 0.9642746895771788, 0.8153618906942393},
             {580.5, 1197.5, 619.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52900,
             1,
             {0.04207296667282127, -0.20088626292466766, 0.9637864083271788, 0.8153618906942393},
             {620.5, 1197.5, 659.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52901,
             1,
             {0.04471055091395864, -0.20088626292466766, 0.9191566365860414, 0.8153618906942393},
             {1020.5, 1197.5, 1057.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52908,
             1,
             {0.04295144017725258, -0.22451994091580502, 0.9882985598227475, 0.8153618906942394},
             {195.5, 1639.5, 235.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52909,
             1,
             {0.03465065892725258, -0.22451994091580502, 0.9799977785727475, 0.8153618906942394},
             {154.5, 1639.5, 194.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52929,
             1,
             {0.03465065892725258, -0.20088626292466766, 0.9799977785727475, 0.8153618906942393},
             {1215.5, 1197.5, 1255.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52964,
             1,
             {0.026986374284527326, -0.22451994091580502, 0.8777987819654727, 0.8153618906942394},
             {77.5, 1639.5, 113.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52965,
             1,
             {0.039681686784527326, -0.22451994091580502, 0.8904940944654727, 0.8153618906942394},
             {40.5, 1639.5, 76.5, 1683.5}});
        intermediate.mGlyphVector.push_back(
            {52968,
             1,
             {0.04178086341395864, -0.1772525849335303, 0.9162269490860414, 0.8153618906942394},
             {1765.5, 494.5, 1802.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {52971,
             1,
             {0.03836289466395864, -0.20088626292466766, 0.9128089803360414, 0.8153618906942393},
             {1022.5, 1550.5, 1059.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {52972,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {1256.5, 1197.5, 1293.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52980,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {1525.5, 1197.5, 1561.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52981,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {1562.5, 1197.5, 1598.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52983,
             1,
             {0.04256124792282127, -0.20088626292466766, 0.9642746895771788, 0.8153618906942393},
             {1713.5, 1197.5, 1752.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {52984,
             1,
             {0.03928933080225258, -0.20088626292466766, 0.9846364504477475, 0.8153618906942393},
             {79.5, 1153.5, 119.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {52985,
             1,
             {0.043587936784527326, -0.20088626292466766, 0.8944003444654727, 0.8153618906942393},
             {120.5, 1153.5, 156.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {52992,
             1,
             {0.02762070716395864, -0.22451994091580502, 0.9020667928360414, 0.8153618906942394},
             {1804.5, 1684.5, 1841.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {52993,
             1,
             {0.048470749284527326, -0.22451994091580502, 0.8992831569654727, 0.8153618906942394},
             {1767.5, 1684.5, 1803.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {52996,
             1,
             {0.03899722754338995, -0.1772525849335303, 0.93707699120661, 0.8153618906942394},
             {1608.5, 193.5, 1646.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {53000,
             1,
             {0.04934922278895864, -0.20088626292466766, 0.9237953084610414, 0.8153618906942393},
             {195.5, 1153.5, 232.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53008,
             1,
             {0.048470749284527326, -0.20088626292466766, 0.8992831569654727, 0.8153618906942393},
             {697.5, 1153.5, 733.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53009,
             1,
             {0.048470749284527326, -0.20088626292466766, 0.8992831569654727, 0.8153618906942393},
             {734.5, 1153.5, 770.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53011,
             1,
             {0.04158468542282127, -0.20088626292466766, 0.9632981270771788, 0.8153618906942393},
             {771.5, 1153.5, 810.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53013,
             1,
             {0.04422226966395864, -0.20088626292466766, 0.9186683553360414, 0.8153618906942393},
             {811.5, 1153.5, 848.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53020,
             1,
             {0.026986374284527326, -0.22451994091580502, 0.8777987819654727, 0.8153618906942394},
             {1495.5, 1684.5, 1531.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53024,
             1,
             {0.03982773841395864, -0.1772525849335303, 0.9142738240860414, 0.8153618906942394},
             {927.5, 193.5, 964.5, 235.5}});
        intermediate.mGlyphVector.push_back(
            {53028,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {924.5, 1153.5, 961.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53036,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {1424.5, 1153.5, 1460.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53037,
             1,
             {0.039681686784527326, -0.20088626292466766, 0.8904940944654727, 0.8153618906942393},
             {1730.5, 1153.5, 1766.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53039,
             1,
             {0.04207296667282127, -0.20088626292466766, 0.9637864083271788, 0.8153618906942393},
             {1767.5, 1153.5, 1806.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {53040,
             1,
             {0.03928933080225258, -0.20088626292466766, 0.9846364504477475, 0.8153618906942393},
             {40.5, 1109.5, 80.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {53041,
             1,
             {0.043587936784527326, -0.20088626292466766, 0.8944003444654727, 0.8153618906942393},
             {81.5, 1109.5, 117.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {53048,
             1,
             {0.03079453528895864, -0.22451994091580502, 0.9052406209610414, 0.8153618906942394},
             {1176.5, 1684.5, 1213.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53076,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {913.5, 33.5, 953.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {53077,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {445.5, 1109.5, 485.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {53080,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {819.5, 108.5, 859.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {53084,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1735.5, 236.5, 1775.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53092,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1694.5, 236.5, 1734.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53093,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1124.5, 236.5, 1164.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53095,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1083.5, 236.5, 1123.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53097,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1042.5, 236.5, 1082.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53104,
             1,
             {0.020294324686115212, -0.22451994091580502, 1.0129088003138849, 0.8153618906942394},
             {817.5, 1684.5, 859.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53105,
             1,
             {0.0201482730566839, -0.22451994091580502, 0.9891290706933162, 0.8153618906942394},
             {775.5, 1684.5, 816.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53108,
             1,
             {0.0225896793066839, -0.20088626292466766, 0.9915704769433162, 0.8153618906942393},
             {974.5, 1065.5, 1015.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {53112,
             1,
             {0.0225896793066839, -0.20088626292466766, 0.9915704769433162, 0.8153618906942393},
             {38.5, 1065.5, 79.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {53120,
             1,
             {0.0225896793066839, -0.1772525849335303, 0.9915704769433162, 0.8153618906942394},
             {1000.5, 236.5, 1041.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53125,
             1,
             {0.0230779605566839, -0.20088626292466766, 0.9920587581933162, 0.8153618906942393},
             {1091.5, 1065.5, 1132.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {53132,
             1,
             {0.010921055668389953, -0.22451994091580502, 0.90900081933161, 0.8153618906942394},
             {580.5, 1684.5, 618.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53153,
             1,
             {0.02762070716395864, -0.20088626292466766, 0.9020667928360414, 0.8153618906942393},
             {1133.5, 1065.5, 1170.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {53160,
             1,
             {0.044076218034527326, -0.22451994091580502, 0.8948886257154727, 0.8153618906942394},
             {506.5, 1684.5, 542.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53168,
             1,
             {0.048959030534527326, -0.20088626292466766, 0.8997714382154727, 0.8153618906942393},
             {1330.5, 1065.5, 1366.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {53188,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {1077.5, 33.5, 1117.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {53216,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {82.5, 236.5, 122.5, 278.5}});
        intermediate.mGlyphVector.push_back(
            {53217,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {77.5, 1021.5, 117.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {53220,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1620.5, 109.5, 1660.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {53224,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1208.5, 279.5, 1248.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53232,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1128.5, 279.5, 1168.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53233,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1087.5, 279.5, 1127.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53235,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {970.5, 279.5, 1010.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53237,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {929.5, 279.5, 969.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53244,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {74.5, 1684.5, 111.5, 1728.5}});
        intermediate.mGlyphVector.push_back(
            {53248,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {1302.5, 1021.5, 1339.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {53252,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {1477.5, 1550.5, 1514.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {53265,
             1,
             {0.053109421159527326, -0.20088626292466766, 0.9039218288404727, 0.8153618906942393},
             {1340.5, 1021.5, 1376.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {53272,
             1,
             {0.0062823837933899535, -0.22451994091580502, 0.90436214745661, 0.8153618906942394},
             {2001.5, 1729.5, 2039.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53293,
             1,
             {0.0070148056683899535, -0.20088626292466766, 0.90509456933161, 0.8153618906942393},
             {1533.5, 1021.5, 1571.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {53300,
             1,
             {0.02420273841395864, -0.22451994091580502, 0.8986488240860414, 0.8153618906942394},
             {1925.5, 1729.5, 1962.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53301,
             1,
             {0.043587936784527326, -0.22451994091580502, 0.8944003444654727, 0.8153618906942394},
             {1888.5, 1729.5, 1924.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53304,
             1,
             {0.04568711341395864, -0.1772525849335303, 0.9201331990860414, 0.8153618906942394},
             {400.5, 279.5, 437.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53308,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {1861.5, 1021.5, 1898.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {53316,
             1,
             {0.041146530534527326, -0.1772525849335303, 0.8919589382154727, 0.8153618906942394},
             {934.5, 408.5, 970.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {53317,
             1,
             {0.043587936784527326, -0.1772525849335303, 0.8944003444654727, 0.8153618906942394},
             {363.5, 279.5, 399.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53319,
             1,
             {0.04207296667282127, -0.20088626292466766, 0.9637864083271788, 0.8153618906942393},
             {450.5, 977.5, 489.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {53321,
             1,
             {0.04226914466395864, -0.20088626292466766, 0.9167152303360414, 0.8153618906942393},
             {611.5, 977.5, 648.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {53328,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7680945347119646},
             {280.5, 279.5, 320.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53332,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1948.5, 109.5, 1988.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {53336,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {123.5, 279.5, 163.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53344,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {82.5, 279.5, 122.5, 321.5}});
        intermediate.mGlyphVector.push_back(
            {53356,
             1,
             {0.02732644017725258, -0.03545051698670606, 0.9726735598227475, 0.7208271787296899},
             {1176.5, 34.5, 1216.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {53357,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {72.5, 933.5, 112.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53360,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.7680945347119646},
             {1661.5, 109.5, 1701.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {53364,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {589.5, 322.5, 629.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {53372,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {511.5, 322.5, 551.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {53373,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {470.5, 322.5, 510.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {53377,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {429.5, 322.5, 469.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {53412,
             1,
             {0.04783641640509601, -0.22451994091580502, 0.875015146094904, 0.8153618906942394},
             {1193.5, 1729.5, 1228.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53413,
             1,
             {0.061996572655096005, -0.22451994091580502, 0.889175302344904, 0.8153618906942394},
             {1157.5, 1729.5, 1192.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53416,
             1,
             {0.06311918678452733, -0.1772525849335303, 0.9139315944654727, 0.8153618906942394},
             {392.5, 322.5, 428.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {53420,
             1,
             {0.06458403053452733, -0.20088626292466766, 0.9153964382154727, 0.8153618906942393},
             {944.5, 933.5, 980.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53428,
             1,
             {0.062973135155096, -0.20088626292466766, 0.890151864844904, 0.8153618906942393},
             {981.5, 933.5, 1016.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53429,
             1,
             {0.063461416405096, -0.20088626292466766, 0.890640146094904, 0.8153618906942393},
             {1386.5, 933.5, 1421.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53431,
             1,
             {0.06341129004338994, -0.20088626292466766, 0.96149105370661, 0.8153618906942393},
             {1422.5, 933.5, 1460.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53433,
             1,
             {0.069809072655096, -0.20088626292466766, 0.896987802344904, 0.8153618906942393},
             {1774.5, 933.5, 1809.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53440,
             1,
             {0.09392886816838994, -0.22451994091580502, 0.99200863183161, 0.8153618906942394},
             {877.5, 1729.5, 915.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53441,
             1,
             {0.08733707129338994, -0.22451994091580502, 0.98541683495661, 0.8153618906942394},
             {838.5, 1729.5, 876.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53444,
             1,
             {0.08880191504338994, -0.20088626292466766, 0.98688167870661, 0.8153618906942393},
             {1810.5, 933.5, 1848.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {53448,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {420.5, 889.5, 458.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53449,
             1,
             {0.08684879004338994, -0.22451994091580502, 0.98492855370661, 0.8153618906942394},
             {732.5, 1729.5, 770.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53456,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {459.5, 889.5, 497.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53457,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {498.5, 889.5, 536.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53459,
             1,
             {0.08684879004338994, -0.20088626292466766, 0.98492855370661, 0.8153618906942393},
             {537.5, 889.5, 575.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53460,
             1,
             {0.06697531042282126, -0.20088626292466766, 0.9886887520771788, 0.8153618906942393},
             {614.5, 889.5, 653.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53461,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {736.5, 889.5, 774.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53468,
             1,
             {0.086410635155096, -0.22451994091580502, 0.913589364844904, 0.8153618906942394},
             {504.5, 1729.5, 539.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53469,
             1,
             {0.09700677102566468, -0.22451994091580502, 0.9005518227243354, 0.8153618906942394},
             {469.5, 1729.5, 503.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53472,
             1,
             {0.08753324928452733, -0.1772525849335303, 0.9383456569654727, 0.8153618906942394},
             {1591.5, 365.5, 1627.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53476,
             1,
             {0.097885244530096, -0.20088626292466766, 0.925063974219904, 0.8153618906942393},
             {1377.5, 889.5, 1412.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53484,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1413.5, 889.5, 1447.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53485,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1448.5, 889.5, 1482.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53487,
             1,
             {0.09705473365952733, -0.20088626292466766, 0.9478671413404727, 0.8153618906942393},
             {1483.5, 889.5, 1519.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53488,
             1,
             {0.09671250403895862, -0.20088626292466766, 0.9711585897110414, 0.8153618906942393},
             {1520.5, 889.5, 1557.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53489,
             1,
             {0.09700677102566468, -0.20088626292466766, 0.9005518227243354, 0.8153618906942393},
             {1601.5, 889.5, 1635.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53496,
             1,
             {0.09392886816838994, -0.22451994091580502, 0.99200863183161, 0.8153618906942394},
             {151.5, 1729.5, 189.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53517,
             1,
             {0.08733707129338994, -0.20088626292466766, 0.98541683495661, 0.8153618906942393},
             {1636.5, 889.5, 1674.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53552,
             1,
             {0.10296423502123336, -0.22451994091580502, 0.8828756087287667, 0.8153618906942394},
             {74.5, 1729.5, 107.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53553,
             1,
             {0.1284529490168021, -0.22451994091580502, 0.8847306447331981, 0.8153618906942394},
             {41.5, 1729.5, 73.5, 1773.5}});
        intermediate.mGlyphVector.push_back(
            {53556,
             1,
             {0.13104040689623336, -0.20088626292466766, 0.9109517806037667, 0.8153618906942393},
             {1865.5, 889.5, 1898.5, 932.5}});
        intermediate.mGlyphVector.push_back(
            {53560,
             1,
             {0.13152868814623336, -0.20088626292466766, 0.9114400618537667, 0.8153618906942393},
             {0.5, 845.5, 33.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53562,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {34.5, 845.5, 66.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53568,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {245.5, 845.5, 277.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53569,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {278.5, 845.5, 310.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53571,
             1,
             {0.12659574928452733, -0.20088626292466766, 0.9774081569654727, 0.8153618906942393},
             {604.5, 845.5, 640.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53572,
             1,
             {0.10672226966395862, -0.20088626292466766, 0.9811683553360414, 0.8153618906942393},
             {641.5, 845.5, 678.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53573,
             1,
             {0.12542517252123336, -0.20088626292466766, 0.9053365462287667, 0.8153618906942393},
             {715.5, 845.5, 748.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53580,
             1,
             {0.09456536477566468, -0.22451994091580502, 0.8981104164743354, 0.8153618906942394},
             {1758.5, 1774.5, 1792.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53581,
             1,
             {0.10066888040066468, -0.22451994091580502, 0.9042139320993354, 0.8153618906942394},
             {1723.5, 1774.5, 1757.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53584,
             1,
             {0.107406728905096, -0.1772525849335303, 0.934585458594904, 0.8153618906942394},
             {1043.5, 365.5, 1078.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53588,
             1,
             {0.101547353905096, -0.20088626292466766, 0.928726083594904, 0.8153618906942393},
             {1025.5, 845.5, 1060.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53596,
             1,
             {0.10555169290066468, -0.20088626292466766, 0.9090967445993354, 0.8153618906942393},
             {1139.5, 845.5, 1173.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53597,
             1,
             {0.10555169290066468, -0.20088626292466766, 0.9090967445993354, 0.8153618906942393},
             {1174.5, 845.5, 1208.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53599,
             1,
             {0.11145903053452733, -0.20088626292466766, 0.9622714382154727, 0.8153618906942393},
             {1502.5, 845.5, 1538.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53601,
             1,
             {0.11116692727566468, -0.20088626292466766, 0.9147119789743354, 0.8153618906942393},
             {1539.5, 845.5, 1573.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53608,
             1,
             {0.10345251627123336, -0.22451994091580502, 0.8833638899787667, 0.8153618906942394},
             {1473.5, 1774.5, 1506.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53612,
             1,
             {0.13055212564623336, -0.20088626292466766, 0.9104634993537667, 0.8153618906942393},
             {1847.5, 845.5, 1880.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53628,
             1,
             {0.09671250403895862, -0.20088626292466766, 0.9711585897110414, 0.8153618906942393},
             {1997.5, 845.5, 2034.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {53636,
             1,
             {0.10091302102566468, -0.22451994091580502, 0.9044580727243354, 0.8153618906942394},
             {1363.5, 1774.5, 1397.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53640,
             1,
             {0.105941885155096, -0.1772525849335303, 0.933120614844904, 0.8153618906942394},
             {931.5, 365.5, 966.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53664,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {339.5, 32.5, 379.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {53665,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 801.5, 40.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {53668,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {890.5, 365.5, 930.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53672,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {815.5, 365.5, 855.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53680,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {0.5, 365.5, 40.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53681,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {410.5, 365.5, 450.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {53683,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1915.5, 408.5, 1955.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {53685,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1229.5, 801.5, 1269.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {53690,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {852.5, 408.5, 892.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {53692,
             1,
             {0.015167371561115212, -0.22451994091580502, 1.0077818471888849, 0.8153618906942394},
             {940.5, 1774.5, 982.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53696,
             1,
             {0.009307996561115212, -0.20088626292466766, 1.0019224721888849, 0.8153618906942393},
             {154.5, 757.5, 196.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53720,
             1,
             {0.0018878525433899535, -0.22451994091580502, 0.89996761620661, 0.8153618906942394},
             {864.5, 1774.5, 902.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53748,
             1,
             {0.02371445716395864, -0.22451994091580502, 0.8981605428360414, 0.8153618906942394},
             {826.5, 1774.5, 863.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53752,
             1,
             {0.03396836341395864, -0.20088626292466766, 0.9084144490860414, 0.8153618906942393},
             {240.5, 757.5, 277.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53767,
             1,
             {0.031086638547821267, -0.20088626292466766, 0.9528000802021788, 0.8153618906942393},
             {649.5, 757.5, 688.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53769,
             1,
             {0.032601608659527326, -0.22451994091580502, 0.8834140163404727, 0.8153618906942394},
             {704.5, 1774.5, 740.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53776,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1694.5, 73.5, 1734.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {53804,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {941.5, 757.5, 981.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53805,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1238.5, 757.5, 1278.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53808,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1621.5, 451.5, 1661.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53812,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1422.5, 451.5, 1462.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53820,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1381.5, 451.5, 1421.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53821,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1340.5, 451.5, 1380.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53823,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1828.5, 757.5, 1868.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {53825,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {109.5, 713.5, 149.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {53832,
             1,
             {0.02517930091395864, -0.22451994091580502, 0.8996253865860414, 0.8153618906942394},
             {341.5, 1774.5, 378.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53852,
             1,
             {0.05354757604782127, -0.22451994091580502, 0.9752610177021788, 0.8153618906942394},
             {301.5, 1774.5, 340.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53860,
             1,
             {0.0067706650433899535, -0.22451994091580502, 0.90485042870661, 0.8153618906942394},
             {262.5, 1774.5, 300.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53888,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {224.5, 1774.5, 261.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53889,
             1,
             {0.046029343034527326, -0.22451994091580502, 0.8968417507154727, 0.8153618906942394},
             {187.5, 1774.5, 223.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53892,
             1,
             {0.04324570716395864, -0.20088626292466766, 0.9176917928360414, 0.8153618906942393},
             {150.5, 713.5, 187.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {53896,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {500.5, 713.5, 537.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {53904,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {694.5, 713.5, 730.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {53905,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1868.5, 1550.5, 1904.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {53909,
             1,
             {0.055062546159527326, -0.22451994091580502, 0.9058749538404727, 0.8153618906942394},
             {0.5, 1774.5, 36.5, 1818.5}});
        intermediate.mGlyphVector.push_back(
            {53916,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1944.5, 1550.5, 1984.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {53920,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1218.5, 451.5, 1258.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53924,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1177.5, 451.5, 1217.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53932,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1136.5, 451.5, 1176.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53937,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1782.5, 713.5, 1822.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {53944,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {872.5, 33.5, 912.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {53945,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {0.5, 669.5, 40.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {53948,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1601.5, 494.5, 1641.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {53951,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {246.5, 451.5, 286.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53952,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {205.5, 451.5, 245.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {53954,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1724.5, 494.5, 1764.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {53960,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1683.5, 494.5, 1723.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {53961,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1642.5, 494.5, 1682.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {53963,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {922.5, 669.5, 962.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {53972,
             1,
             {0.040169968034527326, -0.22451994091580502, 0.8909823757154727, 0.8153618906942394},
             {1545.5, 1819.5, 1581.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {53976,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {1004.5, 669.5, 1041.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {53980,
             1,
             {0.03323594153895864, -0.20088626292466766, 0.9076820272110414, 0.8153618906942393},
             {1459.5, 669.5, 1496.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {53988,
             1,
             {0.032113327409527326, -0.20088626292466766, 0.8829257350904727, 0.8153618906942393},
             {1752.5, 669.5, 1788.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {53989,
             1,
             {0.04637157265509601, -0.20088626292466766, 0.873550302344904, 0.8153618906942393},
             {1789.5, 669.5, 1824.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {54000,
             1,
             {0.11272986002123336, -0.22451994091580502, 0.8926412337287667, 0.8153618906942394},
             {1358.5, 1819.5, 1391.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54001,
             1,
             {0.1289412302668021, -0.22451994091580502, 0.8852189259831981, 0.8153618906942394},
             {1325.5, 1819.5, 1357.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54004,
             1,
             {0.12957556314623336, -0.20088626292466766, 0.9094869368537667, 0.8153618906942393},
             {0.5, 625.5, 33.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54008,
             1,
             {0.12957556314623336, -0.20088626292466766, 0.9094869368537667, 0.8153618906942393},
             {277.5, 625.5, 310.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54016,
             1,
             {0.1308943552668021, -0.20088626292466766, 0.8871720509831981, 0.8153618906942393},
             {34.5, 625.5, 66.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54017,
             1,
             {0.1279646677668021, -0.20088626292466766, 0.8842423634831981, 0.8153618906942393},
             {311.5, 625.5, 343.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54019,
             1,
             {0.12317778053452733, -0.20088626292466766, 0.9739901882154727, 0.8153618906942393},
             {344.5, 625.5, 380.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54021,
             1,
             {0.12542517252123336, -0.20088626292466766, 0.9053365462287667, 0.8153618906942393},
             {498.5, 625.5, 531.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54028,
             1,
             {0.009063855936115212, -0.22451994091580502, 1.0016783315638849, 0.8153618906942394},
             {1074.5, 1819.5, 1116.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54029,
             1,
             {0.03538308080225258, -0.22451994091580502, 0.9807302004477475, 0.8153618906942394},
             {1033.5, 1819.5, 1073.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54030,
             1,
             {0.03538308080225258, -0.22451994091580502, 0.9807302004477475, 0.8153618906942394},
             {992.5, 1819.5, 1032.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54032,
             1,
             {0.03538308080225258, -0.1772525849335303, 0.9807302004477475, 0.8153618906942394},
             {1224.5, 365.5, 1264.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54036,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {833.5, 625.5, 873.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54038,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {874.5, 625.5, 914.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54044,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {915.5, 625.5, 955.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54045,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {1307.5, 625.5, 1347.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54047,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {1348.5, 625.5, 1388.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54048,
             1,
             {0.03782448705225258, -0.20088626292466766, 0.9831716066977475, 0.8153618906942393},
             {1610.5, 625.5, 1650.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54049,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {1801.5, 625.5, 1841.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54053,
             1,
             {0.03538308080225258, -0.20088626292466766, 0.9807302004477475, 0.8153618906942393},
             {269.5, 581.5, 309.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54056,
             1,
             {0.012141758793389953, -0.22451994091580502, 0.91022152245661, 0.8153618906942394},
             {610.5, 1819.5, 648.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54057,
             1,
             {0.02664414466395864, -0.22451994091580502, 0.9010902303360414, 0.8153618906942394},
             {572.5, 1819.5, 609.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54060,
             1,
             {0.028987461918389953, -0.1772525849335303, 0.92706722558161, 0.8153618906942394},
             {325.5, 494.5, 363.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {54064,
             1,
             {0.027522618168389953, -0.20088626292466766, 0.92560238183161, 0.8153618906942393},
             {659.5, 581.5, 697.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54072,
             1,
             {0.02664414466395864, -0.20088626292466766, 0.9010902303360414, 0.8153618906942393},
             {698.5, 581.5, 735.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54073,
             1,
             {0.02664414466395864, -0.20088626292466766, 0.9010902303360414, 0.8153618906942393},
             {778.5, 581.5, 815.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54075,
             1,
             {0.021711205802252585, -0.20088626292466766, 0.9670583254477475, 0.8153618906942393},
             {1048.5, 581.5, 1088.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54076,
             1,
             {0.019758080802252585, -0.20088626292466766, 0.9651052004477475, 0.8153618906942393},
             {1355.5, 581.5, 1395.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54077,
             1,
             {0.018977696293389953, -0.20088626292466766, 0.91705745995661, 0.8153618906942393},
             {1396.5, 581.5, 1434.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54084,
             1,
             {0.0036927621861152123, -0.22451994091580502, 0.9963072378138849, 0.8153618906942394},
             {259.5, 1819.5, 301.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54085,
             1,
             {0.03538308080225258, -0.22451994091580502, 0.9807302004477475, 0.8153618906942394},
             {218.5, 1819.5, 258.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54140,
             1,
             {0.01736680091395864, -0.22451994091580502, 0.8918128865860414, 0.8153618906942394},
             {180.5, 1819.5, 217.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54141,
             1,
             {0.060043447655096005, -0.22451994091580502, 0.887222177344904, 0.8153618906942394},
             {144.5, 1819.5, 179.5, 1863.5}});
        intermediate.mGlyphVector.push_back(
            {54144,
             1,
             {0.06067778053452732, -0.1772525849335303, 0.9114901882154727, 0.8153618906942394},
             {1626.5, 538.5, 1662.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54148,
             1,
             {0.06116606178452732, -0.20088626292466766, 0.9119784694654727, 0.8153618906942393},
             {1803.5, 581.5, 1839.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54156,
             1,
             {0.060043447655096005, -0.20088626292466766, 0.887222177344904, 0.8153618906942393},
             {1840.5, 581.5, 1875.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54157,
             1,
             {0.060043447655096005, -0.20088626292466766, 0.887222177344904, 0.8153618906942393},
             {1832.5, 1550.5, 1867.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54159,
             1,
             {0.062434727543389947, -0.20088626292466766, 0.96051449120661, 0.8153618906942393},
             {1905.5, 1550.5, 1943.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54160,
             1,
             {0.061458165043389947, -0.20088626292466766, 0.95953792870661, 0.8153618906942393},
             {75.5, 537.5, 113.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54161,
             1,
             {0.05701567115952732, -0.20088626292466766, 0.9078280788404727, 0.8153618906942393},
             {114.5, 537.5, 150.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54168,
             1,
             {0.012385899418389953, -0.22451994091580502, 0.91046566308161, 0.8153618906942394},
             {1951.5, 1864.5, 1989.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54169,
             1,
             {0.02664414466395864, -0.22451994091580502, 0.9010902303360414, 0.8153618906942394},
             {1913.5, 1864.5, 1950.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54172,
             1,
             {0.028987461918389953, -0.1772525849335303, 0.92706722558161, 0.8153618906942394},
             {1547.5, 538.5, 1585.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54176,
             1,
             {0.027522618168389953, -0.20088626292466766, 0.92560238183161, 0.8153618906942393},
             {513.5, 537.5, 551.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54184,
             1,
             {0.02664414466395864, -0.20088626292466766, 0.9010902303360414, 0.8153618906942393},
             {863.5, 537.5, 900.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54185,
             1,
             {0.02664414466395864, -0.20088626292466766, 0.9010902303360414, 0.8153618906942393},
             {901.5, 537.5, 938.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54187,
             1,
             {0.020734643302252585, -0.20088626292466766, 0.9660817629477475, 0.8153618906942393},
             {939.5, 537.5, 979.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54189,
             1,
             {0.018977696293389953, -0.20088626292466766, 0.91705745995661, 0.8153618906942393},
             {816.5, 581.5, 854.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54196,
             1,
             {0.00613633216395864, -0.22451994091580502, 0.8805824178360414, 0.8153618906942394},
             {1653.5, 1864.5, 1690.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54200,
             1,
             {0.06067778053452732, -0.1772525849335303, 0.9114901882154727, 0.8153618906942394},
             {1865.5, 538.5, 1901.5, 580.5}});
        intermediate.mGlyphVector.push_back(
            {54204,
             1,
             {0.06116606178452732, -0.20088626292466766, 0.9119784694654727, 0.8153618906942393},
             {464.5, 581.5, 500.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54212,
             1,
             {0.060043447655096005, -0.20088626292466766, 0.887222177344904, 0.8153618906942393},
             {428.5, 581.5, 463.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54213,
             1,
             {0.060043447655096005, -0.20088626292466766, 0.887222177344904, 0.8153618906942393},
             {310.5, 581.5, 345.5, 624.5}});
        intermediate.mGlyphVector.push_back(
            {54216,
             1,
             {0.061946446293389947, -0.20088626292466766, 0.96002620995661, 0.8153618906942393},
             {1842.5, 625.5, 1880.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54217,
             1,
             {0.05701567115952732, -0.22451994091580502, 0.9078280788404727, 0.8153618906942394},
             {1421.5, 1864.5, 1457.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54224,
             1,
             {0.012385899418389953, -0.22451994091580502, 0.91046566308161, 0.8153618906942394},
             {1382.5, 1864.5, 1420.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54232,
             1,
             {0.027522618168389953, -0.20088626292466766, 0.92560238183161, 0.8153618906942393},
             {1571.5, 625.5, 1609.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54241,
             1,
             {0.02664414466395864, -0.20088626292466766, 0.9010902303360414, 0.8153618906942393},
             {795.5, 625.5, 832.5, 668.5}});
        intermediate.mGlyphVector.push_back(
            {54243,
             1,
             {0.021711205802252585, -0.20088626292466766, 0.9670583254477475, 0.8153618906942393},
             {1418.5, 669.5, 1458.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {54252,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1344.5, 73.5, 1384.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {54253,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {1083.5, 669.5, 1123.5, 712.5}});
        intermediate.mGlyphVector.push_back(
            {54256,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {0.5, 108.5, 40.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {54260,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1878.5, 494.5, 1918.5, 536.5}});
        intermediate.mGlyphVector.push_back(
            {54268,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {323.5, 451.5, 363.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54269,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {364.5, 451.5, 404.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54271,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {405.5, 451.5, 445.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54273,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {446.5, 451.5, 486.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54280,
             1,
             {0.017852918436115212, -0.22451994091580502, 1.0104673940638849, 0.8153618906942394},
             {939.5, 1864.5, 981.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54301,
             1,
             {0.006622449686115212, -0.20088626292466766, 0.9992369253138849, 0.8153618906942393},
             {1324.5, 713.5, 1366.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {54336,
             1,
             {0.02224961341395864, -0.22451994091580502, 0.8966956990860414, 0.8153618906942394},
             {862.5, 1864.5, 899.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54340,
             1,
             {0.03543320716395864, -0.20088626292466766, 0.9098792928360414, 0.8153618906942393},
             {1286.5, 713.5, 1323.5, 756.5}});
        intermediate.mGlyphVector.push_back(
            {54364,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.7208271787296898},
             {1530.5, 73.5, 1570.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {54368,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {82.5, 108.5, 122.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {54372,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1504.5, 451.5, 1544.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54381,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1545.5, 451.5, 1585.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54383,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1884.5, 451.5, 1924.5, 493.5}});
        intermediate.mGlyphVector.push_back(
            {54392,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {393.5, 757.5, 433.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {54393,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {278.5, 757.5, 318.5, 800.5}});
        intermediate.mGlyphVector.push_back(
            {54396,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {532.5, 108.5, 572.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {54399,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {971.5, 408.5, 1011.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {54400,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1012.5, 408.5, 1052.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {54402,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1053.5, 408.5, 1093.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {54408,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1956.5, 408.5, 1996.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {54409,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1997.5, 408.5, 2037.5, 450.5}});
        intermediate.mGlyphVector.push_back(
            {54411,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {760.5, 801.5, 800.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {54413,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {569.5, 365.5, 609.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54420,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {215.5, 1864.5, 252.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54441,
             1,
             {0.03738633216395864, -0.20088626292466766, 0.9118324178360414, 0.8153618906942393},
             {599.5, 801.5, 636.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {54476,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {141.5, 1864.5, 178.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54480,
             1,
             {0.04080430091395864, -0.20088626292466766, 0.9152503865860414, 0.8153618906942393},
             {390.5, 801.5, 427.5, 844.5}});
        intermediate.mGlyphVector.push_back(
            {54484,
             1,
             {0.04178086341395864, -0.20088626292466766, 0.9162269490860414, 0.8153618906942393},
             {1959.5, 845.5, 1996.5, 888.5}});
        intermediate.mGlyphVector.push_back(
            {54492,
             1,
             {0.041146530534527326, -0.20088626292466766, 0.8919589382154727, 0.8153618906942393},
             {1515.5, 1550.5, 1551.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54495,
             1,
             {0.04417214330225259, -0.22451994091580502, 0.9895192629477475, 0.8153618906942394},
             {0.5, 1864.5, 40.5, 1908.5}});
        intermediate.mGlyphVector.push_back(
            {54504,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.7917282127031019},
             {2007.5, 933.5, 2047.5, 976.5}});
        intermediate.mGlyphVector.push_back(
            {54508,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {860.5, 108.5, 900.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {54512,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1703.5, 365.5, 1743.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54520,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1744.5, 365.5, 1784.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54523,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1785.5, 365.5, 1825.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54525,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {1987.5, 365.5, 2027.5, 407.5}});
        intermediate.mGlyphVector.push_back(
            {54532,
             1,
             {0.02732644017725258, -0.05908419497784343, 0.9726735598227475, 0.7208271787296899},
             {626.5, 33.5, 666.5, 66.5}});
        intermediate.mGlyphVector.push_back(
            {54536,
             1,
             {0.02732644017725258, -0.1772525849335303, 0.9726735598227475, 0.791728212703102},
             {1065.5, 108.5, 1105.5, 149.5}});
        intermediate.mGlyphVector.push_back(
            {54540,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {310.5, 322.5, 350.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {54548,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {351.5, 322.5, 391.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {54549,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {753.5, 322.5, 793.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {54551,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.791728212703102},
             {915.5, 322.5, 955.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {54588,
             1,
             {0.02884141028895864, -0.22451994091580502, 0.9032874959610414, 0.8153618906942394},
             {1600.5, 1909.5, 1637.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54589,
             1,
             {0.061020010155096005, -0.22451994091580502, 0.888198739844904, 0.8153618906942394},
             {1564.5, 1909.5, 1599.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54592,
             1,
             {0.06214262428452732, -0.1772525849335303, 0.9129550319654727, 0.8153618906942394},
             {956.5, 322.5, 992.5, 364.5}});
        intermediate.mGlyphVector.push_back(
            {54596,
             1,
             {0.06214262428452732, -0.20088626292466766, 0.9129550319654727, 0.8153618906942393},
             {1646.5, 977.5, 1682.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54604,
             1,
             {0.061020010155096005, -0.20088626292466766, 0.888198739844904, 0.8153618906942393},
             {1219.5, 977.5, 1254.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54605,
             1,
             {0.061020010155096005, -0.20088626292466766, 0.888198739844904, 0.8153618906942393},
             {1183.5, 977.5, 1218.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54607,
             1,
             {0.058040196293389947, -0.20088626292466766, 0.95611995995661, 0.8153618906942393},
             {1065.5, 977.5, 1103.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54609,
             1,
             {0.05701567115952732, -0.20088626292466766, 0.9078280788404727, 0.8153618906942393},
             {1028.5, 977.5, 1064.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54616,
             1,
             {0.0296697574316839, -0.22451994091580502, 0.9986505550683162, 0.8153618906942394},
             {1297.5, 1909.5, 1338.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54617,
             1,
             {0.03367409642725258, -0.22451994091580502, 0.9790212160727475, 0.8153618906942394},
             {1256.5, 1909.5, 1296.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54620,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {690.5, 977.5, 730.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54624,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8389955686853767},
             {1174.5, 1909.5, 1214.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54629,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {649.5, 977.5, 689.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54632,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {115.5, 977.5, 155.5, 1020.5}});
        intermediate.mGlyphVector.push_back(
            {54633,
             1,
             {0.028791283927252585, -0.20088626292466766, 0.9741384035727475, 0.8389955686853767},
             {1056.5, 1909.5, 1096.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54635,
             1,
             {0.029279565177252585, -0.20088626292466766, 0.9746266848227475, 0.8153618906942393},
             {1782.5, 1021.5, 1822.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {54637,
             1,
             {0.028791283927252585, -0.20088626292466766, 0.9741384035727475, 0.8153618906942393},
             {736.5, 1021.5, 776.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {54643,
             1,
             {0.03367409642725258, -0.22451994091580502, 0.9790212160727475, 0.8389955686853768},
             {144.5, 2002.5, 184.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54644,
             1,
             {0.019954258793389953, -0.22451994091580502, 0.91803402245661, 0.8153618906942394},
             {902.5, 1909.5, 940.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54645,
             1,
             {0.03128281653895864, -0.22451994091580502, 0.9057289022110414, 0.8153618906942394},
             {864.5, 1909.5, 901.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54648,
             1,
             {0.03240543066838995, -0.20088626292466766, 0.93048519433161, 0.8153618906942393},
             {388.5, 1021.5, 426.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {54652,
             1,
             {0.03240543066838995, -0.20088626292466766, 0.93048519433161, 0.8153618906942393},
             {38.5, 1021.5, 76.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {54660,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {1844.5, 1065.5, 1881.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54661,
             1,
             {0.03128281653895864, -0.20088626292466766, 0.9057289022110414, 0.8153618906942393},
             {1806.5, 1065.5, 1843.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54663,
             1,
             {0.031086638547821267, -0.20088626292466766, 0.9528000802021788, 0.8153618906942393},
             {1725.5, 1065.5, 1764.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54664,
             1,
             {0.027814721427252585, -0.22451994091580502, 0.9731618410727475, 0.8153618906942394},
             {620.5, 1909.5, 660.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54665,
             1,
             {0.03421250403895864, -0.20088626292466766, 0.9086585897110414, 0.8153618906942393},
             {1493.5, 1065.5, 1530.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54672,
             1,
             {0.0272283511816839, -0.22451994091580502, 0.9962091488183162, 0.8153618906942394},
             {538.5, 1909.5, 579.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54693,
             1,
             {0.03367409642725258, -0.20088626292466766, 0.9790212160727475, 0.8153618906942393},
             {858.5, 1065.5, 898.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54728,
             1,
             {0.030160202409527326, -0.22451994091580502, 0.8809726100904727, 0.8153618906942394},
             {463.5, 1909.5, 499.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54729,
             1,
             {0.036263718034527326, -0.22451994091580502, 0.8870761257154727, 0.8153618906942394},
             {426.5, 1909.5, 462.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54732,
             1,
             {0.03689805091395864, -0.20088626292466766, 0.9113441365860414, 0.8153618906942393},
             {779.5, 1065.5, 816.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54736,
             1,
             {0.03738633216395864, -0.20088626292466766, 0.9118324178360414, 0.8153618906942393},
             {741.5, 1065.5, 778.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54738,
             1,
             {0.036263718034527326, -0.20088626292466766, 0.8870761257154727, 0.8153618906942393},
             {391.5, 1065.5, 427.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54744,
             1,
             {0.036263718034527326, -0.20088626292466766, 0.8870761257154727, 0.8153618906942393},
             {354.5, 1065.5, 390.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54745,
             1,
             {0.036263718034527326, -0.20088626292466766, 0.8870761257154727, 0.8153618906942393},
             {317.5, 1065.5, 353.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54747,
             1,
             {0.04060812292282127, -0.20088626292466766, 0.9623215645771788, 0.8153618906942393},
             {237.5, 1065.5, 276.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54749,
             1,
             {0.041146530534527326, -0.20088626292466766, 0.8919589382154727, 0.8153618906942393},
             {200.5, 1065.5, 236.5, 1108.5}});
        intermediate.mGlyphVector.push_back(
            {54756,
             1,
             {0.018489415043389953, -0.22451994091580502, 0.91656917870661, 0.8153618906942394},
             {119.5, 1909.5, 157.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54757,
             1,
             {0.03006211341395864, -0.22451994091580502, 0.9045081990860414, 0.8153618906942394},
             {81.5, 1909.5, 118.5, 1953.5}});
        intermediate.mGlyphVector.push_back(
            {54760,
             1,
             {0.03240543066838995, -0.20088626292466766, 0.93048519433161, 0.8153618906942393},
             {1349.5, 1109.5, 1387.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {54764,
             1,
             {0.030940586918389953, -0.20088626292466766, 0.92902035058161, 0.8153618906942393},
             {1552.5, 1550.5, 1590.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54772,
             1,
             {0.03006211341395864, -0.20088626292466766, 0.9045081990860414, 0.8153618906942393},
             {1591.5, 1550.5, 1628.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54773,
             1,
             {0.03006211341395864, -0.20088626292466766, 0.9045081990860414, 0.8153618906942393},
             {942.5, 1109.5, 979.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {54775,
             1,
             {0.03548116979782127, -0.20088626292466766, 0.9571946114521788, 0.8153618906942393},
             {902.5, 1109.5, 941.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {54777,
             1,
             {0.03421250403895864, -0.20088626292466766, 0.9086585897110414, 0.8153618906942393},
             {864.5, 1109.5, 901.5, 1152.5}});
        intermediate.mGlyphVector.push_back(
            {54784,
             1,
             {0.01883164466395864, -0.22451994091580502, 0.8932777303360414, 0.8153618906942394},
             {1904.5, 1954.5, 1941.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54785,
             1,
             {0.036263718034527326, -0.22451994091580502, 0.8870761257154727, 0.8153618906942394},
             {1867.5, 1954.5, 1903.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54788,
             1,
             {0.03689805091395864, -0.20088626292466766, 0.9113441365860414, 0.8153618906942393},
             {1159.5, 1153.5, 1196.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54792,
             1,
             {0.03738633216395864, -0.20088626292466766, 0.9118324178360414, 0.8153618906942393},
             {1121.5, 1153.5, 1158.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54800,
             1,
             {0.036263718034527326, -0.20088626292466766, 0.8870761257154727, 0.8153618906942393},
             {1084.5, 1153.5, 1120.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54801,
             1,
             {0.036263718034527326, -0.20088626292466766, 0.8870761257154727, 0.8153618906942393},
             {849.5, 1153.5, 885.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54803,
             1,
             {0.04060812292282127, -0.20088626292466766, 0.9623215645771788, 0.8153618906942393},
             {657.5, 1153.5, 696.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54804,
             1,
             {0.03294167455225258, -0.22451994091580502, 0.9782887941977475, 0.8153618906942394},
             {1637.5, 1954.5, 1677.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54805,
             1,
             {0.041146530534527326, -0.20088626292466766, 0.8919589382154727, 0.8153618906942393},
             {507.5, 1153.5, 543.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54812,
             1,
             {0.018489415043389953, -0.22451994091580502, 0.91656917870661, 0.8153618906942394},
             {1560.5, 1954.5, 1598.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54816,
             1,
             {0.03240543066838995, -0.20088626292466766, 0.93048519433161, 0.8153618906942393},
             {348.5, 1153.5, 386.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54820,
             1,
             {0.030940586918389953, -0.20088626292466766, 0.92902035058161, 0.8153618906942393},
             {233.5, 1153.5, 271.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54829,
             1,
             {0.03006211341395864, -0.20088626292466766, 0.9045081990860414, 0.8153618906942393},
             {157.5, 1153.5, 194.5, 1196.5}});
        intermediate.mGlyphVector.push_back(
            {54840,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.8153618906942393},
             {285.5, 69.5, 325.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {54841,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {185.5, 2002.5, 225.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54844,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1484.5, 1197.5, 1524.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {54848,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {226.5, 2002.5, 266.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54853,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {267.5, 2002.5, 307.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54856,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {308.5, 2002.5, 348.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54857,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {349.5, 2002.5, 389.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54859,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {390.5, 2002.5, 430.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54861,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {431.5, 2002.5, 471.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54865,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {472.5, 2002.5, 512.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54868,
             1,
             {0.009063855936115212, -0.22451994091580502, 1.0016783315638849, 0.8153618906942394},
             {1010.5, 1954.5, 1052.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54869,
             1,
             {0.0245428043066839, -0.22451994091580502, 0.9935236019433162, 0.8153618906942394},
             {968.5, 1954.5, 1009.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54872,
             1,
             {0.0264959293066839, -0.20088626292466766, 0.9954767269433162, 0.8153618906942393},
             {1828.5, 1373.5, 1869.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {54876,
             1,
             {0.0264959293066839, -0.20088626292466766, 0.9954767269433162, 0.8389955686853767},
             {888.5, 1954.5, 929.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54887,
             1,
             {0.0269842105566839, -0.22451994091580502, 0.9959650081933162, 0.8153618906942394},
             {846.5, 1954.5, 887.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54889,
             1,
             {0.0269842105566839, -0.20088626292466766, 0.9959650081933162, 0.8153618906942393},
             {1750.5, 1373.5, 1791.5, 1416.5}});
        intermediate.mGlyphVector.push_back(
            {54896,
             1,
             {0.0033526962933899535, -0.22451994091580502, 0.90143245995661, 0.8153618906942394},
             {770.5, 1954.5, 808.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54897,
             1,
             {0.01931992591395864, -0.22451994091580502, 0.8937660115860414, 0.8153618906942394},
             {732.5, 1954.5, 769.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54900,
             1,
             {0.017024571293389953, -0.20088626292466766, 0.91510433495661, 0.8153618906942393},
             {2009.5, 1417.5, 2047.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {54915,
             1,
             {0.012922143302252585, -0.22451994091580502, 0.9582692629477475, 0.8153618906942394},
             {654.5, 1954.5, 694.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54917,
             1,
             {0.019465977543389953, -0.22451994091580502, 0.91754574120661, 0.8153618906942394},
             {615.5, 1954.5, 653.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54924,
             1,
             {0.02640000403895864, -0.22451994091580502, 0.9008460897110414, 0.8153618906942394},
             {577.5, 1954.5, 614.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54925,
             1,
             {0.04783641640509601, -0.22451994091580502, 0.875015146094904, 0.8153618906942394},
             {541.5, 1954.5, 576.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54928,
             1,
             {0.03030625403895864, -0.20088626292466766, 0.9047523397110414, 0.8153618906942393},
             {1667.5, 1417.5, 1704.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {54932,
             1,
             {0.03787461341395864, -0.20088626292466766, 0.9123206990860414, 0.8153618906942393},
             {627.5, 1417.5, 664.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {54941,
             1,
             {0.032113327409527326, -0.20088626292466766, 0.8829257350904727, 0.8153618906942393},
             {268.5, 1417.5, 304.5, 1460.5}});
        intermediate.mGlyphVector.push_back(
            {54943,
             1,
             {0.03865499792282127, -0.22451994091580502, 0.9603684395771788, 0.8153618906942394},
             {389.5, 1954.5, 428.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54945,
             1,
             {0.037240280534527326, -0.22451994091580502, 0.8880526882154727, 0.8153618906942394},
             {352.5, 1954.5, 388.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54952,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.8153618906942393},
             {244.5, 69.5, 284.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {54956,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1839.5, 1461.5, 1879.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {54960,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {513.5, 2002.5, 553.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54969,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {554.5, 2002.5, 594.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54971,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {964.5, 2002.5, 1004.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54980,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {116.5, 1954.5, 156.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {54981,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1046.5, 2002.5, 1086.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54984,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {939.5, 1550.5, 979.5, 1593.5}});
        intermediate.mGlyphVector.push_back(
            {54988,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1251.5, 2002.5, 1291.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54993,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1333.5, 2002.5, 1373.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54996,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1374.5, 2002.5, 1414.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {54999,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1415.5, 2002.5, 1455.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55001,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1456.5, 2002.5, 1496.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55008,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1881.5, 2003.5, 1918.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55012,
             1,
             {0.059701218034527326, -0.20088626292466766, 0.9105136257154727, 0.8153618906942393},
             {653.5, 1505.5, 689.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55016,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {615.5, 1505.5, 652.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55024,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {578.5, 1505.5, 614.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55029,
             1,
             {0.054574264909527326, -0.22451994091580502, 0.9053866725904727, 0.8153618906942394},
             {1753.5, 2003.5, 1789.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55036,
             1,
             {0.0067706650433899535, -0.22451994091580502, 0.90485042870661, 0.8153618906942394},
             {1714.5, 2003.5, 1752.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55037,
             1,
             {0.02029648841395864, -0.22451994091580502, 0.8947425740860414, 0.8153618906942394},
             {1676.5, 2003.5, 1713.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55040,
             1,
             {0.017024571293389953, -0.20088626292466766, 0.91510433495661, 0.8153618906942393},
             {539.5, 1505.5, 577.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55044,
             1,
             {0.020930821293389953, -0.20088626292466766, 0.91901058495661, 0.8153618906942393},
             {500.5, 1505.5, 538.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55057,
             1,
             {0.008479649418389953, -0.20088626292466766, 0.90655941308161, 0.8153618906942393},
             {461.5, 1505.5, 499.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55064,
             1,
             {0.02273789466395864, -0.22451994091580502, 0.8971839803360414, 0.8153618906942394},
             {1580.5, 2003.5, 1617.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55065,
             1,
             {0.041146530534527326, -0.22451994091580502, 0.8919589382154727, 0.8153618906942394},
             {1543.5, 2003.5, 1579.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55068,
             1,
             {0.04275742591395864, -0.20088626292466766, 0.9172035115860414, 0.8153618906942393},
             {345.5, 1505.5, 382.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55072,
             1,
             {0.04666367591395864, -0.20088626292466766, 0.9211097615860414, 0.8153618906942393},
             {195.5, 1505.5, 232.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55080,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {690.5, 1505.5, 726.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55081,
             1,
             {0.046029343034527326, -0.20088626292466766, 0.8968417507154727, 0.8153618906942393},
             {1242.5, 1505.5, 1278.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55083,
             1,
             {0.04466042455225259, -0.22451994091580502, 0.9900075441977475, 0.8153618906942394},
             {1998.5, 2003.5, 2038.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55085,
             1,
             {0.03543320716395864, -0.22451994091580502, 0.9098792928360414, 0.8153618906942394},
             {41.5, 1954.5, 78.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55092,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8153618906942394},
             {0.5, 1954.5, 40.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55093,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1292.5, 2002.5, 1332.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55096,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1103.5, 1461.5, 1143.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {55100,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1210.5, 2002.5, 1250.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55108,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1169.5, 2002.5, 1209.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55111,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1128.5, 2002.5, 1168.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55113,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1087.5, 2002.5, 1127.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55120,
             1,
             {0.02732644017725258, -0.08271787296898081, 0.9726735598227475, 0.8153618906942393},
             {164.5, 69.5, 204.5, 107.5}});
        intermediate.mGlyphVector.push_back(
            {55121,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {1005.5, 2002.5, 1045.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55124,
             1,
             {0.02732644017725258, -0.20088626292466766, 0.9726735598227475, 0.8153618906942393},
             {1798.5, 1461.5, 1838.5, 1504.5}});
        intermediate.mGlyphVector.push_back(
            {55126,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {923.5, 2002.5, 963.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55127,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {882.5, 2002.5, 922.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55128,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {841.5, 2002.5, 881.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55129,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {800.5, 2002.5, 840.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55136,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {759.5, 2002.5, 799.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55137,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {718.5, 2002.5, 758.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55139,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {677.5, 2002.5, 717.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55141,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {636.5, 2002.5, 676.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55145,
             1,
             {0.02732644017725258, -0.22451994091580502, 0.9726735598227475, 0.8389955686853768},
             {595.5, 2002.5, 635.5, 2047.5}});
        intermediate.mGlyphVector.push_back(
            {55148,
             1,
             {0.02835312903895864, -0.22451994091580502, 0.9027992147110414, 0.8153618906942394},
             {239.5, 1954.5, 276.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55152,
             1,
             {0.03250351966395864, -0.20088626292466766, 0.9069496053360414, 0.8153618906942393},
             {562.5, 1285.5, 599.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {55156,
             1,
             {0.053597702409527326, -0.20088626292466766, 0.9044101100904727, 0.8153618906942393},
             {1995.5, 1285.5, 2031.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {55164,
             1,
             {0.05174266640509601, -0.20088626292466766, 0.878921396094904, 0.8153618906942393},
             {1001.5, 1241.5, 1036.5, 1284.5}});
        intermediate.mGlyphVector.push_back(
            {55165,
             1,
             {0.05223094765509601, -0.20088626292466766, 0.879409677344904, 0.8153618906942393},
             {544.5, 1197.5, 579.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {55169,
             1,
             {0.046029343034527326, -0.22451994091580502, 0.8968417507154727, 0.8153618906942394},
             {1221.5, 1954.5, 1257.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55176,
             1,
             {0.04637157265509601, -0.22451994091580502, 0.873550302344904, 0.8153618906942394},
             {1258.5, 1954.5, 1293.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55177,
             1,
             {0.061996572655096005, -0.22451994091580502, 0.889175302344904, 0.8153618906942394},
             {1294.5, 1954.5, 1329.5, 1998.5}});
        intermediate.mGlyphVector.push_back(
            {55180,
             1,
             {0.06409574928452733, -0.20088626292466766, 0.9149081569654727, 0.8153618906942393},
             {1753.5, 1197.5, 1789.5, 1240.5}});
        intermediate.mGlyphVector.push_back(
            {55184,
             1,
             {0.06214262428452732, -0.20088626292466766, 0.9129550319654727, 0.8153618906942393},
             {427.5, 1021.5, 463.5, 1064.5}});
        intermediate.mGlyphVector.push_back(
            {55192,
             1,
             {0.061508291405096005, -0.20088626292466766, 0.888687021094904, 0.8153618906942393},
             {82.5, 1285.5, 117.5, 1328.5}});
        intermediate.mGlyphVector.push_back(
            {55193,
             1,
             {0.061508291405096005, -0.20088626292466766, 0.888687021094904, 0.8153618906942393},
             {716.5, 1329.5, 751.5, 1372.5}});
        intermediate.mGlyphVector.push_back(
            {55195,
             1,
             {0.05867452917282126, -0.20088626292466766, 0.9803879708271788, 0.8153618906942393},
             {155.5, 1505.5, 194.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55197,
             1,
             {0.05750395240952732, -0.20088626292466766, 0.9083163600904727, 0.8153618906942393},
             {118.5, 1505.5, 154.5, 1548.5}});
        intermediate.mGlyphVector.push_back(
            {55203,
             1,
             {0.024837071293389953, -0.22451994091580502, 0.92291683495661, 0.8389955686853768},
             {23.5, 2002.5, 61.5, 2047.5}});
    }

    for (int i = 0; i < intermediate.mGlyphVector.size(); ++i)
    {

#ifdef D3DX
        intermediate.mGlyphVector[i].mAltasBounds.mBottom /= intermediate.mFontAltas.mWidth;
        intermediate.mGlyphVector[i].mAltasBounds.mTop /= intermediate.mFontAltas.mWidth;
        intermediate.mGlyphVector[i].mAltasBounds.mRight /= intermediate.mFontAltas.mWidth;
        intermediate.mGlyphVector[i].mAltasBounds.mLeft /= intermediate.mFontAltas.mWidth;

#endif
        intermediate.mGlyphVector[i].mAltasBounds.mTop = 1.0f - intermediate.mGlyphVector[i].mAltasBounds.mTop;
        intermediate.mGlyphVector[i].mAltasBounds.mBottom = 1.0f - intermediate.mGlyphVector[i].mAltasBounds.mBottom;
    }

    mDefaultFont = CreateAsset(EAssetType::eFont, intermediate, "Engine", true);

    return {pTex, mDefaultFont};
}

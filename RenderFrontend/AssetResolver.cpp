#include "AssetResolver.h"
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetManager.h>

#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/StaticMesh.h>
#include <CoreAsset/Texture.h>
#include <D3DGpuResourceManager/GpuBufferContextSystem.h>
#include <D3DGpuResourceManager/GpuBufferContextSystemImpl.h>
#include <D3DGpuResourceManager/IGpuResource.h>
#include <D3DGpuResourceManager/IGpuResourceManager.h>
#include <RenderFrontend/RenderMaterialResolver.h>
#include <RenderFrontend/RenderUploadManager.h>
#include <RenderFrontend/ShaderData.h>

Render::AssetResolver *Render::AssetResolver::GetInstance()
{
    static AssetResolver instance;

    return &instance;
}

Render::AssetResolver::AssetResolver() : mIsWorkerRunning(true) {}

Render::AssetResolver::~AssetResolver() {}

void Render::AssetResolver::Initialize(CoreAsset::AssetManager *assetManager,
                                       GRM::IGpuResourceManager *gpuResourceManager)
{
    mAssetManager = assetManager;
    mGpuResourceManager = gpuResourceManager;

    RegisterBuiltInAsset();
    BuildGpuBuffers();
    RenderMaterialResolver::GetInstance()->Initialize();

    mWorkerThread = std::thread(&Render::AssetResolver::WokerThreadLoop, this);
}

bool Render::AssetResolver::RequestResolveAsset(CoreAsset::Asset *asset)
{

    if (!asset)
        return true;

    CoreAsset::AssetID assetID = asset->GetID();

    bool requestToWorker = false;

    {
        std::lock_guard lock(mAssetTableMutex);

        switch (asset->GetType())
        {
        case CoreAsset::EAssetType::eTexture:
        {
            if (mAssetGpuResourceTable.find(assetID) == mAssetGpuResourceTable.end())
            {
                requestToWorker = true;
            }
        }
        break;
        // TODO 메시는 사실지금 작동하지않음.
        case CoreAsset::EAssetType::eStaticMesh:
        {
            if (mMeshGpuResourceTable.find(assetID) == mMeshGpuResourceTable.end())
            {
                requestToWorker = true;
            }
        }
        break;
        }
    }

    if (requestToWorker)
    {
        PushResolveRequest(asset);
    }

    return true;
}

bool Render::AssetResolver::ResolveAsset(CoreAsset::Asset *asset) const
{

    if (asset == nullptr)
        return true;

    switch (asset->GetType())
    {
    case CoreAsset::EAssetType::eTexture:
        return ResolveTexture(static_cast<CoreAsset::Texture *>(asset));

        break;

    case CoreAsset::EAssetType::eMaterial:

        return ResolveMaterial(static_cast<CoreAsset::Material *>(asset));

        break;

    case CoreAsset::EAssetType::eStaticMesh:

        return ResolveStaticMesh(static_cast<CoreAsset::StaticMesh *>(asset));
        break;
    }
    // 향후 다른타입에셋들도 추가

    return true;
}

bool Render::AssetResolver::ResolveAsset(CoreAsset::AssetID assetID) const
{

    CoreAsset ::Asset *asset = mAssetManager->GetAssetCommon(assetID).Get();
    if (asset == nullptr)
        return false;

    return ResolveAsset(asset);
}

GRM::GRMPtr Render::AssetResolver::GetGpuResource(CoreAsset::Asset *asset) const
{
    if (asset == nullptr)
        return nullptr;

    CoreAsset::AssetID assetID = asset->GetID();
    if (assetID == NoneAssetID)
        return nullptr;

    std::shared_lock lock(mAssetTableMutex);

    std::unordered_map<CoreAsset::AssetID, GRM::GRMPtr>::const_iterator it = mAssetGpuResourceTable.find(assetID);

    if (it != mAssetGpuResourceTable.cend())
        return it->second;

    if (asset->GetType() == CoreAsset::EAssetType::eTexture)
    {
        return mGpuResourceManager->GetDefaultTexture();
    }
    else
    {
        // TODO
        // 현재는 Texture만 디폴트정보 리턴
        // 다른 리소스들은 nullptr , 아직 나머지 asset들에대해 구조가 잡히지않았다.
        return nullptr;
    }
}

// GRM::GRMPtr Render::AssetResolver::GetGpuResource(CoreAsset::AssetID assetID) const
//{
//
//     // TOOD 이제없으면 대타를 리턴(일단 텍스처에 한해서)
//
//     // mAssetGpuResourceTable
//
//     // return DefaultResource() 일단 텍스처
//     // return mAssetGpuResourceTable[mDefaultTextureAssetID];
// }

Render::MeshGpuResourceContext Render::AssetResolver::GetMeshGpuResourceContext(CoreAsset::Asset *asset) const
{

    if (asset == nullptr)
        return {};

    return GetMeshGpuResourceContext(asset->GetID());
}
Render::MeshGpuResourceContext Render::AssetResolver::GetMeshGpuResourceContext(CoreAsset::AssetID assetID) const
{

    if (assetID == NoneAssetID)
        return {};

    std::shared_lock lock(mAssetTableMutex);

    std::unordered_map<CoreAsset::AssetID, MeshGpuResourceContext>::const_iterator it =
        mMeshGpuResourceTable.find(assetID);

    if (it == mMeshGpuResourceTable.cend())
        return {};

    return it->second;
}

void Render::AssetResolver::EndResourceResolveThread()
{

    mIsWorkerRunning = false;

    mCV.notify_one();

    if (mWorkerThread.joinable())
    {
        mWorkerThread.join();
    }
}

int Render::AssetResolver::GetLightStructuredGpuBufferID() const
{
    return mLightGpuBufferContextID;
}

int Render::AssetResolver::GetBillboardStructuredGpuBufferID() const
{
    return mBillboardGpuBufferContextID;
}

GRM::GRMPtr Render::AssetResolver::GetDefaultTexture() const
{
    return mGpuResourceManager->GetDefaultTexture();
}

bool Render::AssetResolver::ResolveMaterial(CoreAsset::Material *material) const
{

    // material 에서 참조하는 texture resource들을대해서
    // 재귀적으로 ResolveTexture를 호출한다.
    //  material자체는 메타데이터와 raw데이터가 같이 로드되기에따로 raw데이터처리는없다.
    return true;
}

bool Render::AssetResolver::ResolveTexture(CoreAsset::Texture *texture) const
{

    if (texture == nullptr)
        return true;

  // auto assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    bool bEngine = false;

    CoreAsset::AssetID assetID = texture->GetID();

    GRM::GRMPtr grmPtr = GetGpuResource(texture);

    // 먼저 texture의 raw데이터가 로드되어서 gpuResource가 존재하는지
    if (grmPtr.getResource() != mGpuResourceManager->GetDefaultTexture())
    {
        // 이미 존재
        return true;
    }
    // 없다면 assetManagaer에게 rawData 로드 요청

    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    if (texture->GetLoadState() != CoreAsset::Asset::LoadState::Loaded)
    {
        assetManager->LoadAssetRawData(texture);
    }
    GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

    GRM::TextureDesc textureDesc;
    textureDesc = texture->GetProperties().mMetaData;

    GRM::GRMPtr gpuResource = gpuResourceManager->CreateTexture(textureDesc);

    if (gpuResource.getResource() == nullptr)
    {
        // log
        // 실패했다는것을 알려야하고, 상위시스템은 렌더링을 막아야할것이다.
        return false;
    }

    // 그후 gpuResourceManager에게 적절한 데이터를 넘겨서 gpuResourcec생성
    RegisterGpuReosurce(texture->GetID(), gpuResource);
    // 그렇게 생성된 gpuResource를 texture 에셋 멤버변수에 대입

    // empty flag = true 처리 //보류

    // 리턴
    return true;
}

bool Render::AssetResolver::ResolveStaticMesh(CoreAsset::StaticMesh *mesh) const
{

    if (mesh == nullptr)
        return false;

    // mesh index, vertex

    MeshGpuResourceContext meshGpuResourceContext = GetMeshGpuResourceContext(mesh->GetID());
    // vertex buffer 가있다는것은 이미 존재한다는것
    if (meshGpuResourceContext.mVertexBuffer.getResource() == nullptr)
    {
        // 없는 경우에만 raw data load, gpuResource 생성 수행
        CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

        if (mesh->IsEmptyAsset())
            assetManager->LoadAssetRawData(mesh);

        GRM::IGpuResourceManager *gpuResourceManager = GRM::IGpuResourceManager::GetInstance();

        GRM::BufferDesc vertexBufferDesc;
        vertexBufferDesc.mBufferUsage = GRM::EBufferUsage::eVertexBuffer;
        vertexBufferDesc.mElementDataSize = sizeof(CoreAsset::StaticVertex);
        vertexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eGpuOnly;
        vertexBufferDesc.mElementDataNum = mesh->GetVertexNum();
        vertexBufferDesc.mData = mesh->GetVertexVector().data();
        vertexBufferDesc.mBufferSize = vertexBufferDesc.mElementDataSize * vertexBufferDesc.mElementDataNum;

        GRM::GRMPtr vertexBufferGpuResource = gpuResourceManager->CreateBuffer(vertexBufferDesc);

        if (vertexBufferGpuResource.getResource() == nullptr)
        {
            // log
            // 실패했다는것을 알려야하고, 상위시스템은 렌더링을 막아야할것이다.
            return false;
        }

        GRM::BufferDesc indexBufferDesc;
        indexBufferDesc.mBufferUsage = GRM::EBufferUsage::eIndexBuffer;
        indexBufferDesc.mElementDataSize = sizeof(CoreAsset::MeshIndexType);
        indexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eGpuOnly;
        indexBufferDesc.mElementDataNum = mesh->GetIndexNum();
        indexBufferDesc.mData = mesh->GetMeshIndexVector().data();
        indexBufferDesc.mBufferSize = indexBufferDesc.mElementDataSize * indexBufferDesc.mElementDataNum;

        GRM::GRMPtr indexBufferGpuResource = gpuResourceManager->CreateBuffer(indexBufferDesc);

        if (indexBufferGpuResource.getResource() == nullptr)
        {
            // log
            // 실패했다는것을 알려야하고, 상위시스템은 렌더링을 막아야할것이다.
            return false;
        }

        meshGpuResourceContext.mVertexBuffer = vertexBufferGpuResource;
        meshGpuResourceContext.mIndexBuffer = indexBufferGpuResource;

        RegisterMeshGpuResourceContext(mesh->GetID(), meshGpuResourceContext);
    }
    // 재귀적으로 submesh에서사용하는 머터리얼 처리
    // submesh material 재귀적으로

    const std::vector<CoreAsset::SubMesh> &subMeshVector = mesh->GetSubMeshVector();
    for (const auto &subMesh : subMeshVector)
    {
        if (ResolveAsset(subMesh.mMaterialID) == false)
        {
            // LOG
            //  향후 디폴트로 처리하든지 등등 방안필요
        }
    }

    return true;
}

bool ResolveSkinningMesh(CoreAsset::Mesh *mesh)
{

    return true;
}

void Render::AssetResolver::RegisterGpuReosurce(CoreAsset::AssetID assetID, GRM::GRMPtr ptr) const
{

    std::unique_lock lock(mAssetTableMutex);

    mAssetGpuResourceTable[assetID] = ptr;
}

void Render::AssetResolver::RegisterMeshGpuResourceContext(CoreAsset::AssetID assetID,
                                                           MeshGpuResourceContext context) const
{
    std::unique_lock lock(mAssetTableMutex);
    mMeshGpuResourceTable[assetID] = context;
}

void Render::AssetResolver::WokerThreadLoop()
{

    while (1)
    {
        CoreAsset::Asset *asset = nullptr;
        {
            std::unique_lock lock(mRequestQueueMutex);

            mCV.wait(lock, [this]() { return !mRequestQueue.empty() || !mIsWorkerRunning; });

            if (mRequestQueue.empty() && !mIsWorkerRunning)
            {

                break;
            }

            asset = mRequestQueue.front();
            mRequestQueue.pop();
        }

        ResolveAsset(asset);
    }
}

void Render::AssetResolver::PushResolveRequest(CoreAsset::Asset *asset)
{

    std::unique_lock lock(mRequestQueueMutex);

    mRequestQueue.push(asset);

    mCV.notify_one();
}

void Render::AssetResolver::RegisterBuiltInAsset()
{

    std::vector<CoreAsset::AssetPtr> assetVec = mAssetManager->CreateBuiltInAsset();

    for (auto &assetPtr : assetVec)
    {
        CoreAsset::Asset *asset = assetPtr.Get();
        ResolveAsset(asset);
    }

}

void Render::AssetResolver::BuildGpuBuffers()
{

    GRM::GpuBufferContextSystemImpl *gpuBufferContextSystem =
        static_cast<GRM::GpuBufferContextSystemImpl *>(GRM::GpuBufferContextSystem::GetInstance());

    gpuBufferContextSystem->CreateBuffer(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantPass256),
                                         GRM::EBufferUsage::eConstantBuffer, 0, 256);
    gpuBufferContextSystem->CreateBuffer(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantObject128),
                                         GRM::EBufferUsage::eConstantBuffer, 1, 128);
    gpuBufferContextSystem->CreateBuffer(static_cast<uint8_t>(EDefaultGpuBufferType::eConstantObject256),
                                         GRM::EBufferUsage::eConstantBuffer, 1, 256);

    // light structured buffer
    gpuBufferContextSystem->CreateStructuredBuffer(mLightGpuBufferContextID, sizeof(DefaultLightData), true);

    // billboard structured buffer

    gpuBufferContextSystem->CreateStructuredBuffer(mBillboardGpuBufferContextID, sizeof(BillboardData), true);
}

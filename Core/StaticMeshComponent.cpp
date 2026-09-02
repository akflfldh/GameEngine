#include "StaticMeshComponent.h"
#include <CoreAsset/StaticMesh.h>
#include <CoreBase/Arch.h>

#include <Core/IRenderProxyManager.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <Core/PhysicsBridgeSystem.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreBase/CoreAssert.h>

StaticMeshComponent::StaticMeshComponent()
    : mRenderProxy(std::make_unique<Core::StaticMeshRenderProxy>()), mAABBDirty(true) /*, bDrawOutline(false)*/
{
}

StaticMeshComponent::~StaticMeshComponent()
{

    Core::IRenderProxyManager::GetInstance()->UnRegisterRenderProxy(mRenderProxy.get());

    PhysicsBridgeSystem::GetInstance()->UnregisterPhysicsBodyComponent(this);
}

void StaticMeshComponent::OnBegin()
{

    SceneComponent::OnBegin();
    //  mRenderProxy->mStaticMesh = static_cast<CoreAsset::StaticMesh *>(mStaticMeshPtr.Get());
}

void StaticMeshComponent::SetMesh(CoreAsset::StaticMesh *mesh)
{
    mStaticMeshPtr = mesh;

    if (mesh != nullptr)
    {
        // subMesh에대한 material list도 처리
        // 기본은 오리지널 mesh의 submesh material list을 그대로 복사

        const std::vector<CoreAsset::SubMesh> &subMeshList = mesh->GetSubMeshVector();

        mSubMeshMaterialList.resize(subMeshList.size());
        mSubMeshDrawOutlineFlagList.resize(subMeshList.size(), false);

        for (size_t i = 0; i < subMeshList.size(); ++i)
        {
            mSubMeshMaterialList[i] = subMeshList[i].mMaterialID;
        }

        // mRenderProxy의 StaticMesh와 MaterialList도 동기화
        // mRenderProxy->mStaticMesh = mesh;
        // mRenderProxy->mSubMeshMaterialList.clear();
        MarkPropertyDirty();
        //        UpdateRenderProxy();
    }
    else
    {
        mSubMeshMaterialList.clear();
        mSubMeshDrawOutlineFlagList.clear();
        mRenderProxy->mStaticMesh = nullptr;
        mRenderProxy->mSubMeshMaterialList.clear();
        MarkPropertyDirty();
    }

    OnSettedMesh();
}

void StaticMeshComponent::SetMesh(const std::string &meshname)
{

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::StaticMesh *mesh =
        static_cast<CoreAsset::StaticMesh *>(assetManager->GetAsset<CoreAsset::StaticMesh>(meshname.c_str()).Get());

    if (mesh)
        SetMesh(mesh);
}

void StaticMeshComponent::SetMesh(CoreAsset::AssetID id)
{

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::StaticMesh *mesh =
        static_cast<CoreAsset::StaticMesh *>(assetManager->GetAsset<CoreAsset::StaticMesh>(id).Get());

    if (mesh)
        SetMesh(mesh);
}

CoreAsset::StaticMesh *StaticMeshComponent::GetStaticMesh() const
{
    return static_cast<CoreAsset::StaticMesh *>(mStaticMeshPtr.Get());
}

const std::vector<CoreAsset::AssetPtr> &StaticMeshComponent::GetSubMeshMaterialList() const
{

    return mSubMeshMaterialList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void StaticMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::AssetPtr assetPtr)
{
    SetSubMeshMaterial(index, assetPtr.As<CoreAsset::Material>());

    // CHECK(index < mSubMeshMaterialList.size());

    // mSubMeshMaterialList[index].SetAsset(assetPtr.Get());

    // MarkPropertyDirty();
    ////  UpdateRenderProxy();
}

void StaticMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::AssetID id)
{

    SetSubMeshMaterial(index, CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Material>(id));
}

void StaticMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::Material *material)
{

    if (material == nullptr)
        return;

    CHECK(index < mSubMeshMaterialList.size());

    mSubMeshMaterialList[index].SetAsset(material);

    MarkPropertyDirty();
}

void StaticMeshComponent::OnTransformChanged()
{

    SceneComponent::OnTransformChanged();

    if (Object *Owner = GetOwnerObject())
    {
        mRenderProxy->mTransform = GetTransformWorld();
        // MarkPropertyDirty();
    }

    mAABBDirty = true;
}

const CoreMath::AABB &StaticMeshComponent::GetWorldAABB()
{

    if (mAABBDirty)
    {
        UpdateAABB();
        mAABBDirty = false;
    }

    return mWorldAABB;
}

bool StaticMeshComponent::Raycast(const CoreMath::Ray &ray, Core::HitResult &oHitResult)
{
    float distance = -1;
    bool hitResult = CoreMath::IntersectRayAABB(ray, GetWorldAABB(), distance);
    if (hitResult)
    {
        oHitResult.bHit = true;
        oHitResult.mDistance = distance;
        oHitResult.mHitComponent = this;
        return true;
    }

    return false;
}

void StaticMeshComponent::SetDrawOutline(bool bDraw)
{
    for (int i = 0; i < mSubMeshDrawOutlineFlagList.size(); ++i)
    {
        mSubMeshDrawOutlineFlagList[i] = bDraw;
        mRenderProxy->mSubMeshOutlineFlagList[i] = bDraw;
    }
    NotifyOutlineToManager();
}

void StaticMeshComponent::SetDrawOutline(size_t subMeshIndex, bool bDraw)
{

    mSubMeshDrawOutlineFlagList[subMeshIndex] = bDraw;
    mRenderProxy->mSubMeshOutlineFlagList[subMeshIndex] = bDraw;

    NotifyOutlineToManager();
}

void StaticMeshComponent::OnActiveStateChanged(bool state)
{

    if (state)
    {
        Core::IRenderProxyManager::GetInstance()->RegisterRenderProxy(mRenderProxy.get());
    }
    else
    {

        Core::IRenderProxyManager::GetInstance()->UnRegisterRenderProxy(mRenderProxy.get());
    }

    Core::IRenderProxyManager::GetInstance()->SetProxyDrawOutline(mRenderProxy.get(), state);
}

void StaticMeshComponent::DrawDebugCollider()
{
    if (auto owner = GetOwnerObject())
    {
        if (owner->GetActive())
        {
            Core::IRenderProxyManager::GetInstance()->DrawAABB(GetWorldAABB(), owner->GetMap()->GetRenderID());
        }
    }
}

void StaticMeshComponent::FlushPropertyDirty()
{

    UpdateRenderProxy();
}

void StaticMeshComponent::OnSetUpOwnerObject()
{
    SceneComponent::OnSetUpOwnerObject();
}

void StaticMeshComponent::OnOwnerObjectAddedToMap()
{
    SceneComponent::OnOwnerObjectAddedToMap();
    UpdateRenderProxy();
    if (Object *Onwer = GetOwnerObject())
    {

        mRenderProxy->mRenderID = Onwer->GetMap()->GetRenderID();

        if (Onwer->GetActive())
            Core::IRenderProxyManager::GetInstance()->RegisterRenderProxy(mRenderProxy.get());

        PhysicsBridgeSystem::GetInstance()->RegisterPhysicsBodyComponent(this, this, this);
    }
}

void StaticMeshComponent::Tick(float deltaTime)
{

    SceneComponent::Tick(deltaTime);
}

void StaticMeshComponent::OnSettedMesh() {}

void StaticMeshComponent::UpdateRenderProxy()
{
    if (mRenderProxy == nullptr)
        return;

    mRenderProxy->mTransform = GetTransformWorld();
    size_t count = mSubMeshMaterialList.size();

    mRenderProxy->mSubMeshOutlineFlagList.clear();
    if (mSubMeshDrawOutlineFlagList.size() == 0)
        mSubMeshDrawOutlineFlagList.resize(count, false);

    mRenderProxy->mStaticMesh = static_cast<CoreAsset::StaticMesh *>(mStaticMeshPtr.Get());
    mRenderProxy->mSubMeshMaterialList.resize(count);
    mRenderProxy->mSubMeshOutlineFlagList.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        mRenderProxy->mSubMeshMaterialList[i] = (static_cast<CoreAsset::Material *>(mSubMeshMaterialList[i].Get()));
        mRenderProxy->mSubMeshOutlineFlagList[i] = mSubMeshDrawOutlineFlagList[i];
    }

    UpdateAABB();
}

void StaticMeshComponent::SyncPrefabComponentFrom(Component *prefabComponent)
{
    SceneComponent ::SyncPrefabComponentFrom(prefabComponent);

    StaticMeshComponent *prefabStaticMeshCom = dynamic_cast<StaticMeshComponent *>(prefabComponent);

    if (prefabStaticMeshCom == nullptr)
        return;

    CoreAsset::StaticMesh *prefabStaticMesh = prefabStaticMeshCom->GetStaticMesh();

    if (prefabStaticMesh != GetStaticMesh())
    {
        SetMesh(prefabStaticMesh);
    }
    // 머터리얼까지
    const auto &matList = prefabStaticMeshCom->GetSubMeshMaterialList();
    for (int matIndex = 0; matIndex < matList.size(); ++matIndex)
    {
        SetSubMeshMaterial(matIndex, matList[matIndex].GetAssetID());
    }

    UpdateRenderProxy();
}

void StaticMeshComponent::EndTick(float deltaTime)
{
    SceneComponent::EndTick(deltaTime);

    if (GetPropertyDirty())
    {
        UpdateRenderProxy();
    }
}

void StaticMeshComponent::Serialize(Arch &arch)
{

    SceneComponent::Serialize(arch);

    const std::string name = GetOwnerObject()->GetObjectName();

    // 서브메시별 머터리얼

    CoreAsset::StaticMesh *mesh = static_cast<CoreAsset::StaticMesh *>(mStaticMeshPtr.Get());

    bool bHasStaticMesh = mesh ? true : false;

    arch << bHasStaticMesh;

    if (arch.GetLoadingFlag())
    {

        if (bHasStaticMesh)
        {

            CoreAsset::AssetID meshID;
            arch << meshID;

            SetMesh(meshID);

            size_t subMeshMatNum = 0;
            arch << subMeshMatNum;

            mSubMeshMaterialList.resize(subMeshMatNum);

            for (size_t i = 0; i < subMeshMatNum; ++i)
            {
                CoreAsset::AssetID matID;
                arch << matID;
                mSubMeshMaterialList[i] = matID;
            }
        }
    }
    else
    {
        if (bHasStaticMesh)
        {
            CoreAsset::AssetID meshID = mesh->GetID();
            arch << meshID;

            size_t subMeshMatNum = mSubMeshMaterialList.size();
            arch << subMeshMatNum;

            for (const auto &subMeshMat : mSubMeshMaterialList)
            {
                CoreAsset::AssetID matID = subMeshMat.GetAssetID();
                arch << matID;
            }
        }
    }

    arch << mPhysicsContext;
}

void StaticMeshComponent::SetRenderID(uint32_t id)
{

    mRenderProxy->mRenderID = id;
}

void StaticMeshComponent::UpdateAABB()
{

    auto staticMesh = static_cast<CoreAsset::StaticMesh *>(mStaticMeshPtr.Get());

    if (staticMesh)
    {
        CoreMath::AABB localAABB = staticMesh->GetAABB();
        mWorldAABB.Clear();

        CoreMath::Vector3 corners[8] = {CoreMath::Vector3(localAABB.mMin.X, localAABB.mMin.Y, localAABB.mMin.Z),
                                        CoreMath::Vector3(localAABB.mMax.X, localAABB.mMin.Y, localAABB.mMin.Z),
                                        CoreMath::Vector3(localAABB.mMin.X, localAABB.mMax.Y, localAABB.mMin.Z),
                                        CoreMath::Vector3(localAABB.mMax.X, localAABB.mMax.Y, localAABB.mMin.Z),
                                        CoreMath::Vector3(localAABB.mMin.X, localAABB.mMin.Y, localAABB.mMax.Z),
                                        CoreMath::Vector3(localAABB.mMax.X, localAABB.mMin.Y, localAABB.mMax.Z),
                                        CoreMath::Vector3(localAABB.mMin.X, localAABB.mMax.Y, localAABB.mMax.Z),
                                        CoreMath::Vector3(localAABB.mMax.X, localAABB.mMax.Y, localAABB.mMax.Z)};

        CoreMath::Matrix4X4 worldMat = GetTransformWorld();
        for (int i = 0; i < 8; ++i)
        {
            // world transform

            CoreMath::Vector3 pos = worldMat.TransformPoint(corners[i]);
            mWorldAABB.Expand(pos);
        }
    }
}

void StaticMeshComponent::NotifyOutlineToManager()
{
    bool bOutlineDraw = false;
    for (int i = 0; i < mSubMeshDrawOutlineFlagList.size(); ++i)
    {
        if (mSubMeshDrawOutlineFlagList[i] == true)
        {
            if (mRenderProxy->mRenderID != 0)
            {
                bOutlineDraw = true;
            }
        }
    }

    if (mRenderProxy->mRenderID != 0)
    {
        if (bOutlineDraw)
        {
            // 이미등록되어있다면 넘어갈것이다.
            Core::IRenderProxyManager::GetInstance()->SetProxyDrawOutline(mRenderProxy.get(), true);
        }
        else
        {
            // 한개의 서브메시도 outline flag가 true이지않기때문에
            Core::IRenderProxyManager::GetInstance()->SetProxyDrawOutline(mRenderProxy.get(), false);
        }
    }
}

bool StaticMeshComponent::IsPhysicsEnabled() const
{
    return mPhysicsContext.mPhysicsEnabled;
}

EPhysicsBodyType StaticMeshComponent::GetPhysicsBodyType() const
{
    return mPhysicsContext.mPhysicsBodyType;
}

bool StaticMeshComponent::IsPhysicsGravityEnabled() const
{
    return mPhysicsContext.mUseGravity;
}

float StaticMeshComponent::GetPhysicsMass() const
{
    return mPhysicsContext.mMass;
}

void StaticMeshComponent ::SetPhysicsEnabled(bool flag)
{

    mPhysicsContext.mPhysicsEnabled = flag;
}

void StaticMeshComponent::SetPhysicsBodyType(EPhysicsBodyType type)
{

    mPhysicsContext.mPhysicsBodyType = type;
}

void StaticMeshComponent::SetPhysicsGravityEnabled(bool flag)
{

    mPhysicsContext.mUseGravity = flag;
}
void StaticMeshComponent::SetPhysicsMass(float mass)
{

    mPhysicsContext.mMass = mass;
}

// 수정필요
// EPhysicsShapeType StaticMeshComponent::GetPhysicsCollisionShapeType() const
//{
//
//    if (mPhysicsContext.mShapePolicy == EPhysicsCollisionShapePolicy::eAuto)
//        return GuessPhysicsCollisionShapeType();
//
//    return mPhysicsContext.mCollisionShapeType;
//}

// 수정필요
CoreMath::Vector3 StaticMeshComponent::GetPhysicsBoxHalfExtent() const
{

    CoreAsset::StaticMesh *mesh = GetStaticMesh();
    if (mesh == nullptr)
        return {0.5f, 0.5f, 0.5f};

    const CoreMath::AABB &localAABB = mesh->GetAABB();

    CoreMath::Vector3 localHalfExtent = {(localAABB.mMax.X - localAABB.mMin.X) * 0.5f,
                                         (localAABB.mMax.Y - localAABB.mMin.Y) * 0.5f,
                                         (localAABB.mMax.Z - localAABB.mMin.Z) * 0.5f};

    CoreMath::Vector3 scale = GetScaleWorld();

    return {localHalfExtent.X * std::abs(scale.X), localHalfExtent.Y * std::abs(scale.Y),
            localHalfExtent.Z * std::abs(scale.Z)};
}

size_t StaticMeshComponent::GetPhysicsShapeCount()
{
    return 0;
    // return mPhysicsContext.mShapeBuildDataList.size();
}

bool StaticMeshComponent::GetPhysicsShapeBuildData(size_t index, PhysicsShapeBuildData &oData)
{

    // if (mPhysicsContext.mShapeBuildDataList.size() <= index)
    //     return false;

    // oData = mPhysicsContext.mShapeBuildDataList[index];

    return true;
}

// bool StaticMeshComponent::IsEngineBoxMesh(CoreAsset::Asset *mesh) const
//{
//     if (mesh == nullptr)
//         return false;
//
//     auto assetManager = CoreAsset::AssetManager::GetInstance();
//
//     if (mesh->GetID() == assetManager->GetDefaultEngineCubeMeshID())
//     {
//         return true;
//     }
//
//     return false;
// }

void StaticMeshComponent::AddPhysicsShapeBuildData(const PhysicsShapeBuildData &data)
{

    //  mPhysicsContext.mShapeBuildDataList.push_back(data);
}

const PhysicsCollisionPreset &StaticMeshComponent::GetPhysicsCollisionPreset() const
{
    CoreAsset::StaticMesh *mesh = GetStaticMesh();

    // mesh 가 nullptr이면 예외던지기

    return mesh->GetPhysicsCollisionPreset();
}

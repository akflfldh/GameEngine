#include "SkeletalMeshComponent.h"

#include <Core/AnimatorComponent.h>
#include <Core/IRenderProxyManager.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Material.h>
#include <CoreAsset/SkinningMesh.h>
#include <CoreBase/Arch.h>

SkeletalMeshComponent::SkeletalMeshComponent() : mRenderProxy(std::make_unique<Core::SkeletalMeshRenderProxy>()) {}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
    UnRegisterRenderProxy();
}

void SkeletalMeshComponent::SetMesh(CoreAsset::SkinningMesh *mesh)
{
    mSkinningMeshPtr = mesh;
    mSubMeshMaterialList.clear();

    if (mesh != nullptr)
    {
        const std::vector<CoreAsset::SubMesh> &subMeshes = mesh->GetSubMeshVector();
        mSubMeshMaterialList.resize(subMeshes.size());
        mSubMeshDrawOutlineFlagList.resize(subMeshes.size(), false);

        // Material 선택은 mesh 에셋의 기본값으로 시작하지만 이후 변경은 컴포넌트 인스턴스에만 보관한다.
        for (size_t index = 0; index < subMeshes.size(); ++index)
            mSubMeshMaterialList[index] = subMeshes[index].mMaterialID;
    }
    else
    {
        mSubMeshDrawOutlineFlagList.clear();
    }

    MarkPropertyDirty();
    OnMeshChanged();
    UpdateRenderProxy();
    NotifyOutlineToManager();
}

void SkeletalMeshComponent::SetMesh(const std::string &meshAssetName)
{
    CoreAsset::AssetPtr mesh =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::SkinningMesh>(meshAssetName.c_str());
    if (CoreAsset::SkinningMesh *skinningMesh = mesh.As<CoreAsset::SkinningMesh>())
        SetMesh(skinningMesh);
}

void SkeletalMeshComponent::SetMesh(CoreAsset::AssetID meshAssetID)
{
    CoreAsset::AssetPtr mesh = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::SkinningMesh>(meshAssetID);
    if (CoreAsset::SkinningMesh *skinningMesh = mesh.As<CoreAsset::SkinningMesh>())
        SetMesh(skinningMesh);
}

CoreAsset::SkinningMesh *SkeletalMeshComponent::GetSkinningMesh() const
{
    return mSkinningMeshPtr.As<CoreAsset::SkinningMesh>();
}

const CoreAsset::SkinBinding *SkeletalMeshComponent::GetSkinBinding() const
{
    CoreAsset::SkinningMesh *mesh = GetSkinningMesh();
    return mesh != nullptr ? &mesh->GetSkinBinding() : nullptr;
}

const std::vector<CoreAsset::AssetPtr> &SkeletalMeshComponent::GetSubMeshMaterialList() const
{
    return mSubMeshMaterialList;
}

void SkeletalMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::AssetPtr material)
{
    SetSubMeshMaterial(index, material.As<CoreAsset::Material>());
}

void SkeletalMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::AssetID materialAssetID)
{
    CoreAsset::AssetPtr material =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Material>(materialAssetID);
    SetSubMeshMaterial(index, material);
}

void SkeletalMeshComponent::SetSubMeshMaterial(size_t index, CoreAsset::Material *material)
{
    if (material == nullptr || index >= mSubMeshMaterialList.size())
        return;

    mSubMeshMaterialList[index] = material;
    MarkPropertyDirty();
}

void SkeletalMeshComponent::SetDrawOutline(bool bDraw)
{
    for (size_t index = 0; index < mSubMeshDrawOutlineFlagList.size(); ++index)
        mSubMeshDrawOutlineFlagList[index] = bDraw;

    UpdateRenderProxy();
    NotifyOutlineToManager();
}

void SkeletalMeshComponent::SetDrawOutline(size_t subMeshIndex, bool bDraw)
{
    if (subMeshIndex >= mSubMeshDrawOutlineFlagList.size())
        return;

    mSubMeshDrawOutlineFlagList[subMeshIndex] = bDraw;
    UpdateRenderProxy();
    NotifyOutlineToManager();
}

void SkeletalMeshComponent::Serialize(Arch &arch)
{
    SceneComponent::Serialize(arch);

    // EngineClass의 REFLECT_PROPERTY는 자동 저장되지 않으므로 메시와 인스턴스별 머터리얼을 ID로 기록한다.
    bool hasSkinningMesh = GetSkinningMesh() != nullptr;
    arch << hasSkinningMesh;

    if (arch.GetLoadingFlag())
    {
        if (hasSkinningMesh)
        {
            CoreAsset::AssetID meshID = NoneAssetID;
            arch << meshID;

            // SetMesh가 서브메시와 bind-pose palette를 초기화한 뒤 저장된 머터리얼 선택을 덮어쓴다.
            SetMesh(meshID);

            size_t materialCount = 0;
            arch << materialCount;

            for (size_t index = 0; index < materialCount; ++index)
            {
                CoreAsset::AssetID materialID = NoneAssetID;
                arch << materialID;

                // 메시의 서브메시 수가 바뀌어도 현재 메시의 목록 크기를 유지하고 초과 저장값만 소비한다.
                if (index < mSubMeshMaterialList.size())
                    mSubMeshMaterialList[index] = materialID;
            }

            UpdateRenderProxy();
        }
        else
        {
            SetMesh(static_cast<CoreAsset::SkinningMesh *>(nullptr));
        }
    }
    else if (hasSkinningMesh)
    {
        CoreAsset::AssetID meshID = mSkinningMeshPtr.GetAssetID();
        arch << meshID;

        size_t materialCount = mSubMeshMaterialList.size();
        arch << materialCount;

        for (const CoreAsset::AssetPtr &material : mSubMeshMaterialList)
        {
            CoreAsset::AssetID materialID = material.GetAssetID();
            arch << materialID;
        }
    }
}

void SkeletalMeshComponent::OnTransformChanged()
{
    SceneComponent::OnTransformChanged();

    if (mRenderProxy != nullptr)
        mRenderProxy->mTransform = GetTransformWorld();
}

void SkeletalMeshComponent::OnActiveStateChanged(bool state)
{
    SceneComponent::OnActiveStateChanged(state);

    if (state)
        RegisterRenderProxy();
    else
        UnRegisterRenderProxy();
}

void SkeletalMeshComponent::FlushPropertyDirty()
{
    UpdateRenderProxy();
}

void SkeletalMeshComponent::SetRenderID(uint32_t id)
{
    if (mRenderProxy != nullptr)
        mRenderProxy->mRenderID = id;
}

void SkeletalMeshComponent::UpdateRenderProxy()
{
    if (mRenderProxy == nullptr)
        return;

    mRenderProxy->mTransform = GetTransformWorld();
    mRenderProxy->mSkinningMesh = GetSkinningMesh();
    mRenderProxy->mSubMeshMaterialList.resize(mSubMeshMaterialList.size());
    mRenderProxy->mSubMeshOutlineFlagList = mSubMeshDrawOutlineFlagList;

    for (size_t index = 0; index < mSubMeshMaterialList.size(); ++index)
        mRenderProxy->mSubMeshMaterialList[index] = mSubMeshMaterialList[index].As<CoreAsset::Material>();

    // RenderFrontend가 컴포넌트의 mutable vector를 직접 참조하지 않도록 프록시 경계에서 값을 복사한다.
    //  mRenderProxy->mFinalMatrixList = mFinalMatrixList;
}

void SkeletalMeshComponent::OnOwnerObjectAddedToMap()
{
    SceneComponent::OnOwnerObjectAddedToMap();

    Object *owner = GetOwnerObject();
    if (owner == nullptr || owner->GetMap() == nullptr)
        return;

    SetRenderID(owner->GetMap()->GetRenderID());

    UpdateRenderProxy();

    if (owner->GetActive())
        RegisterRenderProxy();
}

void SkeletalMeshComponent::OnOwnerObjectRemovedFromMap()
{
    UnRegisterRenderProxy();
    SceneComponent::OnOwnerObjectRemovedFromMap();
}

void SkeletalMeshComponent::EndTick(float deltaTime)
{
    SceneComponent::EndTick(deltaTime);

    // Property dirty와 별개로 렌더프록시에 항상 반영
    UpdateAnimationFinalMatrix();

    if (GetPropertyDirty())
        UpdateRenderProxy();

    // 매 프레임 palette 갱신은 AnimationSystem 평가 이후의 명시적인 동기화 단계에서 추가해야 한다.
    // 일반 Component Tick 순서에만 의존하면 이전 프레임 pose를 프록시에 전달할 수 있다.
}

void SkeletalMeshComponent::OnMeshChanged()
{

    auto skinningMesh = mSkinningMeshPtr.As<CoreAsset::SkinningMesh>();

    mFinalMatrixList.clear();
    if (mRenderProxy)
        mRenderProxy->mFinalMatrixList.clear();

    if (skinningMesh)
    {
        const CoreAsset::SkinBinding &skinBinding = skinningMesh->GetSkinBinding();

        auto assetManager = CoreAsset::AssetManager::GetInstance();

        auto skeleton = static_cast<CoreAsset::Skeleton *>(
            assetManager->GetAsset<CoreAsset::Skeleton>(skinBinding.mSkeletonAssetID).Get());

        if (skeleton)
        {
            mFinalMatrixList.resize(skinningMesh->GetSkinBinding().mPaletteToSkeletonJoint.size());

            ResetFinalMatrix();

            UpdateRenderProxy();
        }
    }
}

void SkeletalMeshComponent::UpdateAnimationFinalMatrix()
{

    auto mesh = mSkinningMeshPtr.As<CoreAsset::SkinningMesh>();
    if (mesh == nullptr)
        return;

    auto animatorCom = GetAnimatorComponent();
    if (animatorCom == nullptr)
    {
        ResetFinalMatrix();
        return;
    }

    const CoreAsset::SkinBinding &skinBinding = mesh->GetSkinBinding();

    bool ret = animatorCom->BuildFinalMatrix(skinBinding, mFinalMatrixList);

    if (ret == false)
    {
        ResetFinalMatrix();
        return;
        // std::fill(mFinalMatrixList.begin(), mFinalMatrixList.end(), CoreMath::Matrix4X4::Identity);
    }

    if (mRenderProxy.get())
    {
        // final Matrix 를 자체적으로 유지해야한다면 복사를 해야한다.
        mRenderProxy->mFinalMatrixList = mFinalMatrixList;
    }
}

void SkeletalMeshComponent::OnDestoryRequested()
{

    SceneComponent::OnDestoryRequested();

    UnRegisterRenderProxy();
}

AnimatorComponent *SkeletalMeshComponent::GetAnimatorComponent() const
{

    Object *ownerObject = GetOwnerObject();

    if (ownerObject == nullptr)
        return nullptr;

    auto animatorCom = ownerObject->GetComponent<AnimatorComponent>();

    return animatorCom;
}

void SkeletalMeshComponent::RegisterRenderProxy()
{
    if (mRenderProxy == nullptr || mRenderProxyRegistered)
        return;

    Object *owner = GetOwnerObject();
    if (owner == nullptr || owner->GetMap() == nullptr)
        return;

    mRenderProxy->mRenderID = owner->GetMap()->GetRenderID();
    Core::IRenderProxyManager::GetInstance()->RegisterRenderProxy(mRenderProxy.get());
    mRenderProxyRegistered = true;
    NotifyOutlineToManager();
}

void SkeletalMeshComponent::UnRegisterRenderProxy()
{
    if (mRenderProxy == nullptr || !mRenderProxyRegistered)
        return;

    if (mRenderProxy->mDrawOutline)
        Core::IRenderProxyManager::GetInstance()->SetProxyDrawOutline(mRenderProxy.get(), false);
    Core::IRenderProxyManager::GetInstance()->UnRegisterRenderProxy(mRenderProxy.get());
    mRenderProxy->mDrawOutline = false;
    mRenderProxyRegistered = false;
}

void SkeletalMeshComponent::NotifyOutlineToManager()
{
    if (!mRenderProxy)
        return;

    bool drawOutline = false;
    for (bool flag : mSubMeshDrawOutlineFlagList)
        drawOutline = drawOutline || flag;

    // 선택 목록은 등록된 프록시에만 유지하고, 실제 서브메시별 출력은 flag snapshot이 결정한다.
    mRenderProxy->mDrawOutline = drawOutline && mRenderProxyRegistered;
    if (mRenderProxyRegistered)
        Core::IRenderProxyManager::GetInstance()->SetProxyDrawOutline(mRenderProxy.get(), mRenderProxy->mDrawOutline);
}

void SkeletalMeshComponent::ResetFinalMatrix()
{

    std::fill(mFinalMatrixList.begin(), mFinalMatrixList.end(), CoreMath::Matrix4X4::Identity);
    if (mRenderProxy)
        mRenderProxy->mFinalMatrixList = mFinalMatrixList;
}

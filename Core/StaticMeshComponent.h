#pragma once

#include <Core/CoreDllExport.h>
#include <Core/SceneComponent.h>
#include <CoreAsset/AssetPtr.h>
#include <memory>
#include <vector>

#include <Core/CorePhysicsType.h>
#include <Core/IDrawableOutline.h>
#include <Core/IHittable.h>
#include <Core/IInterface.h>
#include <Core/IPhysicsBodyComponent.h>
#include <Core/IPhysicsShapeProvider.h>
#include <CoreMath/Geometry.h>
#include <Physics/PhysicsType.h>

#include <StaticMeshComponent.generated.h>
namespace CoreAsset
{
class StaticMesh;
class Material;

} // namespace CoreAsset

namespace Core
{
class RenderProxy;
class StaticMeshRenderProxy;
}; // namespace Core

// struct StaticMeshPhysicsContext
//{
//     // PhysicsBodyHandle mPhysicsBodyHandle = PhysicsBodyHandleInValid;
//     bool mPhysicsEnabled = false;
//     EPhysicsBodyType mPhysicsBodyType = EPhysicsBodyType::eStatic;
//     bool mUseGravity = false;
//     float mMass = 1.0f;
//     //   CoreMath::Vector3 mPhysicsBoxHalfExtent = {0.5f, 0.5f, 0.5f};
// };

class CORE_API_LIB REFLECT_CLASS(EngineClass) StaticMeshComponent : public SceneComponent,
                                                                    public Core::IHittable,
                                                                    public Core::IDrawableOutline,
                                                                    public Core::IRenderableComponent,
                                                                    public IPhysicsBodyComponent,
                                                                    public IPhysicsShapeProvider
{
    GENERATED_BODY(StaticMeshComponent)

  public:
    StaticMeshComponent();
    virtual ~StaticMeshComponent() override;

    virtual void OnBegin() override;

    // nullptr은 아무것도 설정하지않겠다라는의미
    void SetMesh(CoreAsset::StaticMesh *mesh);
    void SetMesh(const std::string &meshname);
    void SetMesh(CoreAsset::AssetID id);

    CoreAsset::StaticMesh *GetStaticMesh() const;
    const std::vector<CoreAsset::AssetPtr> &GetSubMeshMaterialList() const;

    void SetSubMeshMaterial(size_t index, CoreAsset::AssetPtr assetPtr);
    void SetSubMeshMaterial(size_t index, CoreAsset::AssetID id);
    void SetSubMeshMaterial(size_t index, CoreAsset::Material *material);

    //  virtual void Serialize(Arch &arch) override;

    virtual void OnTransformChanged() override;

    const CoreMath::AABB &GetWorldAABB();
    virtual bool Raycast(const CoreMath::Ray &ray, Core::HitResult &oHitResult) override;

    virtual void SetDrawOutline(bool bDraw) override;
    virtual void SetDrawOutline(size_t subMeshIndex, bool bDraw) override;

    virtual void OnActiveStateChanged(bool state) override;

    void DrawDebugCollider() override;
    virtual void FlushPropertyDirty() override;

    virtual void Serialize(Arch &arch) override;

    void SetRenderID(uint32_t id) override;

    void UpdateRenderProxy();

    virtual void SyncPrefabComponentFrom(Component *prefabComponent) override;

#pragma region PhysicsAPI

    void SetPhysicsEnabled(bool flag);
    void SetPhysicsBodyType(EPhysicsBodyType type);
    void SetPhysicsGravityEnabled(bool flag);
    void SetPhysicsMass(float mass);

    virtual bool IsPhysicsEnabled() const override;
    virtual EPhysicsBodyType GetPhysicsBodyType() const override;
    virtual bool IsPhysicsGravityEnabled() const override;
    virtual float GetPhysicsMass() const override;

    //  virtual EPhysicsShapeType GetPhysicsCollisionShapeType() const override;
    CoreMath::Vector3 GetPhysicsBoxHalfExtent() const;
    virtual size_t GetPhysicsShapeCount() override;
    virtual bool GetPhysicsShapeBuildData(size_t index, PhysicsShapeBuildData &oData) override;

    virtual const PhysicsCollisionPreset &GetPhysicsCollisionPreset() const override;

    // 물리 에딧시에 사용하게될거다.(일단 기본적으로 싱글 box shape를설정하기위해 사용)
    void AddPhysicsShapeBuildData(const PhysicsShapeBuildData &oData);

#pragma endregion

  protected:
    virtual void OnSetUpOwnerObject() override;
    virtual void OnOwnerObjectAddedToMap() override;

    virtual void Tick(float deltaTime) override;

    virtual void OnSettedMesh();

    virtual void EndTick(float deltaTime) override;

  private:
    void UpdateAABB();
    void NotifyOutlineToManager();

    //   bool IsEngineBoxMesh(CoreAsset::Asset *mesh) const;

  protected:
    REFLECT_PROPERTY()
    CoreAsset::AssetPtr mStaticMeshPtr;

    // 서브메시별 머터리얼
    REFLECT_PROPERTY()
    std::vector<CoreAsset::AssetPtr> mSubMeshMaterialList;
    std::vector<bool> mSubMeshDrawOutlineFlagList;

    std::unique_ptr<Core::StaticMeshRenderProxy> mRenderProxy;

    CoreMath::AABB mWorldAABB;
    bool mAABBDirty;

    PhysicsComponentSettings mPhysicsContext;
};

#pragma once

#include <Core/StaticMeshComponent.h>

#include <StaticMeshComponent.generated.h>

namespace Quad
{
class REFLECT_CLASS(EngineClass) GizmoMeshComponent : public StaticMeshComponent
{
    GENERATED_BODY(GizmoMeshComponent)
  public:
    GizmoMeshComponent();
    virtual ~GizmoMeshComponent();

    virtual void OnBegin() override;

    void SetColor(const CoreMath::Vector4 &color);

  private:
    virtual void OnSettedMesh() override;

  private:
    CoreAsset::AssetPtr mGizmoMaterial;
    CoreMath::Vector4 mColor;
};

} // namespace Quad
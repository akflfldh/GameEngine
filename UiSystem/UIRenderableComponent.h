#pragma once

#include "UiSystem/IUIComponent.h"

#include "UiSystem/Generated/UIRenderableComponent.generated.h"

namespace UI
{

class UISYSTEM_API REFLECT_CLASS UIRenderableComponent : public IUIComponent
{
    GENERATED_BODY(UIRenderableComponent)

  public:
    UIRenderableComponent();
    virtual ~UIRenderableComponent() = 0;

    virtual void Update() override;

    const UIMeshComponent &GetUIMeshComponentRef() const;
    const UIMeshComponent *GetUIMeshComponentPtr() const;

    void SetColor(const CoreMath::Vector4 &color);

    // buffer의 사이즈는 4개이여야 한다.
    // GetVertices호출시 필요한 정점버퍼의 크기 반환
    virtual size_t GetVertexNum() const = 0;
    virtual void GetVertices(UIVertex *oUIVertices) const = 0;

    virtual size_t GetIndexNum() const = 0;
    virtual void GetIndices(uint32_t *oIndices) const = 0;

    void SetActiveState(bool state);
    bool GetActiveState() const;

  protected:
    UIMeshComponent mMeshComponent;

    // 렌더 활성화,비활성화 여부
    bool mRenderActiveState;
};

} // namespace UI

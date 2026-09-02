#pragma once

#include "UiSystem/IUIComponent.h"
#include "UiSystem/IUIRenderProxyManager.h"
#include <CoreBase/CoreBaseType.h>

#include "UIRenderableComponent.generated.h"

namespace UI
{

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIRenderableComponent : public IUIComponent
{
    GENERATED_BODY(UIRenderableComponent)

  public:
    UIRenderableComponent();
    virtual ~UIRenderableComponent() = 0;

    virtual void Update(float deltaTime) override;

    const UIMeshComponent &GetUIMeshComponentRef() const;
    const UIMeshComponent *GetUIMeshComponentPtr() const;

    void SetColor(const glm::vec4 &color);

    // buffer의 사이즈는 4개이여야 한다.
    // GetVertices호출시 필요한 정점버퍼의 크기 반환
    virtual size_t GetVertexNum() const = 0;
    virtual uint32_t GetVertices(UIVertex *oUIVertices) const = 0; // nullptr 전달시 개수 리턴

    virtual size_t GetIndexNum() const = 0;
    virtual void GetIndices(uint32_t *oIndices) const = 0;

    void SetActiveState(bool state);
    bool GetActiveState() const;

    virtual void OnOwnerAddedToCavas() override;

    UIRenderProxy *GetRenderProxy() const;

  protected:
    UIMeshComponent mMeshComponent;

    // 렌더 활성화,비활성화 여부
    bool mRenderActiveState;

    std::unique_ptr<UIRenderProxy> mRenderProxy;
};

} // namespace UI

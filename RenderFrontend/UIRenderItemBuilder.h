#pragma once

#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/RenderType.h>
#include <UiSystem/IUIRenderProxyManager.h>
#include <UiSystem/UIManager.h>
#include <UiSystem/UIType.h>
#include <unordered_map>
#include <vector>

namespace GRM
{
class IGpuResource;
class IGpuResourceManager;
} // namespace GRM
namespace UI
{
class UIElement;
class UIRenderableComponent;
class UIResizeGizmoRenderableComponent;
} // namespace UI

namespace Core
{
class ViewportController;
}

namespace Render
{

class AssetResolver;

class RENDER_FRONTEND_API UIRenderItemBuilder : public UI::IUIRenderProxyManager
{

    // 지금 머터리얼 별로만 분류하였는데
    // ScissorRect라는 기준도 추가해야한다.
    using MaterialRenderItemTable = std::unordered_map<Render::MaterialID, Render::RenderItem>;

  public:
    static UIRenderItemBuilder *GetInstance();

    UIRenderItemBuilder(Render::IRenderSystem *renderSystem, UI::UIManager *uiManager,
                        GRM::IGpuResourceManager *gpuResourceManager, Render::AssetResolver *assetResolver);
    ~UIRenderItemBuilder();

    // frame종료후 호출
    // 다음프레임을위해 데이터들을 리셋한다.
    void EndFrame();

#pragma region IUIRenderProxyManager
    void RegisterRenderProxy(UI::UIRenderProxy *renderProxy) override;
    void UnRegisterRenderProxy(UI::UIRenderProxy *renderProxy) override;

    void UploadStart(GRM::GRMPtr &oVertexBuffer, GRM::GRMPtr &oIndexBuffer);

    // 수차례 호출 ( 들어오는 RenderProxy 그대로 vertex buffer, index buffer에 넣는다,)
    // 배칭,순서등 외부에서 온전히 컨트롤
    void UploadVertexBuffer(const std::vector<UIRenderCommand> &renderProxyList, uint32_t &oVertexOffset,
                            uint32_t &oIndexOffset, uint32_t &oVertexNum, uint32_t &oIndexNum,
                            const std::vector<UI::UIVertex> &vertexBuffer, const std::vector<uint32_t> &indexBuffer);
    void UploadIndexBuffer(const std::vector<UIRenderCommand> &renderProxyList);

    void UploadEnd(GRM::GRMPtr &vertexPtr, GRM::GRMPtr &indexPtr);

    std::vector<UI::UIRenderProxy *> GetRenderProxyList(UI::UICanvasID canvasID) const;

    // Vertex,Index
    void ResetBufferOffset(int nextFrame);
#pragma endregion

  private:
    static UIRenderItemBuilder *mInstance;

  private:
    Render::IRenderSystem *mRenderSystem;
    UI::UIManager *mUiManager;
    GRM::IGpuResourceManager *mGpuResourceManager;
    Render::AssetResolver *mAssetResolver;

    // 프레임별 UI버퍼
    std::vector<GRM::GRMPtr> mVertexBufferGpuPerFrame;
    std::vector<GRM::GRMPtr> mIndexBufferGpuPerFrame;
    // GRM::IGpuResource* mVertexBufferGpu;
    // GRM::IGpuResource* mIndexBufferGpu;

    //// 매프레임마다 초기화된다.
    //// 렌더채널별  <  머터리얼 - 렌더아이템> 테이블
    // std::unordered_map<Render::RenderChannelID, std::vector<UIElementContext>>
    // mUIElementContextVectorPerRenderChannel;

    uint32_t mCurrVertexBufferOffset;
    uint32_t mCurrIndexBufferOffset;

#pragma region IUIRenderProxyManager
    std::unordered_map<UI::UICanvasID, std::vector<UI::UIRenderProxy *>> mUIRenderProxyListTable;
#pragma endregion

    int mCurrFrame = 0;
};

} // namespace Render

#pragma once

#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderSystem/IRenderSystem.h>
#include <RenderSystem/RenderType.h>
#include <UISystem/UIManager.h>
#include <UISystem/UIType.h>
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

// RenderItem분류 기준이된다.

// 빌더에서 내부적으로 Element들을 정렬하기위해 사용할 Batch데이터모음
struct RENDER_FRONTEND_API UIElementBatchKey
{
    Render::RenderChannelID mRenderChannelID;
    Render::MaterialID mMaterialID;
    Render::ScissorRect mScissorRect;

    bool operator<(const UIElementBatchKey &batchKey) const
    {
        if (mRenderChannelID < batchKey.mRenderChannelID)
            return mRenderChannelID < batchKey.mRenderChannelID;

        if (mMaterialID != batchKey.mMaterialID)
            return mMaterialID < batchKey.mMaterialID;

        if (mScissorRect.Left != batchKey.mScissorRect.Left)
            return mScissorRect.Left < batchKey.mScissorRect.Left;
        if (mScissorRect.Right != batchKey.mScissorRect.Right)
            return mScissorRect.Right < batchKey.mScissorRect.Right;
        if (mScissorRect.Top != batchKey.mScissorRect.Top)
            return mScissorRect.Top < batchKey.mScissorRect.Top;
        if (mScissorRect.Bottom != batchKey.mScissorRect.Bottom)
            return mScissorRect.Bottom < batchKey.mScissorRect.Bottom;

        return false;
    }

    bool operator==(const UIElementBatchKey &batchKey) const
    {
        if (mRenderChannelID != batchKey.mRenderChannelID)
            return false;

        if (mMaterialID != batchKey.mMaterialID)
            return false;

        if (mScissorRect.Left != batchKey.mScissorRect.Left)
            return false;
        if (mScissorRect.Right != batchKey.mScissorRect.Right)
            return false;
        if (mScissorRect.Top != batchKey.mScissorRect.Top)
            return false;
        if (mScissorRect.Bottom != batchKey.mScissorRect.Bottom)
            return false;

        return true;
    }
};

// 빌더에서 내부적으로 렌더아이템을 구축하기위해서 사용할 UIElement 데이터모음
struct UIElementContext
{
    //  UI::UIElement *mUIElement;
    UI::UIRenderableComponent *mUIRenderableComponent;
    UIElementBatchKey mBatchKey;
};

class RENDER_FRONTEND_API UIRenderItemBuilder
{

    // 지금 머터리얼 별로만 분류하였는데
    // ScissorRect라는 기준도 추가해야한다.
    using MaterialRenderItemTable = std::unordered_map<Render::MaterialID, Render::RenderItem>;

  public:
    static UIRenderItemBuilder *GetInstance();

    UIRenderItemBuilder(Render::IRenderSystem *renderSystem, UI::UIManager *uiManager,
                        GRM::IGpuResourceManager *gpuResourceManager, Render::AssetResolver *assetResolver);
    ~UIRenderItemBuilder();

    void SubmitUIElement(UI::UICanvas *canvas, Render::RenderChannelID renderChannelID,
                         const Core::ViewportController &viewportController);

    // ndc공간으로 보낼때 ui들이 viewport기준인가, window기준인가, //씬내에 ui들은카메라에영향을받는다.
    // 현재는viewport로 고정되어, 어느뷰포트이건 ui가동일한크기로 보인다.
    // false시 심각한 에러 , 프로그램종료
    bool BuildAndSubmitRenderItem(Render::RenderChannelID renderChannelID, const Render::Viewport &viewport,
                                  int clientWidth, int clientHeight);

    // frame종료후 호출
    // 다음프레임을위해 데이터들을 리셋한다.
    void EndFrame();

  private:
    static UIRenderItemBuilder *mInstance;

  private:
    Render::IRenderSystem *mRenderSystem;
    UI::UIManager *mUiManager;
    GRM::IGpuResourceManager *mGpuResourceManager;
    Render::AssetResolver *mAssetResolver;
    GRM::GRMPtr mVertexBufferGpu;
    GRM::GRMPtr mIndexBufferGpu;
    // GRM::IGpuResource* mVertexBufferGpu;
    // GRM::IGpuResource* mIndexBufferGpu;

    // 매프레임마다 초기화된다.
    // 렌더채널별  <  머터리얼 - 렌더아이템> 테이블
    std::unordered_map<Render::RenderChannelID, std::vector<UIElementContext>> mUIElementContextVectorPerRenderChannel;

    uint32_t mCurrVertexBufferOffset;
    uint32_t mCurrIndexBufferOffset;
};

} // namespace Render

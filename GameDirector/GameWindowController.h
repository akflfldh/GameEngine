#pragma once

#include <Core/WindowedFrameController.h>

namespace Quad
{
class GameWindow;
class Game3DSystem;
class GameUiSystem;
class RenderSystem;

class GameWindowController : public Core::WindowedFrameController
{
  public:
    GameWindowController();
    ~GameWindowController() = default;

    void Initialize(Render::RenderPipelineManager &renderPipelineManager);

  protected:
    void DrawWindow() override;
    void OnRenderSurfaceResize(uint32_t width, uint32_t height) override;

  private:
    Render::RenderPipelineManager *mRenderPipelineManager = nullptr;
};

} // namespace Quad
#pragma once

#include <Core/SuperController.h>

namespace Quad
{
class BaseWindow;

class SuperAssetBrowerController : public Core::SuperController
{
  public:
    static SuperAssetBrowerController *GetInstance();
    virtual ~SuperAssetBrowerController();
    virtual void Initialize(Render::RenderPipelineManager &renderPipelineManager) override;

    virtual void Begin() override;
    virtual void PreUpdate() override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate() override;
    virtual void Draw(Render::RenderPipelineManager &renderPipelineManager) override;
    virtual void ShutDownWindow() override;
    virtual BaseWindow *GetWindow() const override;

    void TestRButtonDown();
    //   virtual void UpdateMouseInput(MouseContext &mouseContext) override;

    virtual void SetMouseCapture(Core::LogicalWindow *window) override;
    virtual void ReleaseMouseCapture() override;

    virtual void SetKeyboardCapture(Core::LogicalWindow *window) override;
    virtual void ReleaseKeyboardCapture() override;

    std::pair<uint32_t, uint32_t> GetWindowSize() const override;

  private:
    SuperAssetBrowerController();

  private:
    BaseWindow *mWindow;
};

} // namespace Quad

#pragma once

#include "EditorDirector/SuperController.h"
#include <Windows.h>
namespace Quad
{
class BaseWindow;

class SuperAssetBrowerController : public SuperController
{
  public:
    static SuperAssetBrowerController *GetInstance();
    virtual ~SuperAssetBrowerController();
    virtual void Initialize() override;

    virtual void PreUpdate() override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate() override;
    virtual void Draw() override;

    virtual BaseWindow *GetWindow() override;

    void TestRButtonDown();
    virtual void UpdateMouseInput(MouseContext &mouseContext) override;

  private:
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    SuperAssetBrowerController();

  private:
    BaseWindow *mWindow;
};

} // namespace Quad

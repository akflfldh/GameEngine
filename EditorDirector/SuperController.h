#pragma once
#include <InputSystem/InputType.h>
#include <vector>

namespace Core
{
class LogicalWindow;
}

namespace Quad
{

// 장치 입력에대한 컨트롤러에서 관리하는 정보
struct SuperControllerInputContext
{
    // 해당 컨트롤러의 창이 마우스를 캡쳐했는지여부
    bool mMouseCaptureFlag = false;
    // 마우스 캡처를 한 논리적윈도우
    Core::LogicalWindow *mTargetLogicalWindow = nullptr;
};

class BaseWindow;

class SuperController
{
  public:
    SuperController();
    virtual ~SuperController() = 0;
    virtual void Initialize() = 0;

    virtual void PreUpdate() = 0;

    virtual void Update(float deltaTime) = 0;

    // 여기서는 렌더링 위한 사전작업등을한다(렌더아이템구축,제출)
    virtual void EndUpdate() = 0;
    virtual void Draw() = 0;

    virtual void UpdateMouseInput(MouseContext &mouseContext) = 0;

    virtual BaseWindow *GetWindow() = 0;

  protected:
    SuperControllerInputContext mControllerInputContext;
    std::vector<Core::LogicalWindow *> mLogicalWindowList;
};

} // namespace Quad

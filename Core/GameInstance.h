#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreBase/BaseClass.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <cstdint>

#include "GameInstance.generated.h"

namespace UI
{
class UICanvas;
class UIText;
} // namespace UI

class World;
// 실행 모드가 소유하는 플레이 세션의 진입점이다. 맵 전환과 무관한 상태 및 공통 UI를 준비하고 갱신한다.
// Canvas와 UI 요소의 실제 관리는 UIManager에 맡기며, 이 타입은 자신이 만든 FPS 텍스트의 제거를 요청한다.
// BaseClass와 리플렉션은 파생 타입의 조회·생성을 위한 기반이며, 이 객체를 월드 Object나 저장 에셋으로 만들지 않는다.
class CORE_API_LIB REFLECT_CLASS(EngineClass) GameInstance : public BaseClass
{
    GENERATED_BODY(GameInstance)
  public:
    GameInstance();
    virtual ~GameInstance() override;

    virtual void Serialize(Arch &arch) override;

    virtual void Initialize();

    virtual void Start();

    virtual void Update(float deltaTime);

    virtual void EndUpdate();

    virtual void Shutdown();

    void SetCanvas(UI::UICanvas *canvas);

    void SetWorld(World *world);
    World *GetWorld() const;

    void RequestChangeMap(const std::string &mapName);

    void SetMouseMode(Core::MouseMode mode);
    Core::MouseMode GetMouseMode() const;

  protected:
    UI::UICanvas *GetCanvas() const;

  private:
    // 실행 모드에서 전달받는 비소유 참조다. Shutdown이 끝날 때까지 Canvas가 유효해야 한다.
    UI::UICanvas *mCanvas;
    World *mWorld;
    UI::UIText *mFPSText = nullptr;

    float mFPSAccumulatedSeconds = 0.0f;
    uint32_t mFPSFrameCount = 0;

    Core::MouseMode mMouseMode;
};

// 실행 모드가 리플렉션으로 생성한 GameInstance를 소유할 때 사용하는 파괴 정책이다.
// Shutdown은 모드의 EndPlay가 먼저 호출한다. 파괴 시까지 사용자 DLL, 리플렉션 등록과 allocator가 유효해야 한다.
struct CORE_API_LIB ReflectionGameInstanceDeleter
{
    void operator()(GameInstance *instance) const noexcept;
};

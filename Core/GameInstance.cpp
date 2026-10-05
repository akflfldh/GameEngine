#include "GameInstance.h"

#include <Core/World.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIText.h>
#include <cmath>
#include <iomanip>
#include <sstream>

GameInstance::GameInstance() : mCanvas(nullptr), mWorld(nullptr), mMouseMode(Core::MouseMode::Free) {}

GameInstance::~GameInstance() {}

void ReflectionGameInstanceDeleter::operator()(GameInstance *instance) const noexcept
{
    // placement 생성과 엔진 allocator 할당에 대응하여 등록된 실제 타입의 소멸자와 메모리 반환을 사용한다.
    // 일반 delete는 리플렉션의 메모리 반환 경로와 짝이 맞지 않는다.
    if (instance)
        Quad::ReflectionSystem::GetInstance()->DestoryClassInstance(instance);
}

void GameInstance::Serialize(Arch &)
{
    // 플레이 세션 객체는 저장 에셋이 아니다. BaseClass의 공통 인터페이스는 제공하되,
    // Canvas/UI 참조와 FPS 누적 상태는 실행 중에만 유효하므로 직렬화하지 않는다.
}

void GameInstance::Initialize() {}

void GameInstance::Start()
{
    if (mCanvas == nullptr || mFPSText != nullptr)
        return;

    mFPSText = mCanvas->CreateUIElement<UI::UIText>("GameInstanceFPSText");
    if (mFPSText == nullptr)
        return;

    // FPS 표시는 Canvas의 테마에 의존하지 않으며, Begin 시에도 명시한 크기와 색상을 유지한다.
    mFPSText->SetStyleRole(UI::EUIStyleRole::eNone);
    mFPSText->SetSize(160.0f, 28.0f);
    mFPSText->SetFontSize(20.0f);
    mFPSText->SetTextColor(UI::UIColor::White);
    mFPSText->SetText("FPS: --");
    mFPSText->SetOnlyVisible(true);

    // 절대 좌표 대신 Canvas의 오른쪽·위쪽 가장자리를 기준으로 삼아 창 크기 변경에도 위치를 유지한다.
    mFPSText->SetHorizontalAnchor(1.0f);
    mFPSText->SetHorizontalOffset(-(mFPSText->GetSize().X + 12.0f));
    mFPSText->SetVerticalAnchor(0.0f);
    mFPSText->SetVerticalOffset(12.0f);
    // 자체 Pivot이 (0, 0)이므로 기존 우측/하단 정렬은 크기를 포함한 음수 Offset으로 보존한다.
    mFPSText->mOnChangedSizeCallbackSystem.Register(
        [](UI::UIElement *element)
        {
            element->SetHorizontalOffset(-(element->GetSize().X + 12.0f));
            element->UpdatePosAnchor();
        });
    mFPSText->UpdatePosAnchor();

    mFPSAccumulatedSeconds = 0.0f;
    mFPSFrameCount = 0;
}

void GameInstance::Update(float deltaTime)
{
    if (mFPSText == nullptr || !std::isfinite(deltaTime) || deltaTime <= 0.0f)
        return;

    mFPSAccumulatedSeconds += deltaTime;
    ++mFPSFrameCount;

    // deltaTime은 초 단위다. 프레임마다 역수를 표시하지 않고, 구간의 프레임 수/실제 누적 시간을 사용한다.
    // 0.5초마다 표시를 갱신하여 숫자의 흔들림과 텍스트 메시 재생성 빈도를 줄인다.
    if (mFPSAccumulatedSeconds < 0.5f)
        return;

    const float fps = static_cast<float>(mFPSFrameCount) / mFPSAccumulatedSeconds;
    std::ostringstream text;
    text << "FPS: " << std::fixed << std::setprecision(1) << fps;
    mFPSText->SetText(text.str());

    mFPSAccumulatedSeconds = 0.0f;
    mFPSFrameCount = 0;
}

void GameInstance::EndUpdate() {}

void GameInstance::Shutdown()
{
    // UIManager의 지연 삭제 경로를 사용한다. 공유 Canvas는 남기고 이 세션이 만든 텍스트만 제거한다.
    if (mFPSText)
    {
        mFPSText->Destroy();
        mFPSText = nullptr;
    }

    mFPSAccumulatedSeconds = 0.0f;
    mFPSFrameCount = 0;
    mCanvas = nullptr;
}

void GameInstance::SetCanvas(UI::UICanvas *canvas)
{

    mCanvas = canvas;
}

UI::UICanvas *GameInstance::GetCanvas() const
{
    return mCanvas;
}

void GameInstance::SetWorld(World *world)
{

    mWorld = world;
}

World *GameInstance::GetWorld() const
{

    return mWorld;
}

void GameInstance::RequestChangeMap(const std::string &mapName)
{

    if (mWorld)
    {
        mWorld->RequestChangeMap(mapName);
    }
}

void GameInstance::SetMouseMode(Core::MouseMode mode)
{

    mMouseMode = mode;
}
Core::MouseMode GameInstance::GetMouseMode() const
{

    return mMouseMode;
}

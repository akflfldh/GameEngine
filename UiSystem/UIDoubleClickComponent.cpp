#include "UIDoubleClickComponent.h"
#include <CoreBase/TimerManager.h>
UI::UIDoubleClickComponent::UIDoubleClickComponent() : mHover(false), mLastClickTime(0.0f) {}

UI::UIDoubleClickComponent::~UIDoubleClickComponent() {}

void UI::UIDoubleClickComponent::Update(float deltaTime) {}

int UI::UIDoubleClickComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UIDoubleClickComponent::OnHover(int x, int y)
{

    mHover = true;
}

void UI::UIDoubleClickComponent::OnReleaseHover()
{

    mHover = false;
}

void UI::UIDoubleClickComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                             bool &bConsume)
{

    if (mHover)
    {

        float currentTime = TimerManager::GetInstance()->GetMainTimer().TotalTime();
        // 시간 차이 확인
        float deltaTime = currentTime - mLastClickTime;
        // 더블클릭 : true or false
        if (mDoubleClickUpperDeltaTime > deltaTime)
        {
            // true
            // true이면 콜백호출 , mPrePress = false;
            mOnDoubleClickedCallbackSystem.ExecuteCallbacks();
            mLastClickTime = -100.0f;
        }
        else
        {
            // false
            // false이면 이 클릭을 더블클릭 시작으로 간주 prePress를 true로 유지
            // 시간저장
            mLastClickTime = currentTime;
        }
    }
}

void UI::UIDoubleClickComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                           bool &bConsume)
{
}

bool UI::UIDoubleClickComponent::IsHovered() const
{
    return mHover;
}

void UI::UIDoubleClickComponent::SetDoubleClickUpperDeltaTime(float deltaTime)
{

    mDoubleClickUpperDeltaTime = deltaTime;
}

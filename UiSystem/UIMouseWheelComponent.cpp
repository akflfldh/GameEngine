#include "UIMouseWheelComponent.h"

void UI::UIMouseWheelComponent::Update(float deltaTime) {}

void UI::UIMouseWheelComponent::OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                             bool &bConsume)
{
    mOnMouseWheelCallbackSystem.ExecuteCallbacks(inputData.mouseWheelData.mWheelDelta);
    bConsume = true;
}
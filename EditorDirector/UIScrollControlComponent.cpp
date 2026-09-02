#include "UIScrollControlComponent.h"
#include <UiSystem/UIElement.h>
#include <UiSystem/UIImageComponent.h>

#include <algorithm>

UIScrollControlComponent::UIScrollControlComponent()
    : mHover(false), mHoverColorIntensity(2.0f), mScrollDirection(EScrollDirection::eVertical), mPress(false),
      mDestElement(nullptr), mClickOffset(0.0f)
{
}

UIScrollControlComponent::~UIScrollControlComponent() {}

void UIScrollControlComponent::Update(float deltaTime) {}

int UIScrollControlComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y); // 1
}

void UIScrollControlComponent::HandleInput(const Quad::RawInputData &inputData, bool &bConsume) {}

void UIScrollControlComponent::OnHover(int x, int y)
{

    mHover = true;
    UI::UIImageComponent *imageCom = nullptr;
    GetOwnerUIElement()->GetComponents<UI::UIImageComponent>(&imageCom, 1);

    if (imageCom)
    {
        glm::vec3 imageColor = imageCom->GetColor();
        mOriginalColor = imageColor;
        imageColor *= mHoverColorIntensity;
        imageCom->SetColor(imageColor.r, imageColor.g, imageColor.b);
    }
}

void UIScrollControlComponent::OnReleaseHover()
{

    if (mHover)
    {

        mHover = false;

        if (mPress)
        {
            return;
        }

        UI::UIImageComponent *imageCom = nullptr;
        GetOwnerUIElement()->GetComponents<UI::UIImageComponent>(&imageCom, 1);
        if (imageCom)
        {
            imageCom->SetColor(mOriginalColor.r, mOriginalColor.g, mOriginalColor.b);
        }
    }
}

void UIScrollControlComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    if (mPress)
    {
        if (inputData.mouseMoveData.mAccumulateFlag == false)
            return;

        if (mScrollDirection == EScrollDirection::eVertical)
        {

            UI::UIElement *ownerElement = GetOwnerUIElement();
            UI::UIElement *destElement = mDestElement;
            UI::UIElement *parentElement = ownerElement->GetParent();

            if (destElement == nullptr)
                return;

            glm::vec2 destLocalPos = destElement->mTransform.GetLocalPosition();

            glm::vec2 controlPanelPos = ownerElement->mTransform.GetWorldPosition();
            glm::vec2 destWorldPos = destElement->mTransform.GetWorldPosition();
            glm::vec2 parentWorldPos = parentElement->mTransform.GetWorldPosition();

            glm::vec2 parentSize = parentElement->mTransform.GetSize();
            glm::vec2 destSize = destElement->mTransform.GetSize();

            float controlPanelHeight = ownerElement->mTransform.GetSize().y;

            //[새로운 control의 위치]
            float newWorldPosY = worldPosY - mClickOffset;

            // 위치 조정
            // parentsize보다 control size가 작아서 min> max 문제발생

            float minWorldPosY = parentWorldPos.y;
            float maxWorldPosY = parentWorldPos.y + parentSize.g - controlPanelHeight;
            //-controlPanelHeight;
            //[parentY    parentBottom - controlHeight]

            if (minWorldPosY > maxWorldPosY)
                std::swap(minWorldPosY, maxWorldPosY);
            newWorldPosY = std::clamp(newWorldPosY, minWorldPosY, maxWorldPosY);

            GetOwnerUIElement()->SetPositionWorld({controlPanelPos.r, newWorldPosY});

            float localPosY = newWorldPosY - parentWorldPos.g;
            // 범위 :  [0  parentSize]
            // 정규화  [0.0f 1.0f]
            localPosY /= (parentSize.g - controlPanelHeight);
            float destShiftRatio = std::clamp(localPosY, 0.0f, 1.0f);

            float destOriginWorldPosY = parentWorldPos.y;

            float destScrollRange = destSize.y - parentElement->mTransform.GetSize().y;

            // 음수이면 destSize.y가 parentElement.y보다 작다는것임으로 컨트롤러를 움직여도 움직이지않는게 맞다.
            if (destScrollRange < 0.0f)
                destScrollRange = 0.0f;

            float destNewWorldPosY = -destScrollRange * destShiftRatio;
            // mDestElement->SetPositionWorld({destWorldPos.x, destNewWorldPosY});

            mDestElement->SetPositionLocal(destLocalPos.x, destNewWorldPosY);
        }
    }
    else
    {
    }
}

void UIScrollControlComponent::OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume) {}

void UIScrollControlComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                           bool &bConsume)
{

    if (mHover)
    {
        RequestMouseCaptureInput();

        mPress = true;
        UI::UIImageComponent *imageCom = nullptr;
        GetOwnerUIElement()->GetComponents<UI::UIImageComponent>(&imageCom, 1);
        if (imageCom)
        {
            imageCom->SetColor(0.0f, 0.6f, 0);
        }

        glm::vec2 posW = GetOwnerUIElement()->mTransform.GetWorldPosition();
        mClickOffset = worldPosY - posW.g; // 클릭한 지점에대한 offset

        bConsume = true;
    }
}

void UIScrollControlComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                         bool &bConsume)
{

    mPress = false;

    ReleaseMouseCaptureInput();
    if (mHover)
    {
        glm::vec3 imageColor = mOriginalColor;
        imageColor *= mHoverColorIntensity;
        UI::UIImageComponent *imageCom = nullptr;
        GetOwnerUIElement()->GetComponents<UI::UIImageComponent>(&imageCom, 1);
        if (imageCom)
        {
            imageCom->SetColor(imageColor.r, imageColor.g, imageColor.b);
        }
        bConsume = true;
    }
    else
    {
        UI::UIImageComponent *imageCom = nullptr;
        GetOwnerUIElement()->GetComponents<UI::UIImageComponent>(&imageCom, 1);
        if (imageCom)
        {
            imageCom->SetColor(mOriginalColor.r, mOriginalColor.g, mOriginalColor.b);
        }

        bConsume = true;
    }
    mClickOffset = 0.0f;
}

void UIScrollControlComponent::OnKeyDown(const Quad::EKeyCode &key, bool &bConsume) {}

bool UIScrollControlComponent::IsHovered() const
{
    return mHover;
}

void UIScrollControlComponent::OnChangeHoverPart(int before, int after) {}

void UIScrollControlComponent::SetScrollDirection(EScrollDirection dir)
{

    mScrollDirection = dir;
}

EScrollDirection UIScrollControlComponent::GetScrollDirection() const
{
    return mScrollDirection;
}

void UIScrollControlComponent::SetDestElement(UI::UIElement *dest)
{
    mDestElement = dest;
}

void UIScrollControlComponent::UpdateDestPanelPos()
{
    UI::UIElement *ownerElement = GetOwnerUIElement();
    UI::UIElement *destElement = mDestElement;
    UI::UIElement *parentElement = ownerElement->GetParent();

    if (destElement == nullptr)
        return;

    glm::vec2 controlPanelPos = ownerElement->mTransform.GetWorldPosition();
    glm::vec2 destWorldPos = destElement->mTransform.GetWorldPosition();
    glm::vec2 parentWorldPos = parentElement->mTransform.GetWorldPosition();

    glm::vec2 parentSize = parentElement->mTransform.GetSize();
    glm::vec2 destSize = destElement->mTransform.GetSize();

    float localPosY = parentWorldPos.y - controlPanelPos.y;
    // 범위 :  [ -parnetSiZE/2  parentSize/2]
    // 정규화  [0.0f 1.0f]
    localPosY += parentSize.g / 2;
    localPosY /= parentSize.g;
    float destShiftRatio = std::clamp(localPosY, 0.0f, 1.0f);

    float destOriginWorldPosY = parentWorldPos.y + parentElement->mTransform.GetSize().y / 2 - destSize.g / 2;

    float destScrollRange = destSize.y - parentElement->mTransform.GetSize().y;

    mDestElement->SetPositionWorld({destWorldPos.x, destScrollRange * destShiftRatio + destOriginWorldPosY});
}

#include "UiSystem/UIResizeGizmoRenderableComponent.h"
#include "UiSystem/UIElement.h"
#include <CoreAsset/UIMaterialManager.h>

UI::UIResizeGizmoRenderableComponent::UIResizeGizmoRenderableComponent()
    : mThickness(10.0f), mActiveBorder(-1), mActiveColor({1.0f, 0.0f, 0.0f, 1.0f})
{

    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();
    mMeshComponent.mUIMaterial = uiMaterialManager->GetDefaultGizmoMaterial();

    SetDepthValue(UINT32_MAX); // 항상 가장위
}

UI::UIResizeGizmoRenderableComponent::~UIResizeGizmoRenderableComponent() {}

void UI::UIResizeGizmoRenderableComponent::Update() {}

void UI::UIResizeGizmoRenderableComponent::SetColor(const CoreMath::Vector4 &color)
{
    mColor = color;
}
const CoreMath::Vector4 &UI::UIResizeGizmoRenderableComponent::GetColor() const
{
    return mColor;
}

void UI::UIResizeGizmoRenderableComponent::SetThickness(float thickness)
{
    mThickness = thickness;
}
float UI::UIResizeGizmoRenderableComponent::GetThickness() const
{

    return mThickness;
}

inline size_t UI::UIResizeGizmoRenderableComponent::GetVertexNum() const
{
    return 16;
}

void UI::UIResizeGizmoRenderableComponent::GetVertices(UIVertex *oUIVertices) const
{
    float halfThickness = mThickness / 2;

    UIElement *uiElement = GetOwnerUIElement();

    // 4개의 꼭짓점 좌표를 얻는다.
    CoreMath::Vector2 vertices[4];
    uiElement->mTransform.GetQuadWorldPoints(vertices);

    // 각 모서리별로 4개의 정점을 계산한다.

    // 0 - 1 (topleft, top right)
    oUIVertices[0].mPos = vertices[0];
    oUIVertices[1].mPos = vertices[1];
    oUIVertices[2].mPos = vertices[1];
    oUIVertices[3].mPos = vertices[0];

    oUIVertices[0].mPos.Y += halfThickness;
    oUIVertices[3].mPos.Y -= halfThickness;

    oUIVertices[1].mPos.Y += halfThickness;
    oUIVertices[2].mPos.Y -= halfThickness;

    // 1-2 (top right, bottom right)
    oUIVertices[4].mPos = vertices[1];
    oUIVertices[5].mPos = vertices[1];
    oUIVertices[6].mPos = vertices[2];
    oUIVertices[7].mPos = vertices[2];

    oUIVertices[4].mPos.X -= halfThickness;
    oUIVertices[7].mPos.X -= halfThickness;

    oUIVertices[5].mPos.X += halfThickness;
    oUIVertices[6].mPos.X += halfThickness;

    // 0 - 3 (top left , bottom left)
    oUIVertices[8].mPos = vertices[0];
    oUIVertices[9].mPos = vertices[0];
    oUIVertices[10].mPos = vertices[3];
    oUIVertices[11].mPos = vertices[3];

    oUIVertices[8].mPos.X -= halfThickness;
    oUIVertices[9].mPos.X += halfThickness;

    oUIVertices[10].mPos.X += halfThickness;
    oUIVertices[11].mPos.X -= halfThickness;

    // 3 - 2 (bottom left, bottom right)
    oUIVertices[12].mPos = vertices[3];
    oUIVertices[13].mPos = vertices[2];
    oUIVertices[14].mPos = vertices[2];
    oUIVertices[15].mPos = vertices[3];

    oUIVertices[12].mPos.Y += halfThickness;
    oUIVertices[15].mPos.Y -= halfThickness;

    oUIVertices[13].mPos.Y += halfThickness;
    oUIVertices[14].mPos.Y -= halfThickness;
    // 컬러를 적용한다.

    for (int i = 0; i < 16; ++i)
    {
        oUIVertices[i].mColor = mColor;
    }

    if (mActiveBorder != -1)
    {
        for (int i = 0; i < 4; ++i)
            oUIVertices[mActiveBorder * 4 + i].mColor = mActiveColor;
    }
}

size_t UI::UIResizeGizmoRenderableComponent::GetIndexNum() const
{
    return 24;
}

void UI::UIResizeGizmoRenderableComponent::GetIndices(uint32_t *oIndices) const
{

    for (int i = 0; i < GetIndexNum() / 6; ++i)
    {
        // 삼각형 1
        //  0  1 2
        oIndices[i * 6 + 0] = i * 4 + 0;
        oIndices[i * 6 + 1] = i * 4 + 1;
        oIndices[i * 6 + 2] = i * 4 + 3;

        // 삼각형 2
        // 3 1 2
        oIndices[i * 6 + 3] = i * 4 + 3;
        oIndices[i * 6 + 4] = i * 4 + 1;
        oIndices[i * 6 + 5] = i * 4 + 2;
    }
}

bool UI::UIResizeGizmoRenderableComponent::IsPointInside(float x, float y) const
{

    std::vector<UI::UIVertex> vertices(16);
    GetVertices(vertices.data());

    // 여기서는 4개의 사각형에대해서 어느방향인지는 고려할 필요가없다
    // 그것은 input event가 전달되면 고려할것

    for (int i = 0; i < 4; ++i)
    {
        // 왼쪽위, 오른쪽아래 정점위치만으로도 판정가능
        int leftTopIndex = i * 4 + 0;
        int rightBottomIndex = i * 4 + 2;

        if (vertices[leftTopIndex].mPos.X > x)
            continue;
        if (vertices[rightBottomIndex].mPos.X < x)
            continue;
        if (vertices[leftTopIndex].mPos.Y < y)
            continue;
        if (vertices[rightBottomIndex].mPos.Y > y)
            continue;

        return true;
    }
    return false;
}

void UI::UIResizeGizmoRenderableComponent::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                                                 bool &oCaptureActiveRequestFlag,
                                                                 bool &oCaptureReleaseRequestFlag)
{

    if (mouseInputContext.mCaptureState != UI::EUIMouseCaptureType::eNone)
        UpdateOnCapture(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    else
        UpdateOnHover(mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
}

int UI::UIResizeGizmoRenderableComponent::EvaluateActiveBorder(float x, float y) const
{
    std::vector<UI::UIVertex> vertices(16);
    GetVertices(vertices.data());

    for (int i = 0; i < 4; ++i)
    {
        // 테두리의 각각 왼쪽위, 오른쪽아래 정점의 위치로 범위체크
        int topLeftIndex = i * 4 + 0;
        int bottomRightIndex = i * 4 + 2;

        if (vertices[topLeftIndex].mPos.X > x)
            continue;

        if (vertices[bottomRightIndex].mPos.X < x)
            continue;

        if (vertices[topLeftIndex].mPos.Y < y)
            continue;

        if (vertices[bottomRightIndex].mPos.Y > y)
            continue;

        return i;
    }

    return -1;
}

void UI::UIResizeGizmoRenderableComponent::UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext,
                                                           bool &captureActiveRequestFlag,
                                                           bool &oCaptureReleaseRequestFlag)
{
    if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eEnter)
    {
        // 캡처 요청이 수락됨

        mActiveColor = {1, 1, 0, 1};
    }
    else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eHeld)
    {

        // l버튼이 release되었다 그러면 capture해제요청
        if (mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eLButtonReleased)
        {
            oCaptureReleaseRequestFlag = true;
            mActiveColor = {1, 0, 0, 1};
            mActiveBorder = -1;
        }

        if (mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eMoved)
        {
            float cursorX = mouseInputContext.mMouseContext.mWorldPosX;
            float cursorY = mouseInputContext.mMouseContext.mWorldPosY;

            UIElement *uiElement = GetOwnerUIElement();
            CoreMath::Vector2 centerWorld = uiElement->mTransform.GetWorldPosition();
            CoreMath::Vector2 centerLocal = uiElement->mTransform.GetLocalPosition();

            CoreMath::Vector2 size = uiElement->mTransform.GetSize();

            float distance = 0.0f;
            if (mActiveBorder == 0)
            {
                // 위
                float topPos = centerWorld.Y + size.Y / 2;

                distance = cursorY - topPos;

                centerLocal.Y += distance / 2;
                size.Y += distance;
            }
            else if (mActiveBorder == 1)
            {
                // 오
                float rightPos = centerWorld.X + size.X / 2;
                // 커서와 왼쪽 좌표사이의거리 만큼 center를 이동시켜 조정하고
                distance = cursorX - rightPos;

                centerLocal.X += distance / 2;
                size.X += distance;
            }
            else if (mActiveBorder == 2)
            {
                // 왼
                float leftPos = centerWorld.X - size.X / 2;
                // 커서와 왼쪽 좌표사이의거리 만큼 center를 이동시켜 조정하고
                distance = cursorX - leftPos;

                centerLocal.X += distance / 2;
                size.X -= distance;
            }
            else if (mActiveBorder == 3)
            {
                // 아래

                float bottomPos = centerWorld.Y - size.Y / 2;

                distance = cursorY - bottomPos;

                centerLocal.Y += distance / 2;
                size.Y -= distance;
            }

            uiElement->mTransform.SetSize(size);
            uiElement->mTransform.SetPositionLocal(centerLocal);
        }
    }
    else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eRelease)
    {
    }
}

void UI::UIResizeGizmoRenderableComponent::UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext,
                                                         bool &oCaptureActiveRequestFlag,
                                                         bool &oCaptureReleaseRequestFlag)
{
    // hover에 들어갔든 빠져나갔든, l버튼이 release되었다면 , 색상은 빨강, 테투리막대 비활성화
    if ((mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eLButtonReleased))
    {
        mActiveColor = {1, 0, 0, 1};
        mActiveBorder = -1;
    }

    switch (mouseInputContext.mHoverState)
    {
    case EUIMouseHoverType::eEnter:
    {
        // 어떤 테투리에 HOVER인지 판정
        mActiveBorder = EvaluateActiveBorder(mouseInputContext.mMouseContext.mWorldPosX,
                                             mouseInputContext.mMouseContext.mWorldPosY); // 활성화된 테투리 번호 지정
    }
    break;

    case EUIMouseHoverType::eHeld:

        // hover상태에서 처음눌르는경우
        //
        if ((mouseInputContext.mMouseContext.mMouseState & Quad::EMouseState::eLButtonPressed))
        {
            // hover상태에서 lbutton 클릭 -> capture 요청
            // 캡처상테에서만 노란색이 유지될거다.
            oCaptureActiveRequestFlag = true;
        }
        else
        {
            mActiveBorder =
                EvaluateActiveBorder(mouseInputContext.mMouseContext.mWorldPosX,
                                     mouseInputContext.mMouseContext.mWorldPosY); // 활성화된 테투리 번호 지정

            mActiveColor = {1, 0, 0, 1};
        }

        break;
    case EUIMouseHoverType::eRelease:
        // release가 필요없을거 같다
        {

            mActiveBorder = -1;
            mActiveColor = {1, 0, 0, 1};
        }

        break;
    }
}

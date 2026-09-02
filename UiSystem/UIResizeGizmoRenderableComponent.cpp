#include "UiSystem/UIResizeGizmoRenderableComponent.h"
#include "UIImageComponent.h"
#include "UiSystem/UIElement.h"
#include <CoreAsset/UIMaterialManager.h>

UI::UIResizeGizmoRenderableComponent::UIResizeGizmoRenderableComponent()
    : mThickness(10.0f), mActiveBorder(-1), mActiveColor({1.0f, 0.0f, 0.0f, 1.0f}), bHover(false), bPress(false)

{

    CoreAsset::UIMaterialManager *uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();
    mMeshComponent.mUIMaterial = uiMaterialManager->GetDefaultGizmoMaterial();

    SetDepthValue(UINT32_MAX); // 항상 가장위
}

UI::UIResizeGizmoRenderableComponent::~UIResizeGizmoRenderableComponent() {}

void UI::UIResizeGizmoRenderableComponent::Update(float deltaTime) {}

void UI::UIResizeGizmoRenderableComponent::SetColor(const glm::vec4 &color)
{
    mColor = color;
}

const glm::vec4 &UI::UIResizeGizmoRenderableComponent::GetColor() const
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

uint32_t UI::UIResizeGizmoRenderableComponent::GetVertices(UIVertex *oUIVertices) const
{

    if (oUIVertices == nullptr)
    {
        return 16;
    }

    float halfThickness = mThickness / 2;

    UIElement *uiElement = GetOwnerUIElement();

    // 4개의 꼭짓점 좌표를 얻는다.
    glm::vec2 vertices[4];
    uiElement->mTransform.GetQuadWorldPoints(vertices);

    //    각 모서리별로 4개의 정점을 계산한다.

    // 0 - 1 (topleft, top right)
    oUIVertices[0].mPos = vertices[0];
    oUIVertices[1].mPos = vertices[1];
    oUIVertices[2].mPos = vertices[1];
    oUIVertices[3].mPos = vertices[0];

    oUIVertices[0].mPos.y -= halfThickness;
    oUIVertices[3].mPos.y += halfThickness;

    oUIVertices[1].mPos.y -= halfThickness;
    oUIVertices[2].mPos.y += halfThickness;

    // 1-2 (top right, bottom right)
    oUIVertices[4].mPos = vertices[1];
    oUIVertices[5].mPos = vertices[1];
    oUIVertices[6].mPos = vertices[2];
    oUIVertices[7].mPos = vertices[2];

    oUIVertices[4].mPos.x -= halfThickness;
    oUIVertices[5].mPos.x += halfThickness;

    oUIVertices[6].mPos.x += halfThickness;
    oUIVertices[7].mPos.x -= halfThickness;

    // 0 - 3 (top left , bottom left)
    oUIVertices[8].mPos = vertices[0];
    oUIVertices[9].mPos = vertices[0];
    oUIVertices[10].mPos = vertices[3];
    oUIVertices[11].mPos = vertices[3];

    oUIVertices[8].mPos.x -= halfThickness;
    oUIVertices[9].mPos.x += halfThickness;

    oUIVertices[10].mPos.x += halfThickness;
    oUIVertices[11].mPos.x -= halfThickness;

    // 3 - 2 (bottom left, bottom right)
    oUIVertices[12].mPos = vertices[3];
    oUIVertices[13].mPos = vertices[2];
    oUIVertices[14].mPos = vertices[2];
    oUIVertices[15].mPos = vertices[3];

    oUIVertices[12].mPos.y -= halfThickness;
    oUIVertices[15].mPos.y += halfThickness;

    oUIVertices[13].mPos.y -= halfThickness;
    oUIVertices[14].mPos.y += halfThickness;
    // 컬러를 적용한다.

    for (int i = 0; i < 16; ++i)
    {
        oUIVertices[i].mColor = UIColorUtility::PackColor(mColor);
    }

    if (mActiveBorder != -1)
    {
        for (int i = 0; i < 4; ++i)
            oUIVertices[mActiveBorder * 4 + i].mColor = UIColorUtility::PackColor(mActiveColor);
    }

    return 16;
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

int UI::UIResizeGizmoRenderableComponent::IsPointInside(float x, float y) const
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

        if (vertices[leftTopIndex].mPos.x > x)
            continue;
        if (vertices[rightBottomIndex].mPos.x < x)
            continue;
        if (vertices[leftTopIndex].mPos.y > y)
            continue;
        if (vertices[rightBottomIndex].mPos.y < y)
            continue;

        return i + 1;
    }
    return 0;
}

void UI::UIResizeGizmoRenderableComponent::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                                                 bool &oCaptureActiveRequestFlag,
                                                                 bool &oCaptureReleaseRequestFlag)
{
}

void UI::UIResizeGizmoRenderableComponent::OnHover(int x, int y)
{

    bHover = true;

    // 이미 border이 눌러지고있다면 새로운 hover는 무시한다.
    // if (bPress == true)
    //    return;

    // int borderNum = EvaluateActiveBorder(x, y);
    // if (borderNum != -1)
    //{
    //     if (mActiveBorder != borderNum)
    //     {
    //         // 기존에 활성화된 border을 비활성화한다.
    //     }

    //    mActiveBorder = borderNum;
    //    bHover = true;
    //}
}

void UI::UIResizeGizmoRenderableComponent::OnReleaseHover()
{
    bHover = false;
    if (bPress == false)
    {
        mActiveBorder = -1;
    }
}

void UI::UIResizeGizmoRenderableComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX,
                                                       float worldPosY)
{

    if (bPress && inputData.mouseMoveData.mAccumulateFlag)
    {

        float cursorX = worldPosX;
        float cursorY = worldPosY;

        UIElement *uiElement = GetOwnerUIElement();
        glm::vec2 centerWorld = uiElement->mTransform.GetWorldPosition();
        glm::vec2 centerLocal = uiElement->mTransform.GetLocalPosition();

        glm::vec2 size = uiElement->mTransform.GetSize();

        float distance = 0.0f;
        if (mActiveBorder == 0)
        {
            // 위
            float topPos = centerWorld.y;

            distance = topPos - cursorY;

            centerLocal.y -= distance;
            size.y += distance;
        }
        else if (mActiveBorder == 1)
        {
            // 오
            float rightPos = centerWorld.x + size.x;
            // 커서와 왼쪽 좌표사이의거리 만큼 center를 이동시켜 조정하고
            distance = cursorX - rightPos;

            size.x += distance;
        }
        else if (mActiveBorder == 2)
        {
            // 왼
            float leftPos = centerWorld.x;
            // 커서와 왼쪽 좌표사이의거리 만큼 center를 이동시켜 조정하고
            distance = cursorX - leftPos;

            centerLocal.x += distance;
            size.x -= distance;
        }
        else if (mActiveBorder == 3)
        {
            // 아래

            float bottomPos = centerWorld.y + size.y;

            distance = cursorY - bottomPos;
            size.y += distance;
        }

        uiElement->SetSize(size);
        uiElement->SetPositionLocal(centerLocal);
    }
}

void UI::UIResizeGizmoRenderableComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX,
                                                       float worldPosY, bool &bConsume)
{
    if (bHover)
    {

        bPress = true;
        bConsume = true;
        GetOwnerUIElement()->RequestMouseCaptureInput(this);
    }
}

void UI::UIResizeGizmoRenderableComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                                     bool &bConsume)
{
    if (bPress)
    {
        bPress = false;
        bConsume = true;

        GetOwnerUIElement()->ReleaseMouseCaptureInput();
    }
}

bool UI::UIResizeGizmoRenderableComponent::IsHovered() const
{
    return bHover;
}

void UI::UIResizeGizmoRenderableComponent::OnChangeHoverPart(int before, int after)
{
    // press상태라면 캡처상태이니 영역을 교체하지않는다.
    // 따라서 press상태가 아닐때만 교체된다.
    if (!bPress)
    {
        UIRenderableComponent::OnChangeHoverPart(before, after);
        mActiveBorder = after - 1;
    }
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

        if (vertices[topLeftIndex].mPos.x > x)
            continue;

        if (vertices[bottomRightIndex].mPos.x < x)
            continue;

        if (vertices[topLeftIndex].mPos.y > y)
            continue;

        if (vertices[bottomRightIndex].mPos.y < y)
            continue;

        return i;
    }

    return -1;
}

#include "UiSystem/UIManager.h"
#include "UiSystem/UICanvas.h"
#include "UiSystem/UIComponentFactory.h"
#include "UiSystem/UIElement.h"
#include <Logger/Logger.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UiSystem/IViewContextInterface.h>
#include <stack>

#include <InputSystem/InputSystem.h>

UI::UIManager *UI::UIManager::GetInstance()
{
    static UIManager manager;
    return &manager;
}

bool UI::UIManager::HiTest(UICanvas *canvas, float worldPosX, float worldPosY)
{

    const auto &childList = canvas->GetTopChildUIElement();

    for (auto element : childList)
    {
        if (element == nullptr || element->GetActiveFlag() == false || element->GetOnlyVisible())
            continue;

        if (element->IsPointInside(worldPosX, worldPosY))
            return true;
    }

    return false;
}

UI::UIElement *UI::UIManager::GetHittedElement(UICanvas *canvas, float worldPosX, float worldPosY)
{

    if (canvas == nullptr)
        return nullptr;

    return canvas->GetHittedElement(worldPosX, worldPosY);
}

UI::UIManager::UIManager() : mNextUICanvasID(1), mNextUIElementID(0) {}

void UI::UIManager::Begin()
{

    for (UICanvas *canvas : mActiveCanvasList)
    {
        canvas->Begin();
    }
}

void UI::UIManager::BeginCanvas(UICanvasID canvasID)
{

    auto it = mUICanvasTable.find(canvasID);

    if (it != mUICanvasTable.end())
    {

        UICanvas *canvas = it->second;

        canvas->Begin();
    }
}

void UI::UIManager::Update(float deltaTime)
{

    for (UICanvas *canvas : mActiveCanvasList)
    {
        canvas->Update(deltaTime);
    }
}

void UI::UIManager::CleanUp()
{

    DestroyDeadUIElement();
}

void UI::UIManager::EndFrame()
{

    //  DestroyDeadUIElement();
}

void UI::UIManager::EvaulateInputState(const Quad::MouseContext &mouseContext,
                                       const std::vector<UICanvas *> &canvasList, size_t canvasNum)
{

    // 이미 캡처가 설정된 ui가있다면 그 ui에게만 마우스 입력상태가 전달될것이기에
    // hover판정을 할필요가없다.
    UIElement *newHoverUIElement = nullptr;

    uint32_t topUICanvasDepthValue = 0;
    uint32_t topUIElementDepthValue = 0;
    if (mInputStateContext.mCurrMouseCapturedUIElement == nullptr)
    {

        float mousePosX = mouseContext.mWorldPosX;
        float mousePosY = mouseContext.mWorldPosY;

        UIElement *newCapturedUIElement = nullptr;

        // top -> down방식으로 부모-자식 계층적으로 hover 판정수행
        // stack을통해 비재귀적으로 구현

        for (int i = 0; i < canvasNum; ++i)
        {

            UICanvas *canvas = canvasList[i];
            if (canvas->GetActiveFlag() == false)
                continue;

            if (canvas->GetDepthValue() < topUICanvasDepthValue)
                continue;

            topUICanvasDepthValue = canvas->GetDepthValue();

            const std::vector<UIElement *> &childUIElementVector = canvas->GetChildUIElementAll();
            std::stack<UI::UIElement *> uiElementStack;

            for (auto element : childUIElementVector)
            {
                uiElementStack.push(element);
            }

            while (!uiElementStack.empty())
            {
                UIElement *uiElement = uiElementStack.top();
                uiElementStack.pop();

                if (uiElement->GetActiveFlag() == false)
                    continue;

                // 영역판정
                // 영역판정함수 호출()

                if (uiElement->IsPointInside(mousePosX, mousePosY) == false ||
                    topUIElementDepthValue > uiElement->GetDepthValue())
                    continue;

                newHoverUIElement = uiElement;
                topUIElementDepthValue = uiElement->GetDepthValue();

                // 영역판정 성공?
                // 그럼 현재임시로설정된 hover element와 overlay값비교후 설정
                // 그리고 uiElement의 자식도 판정에 참여가능
                // 따라서 스택에 넣는다

                // 영역판정실패? -> 자식들도 모두실패한다. 따라서 바로 넘어간다.

                for (auto element : uiElement->GetChildVector())
                {
                    if (element->GetActiveFlag() == false)
                        continue;
                    uiElementStack.push(element);
                }
            }
        }
        // 최종 평가 상태 저장
        mInputStateContext.mCurrMouseCapturedUIElement = newCapturedUIElement;
    }

    // 최종 평가 상태 저장
    mInputStateContext.mPreHoverUIElement = mInputStateContext.mCurrHoverUIElement;
    mInputStateContext.mCurrHoverUIElement = newHoverUIElement;
}

void UI::UIManager::SendInputEventToTargetElements(const Quad::MouseContext &mouseContext)
{

    UIManagerMouseInputContext uiMouseInputContext;
    uiMouseInputContext.mMouseContext = mouseContext;

    // 각 target element들이 속한 canvas -> window 를얻어
    // client 영역으로 변환 , 그후 viewport를 얻어야해,  viewport는 uiManager 담당이아닌데.
    // 그후 ndc공간에서 -> world 공간으로 변환이 필요

    // 가장좋은것은 몰라도 가능하게하는거지

    bool captureActiveFlag = false;
    bool captureReleaseFlag = false;

    // 캡처된 요소가 있다면 그 요소에게만 입력이벤트를 전달한다.
    if (mInputStateContext.mCurrMouseCapturedUIElement != nullptr)
    {
        if (mInputStateContext.mCaptureEnterFlag == true)
        {
            uiMouseInputContext.mCaptureState = EUIMouseCaptureType::eEnter;
        }
        else
        {
            uiMouseInputContext.mCaptureState = EUIMouseCaptureType::eHeld;
        }

        mInputStateContext.mCurrMouseCapturedUIElement->UpdateMouseInputEvent(uiMouseInputContext, captureActiveFlag,
                                                                              captureReleaseFlag);

        if (captureReleaseFlag == true)
        {
            mInputStateContext.mCurrMouseCapturedUIElement = nullptr;
        }

        mInputStateContext.mCaptureEnterFlag = false;
    }
    else
    {
        uiMouseInputContext.mCaptureState = EUIMouseCaptureType::eNone;
        // 캡처된요소가없다면
        // preHover, currHover에게 각각 적절한 입력이벤트를 전달
        if (mInputStateContext.mPreHoverUIElement == mInputStateContext.mCurrHoverUIElement)
        {
            // 만약 preHover,currHover가 동일하다면 HoverHeld 이벤트 전달

            if (mInputStateContext.mPreHoverUIElement != nullptr)
            {

                uiMouseInputContext.mHoverState = EUIMouseHoverType::eHeld;

                mInputStateContext.mPreHoverUIElement->UpdateMouseInputEvent(uiMouseInputContext, captureActiveFlag,
                                                                             captureReleaseFlag);
            }
        }
        else
        {

            // 그렇지않으면 preHover에게는 ReleaseHover이벤트,
            // currHover에는 EnterHover이벤트 전달
            // 당연히 mouse 정보들도 함께 전달.
            // uiMouseEventContext 가 해당 이벤트 id와 마우스정보를 필드로 가지는 구조체일것이다.

            if (mInputStateContext.mPreHoverUIElement != nullptr)
            {
                uiMouseInputContext.mHoverState = EUIMouseHoverType::eRelease;
                mInputStateContext.mPreHoverUIElement->UpdateMouseInputEvent(uiMouseInputContext, captureActiveFlag,
                                                                             captureReleaseFlag);
            }

            if (mInputStateContext.mCurrHoverUIElement != nullptr)
            {
                uiMouseInputContext.mHoverState = EUIMouseHoverType::eEnter;
                mInputStateContext.mCurrHoverUIElement->UpdateMouseInputEvent(uiMouseInputContext, captureActiveFlag,
                                                                              captureReleaseFlag);
            }
        }

        if (captureActiveFlag)
        {

            mInputStateContext.mCurrMouseCapturedUIElement = mInputStateContext.mCurrHoverUIElement;
            mInputStateContext.mCaptureEnterFlag = true;
        }
    }
}

UI::UIElement *UI::UIManager::EvaulateHoverElement(float mouseWorldPosX, float mouseWorldPosY, UICanvas *canvas)
{

    // float mouseX = mouseWorldPosX;
    // float mouseY = mouseWorldPosY;
    // uint32_t topUIElementDepthValue = UINT_MAX;

    if (canvas == nullptr /* pCanvas->IsActive() == false */)
        return nullptr;

    return canvas->GetHittedElement(mouseWorldPosX, mouseWorldPosY);

    /* const std::vector<UIElement *> &childUIElementList = canvas->GetTopChildUIElement();

     UI::UIElement *targetElement = nullptr;

     for (auto it = childUIElementList.rbegin(); it != childUIElementList.rend(); ++it)
     {
         UIElement *element = *it;

         UIElement *hoverElement = EvaluateHoverElementRecursive(element, mouseX, mouseY);

         if (hoverElement)
         {
             targetElement = hoverElement;
             break;
         }
     }
     return targetElement;*/
}

UI::UIElement *UI::UIManager::EvaluateHoverElementRecursive(UIElement *element, float mouseWorldPosX,
                                                            float mouseWorldPosY)
{

    if ((element == nullptr) || (element->GetActiveFlag() == false) || (element->GetOnlyVisible()))
        return nullptr;

    if (element->IsPointInside(mouseWorldPosX, mouseWorldPosY))
    {
        const std::vector<UIElement *> &childVec = element->GetChildVector();
        for (auto it = childVec.rbegin(); it != childVec.rend(); ++it)
        {
            UIElement *childElement = *it;

            UIElement *hoverElement = EvaluateHoverElementRecursive(childElement, mouseWorldPosX, mouseWorldPosY);

            if (hoverElement != nullptr)
            {
                return hoverElement;
            }
        }

        return element;
    }

    return nullptr;
}

UI::UIElement *UI::UIManager::ResolveMouseInputTarget(UIElement *hovered) const
{

    return IsCurrentMouseInputScope(hovered) ? hovered : GetCurrentMouseInputScope();
}

void UI::UIManager::RegisterToSlotPool(UIElement *uiElement)
{

    if (uiElement == nullptr)
        return;

    uint64_t newSlotIndex = mNextPoolIndex;
    if (mUIElementFreeIndexPool.empty())
    {
        mUIElementSlotPool.push_back({});
        mNextPoolIndex++;
    }
    else
    {
        newSlotIndex = mUIElementFreeIndexPool.front();
        mUIElementFreeIndexPool.pop();
    }

    mUIElementSlotPool[newSlotIndex].mGeneration++;
    mUIElementSlotPool[newSlotIndex].mUIElement = uiElement;

    uiElement->SetUIElementHandle({mUIElementSlotPool[newSlotIndex].mGeneration, newSlotIndex});
}

void UI::UIManager::UnRegisterToSlotPool(uint64_t slotIndex)
{

    if (mUIElementSlotPool.size() <= slotIndex)
        return;

    if (mUIElementSlotPool[slotIndex].mUIElement == nullptr)
        return;

    mUIElementFreeIndexPool.push(slotIndex);
    mUIElementSlotPool[slotIndex].mUIElement = nullptr;
}

void UI::UIManager::DestroyDeadUIElement()
{
    for (auto element : mDeadUIElementList)
    {
        for (auto child : element->GetChildVector())
        {
            child->SetParent(nullptr);
        }
        element->mChildVector.clear();

        if (element->GetParent())
        {

            auto &childVec = element->GetParent()->mChildVector;
            auto it = std::find(childVec.begin(), childVec.end(), element);

            if (it != childVec.end())
            {
                childVec.erase(it);
            }
        }

        Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
        reflectionSystem->DestoryClassInstance(element);
    }

    mDeadUIElementList.clear();
}

bool UI::UIManager::IsSameOrDescendant(UIElement *target, UIElement *ancestor) const
{

    if (target == nullptr || ancestor == nullptr)
        return false;

    while (target)
    {
        if (target == ancestor)
            return true;

        target = target->GetParent();
    }

    return false;
}

void UI::UIManager::PushMouseInputScope(UIElement *root)
{

    if (root == nullptr)
        return;

    UIMouseInputScopeContext scopeContext;
    scopeContext.mRoot = root;

    mInputStateContext.mMouseInputScopeStack.push_back(scopeContext);
}

void UI::UIManager::PopMouseInputScope(UI::UIElement *root)
{

    if (root == nullptr || mInputStateContext.mMouseInputScopeStack.empty())
        return;

    if (root != mInputStateContext.mMouseInputScopeStack.back().mRoot)
        return;

    mInputStateContext.mMouseInputScopeStack.pop_back();
}

UI::UIElement *UI::UIManager::GetCurrentMouseInputScope() const
{
    if (mInputStateContext.mMouseInputScopeStack.empty())
        return nullptr;

    return mInputStateContext.mMouseInputScopeStack.back().mRoot;
}

bool UI::UIManager::IsCurrentMouseInputScope(UIElement *element) const
{

    UI::UIElement *root = GetCurrentMouseInputScope();
    if (root == nullptr)
        return true;

    return IsSameOrDescendant(element, root);
}

UI::UIElement *UI::UIManager::CreateUIElement(const char *uiElementClassName, const char *instanceName)
{

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    // size_t classSize = reflectionSystem->GetClassSize(uiElementClassName);

    void *instance = reflectionSystem->CreateClassInstance(uiElementClassName);

    if (instance == nullptr)
    {
        return nullptr;
    }
    UIElement *uiElement = (UIElement *)instance;
    uiElement->mName = instanceName;
    uiElement->mID = mNextUIElementID++;

    mUIElementNameTable[instanceName].push_back(uiElement);
    mUIElementTable[uiElement->mID] = uiElement;

    RegisterToSlotPool(uiElement);

    return uiElement;
}

void UI::UIManager::DestoryUIElement(UIElement *element)
{

    UnRegisterToSlotPool(element->GetHandle().mPoolSlotIndex);

    mUIElementNameTable.erase(element->mName);
    mUIElementTable.erase(element->GetID());
    mFreeUIElementIDVector.push_back(element->GetID());

    mDeadUIElementList.push_back(element);

    // delete[] (void *)element;
}

void UI::UIManager::HandleInput(const Quad::RawInputData &inputData, const IViewContextInterface &viewContext,
                                UICanvas *canvas, bool &bConsume)
{

    // 역할 :  worldPos계산, UIElement수준에서 HOVER판정,세세한 이벤트 처리함수 호출

    float mouseWorldPosX = 0.0f;
    float mouseWorldPosY = 0.0f;

    if (inputData.IsMouseEvent())
    {
        mouseWorldPosX = inputData.mouseMoveData.mClientX;
        mouseWorldPosY = inputData.mouseMoveData.mClientY;
        glm::vec2 mouseWorldPos = viewContext.ConverToWorldPos(mouseWorldPosX, mouseWorldPosY);

        mouseWorldPosX = mouseWorldPos.r;
        mouseWorldPosY = mouseWorldPos.g;
    }

    if (inputData.IsMouseEvent())
    {
        UIElement *newHoverElement = EvaulateHoverElement(mouseWorldPosX, mouseWorldPosY, canvas);
        UIElement *targetElement = ResolveMouseInputTarget(newHoverElement);

        if (targetElement == nullptr)
        {
            bConsume = false;
            return;
        }

        DispatchMouseMoveEvent(targetElement, mouseWorldPosX, mouseWorldPosY, inputData);
        DispatchMouseButtonEvent(targetElement, mouseWorldPosX, mouseWorldPosY, inputData);
        DispatchMouseWheelEvent(targetElement, mouseWorldPosX, mouseWorldPosY, inputData);
        // DispatchMouseButtonEvent();

        bConsume = true;

        // if (mInputStateContext.mCurrMouseCapturedUIElement)
        //{

        //    // 캡처된 친구가있다면 그친구에게만 보낸다.

        //    {
        //        // MouseEvent
        //        // mouse move
        //        if (inputData.mInputState & EInputState::eMouseMove)
        //        {
        //            //   그리고 그 캡처된 친구에서만 hover 판정수해
        //            int x = mouseWorldPosX;
        //            int y = mouseWorldPosY;

        //            bool bHover = mInputStateContext.mCurrMouseCapturedUIElement->IsPointInside(x, y);

        //            if ((mInputStateContext.mCurrHoverUIElement != mInputStateContext.mCurrMouseCapturedUIElement) &&
        //                bHover)
        //            {

        //                mInputStateContext.mCurrMouseCapturedUIElement->OnHover(x, y);

        //                mInputStateContext.mCurrHoverUIElement = mInputStateContext.mCurrMouseCapturedUIElement;
        //            }
        //            else if ((bHover == false) &&
        //                     (mInputStateContext.mCurrHoverUIElement ==
        //                     mInputStateContext.mCurrMouseCapturedUIElement))
        //            {

        //                mInputStateContext.mCurrMouseCapturedUIElement->OnReleaseHover();
        //                mInputStateContext.mCurrHoverUIElement = nullptr;
        //            }

        //            mInputStateContext.mCurrMouseCapturedUIElement->OnMouseMove(inputData, x, y);
        //        }
        //        else
        //        {

        //            // mouse click
        //            if ((inputData.mInputState & EInputState::eMouseLButtonDown) ||
        //                (inputData.mInputState & EInputState::eMouseRButtonDown))
        //            {
        //                mInputStateContext.mCurrMouseCapturedUIElement->OnMouseDown(inputData, mouseWorldPosX,
        //                                                                            mouseWorldPosY);
        //            }
        //            else if ((inputData.mInputState & EInputState::eMouseLButtonUp) ||
        //                     (inputData.mInputState & EInputState::eMouseRButtonUp))
        //            {
        //                mInputStateContext.mCurrMouseCapturedUIElement->OnMouseUp(inputData, mouseWorldPosX,
        //                                                                          mouseWorldPosY);
        //            }
        //            else if ((inputData.mInputState & EInputState::eMouseWheel))
        //            {
        //                bool bConsume = false;
        //                mInputStateContext.mCurrMouseCapturedUIElement->OnMouseWheel(inputData, mouseWorldPosX,
        //                                                                             mouseWorldPosY, bConsume);
        //            }
        //        }
        //    }

        //    bConsume = true;

        //    return;
        //}
    }

    // 키보드 이벤트는 전송 불필요 캡처된 친구가 아니라면
    if (inputData.IsKeyboardEvent() && mInputStateContext.mCurrKeyboardCapturedUIElement)
    {
        if (inputData.mInputState & EInputState::eKeyDown)
        {
            mInputStateContext.mCurrKeyboardCapturedUIElement->OnKeyDown(inputData.keyEvent.mKeyCode);
        }
        else if (inputData.mInputState & EInputState::eKeyUp)
        {
            mInputStateContext.mCurrKeyboardCapturedUIElement->OnKeyUp(inputData.keyEvent.mKeyCode);
        }
        else if (inputData.mInputState & EInputState::eKeyChar)
        {
            mInputStateContext.mCurrKeyboardCapturedUIElement->OnChar(inputData.keyChar.mCharCode);
        }
    }

    //// hover event처리
    // if (inputData.mInputState & EInputState::eMouseMove)
    //{
    //     // 가공이 필요하다
    //     // hover on , hover release  등등

    //    UIElement *newHoverElement = EvaulateHoverElement(mouseWorldPosX, mouseWorldPosY, canvas);
    //    // 일단 먼저 hover 인 요소들 검색한다.

    //    // 현재 원래 hover 요소와 비교
    //    if (mInputStateContext.mCurrHoverUIElement != newHoverElement)
    //    {
    //        // TO DO
    //        if (mInputStateContext.mCurrHoverUIElement)
    //        {
    //            mInputStateContext.mCurrHoverUIElement->OnReleaseHover();
    //        }

    //        mInputStateContext.mPreHoverUIElement = mInputStateContext.mCurrHoverUIElement;
    //        // 마우스 이동으로 hover 요소가 교체됬다.
    //        // 새로운 hover에는 hover on event를 , 이전 hover 요소에는 hover release이벤트를 보낸다.
    //        // 아니면 hover 인터페이스를 호출

    //        mInputStateContext.mCurrHoverUIElement = newHoverElement;

    //        if (mInputStateContext.mCurrHoverUIElement)
    //        {

    //            mInputStateContext.mCurrHoverUIElement->OnHover(mouseWorldPosX, mouseWorldPosY);
    //        }
    //    }

    //    if (mInputStateContext.mCurrHoverUIElement)
    //        mInputStateContext.mCurrHoverUIElement->OnMouseMove(inputData, mouseWorldPosX, mouseWorldPosY);
    //}

    // bool isMouseButtonDown = (inputData.mInputState & EInputState::eMouseLButtonDown) ||
    //                          (inputData.mInputState & EInputState::eMouseRButtonDown);

    // if (isMouseButtonDown)
    //{
    //     if (mInputStateContext.mCurrKeyboardCapturedUIElement)
    //     {

    //        UIElement *scope = mInputStateContext.mCurrKeyboardCapturedUIElement->GetKeyboardCaptureScope();

    //        if (scope == nullptr)
    //            scope = mInputStateContext.mCurrKeyboardCapturedUIElement;

    //        if (!IsSameOrDescendant(mInputStateContext.mCurrHoverUIElement, scope))
    //        {
    //            ReleaseKeyboardCapture();
    //        }
    //    }

    //    if (mInputStateContext.mCurrHoverUIElement)
    //        mInputStateContext.mCurrHoverUIElement->OnMouseDown(inputData, mouseWorldPosX, mouseWorldPosY);
    //}

    //// 캡처된 친구가없다면 hover친구에게 buttonclick 정도는 보낸다 .
    // if (mInputStateContext.mCurrHoverUIElement)
    //{
    //     if ((inputData.mInputState & EInputState::eMouseLButtonUp) ||
    //         (inputData.mInputState & EInputState::eMouseRButtonUp))
    //     {
    //         mInputStateContext.mCurrHoverUIElement->OnMouseUp(inputData, mouseWorldPosX, mouseWorldPosY);
    //     }
    //     else if ((inputData.mInputState & EInputState::eMouseWheel))
    //     {

    //        // 마우스 휠은 버블링 방식으로 자식에서 부모로 이벤트로  소비가될때까지 거슬러서 전달
    //        UIElement *target = mInputStateContext.mCurrHoverUIElement;
    //        bool bConsume = false;
    //        while (target)
    //        {
    //            target->OnMouseWheel(inputData, mouseWorldPosX, mouseWorldPosY, bConsume);
    //            if (bConsume)
    //                break;

    //            target = target->GetParent();
    //        }
    //    }
    //}
}

// void UI::UIManager::OnMouseCaptureLost()
//{
//
//     //    mInputStateContext.mCurrCapturedUIElement->OnMouseCaptureLost();
// }

void UI::UIManager::SetMouseCapture(UIElement *element)
{
    if (element == nullptr)
        return;

    UIElement *preScope = GetCurrentMouseInputScope();

    if (preScope == element)
        return;

    if (preScope)
    {
        preScope->OnLostMouseFocus();
    }

    // if (mInputStateContext.mCurrMouseCapturedUIElement)
    //{
    //     // 캡처해제되엇다는것을 element에게알림
    //     mInputStateContext.mCurrMouseCapturedUIElement->OnLostMouseFocus();
    // }

    // mInputStateContext.mCurrMouseCapturedUIElement = element;

    element->OnSetMouseFocus();

    PushMouseInputScope(element);

    mMouseCaptureCallbackSystem.ExecuteCallbacks();

    // 내부적으로는 입력시스템에 바로 ,또는 외부로 요청 -> 논리적윈도우의 캡처컨트롤러로 설정 ? -> 입력시스템
}
void UI::UIManager::ReleaseMouseCapture(UI::UIElement *element)
{

    UIElement *scope = GetCurrentMouseInputScope();
    if (scope == nullptr || scope != element)
        return;

    scope->OnLostMouseFocus();
    PopMouseInputScope(scope);

    // capture 복원
    UIElement *restoredScope = GetCurrentMouseInputScope();
    if (restoredScope)
    {
        restoredScope->OnSetMouseFocus();
    }

    mMouseReleaseCaptureCallbackSystem.ExecuteCallbacks();
}

void UI::UIManager::SetKeyboardCapture(UIElement *element)
{
    if (mInputStateContext.mCurrKeyboardCapturedUIElement)
    {
        // 캡처해제되엇다는것을 element에게알림
        mInputStateContext.mCurrKeyboardCapturedUIElement->OnLostKeyboardFocus();
    }

    mInputStateContext.mCurrKeyboardCapturedUIElement = element;
    mInputStateContext.mCurrKeyboardCapturedUIElement->OnSetKeyboardFocus();

    auto inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem)
    {
        inputSystem->SetGameInputBlocked(true);
    }

    mKeyboardCaptureCallbackSystem.ExecuteCallbacks();
}

void UI::UIManager::ReleaseKeyboardCapture()
{

    if (mInputStateContext.mCurrKeyboardCapturedUIElement)
    {
        mInputStateContext.mCurrKeyboardCapturedUIElement->OnLostKeyboardFocus();

        mInputStateContext.mCurrKeyboardCapturedUIElement = nullptr;

        mKeyboardReleaseCaptureCallbackSystem.ExecuteCallbacks();

        auto inputSystem = Quad::InputSystem::GetInstance();
        if (inputSystem)
        {
            inputSystem->SetGameInputBlocked(false);
        }
    }
}

void UI::UIManager::ReleaseKeyboardCaptureProgrammatically()
{

    if (mInputStateContext.mCurrKeyboardCapturedUIElement != nullptr)
    {
        mInputStateContext.mCurrKeyboardCapturedUIElement->OnLostKeyboardFocus();
        mInputStateContext.mCurrKeyboardCapturedUIElement = nullptr;

        auto inputSystem = Quad::InputSystem::GetInstance();
        if (inputSystem)
        {
            inputSystem->SetGameInputBlocked(false);
        }
    }
}

void UI::UIManager::DispatchMouseMoveEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                           const Quad::RawInputData &inputData)
{
    if (targetElement == nullptr)
    {
        return;
    }

    if ((inputData.mInputState & EInputState::eMouseMove) == false)
    {
        return;
    }

    //   그리고 그 캡처된 친구에서만 hover 판정수해
    int x = mouseWorldPosX;
    int y = mouseWorldPosY;

    bool bHover = targetElement->IsPointInside(x, y);

    targetElement->OnMouseMove(inputData, x, y);

    if ((mInputStateContext.mCurrHoverUIElement != targetElement) && bHover)
    {

        if (mInputStateContext.mCurrHoverUIElement)
            mInputStateContext.mCurrHoverUIElement->OnReleaseHover();

        targetElement->OnHover(x, y);
        mInputStateContext.mCurrHoverUIElement = targetElement;
    }
    else if ((bHover == false) && (mInputStateContext.mCurrHoverUIElement == targetElement))
    {

        targetElement->OnReleaseHover();
        mInputStateContext.mCurrHoverUIElement = nullptr;
    }
}

void UI::UIManager::DispatchMouseButtonEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                             const Quad::RawInputData &inputData)
{
    if (targetElement == nullptr)
        return;

    if ((inputData.mInputState & EInputState::eMouseLButtonDown) ||
        (inputData.mInputState & EInputState::eMouseRButtonDown))
    {

        if (mInputStateContext.mCurrKeyboardCapturedUIElement)
        {

            UIElement *scope = mInputStateContext.mCurrKeyboardCapturedUIElement->GetKeyboardCaptureScope();

            if (scope == nullptr)
                scope = mInputStateContext.mCurrKeyboardCapturedUIElement;

            if (!IsSameOrDescendant(targetElement, scope))
            {
                ReleaseKeyboardCapture();
            }
        }

        targetElement->OnMouseDown(inputData, mouseWorldPosX, mouseWorldPosY);
    }
    else if ((inputData.mInputState & EInputState::eMouseLButtonUp) ||
             (inputData.mInputState & EInputState::eMouseRButtonUp))
    {
        targetElement->OnMouseUp(inputData, mouseWorldPosX, mouseWorldPosY);
    }
}

void UI::UIManager::DispatchMouseWheelEvent(UIElement *targetElement, int mouseWorldPosX, int mouseWorldPosY,
                                            const Quad::RawInputData &inputData)
{
    if ((inputData.mInputState & EInputState::eMouseWheel))
    {
        bool bConsume = false;
        targetElement->OnMouseWheel(inputData, mouseWorldPosX, mouseWorldPosY, bConsume);
    }
}

void UI::UIManager::OnMouseLeaveCanvas(UICanvas *canvas)
{
    UIElement *hovered = mInputStateContext.mCurrHoverUIElement;

    if (hovered == nullptr)
        return;

    if (hovered->GetDestCanvas() != canvas)
        return;

    hovered->OnReleaseHover();
    mInputStateContext.mPreHoverUIElement = hovered;
    mInputStateContext.mCurrHoverUIElement = nullptr;
}

UI::UIManager::~UIManager() {}

UI::UICanvasID UI::UIManager::CreateCanvas(const std::string &name, ECanvasSizeMode sizeMode)
{

    UICanvasID newID = mNextUICanvasID;
    if (!mFreeCanvasIDVector.empty())
    {
        newID = mFreeCanvasIDVector.back();
        mFreeCanvasIDVector.pop_back();
    }
    else
        mNextUICanvasID++;

    UICanvas *canvas = new UICanvas(newID, name, sizeMode); /// 할당자를 사용하자
    mUICanvasTable[newID] = canvas;
    return newID;
}

void UI::UIManager::DestroyCanvas(UICanvasID id)
{
    auto it = mUICanvasTable.find(id);
    if (it == mUICanvasTable.end())
        return;

    delete it->second;
    mUICanvasTable.erase(id);
    mFreeCanvasIDVector.push_back(id);
}

UI::UICanvas *UI::UIManager::GetCanvas(UICanvasID id) const
{

    auto it = mUICanvasTable.find(id);

    return it == mUICanvasTable.cend() ? nullptr : it->second;
}

void UI::UIManager::ActivateCanvas(UICanvas *canvas)
{
    // 후에 동기화 코드가 필요할수있다.

    if (canvas == nullptr)
        return;

    auto it = std::find(mActiveCanvasList.begin(), mActiveCanvasList.end(), canvas);
    if (it != mActiveCanvasList.end())
        return;

    mActiveCanvasList.push_back(canvas);
}

void UI::UIManager::RequestDeActivateCanvas(UICanvas *canvas)
{
    // 후에 동기화 코드가 필요할수있다.

    if (canvas == nullptr)
        return;

    canvas->DecreaseRefCount();
    if (canvas->GetRefCount() != 0)
        return;

    auto it = std::find(mActiveCanvasList.begin(), mActiveCanvasList.end(), canvas);
    if (it == mActiveCanvasList.end())
        return;

    mActiveCanvasList.erase(it);
}

// UI::UIElement *UI::UIManager::CreateUIElement(const std::string &name)
//{
//
//     UIElementID newID = mNextUIElementID;
//     if (!mFreeUIElementIDVector.empty())
//     {
//         newID = mFreeUIElementIDVector.back();
//         mFreeUIElementIDVector.pop_back();
//     }
//     else
//         mNextUIElementID++;
//
//     mUIElementTable[newID] = new UIElement(newID, name);
//     return mUIElementTable[newID];
// }

void UI::UIManager::DestoryUIElement(UIElementID uiElementID)
{

    auto it = mUIElementTable.find(uiElementID);

    if (it == mUIElementTable.end())
        return;

    UIElement *uiElement = it->second;

    DestoryUIElement(it->second);
}

void UI::UIManager::DestoryUIElement(const char *uiElementName)
{

    /* std::unordered_map<std::string, std::vector<UIElement *>::iterator it = mUIElementNameTable.find(uiElementName);

     if (it == mUIElementNameTable.end())
     {
         return;
     }

     DestoryUIElement(it->second);*/
}

UI::UIElement *UI::UIManager::GetUIElement(UIElementID uiElementID) const
{
    auto it = mUIElementTable.find(uiElementID);

    return it == mUIElementTable.end() ? nullptr : it->second;
}

UI::UIElement *UI::UIManager::GetUIElement(UIElementHandle uiElementHandle) const
{

    if (mUIElementSlotPool[uiElementHandle.mPoolSlotIndex].mGeneration == uiElementHandle.mGeneration)
    {
        return mUIElementSlotPool[uiElementHandle.mPoolSlotIndex].mUIElement;
    }

    return nullptr;
}

void UI::UIManager::AddUIElement(UICanvasID canvasID, UIElementID uiElementID)
{
    UICanvas *uiCanvas = GetCanvas(canvasID);
    UIElement *uiElement = GetUIElement(uiElementID);

    AddUIElement(uiCanvas, uiElement);
}

void UI::UIManager::AddUIElement(UICanvas *canvas, UIElement *uiElement)
{
    if (uiElement->mParent != nullptr)
        uiElement->mParent->RemoveChildInternal(uiElement);

    uiElement->mParent = nullptr;
    canvas->AddChildInternal(uiElement);
}

void UI::UIManager::SetParent(UIElement *parent, UIElement *child)
{
    if (parent == child->GetParent())
        return;

    if (child->mParent != nullptr)
    {
        child->mParent->RemoveChildInternal(child);
    }
    else
    {

        // TODO 수정필요 ,
        // 불안전함
        auto &elementVector = child->mDestCanvas->mTopChildUIElementList;

        auto it = std::find(elementVector.begin(), elementVector.end(), child);
        if (it != elementVector.end())
            elementVector.erase(it);
    }

    if (parent)
        parent->AddChildInternal(child);
    else
    {
        child->mParent = nullptr;
        child->mDestCanvas->mTopChildUIElementList.push_back(child);
    }
}

const std::unordered_map<UI::UICanvasID, UI::UICanvas *> &UI::UIManager::GetUICanvasAll() const
{

    return mUICanvasTable;

    // TODO: 여기에 return 문을 삽입합니다.
}

void UI::UIManager::UpdateInputState(const Quad::MouseContext &mouseContext, const std::vector<UICanvas *> &canvasList,
                                     size_t canvasNum)
{
    // 평가 - 타켓update로 분할
    EvaulateInputState(mouseContext, canvasList, canvasNum);
    SendInputEventToTargetElements(mouseContext);
}

bool UI::UIManager::IsMouseCaptureOn() const
{
    return mInputStateContext.mMouseInputScopeStack.empty() ? false : true;
}

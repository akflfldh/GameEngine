#include "UiSystem/UIManager.h"
#include "UiSystem/UICanvas.h"
#include "UiSystem/UIComponentFactory.h"
#include "UiSystem/UIElement.h"
#include <Logger/Logger.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <stack>

UI::UIManager *UI::UIManager::GetInstance()
{
    static UIManager manager;
    return &manager;
}

UI::UIManager::UIManager() : mNextUICanvasID(0), mNextUIElementID(0) {}

void UI::UIManager::EvaulateInputState(const Quad::MouseContext &mouseContext, UICanvas **canvasList, size_t canvasNum)
{

    // 이미 캡처가 설정된 ui가있다면 그 ui에게만 마우스 입력상태가 전달될것이기에
    // hover판정을 할필요가없다.
    UIElement *newHoverUIElement = nullptr;

    uint32_t topUICanvasDepthValue = 0;
    uint32_t topUIElementDepthValue = 0;
    if (mInputStateContext.mCurrCapturedUIElement == nullptr)
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
        mInputStateContext.mCurrCapturedUIElement = newCapturedUIElement;
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
    if (mInputStateContext.mCurrCapturedUIElement != nullptr)
    {
        if (mInputStateContext.mCaptureEnterFlag == true)
        {
            uiMouseInputContext.mCaptureState = EUIMouseCaptureType::eEnter;
        }
        else
        {
            uiMouseInputContext.mCaptureState = EUIMouseCaptureType::eHeld;
        }

        mInputStateContext.mCurrCapturedUIElement->UpdateMouseInputEvent(uiMouseInputContext, captureActiveFlag,
                                                                         captureReleaseFlag);

        if (captureReleaseFlag == true)
        {
            mInputStateContext.mCurrCapturedUIElement = nullptr;
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

            mInputStateContext.mCurrCapturedUIElement = mInputStateContext.mCurrHoverUIElement;
            mInputStateContext.mCaptureEnterFlag = true;
        }
    }
}

UI::UIElement *UI::UIManager::CreateUIElement(const char *uiElementClassName, const char *instanceName)
{

    std::unordered_map<std::string, UIElement *>::iterator it = mUIElementNameTable.find(instanceName);
    if (it != mUIElementNameTable.end())
    {
        // log
        return nullptr;
    }

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    size_t classSize = reflectionSystem->GetClassSize(uiElementClassName);

    if (classSize == 0)
    {
        // 등록되지않은 class인것이다
        return nullptr;
    }

    // 여기는 이제 할당자에서 메모리를 얻는것으로 교체해야한다.
    char *memBuffer = new char[classSize];

    void *instance = reflectionSystem->CreateClassInstance(uiElementClassName, memBuffer);

    if (instance == nullptr)
    {
        delete[] memBuffer;
        return nullptr;
    }
    UIElement *uiElement = (UIElement *)instance;
    uiElement->mName = instanceName;
    uiElement->mID = mNextUIElementID++;

    mUIElementNameTable[instanceName] = uiElement;
    mUIElementTable[uiElement->mID] = uiElement;

    return uiElement;
}

void UI::UIManager::DestoryUIElement(UIElement *element)
{
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    reflectionSystem->DestoryClassInstance(element->GetClassName(), element);

    mUIElementNameTable.erase(element->mName);
    mUIElementTable.erase(element->GetID());
    mFreeUIElementIDVector.push_back(element->GetID());

    delete[] (void *)element;
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

//UI::UIElement *UI::UIManager::CreateUIElement(const std::string &name)
//{
//
//    UIElementID newID = mNextUIElementID;
//    if (!mFreeUIElementIDVector.empty())
//    {
//        newID = mFreeUIElementIDVector.back();
//        mFreeUIElementIDVector.pop_back();
//    }
//    else
//        mNextUIElementID++;
//
//    mUIElementTable[newID] = new UIElement(newID, name);
//    return mUIElementTable[newID];
//}

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

    std::unordered_map<std::string, UIElement *>::iterator it = mUIElementNameTable.find(uiElementName);

    if (it == mUIElementNameTable.end())
    {
        return;
    }

    DestoryUIElement(it->second);
}

UI::UIElement *UI::UIManager::GetUIElement(UIElementID uiElementID) const
{
    auto it = mUIElementTable.find(uiElementID);

    return it == mUIElementTable.end() ? nullptr : it->second;
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
    if (child->mParent != nullptr)
    {
        child->mParent->RemoveChildInternal(child);
    }
    else
    {
        auto &elementVector = child->mDestCanvas->mChildUIElement;

        elementVector.erase(std::find(elementVector.begin(), elementVector.end(), child));
    }
    parent->AddChildInternal(child);
}

const std::unordered_map<UI::UICanvasID, UI::UICanvas *> &UI::UIManager::GetUICanvasAll() const
{

    return mUICanvasTable;

    // TODO: 여기에 return 문을 삽입합니다.
}

void UI::UIManager::UpdateInputState(const Quad::MouseContext &mouseContext, UICanvas **canvasList, size_t canvasNum)
{
    // 평가 - 타켓update로 분할
    EvaulateInputState(mouseContext, canvasList, canvasNum);
    SendInputEventToTargetElements(mouseContext);
}

bool UI::UIManager::IsMouseCaptureOn() const
{
    return mInputStateContext.mCurrCapturedUIElement != nullptr ? true : false;
}

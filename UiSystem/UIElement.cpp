#include "UiSystem/UIElement.h"
#include "UiSystem/UIManager.h"
#include <Logger/Logger.h>
#include <sstream>
UI::UIElement::UIElement() : mParent(nullptr), mTransform(this) {}

UI::UIElement::UIElement(UIElementID id, const std::string &name)
    : mName(name), mID(id), mParent(nullptr), mActiveFlag(true), mTransform(this), mDepthValue(0)
{
}

UI::UIElement::~UIElement()
{
    std::vector<IUIComponent *> comVector;

    comVector.reserve(mComponentContainer.size());
    for (auto &uiCom : mComponentContainer)
    {
        comVector.push_back(uiCom.second);
    }

    for (auto uiCom : comVector)
    {
        ReleaseUIComponent(uiCom->GetComponentName());
    }
}
void UI::UIElement::Begin() {}

void UI::UIElement::Update(float deltaTime)
{
    if (!mActiveFlag)
        return;

    // mTransform->Update() // RectTransform 업데이트 (월드 변환 계산 등)

    for (auto &child : mChildVector)
    {
        child->Update(deltaTime);
    }
}

UI::UIElementID UI::UIElement::GetID() const
{
    return mID;
}

UI::UIElement *UI::UIElement::GetParent() const
{
    return mParent;
}

void UI::UIElement::AddChildInternal(UIElement *child)
{
    if (!child)
        return;
    child->mParent = this;
    mChildVector.push_back(child);
}

void UI::UIElement::RemoveChildInternal(UIElement *child)
{
    if (child == nullptr)
        return;

    mChildVector.erase(std::find(mChildVector.begin(), mChildVector.end(), child));
    child->mParent = nullptr;
}

void UI::UIElement::SetCanvasInternal(UICanvas *canvas)
{
    mDestCanvas = canvas;
}

void UI::UIElement::ReleaseUIComponent(const char *name)
{

    std::unordered_map<const char *, IUIComponent *>::iterator it = mComponentContainer.find(name);

    if (it == mComponentContainer.end())
        return;

    IUIComponent *com = it->second;
    mComponentContainer.erase(it);

    UIComponentFactory::GetInstance()->Release(com);
}

bool UI::UIElement::IsPointInside(int x, int y)
{

    // 실제 기능,영역을 가지는것은 component들이기에

    // component를 순회하면서 실제 판정을 수행
    bool ret = false;
    for (auto element : mComponentContainer)
    {
        IUIComponent *com = element.second;

        if (com->IsPointInside(x, y))
        {
            ret = true;
            LOG_MESSAGE_INFO("ComName", com->GetClassName());
            break;
        }
    }

    return ret;
}

void UI::UIElement::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                          bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    // UIManagerMouseInputContext는 uiElement수준에서의 마우스입력과 관련된 정보

    // 여기서 자체적으로 한 UI요소 내에 컴포넌트들사이에서의 HOVER(enter,release,held),CAPTURE( .. )등을
    // 계산해서 처리한다.
    // component들에게 전달된다.
    UIManagerMouseInputContext mouseInputComContext = mouseInputContext;

    float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
    float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;

    // cpature
    if (mouseInputContext.mCaptureState != EUIMouseCaptureType::eNone)
    {
        // UpdateOnCapture(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
        mMouseInputStateContext.mCurrCapturedUIComponent->UpdateMouseInputEvent(
            mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
    else
    {
        // hover
        UpdateOnHover(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
}

void UI::UIElement::SetDepthValue(uint32_t depth)
{
    mDepthValue = depth;
}

uint32_t UI::UIElement::GetDepthValue() const
{
    return mDepthValue;
}

UI::UICanvas *UI::UIElement::GetDestCanvas() const
{
    return mDestCanvas;
}

size_t UI::UIElement::GetComponentsNum(const char *className) const
{
    size_t num = 0;
    UIComponentFactory *componentFactory = UIComponentFactory::GetInstance();

    for (auto &com : mComponentContainer)
    {
        IUIComponent *component = com.second;

        const char *comClassName = component->GetClassName();

        if (componentFactory->IsBaseClass(className, comClassName))
        {
            num++;
        }
    }

    return num;
}

size_t UI::UIElement::GetComponentsInner(IUIComponent **comArray, size_t maxCount, const char *className)
{
    size_t num = 0;
    UIComponentFactory *componentFactory = UIComponentFactory::GetInstance();

    for (auto &com : mComponentContainer)
    {
        IUIComponent *component = com.second;

        // 팩토리에게 찾는 컴포넌트와 소유한컴포넌트가 일치하는지 혹은 찾는컴포넌트가 부모컴포넌트인지
        //  팩토리는 리플렉션시스템에게 부모자식관게인지물어보고 리턴해줄것이다.
        if (componentFactory->IsBaseClass(className, component->GetClassName()))
        {
            comArray[num] = component;
            num++;
        }
        // if (std::strcmp(component->GetClassName(), className) == 0)
        //{
        // comArray[num] = component;
        // num++;
        // }

        if (num >= maxCount)
            break;
    }

    return num;
}

UI::IUIComponent *UI::UIElement::InPointInside(int x, int y)
{
    uint32_t topUIComponentDepthValue = 0;
    IUIComponent *topCom = nullptr;
    for (auto element : mComponentContainer)
    {
        IUIComponent *com = element.second;

        if (com->IsPointInside(x, y) && topUIComponentDepthValue <= com->GetDepthValue())
        {
            topCom = com;
        }
    }

    return topCom;
}

void UI::UIElement::SetActiveFlag(bool flag)
{
    mActiveFlag = flag;
}

bool UI::UIElement::GetActiveFlag() const
{

    return mActiveFlag;
}
const std::vector<UI::UIElement *> UI::UIElement::GetChildVector() const
{
    return mChildVector;
}

void UI::UIElement::SetParent(UIElement *parent)
{
    auto manager = UIManager::GetInstance();
    manager->SetParent(parent, this);
}

// 캡처상태가아닌 hover상태에서의 업데이트
void UI::UIElement::UpdateOnHover(const UI::UIManagerMouseInputContext &mouseInputContext,
                                  bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    UIManagerMouseInputContext mouseInputComContext = mouseInputContext;

    float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
    float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;

    // hover
    mMouseInputStateContext.mPreHoverUIComponent = mMouseInputStateContext.mCurrHoverUIComponent;

    if (mouseInputContext.mHoverState == EUIMouseHoverType::eEnter)
    {
        // 어떤 컴포넌트가 hover가 되었는지  판정하고 내부적으로 저장한다.

        IUIComponent *com = InPointInside(worldPosX, worldPosY);
        mMouseInputStateContext.mCurrHoverUIComponent = com;
        com->UpdateMouseInputEvent(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
    else if (mouseInputContext.mHoverState == EUIMouseHoverType::eHeld)
    {
        // ui요소 단위로는 held지만. 실제로 내부 컴포넌트들사이에는 바뀔수잇다 (mouse 가 ui요소내에서 움직여서)
        // 컴포넌트들에대해서 판정필요
        // 각각의 타켓 컴포넌트들에대해서 전달
        IUIComponent *com = InPointInside(worldPosX, worldPosY);
        mMouseInputStateContext.mCurrHoverUIComponent = com;

        if (mMouseInputStateContext.mCurrHoverUIComponent == mMouseInputStateContext.mPreHoverUIComponent)
        {
            com->UpdateMouseInputEvent(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
        }
        else
        {
            if (mMouseInputStateContext.mPreHoverUIComponent != nullptr)
            {
                mouseInputComContext.mHoverState = EUIMouseHoverType::eRelease;
                mMouseInputStateContext.mPreHoverUIComponent->UpdateMouseInputEvent(
                    mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
            }

            if (mMouseInputStateContext.mCurrHoverUIComponent != nullptr)
            {

                mouseInputComContext.mHoverState = EUIMouseHoverType::eEnter;
                mMouseInputStateContext.mCurrHoverUIComponent->UpdateMouseInputEvent(
                    mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
            }
        }
    }
    else if (mouseInputContext.mHoverState == EUIMouseHoverType::eRelease)
    {
        // 이것은 정말로 ui요소 영역을 빠져나간것
        // 현재 hover ui컴포넌트에게 release 이벤트를 전달해야한다.
        mMouseInputStateContext.mCurrHoverUIComponent->UpdateMouseInputEvent(
            mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
        mMouseInputStateContext.mCurrHoverUIComponent = nullptr;
    }

    // 캡처는 일반적으로 currHover compoennt가 요청할것이다.

    if (mMouseInputStateContext.mCurrHoverUIComponent != nullptr && oCaptureActiveRequestFlag == true)
    {

        mMouseInputStateContext.mCurrCapturedUIComponent =
            mMouseInputStateContext.mCurrHoverUIComponent; // 설정해둔다(다만 실제 작동하는것은
        // 상위시스템에서 캡처플래그가 켜진 이벤트를 전달할때만동작하니 안전하다)
    }
}

// 캡처상태일떄의 업데이트
void UI::UIElement::UpdateOnCapture(const UI::UIManagerMouseInputContext &mouseInputContext,
                                    bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{

    UIManagerMouseInputContext mouseInputComContext = mouseInputContext;

    float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
    float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;

    if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eEnter)
    {
        // ui요소내에서 한 컴포넌트가 캡처요청을 한거지  수락됬다.
        // 그 컴포넌트를 그 전에 알고있어야한다.

        mMouseInputStateContext.mCurrCapturedUIComponent->UpdateMouseInputEvent(
            mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
    else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eHeld)
    {
        // 계속 캡처가 유지되고있다.

        // enter, held일떄는 그 컴포넌트에게만 전달된다.
        mMouseInputStateContext.mCurrCapturedUIComponent->UpdateMouseInputEvent(
            mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    }
    else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eRelease)
    {
        // 캡처가 해제되었다.
    }
}

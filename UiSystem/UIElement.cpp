#include "UiSystem/UIElement.h"
#include "UiSystem/UIManager.h"
#include <Logger/Logger.h>
#include <algorithm>
#include <sstream>

UI::UIElement::UIElement()
    : mParent(nullptr), mTransform(this), mDepthValue(4), mActiveFlag(true), mMouseCapturedComponent(nullptr),
      mKeyboardCapturedComponent(nullptr), mDestCanvas(nullptr), mUseScissorRECT(false), mDeadState(false),
      mIsBegun(false), mOnlyVisible(false)
{
}

UI::UIElement::UIElement(UIElementID id, const std::string &name)
    : mName(name), mID(id), mParent(nullptr), mActiveFlag(true), mTransform(this), mDepthValue(4),
      mMouseCapturedComponent(nullptr), mKeyboardCapturedComponent(nullptr), mDestCanvas(nullptr),
      mUseScissorRECT(false), mDeadState(false)
{
}

UI::UIElement::~UIElement()
{
    /*  std::vector<IUIComponent *> comVector;

      comVector.reserve(mComponentContainer.size());
      for (auto &uiCom : mComponentContainer)
      {
          comVector.push_back(uiCom);
      }*/

    for (auto uiCom : mComponentContainer)
    {
        ReleaseUIComponent(uiCom->GetComponentName());
    }
}
void UI::UIElement::Serialize(Arch &arch) {}

void UI::UIElement::Begin()
{
    mIsBegun = true;
    for (auto com : mComponentContainer)
    {
        com->Begin();
    }

    OnBegin();

    OnTransformChanged(ETransformChangeType::eAll);
}

void UI::UIElement::Update(float deltaTime)
{
    if (!mActiveFlag)
        return;

    UpdatePosPivot();

    for (auto com : mComponentContainer)
    {

        com->Update(deltaTime);
    }

    BroadCastChangedSize();

    // mTransform->Update() // RectTransform 업데이트 (월드 변환 계산 등)

    for (auto &child : mChildVector)
    {
        child->Update(deltaTime);
    }
}
const std::string &UI::UIElement::GetName() const
{

    return mName;
}

UI::UIElementID UI::UIElement::GetID() const
{
    return mID;
}

void UI::UIElement::Destroy()
{
    if (mDestCanvas)
    {
        mDestCanvas->DestroyUIElement(this);
    }
}

UI::UIElement *UI::UIElement::GetParent() const
{
    return mParent;
}

void UI::UIElement::OnWindowResize(float w, float h)
{

    if (mParent != nullptr)
        return;

    ApplyPosPivotInParent({w, h});
}

SRECT UI::UIElement::GetScissorRectRegion() const
{
    SRECT rect;
    glm::vec2 points[4];
    mTransform.GetQuadWorldPoints(points);

    SRECT parentRect = {points[0].r, points[1].r, points[0].g, points[2].g};
    if (mParent)
    {
        parentRect = mParent->GetScissorRectRegion();
    }

    rect.mLeft = std::max(points[0].r, parentRect.mLeft);
    rect.mRight = std::min(points[1].r, parentRect.mRight);
    rect.mTop = std::max(points[0].g, parentRect.mTop);
    rect.mBottom = std::min(points[2].g, parentRect.mBottom);

    if (rect.mLeft >= rect.mRight || rect.mTop >= rect.mBottom)
        return {0, 0, 0, 0};

    return rect;
}

void UI::UIElement::AddChildInternal(UIElement *child)
{
    if (!child)
        return;
    child->mParent = this;
    mChildVector.push_back(child);
    OnChangedChildDepthValue();

    mOnAddedChildElementCallbackSystem.ExecuteCallbacks(child);
}

void UI::UIElement::RemoveChildInternal(UIElement *child)
{
    if (child == nullptr)
        return;

    mChildVector.erase(std::find(mChildVector.begin(), mChildVector.end(), child));
    child->mParent = nullptr;

    mOnRemovedChildElementCallbackSystem.ExecuteCallbacks(child);
}

void UI::UIElement::SetCanvasInternal(UICanvas *canvas)
{
    mDestCanvas = canvas;
    for (UI::IUIComponent *component : mComponentContainer)
    {
        component->OnOwnerAddedToCavas();
    }
}

void UI::UIElement::ReleaseUIComponent(const char *name)
{

    // std::unordered_map<const char *, IUIComponent *>::iterator it = mComponentContainer.find(name);

    // if (it == mComponentContainer.end())
    //     return;

    IUIComponent *com = GetComponent(name);
    //  mComponentContainer.erase(it);
    if (com)
        ReleaseUIComponent(com);
}

void UI::UIElement::ReleaseUIComponent(IUIComponent *com)
{

    UIComponentFactory::GetInstance()->Release(this, com);
}

bool UI::UIElement::IsPointInside(int x, int y, bool bInputTest)
{
    bool ret = false;
    if (bInputTest)
    {
        for (auto element : mComponentContainer)
        {
            IUIComponent *com = element;

            if (com->GetInputEnable() == false)
                continue;

            if (com->IsPointInside(x, y))
            {
                ret = true;
                break;
            }
        }
    }
    else
    {
        return IsPointInsideDefault(x, y);

        //// 실제 기능,영역을 가지는것은 component들이기에
        //// component를 순회하면서 실제 판정을 수행
        // for (auto element : mComponentContainer)
        //{
        //     IUIComponent *com = element;

        //    if (com->IsPointInside(x, y))
        //    {
        //        ret = true;
        //        break;
        //    }
        //}
    }

    return ret;
}

void UI::UIElement::UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                          bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
{
    //// UIManagerMouseInputContext는 uiElement수준에서의 마우스입력과 관련된 정보

    //// 여기서 자체적으로 한 UI요소 내에 컴포넌트들사이에서의 HOVER(enter,release,held),CAPTURE( .. )등을
    //// 계산해서 처리한다.
    //// component들에게 전달된다.
    // UIManagerMouseInputContext mouseInputComContext = mouseInputContext;

    // float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
    // float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;

    //// cpature
    // if (mouseInputContext.mCaptureState != EUIMouseCaptureType::eNone)
    //{
    //     // UpdateOnCapture(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    //     mMouseInputStateContext.mCurrMouseCapturedUIComponent->UpdateMouseInputEvent(
    //         mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    // }
    // else
    //{
    //     // hover
    //     UpdateOnHover(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
    // }
}

void UI::UIElement::SetDepthValue(uint32_t depth)
{
    mDepthValue = depth;

    if (mParent)
    {
        mParent->OnChangedChildDepthValue();
    }
    else
    {
        // canvas -> TopElementChangedDepthValue();
    }
}

void UI::UIElement::RegisterComponent(const char *instacneName, IUIComponent *component)
{
    mComponentContainer.push_back(component);
    SortComponents();
}

void UI::UIElement::UnRegisterComponent(IUIComponent *component)
{

    auto it = std::find(mComponentContainer.begin(), mComponentContainer.end(), component);

    if (it != mComponentContainer.end())
        mComponentContainer.erase(it);

    SortComponents();
}

void UI::UIElement::SortComponents()
{

    std::sort(mComponentContainer.begin(), mComponentContainer.end(),
              [](IUIComponent *a, IUIComponent *b) { return a->GetDepthValue() > b->GetDepthValue(); });
}

void UI::UIElement::SetUIElementHandle(const UIElementHandle &handle)
{

    mUIElementHandle = handle;
}

void UI::UIElement::OnChangedChildDepthValue()
{

    std::sort(mChildVector.begin(), mChildVector.end(),
              [](const UI::UIElement *a, const UI::UIElement *b) { return a->GetDepthValue() > b->GetDepthValue(); });
}

void UI::UIElement::SetDeadState()
{
    mDeadState = true;
}

void UI::UIElement::SetKeyboardCaptureScope(UIElement *scope)
{

    mKeyboardCaptureScope = scope;
}

UI::UIElement *UI::UIElement::GetKeyboardCaptureScope() const
{

    return mKeyboardCaptureScope;
}

uint32_t UI::UIElement::GetDepthValue() const
{
    return mDepthValue;
}

UI::UICanvas *UI::UIElement::GetDestCanvas() const
{
    return mDestCanvas;
}
UI::IUIComponent *UI::UIElement::GetComponent(const char *instacneName) const
{

    auto it = std::find_if(mComponentContainer.begin(), mComponentContainer.end(),
                           [instacneName](IUIComponent *com)
                           {
                               if (com)
                               {
                                   return strcmp(com->GetComponentName(), instacneName) == 0 ? true : false;
                               }
                               return false;
                           });

    if (it == mComponentContainer.cend())
    {
        return nullptr;
    }

    return *it;
}
UI::UIElementHandle UI::UIElement::GetHandle() const
{
    return mUIElementHandle;
}

void UI::UIElement::OnHover(int x, int y)
{

    UpdateComponentMouseState({}, x, y);

    // if (mCapturedComponent)
    //{
    //     mCapturedComponent->OnHover(x, y);
    //     return;
    // }

    //// TO DO

    //// 우선순위대로 이벤트를 전달

    //// 브로드캐스팅, 일단 모두전달 , 소비는 선택

    // for (auto pComponent : mComponentContainer)
    //{
    //     pComponent->OnHover(x, y);
    // }
}

void UI::UIElement::OnReleaseHover()
{

    // TO DO
    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->OnReleaseHover();
        return;
    }

    // 우선순위대로 이벤트를 전달

    // 브로드캐스팅, 일단 모두전달 , 소비는 선택
    for (auto pComponent : mComponentContainer)
    {
        pComponent->OnReleaseHover();
    }
}

void UI::UIElement::HandleInput(const Quad::RawInputData &inputData)
{
    bool bConsume = false;
    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->HandleInput(inputData, bConsume);
        return;
    }

    // 컴포넌들, 우선순위로, event 전달
    // 소비하면 즉시 종료

    // 깊이 순으로 정렬된

    for (auto pComponent : mComponentContainer)
    {

        pComponent->HandleInput(inputData, bConsume);

        // 소비했다면 더이상 다른 컴포넌트에게 전달할 필요가없다. (더 높은 우선순위를 가지는 컴포넌트가 소비했기에 )
        if (bConsume)
            break;
    }
}
void UI::UIElement::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    UpdateComponentMouseState(inputData, worldPosX, worldPosY);

    // if (mCapturedComponent)
    //{
    //     mCapturedComponent->OnMouseMove(inputData, worldPosX, worldPosY);
    //     return;
    // }

    //// 컴포넌트로 브로드캐스팅

    // for (auto pComponent : mComponentContainer)
    //{
    //     pComponent->OnMouseMove(inputData, worldPosX, worldPosY);
    // }
}
void UI::UIElement::OnMouseClick(const Quad::RawInputData &inputData)
{
    bool bConsume = false;
    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->OnMouseClick(inputData, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        pComponent->OnMouseClick(inputData, bConsume);

        if (bConsume)
            break;
    }
    // 컴포넌트로 브로드캐스팅
}
void UI::UIElement::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    bool bConsume = false;
    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->OnMouseDown(inputData, worldPosX, worldPosY, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        pComponent->OnMouseDown(inputData, worldPosX, worldPosY, bConsume);

        if (bConsume)
            break;
    }
    // 컴포넌트로 브로드캐스팅
}
void UI::UIElement::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{
    for (auto pComponent : mComponentContainer)
    {
        bool bSteal = false;
        if (pComponent->GetPreviewInputListener())
        {
            pComponent->OnPreviewMouseUp(inputData, worldPosX, worldPosY, bSteal);
        }

        if (bSteal)
            return;
    }

    bool bConsume = false;
    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->OnMouseUp(inputData, worldPosX, worldPosY, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        pComponent->OnMouseUp(inputData, worldPosX, worldPosY, bConsume);

        if (bConsume)
            break;
    }
}

void UI::UIElement::OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume)
{

    if (mMouseCapturedComponent)
    {
        mMouseCapturedComponent->OnMouseWheel(inputData, worldPosX, worldPosY, bConsume);
        if (bConsume)
            return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent == nullptr || !pComponent->GetInputEnable())
            continue;

        pComponent->OnMouseWheel(inputData, worldPosX, worldPosY, bConsume);

        if (bConsume)
            return;
    }
}

void UI::UIElement::OnKeyDown(const Quad::EKeyCode &key)
{

    bool bConsume = false;
    if (mKeyboardCapturedComponent)
    {
        mKeyboardCapturedComponent->OnKeyDown(key, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        pComponent->OnKeyDown(key, bConsume);

        if (bConsume)
            break;
    }
}

void UI::UIElement::OnKeyUp(const Quad::EKeyCode &key)
{

    bool bConsume = false;
    if (mKeyboardCapturedComponent)
    {
        mKeyboardCapturedComponent->OnKeyUp(key, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        pComponent->OnKeyUp(key, bConsume);

        if (bConsume)
            break;
    }
}

void UI::UIElement::OnChar(const uint32_t keychar)
{

    bool bConsume = false;
    if (mKeyboardCapturedComponent)
    {
        mKeyboardCapturedComponent->OnChar(keychar, bConsume);
        return;
    }

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        pComponent->OnChar(keychar, bConsume);

        if (bConsume)
            break;
    }
}

void UI::UIElement::UpdateComponentMouseState(const Quad::RawInputData &inputData, int x, int y)
{

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        if (pComponent->GetPreviewInputListener())
        {
            bool bSteal = false;
            if (inputData.mInputState & EInputState::eMouseMove)
            {

                pComponent->OnPreviewMouseMove(inputData, x, y, bSteal);
            }

            if (bSteal)
            {
                return;
            }
        }
    }

    if (mMouseCapturedComponent)
    {
        int hitNum = mMouseCapturedComponent->IsPointInside(x, y);
        if (hitNum != mMouseCapturedComponent->GetHoverPart())
        {
            mMouseCapturedComponent->OnChangeHoverPart(mMouseCapturedComponent->GetHoverPart(), hitNum);
        }

        if (inputData.mInputState & EInputState::eMouseMove)
            mMouseCapturedComponent->OnMouseMove(inputData, x, y);

        if (hitNum == 0)
        {
            mMouseCapturedComponent->OnReleaseHover();
        }

        return;
    }

    UI::IUIComponent *hitCom = nullptr;

    for (auto pComponent : mComponentContainer)
    {
        if (pComponent->GetInputEnable() == false)
            continue;

        int hitNum = pComponent->IsPointInside(x, y);

        if (hitNum > 0)
        {

            // 첫 hover일떄
            if (!pComponent->IsHovered())
            {
                pComponent->OnHover(x, y);
                pComponent->OnChangeHoverPart(0, hitNum);
            }
            else if (pComponent->GetHoverPart() != hitNum)
            {

                pComponent->OnChangeHoverPart(pComponent->GetHoverPart(), hitNum);
            }

            if (inputData.mInputState & EInputState::eMouseMove)
                pComponent->OnMouseMove(inputData, x, y);
        }
        else
        {
            if (pComponent->IsHovered())
            {
                pComponent->OnChangeHoverPart(pComponent->GetHoverPart(), 0);
                pComponent->OnReleaseHover();
            }
        }
    }
}
void UI::UIElement::RequestMouseCaptureInput(IUIComponent *component)
{
    mMouseCapturedComponent = component;
    UI::UIManager::GetInstance()->SetMouseCapture(this);
}
void UI::UIElement::ReleaseMouseCaptureInput()
{
    mMouseCapturedComponent = nullptr;
    UI::UIManager::GetInstance()->ReleaseMouseCapture(this);
}
void UI::UIElement::RequestKeyboardCaptureInput(IUIComponent *component)
{

    mKeyboardCapturedComponent = component;
    UI::UIManager::GetInstance()->SetKeyboardCapture(this);
}
void UI::UIElement::ReleaseKeyboardCaptureInput()
{

    mKeyboardCapturedComponent = nullptr;
    UI::UIManager::GetInstance()->ReleaseKeyboardCapture();
}

void UI::UIElement::OnSetMouseFocus()
{
    for (auto com : mComponentContainer)
    {

        bool isRequester = (com == mMouseCapturedComponent);
        com->OnSetMouseFocus(isRequester);
    }

    mOnSetMouseFocusCallbackSystem.ExecuteCallbacks();
}
void UI::UIElement::OnLostMouseFocus()
{

    for (auto com : mComponentContainer)
    {
        com->OnLostMouseFocus();
    }

    mOnLostMouseFocusCallbackSystem.ExecuteCallbacks();
}

void UI::UIElement::OnSetKeyboardFocus()
{

    for (auto com : mComponentContainer)
    {
        bool isRequester = (com == mKeyboardCapturedComponent);
        com->OnSetKeyboardFocus(isRequester);
    }

    mOnSetKeyboardFocusCallbackSystem.ExecuteCallbacks();
}

void UI::UIElement::OnLostKeyboardFocus()
{
    for (auto com : mComponentContainer)
    {
        com->OnLostKeyboardFocus();
    }

    mOnLostKeyboardFocusCallbackSystem.ExecuteCallbacks();
}

bool UI::UIElement::GetUseScissorRect() const
{
    return mUseScissorRECT;
}
void UI::UIElement::SetUseScissorRect(bool flag)
{
    mUseScissorRECT = flag;

    for (auto child : mChildVector)
    {
        child->SetUseScissorRect(flag);
    }
}
bool UI::UIElement::GetDeadState() const
{
    return mDeadState;
}

#pragma region Transform

void UI::UIElement::SetPositionLocal(const glm::vec2 &pos)
{

    mTransform.SetPositionLocal(pos);

    NotifyTransformChanged(ETransformChangeType::ePosition);
}
void UI::UIElement::SetPositionLocal(float x, float y)
{

    mTransform.SetPositionLocal(x, y);
    NotifyTransformChanged(ETransformChangeType::ePosition);
}

void UI::UIElement::SetPositionWorld(const glm::vec2 &pos)
{

    mTransform.SetPositionWorld(pos);
    NotifyTransformChanged(ETransformChangeType::ePosition);
}

void UI::UIElement::TranslateLocal(const glm::vec2 &shift)
{

    mTransform.TranslateLocal(shift);
    NotifyTransformChanged(ETransformChangeType::ePosition);
}

void UI::UIElement::SetSize(const glm::vec2 &size)
{

    mTransform.SetSize(size);

    NotifyTransformChanged(ETransformChangeType::eSize);
}
void UI::UIElement::SetSize(float w, float h)
{

    mTransform.SetSize(w, h);
    NotifyTransformChanged(ETransformChangeType::eSize);
}
void UI::UIElement::SetHeight(float h)
{

    mTransform.SetHeight(h);
    NotifyTransformChanged(ETransformChangeType::eSize);
}
void UI::UIElement::SetWidth(float w)
{
    mTransform.SetWidth(w);
    NotifyTransformChanged(ETransformChangeType::eSize);
}

int UI::UIElement::GetWidth() const
{

    return mTransform.GetSize().x;
}
int UI::UIElement::GetHeight() const
{

    return mTransform.GetSize().y;
}

CoreMath::Vector2 UI::UIElement::GetSize() const
{
    auto size = mTransform.GetSize();
    return {size.x, size.y};
}

void UI::UIElement::NotifyTransformChanged(ETransformChangeType type)
{
    this->OnTransformChanged(type);

    for (auto com : mComponentContainer)
    {
        com->OnTransformChanged(type);
    }
    if (type == ETransformChangeType::eSize || type == ETransformChangeType::eAll)
    {

        if (GetPosPivotActive())
        {
            mPosPviotContext.mUpdateDirty = true;
        }

        for (auto child : mChildVector)
        {
            if (child && !child->GetDeadState())
                child->OnParentSizeChanged();
        }
        mChangedSizeDirty = true;
    }
}

void UI::UIElement::BroadCastChangedSize()
{

    if (mChangedSizeDirty)
    {
        mOnChangedSizeCallbackSystem.ExecuteCallbacks(this);
        mChangedSizeDirty = false;
    }
}

void UI::UIElement::SetPosPivotActive(bool flag)
{

    mPosPviotContext.mPosPivotActive = flag;
}

bool UI::UIElement::GetPosPivotActive() const
{
    return mPosPviotContext.mPosPivotActive;
}

void UI::UIElement::SetHorizontalPivotSide(EUIPosPivotHorizontal pivotSide)
{

    mPosPviotContext.mPivotHorizontal = pivotSide;

    if (mPosPviotContext.mPivotHorizontal != EUIPosPivotHorizontal::eNone)
    {
        mPosPviotContext.mPosPivotActive = true;
        mPosPviotContext.mUpdateDirty = true;
    }
}

void UI::UIElement::SetVerticalPivotSide(EUIPosPivotVertical pivotSide)
{

    mPosPviotContext.mPivotVertical = pivotSide;
    if (mPosPviotContext.mPivotVertical != EUIPosPivotVertical::eNone)
    {
        mPosPviotContext.mPosPivotActive = true;
        mPosPviotContext.mUpdateDirty = true;
    }
}

void UI::UIElement::SetHorizontalPivotOffset(float offset)
{

    mPosPviotContext.mHorizontalOffset = offset;
    mPosPviotContext.mUpdateDirty = true;
}

void UI::UIElement::SetVerticalPivotOffset(float offset)
{

    mPosPviotContext.mVerticalOffset = offset;
    mPosPviotContext.mUpdateDirty = true;
}

void UI::UIElement::OnParentSizeChanged()
{
    mPosPviotContext.mUpdateDirty = true;
    UpdatePosPivot();
}

void UI::UIElement::UpdatePosPivot()
{
    if (GetPosPivotActive() == false || (mPosPviotContext.mUpdateDirty == false))
        return;

    mPosPviotContext.mUpdateDirty = false;

    CoreMath::Vector2 size;

    if (mParent == nullptr)
    {
        if (mDestCanvas)
        {
            size = mDestCanvas->GetWindowSize();
        }
    }
    else
    {
        size = mParent->GetSize();
    }

    ApplyPosPivotInParent(size);
}

void UI::UIElement::ApplyPosPivotInParent(const CoreMath::Vector2 &parentSize)
{

    float width = mTransform.GetSize().x;
    float height = mTransform.GetSize().y;

    float x = mTransform.GetLocalPosition().x;
    float y = mTransform.GetLocalPosition().y;

    switch (mPosPviotContext.mPivotHorizontal)
    {
    case EUIPosPivotHorizontal::eLeft:
    {
        x = mPosPviotContext.mHorizontalOffset;
    }
    break;
    case EUIPosPivotHorizontal::eRight:
    {
        x = parentSize.X - width - mPosPviotContext.mHorizontalOffset;
    }
    break;
    }

    switch (mPosPviotContext.mPivotVertical)
    {
    case EUIPosPivotVertical::eTop:
    {
        y = mPosPviotContext.mVerticalOffset;
    }
    break;
    case EUIPosPivotVertical::eBottom:
    {
        y = parentSize.Y - mPosPviotContext.mVerticalOffset - height;
    }
    break;
    }

    SetPositionLocal(x, y);
}

#pragma endregion

size_t UI::UIElement::GetComponentsNum(const char *className) const
{
    size_t num = 0;
    UIComponentFactory *componentFactory = UIComponentFactory::GetInstance();

    for (auto &com : mComponentContainer)
    {
        IUIComponent *component = com;

        const char *comClassName = component->GetRunTimeClassName();

        if (componentFactory->IsAncestorClass(className, comClassName))
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
        IUIComponent *component = com;

        // 팩토리에게 찾는 컴포넌트와 소유한컴포넌트가 일치하는지 혹은 찾는컴포넌트가 부모컴포넌트인지
        //  팩토리는 리플렉션시스템에게 부모자식관게인지물어보고 리턴해줄것이다.
        if (componentFactory->IsAncestorClass(className, component->GetRunTimeClassName()))
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
    // TODO
    // UIElement가 그냥 사각형영역을 차지하도록하자 ?
    if (mMouseCapturedComponent)
    {
        if (mMouseCapturedComponent->IsPointInside(x, y))
        {
            return mMouseCapturedComponent;
        }
        return nullptr;
    }

    uint32_t topUIComponentDepthValue = 0;
    IUIComponent *topCom = nullptr;
    for (auto element : mComponentContainer)
    {
        IUIComponent *com = element;

        if (com->IsPointInside(x, y) && topUIComponentDepthValue <= com->GetDepthValue())
        {
            topCom = com;
        }
    }

    return topCom;
}

bool UI::UIElement::IsPointInsideDefault(float x, float y) const
{

    glm::vec2 vertices[4];
    mTransform.GetQuadWorldPoints(vertices);

    float leftX = vertices[0].x;
    float rightX = vertices[1].x;
    float topY = vertices[0].y;
    float bottomY = vertices[2].y;

    if (leftX > x)
        return false;
    if (rightX < x)
        return false;
    if (topY > y)
        return false;
    if (bottomY < y)
        return false;

    return true;
}

void UI::UIElement::SetActiveFlag(bool flag)
{

    if (mActiveFlag != flag)
    {
        mActiveFlag = flag;
        mOnActiveElementCallbackSystem.ExecuteCallbacks(mActiveFlag);
        mDestCanvas->MarkDirty();
    }
}

bool UI::UIElement::GetActiveFlag() const
{

    return mActiveFlag;
}
void UI::UIElement::SetOnlyVisible(bool flag)
{

    mOnlyVisible = flag;
}
bool UI::UIElement::GetOnlyVisible() const
{
    return mOnlyVisible;
}
void UI::UIElement::SetRenderLayer(UI::EUIRenderLayer layer)
{

    mRenderLayer = layer;
}

UI::EUIRenderLayer UI::UIElement::GetRenderLayer() const
{
    return mRenderLayer;
}

const std::vector<UI::UIElement *> &UI::UIElement::GetChildVector() const
{
    return mChildVector;
}

void UI::UIElement::SetParent(UIElement *parent)
{
    auto manager = UIManager::GetInstance();
    manager->SetParent(parent, this);
}
//
//// 캡처상태가아닌 hover상태에서의 업데이트
// void UI::UIElement::UpdateOnHover(const UI::UIManagerMouseInputContext &mouseInputContext,
//                                   bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
//{
//     UIManagerMouseInputContext mouseInputComContext = mouseInputContext;
//
//     float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
//     float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;
//
//     // hover
//     mMouseInputStateContext.mPreHoverUIComponent = mMouseInputStateContext.mCurrHoverUIComponent;
//
//     if (mouseInputContext.mHoverState == EUIMouseHoverType::eEnter)
//     {
//         // 어떤 컴포넌트가 hover가 되었는지  판정하고 내부적으로 저장한다.
//
//         IUIComponent *com = InPointInside(worldPosX, worldPosY);
//         mMouseInputStateContext.mCurrHoverUIComponent = com;
//         com->UpdateMouseInputEvent(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//     }
//     else if (mouseInputContext.mHoverState == EUIMouseHoverType::eHeld)
//     {
//         // ui요소 단위로는 held지만. 실제로 내부 컴포넌트들사이에는 바뀔수잇다 (mouse 가 ui요소내에서 움직여서)
//         // 컴포넌트들에대해서 판정필요
//         // 각각의 타켓 컴포넌트들에대해서 전달
//         IUIComponent *com = InPointInside(worldPosX, worldPosY);
//         mMouseInputStateContext.mCurrHoverUIComponent = com;
//
//         if (mMouseInputStateContext.mCurrHoverUIComponent == mMouseInputStateContext.mPreHoverUIComponent)
//         {
//             com->UpdateMouseInputEvent(mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//         }
//         else
//         {
//             if (mMouseInputStateContext.mPreHoverUIComponent != nullptr)
//             {
//                 mouseInputComContext.mHoverState = EUIMouseHoverType::eRelease;
//                 mMouseInputStateContext.mPreHoverUIComponent->UpdateMouseInputEvent(
//                     mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//             }
//
//             if (mMouseInputStateContext.mCurrHoverUIComponent != nullptr)
//             {
//
//                 mouseInputComContext.mHoverState = EUIMouseHoverType::eEnter;
//                 mMouseInputStateContext.mCurrHoverUIComponent->UpdateMouseInputEvent(
//                     mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//             }
//         }
//     }
//     else if (mouseInputContext.mHoverState == EUIMouseHoverType::eRelease)
//     {
//         // 이것은 정말로 ui요소 영역을 빠져나간것
//         // 현재 hover ui컴포넌트에게 release 이벤트를 전달해야한다.
//         mMouseInputStateContext.mCurrHoverUIComponent->UpdateMouseInputEvent(
//             mouseInputComContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//         mMouseInputStateContext.mCurrHoverUIComponent = nullptr;
//     }
//
//     // 캡처는 일반적으로 currHover compoennt가 요청할것이다.
//
//     if (mMouseInputStateContext.mCurrHoverUIComponent != nullptr && oCaptureActiveRequestFlag == true)
//     {
//
//         mMouseInputStateContext.mCurrMouseCapturedUIComponent =
//             mMouseInputStateContext.mCurrHoverUIComponent; // 설정해둔다(다만 실제 작동하는것은
//         // 상위시스템에서 캡처플래그가 켜진 이벤트를 전달할때만동작하니 안전하다)
//     }
// }
//
//// 캡처상태일떄의 업데이트
// void UI::UIElement::UpdateOnCapture(const UI::UIManagerMouseInputContext &mouseInputContext,
//                                     bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag)
//{
//
//     UIManagerMouseInputContext mouseInputComContext = mouseInputContext;
//
//     float worldPosX = mouseInputContext.mMouseContext.mWorldPosX;
//     float worldPosY = mouseInputContext.mMouseContext.mWorldPosY;
//
//     if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eEnter)
//     {
//         // ui요소내에서 한 컴포넌트가 캡처요청을 한거지  수락됬다.
//         // 그 컴포넌트를 그 전에 알고있어야한다.
//
//         mMouseInputStateContext.mCurrMouseCapturedUIComponent->UpdateMouseInputEvent(
//             mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//     }
//     else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eHeld)
//     {
//         // 계속 캡처가 유지되고있다.
//
//         // enter, held일떄는 그 컴포넌트에게만 전달된다.
//         mMouseInputStateContext.mCurrMouseCapturedUIComponent->UpdateMouseInputEvent(
//             mouseInputContext, oCaptureActiveRequestFlag, oCaptureReleaseRequestFlag);
//     }
//     else if (mouseInputContext.mCaptureState == EUIMouseCaptureType::eRelease)
//     {
//         // 캡처가 해제되었다.
//     }
// }

UI::UIElement *UI::CreateUIElement(UI::UIElement *parent, const char *staticClassName, const char *instanceName)
{

    auto canvas = parent->GetDestCanvas();
    auto childElement = canvas->CreateUIElement(staticClassName, instanceName);

    if (childElement && parent)
    {
        childElement->SetParent(parent);
    }

    return childElement;
}

void UI::UIElement::SetPositionWorld(float x, float y)
{

    mTransform.SetPositionWorld({x, y});
    NotifyTransformChanged(ETransformChangeType::ePosition);
}

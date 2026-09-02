#include "UITextInputComponent.h"
#include <InputSystem/InputSystem.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIMultiRectComponent.h>
#include <UiSystem/UITextComponent.h>
UI::UITextInputComponent::UITextInputComponent()
    : mIsFocused(false), mUITextCom(nullptr), mHover(false), mCursorIndex(0), mCursorBlinkTimeInterval(0.5f),
      mCursorBlinkTime(0.0f), mCursorSelectionStartIndex(-1), mIsDragging(false), mMultiRectCom(nullptr),
      mIsSelected(false)
{
}

UI::UITextInputComponent::~UITextInputComponent() {}

void UI::UITextInputComponent::Update(float deltaTime)
{

    if (mIsFocused)
    {
        mCursorBlinkTime += deltaTime;

        if (mCursorBlinkTime > mCursorBlinkTimeInterval)
        {

            UIEditBox *editBox = static_cast<UI::UIEditBox *>(GetOwnerUIElement());

            UIElement *cursorImageElement = editBox->GetCursorImageElement();
            if (cursorImageElement)
            {
                bool bActive = cursorImageElement->GetActiveFlag();
                cursorImageElement->SetActiveFlag(!bActive);
            }

            mCursorBlinkTime = 0.0f;
        }
    }
}

void UI::UITextInputComponent::RequestKeyboardFocus()
{
    /*  if (GetOwnerUIElement()->GetActiveFlag() == false)
          return;*/

    RequestKeyboardCaptureInput();
    ResetCursorBlinkTimer();
}

void UI::UITextInputComponent::ReleaseKeyboardFocus()
{
    if (mIsFocused)
    {
        ReleaseKeyboardCaptureInput();
    }
}

void UI::UITextInputComponent::OnSetMouseFocus(bool isRequester)
{

    IUIComponent::OnSetMouseFocus(isRequester);
    // mMultiRectCom->SetActiveState(true);
    mMultiRectCom->Reset();
    ReleaseSelectedRegion();
}

void UI::UITextInputComponent::OnLostMouseFocus()
{
    IUIComponent::OnLostMouseFocus();

    if (mIsDragging)
    {
        mIsDragging = false;

        if (mCursorSelectionStartIndex == mCursorIndex)
        {
            mMultiRectCom->SetActiveState(false);
        }

        //   mMultiRectCom->SetActiveState(false);
    }
}

void UI::UITextInputComponent::OnSetKeyboardFocus(bool isRequester)
{
    // 사실상 활성화 시작

    UI::IUIComponent::OnSetKeyboardFocus(isRequester);

    if (isRequester)
    {
        mIsFocused = true;
        mCursorIndex = 0; // 무조건 0으로 초기화되고있음
                          //  RequestMouseCaptureInput();

        ReleaseSelectedRegion();
        mIsDragging = false;
        UpdateCursorPosition();

        mOnStartInputCallbackSystem.ExecuteCallbacks();
    }
}

void UI::UITextInputComponent::OnLostKeyboardFocus()
{

    UI::IUIComponent::OnLostKeyboardFocus();
    mIsFocused = false;

    UI::UIEditBox *editBox = static_cast<UI::UIEditBox *>(GetOwnerUIElement());
    UIElement *cursorImageElement = editBox->GetCursorImageElement();
    if (cursorImageElement)
    {
        cursorImageElement->SetActiveFlag(false);
    }

    ReleaseSelectedRegion();
    mOnEnterInputCallbackSystem.ExecuteCallbacks(mUITextCom->GetText());
}

int UI::UITextInputComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

void UI::UITextInputComponent::OnHover(int x, int y)
{
    mHover = true;
}

void UI::UITextInputComponent::OnReleaseHover()
{
    mHover = false;
}

void UI::UITextInputComponent::OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                           bool &bConsume)
{

    if (mIsFocused == false)
    {
        mIsFocused = true;
        RequestKeyboardCaptureInput();
    }
    else
    {
        ResetCursorBlinkTimer();
    }
    RequestMouseCaptureInput();

    // worldPos - > localPos
    glm::vec2 localPos = GetOwnerUIElement()->mTransform.WorldToLocal(worldPosX, worldPosY);
    SetCursorPos(localPos);

    mCursorSelectionStartIndex = mCursorIndex;
    mIsDragging = true; // 드래그는 시작이지만
    bConsume = true;
}

void UI::UITextInputComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                         bool &bConsume)
{
    if (mIsFocused == true)
    {
        ReleaseMouseCaptureInput();
        bConsume = true;
    }
}

void UI::UITextInputComponent::OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY)
{

    if (mIsDragging)
    {
        // cursor index 계산
        glm::vec2 localPos = GetOwnerUIElement()->mTransform.WorldToLocal(worldPosX, worldPosY);
        SetCursorPos(localPos);

        // cursorSelectionStartIndex -  현재 cursorIndex 까지 파란색 사각형

        int startIndex = mCursorSelectionStartIndex;
        int endIndex = mCursorIndex;

        if (startIndex == endIndex)
        {
            // 드래그중에 임시로 선택영이없어 해제됨 , 완전히 해제된것은 아님
            mMultiRectCom->SetActiveState(false);
            mIsSelected = false;
            return;
        }
        else
        {
            if (mMultiRectCom->GetActiveState() == false)
            {
                mMultiRectCom->SetActiveState(true);
                mIsSelected = true;
            }
        }

        if (startIndex > endIndex)
            std::swap(startIndex, endIndex);

        const std::vector<UI::TextSelectionRect> &selectionRects = mUITextCom->GetSelectionRects(startIndex, endIndex);

        std::vector<UI::RenderRect> rects;
        for (const auto &rect : selectionRects)
        {
            rects.push_back({{rect.mPos.x, rect.mPos.y}, {rect.mPos.x + rect.mSize.x, rect.mPos.y + rect.mSize.y}});
        }

        mMultiRectCom->SetRects(rects);
    }
}

void UI::UITextInputComponent::OnKeyDown(const Quad::EKeyCode &key, bool &bConsume)
{

    if (mIsFocused == false)
        return;

    // Enter
    if (key == Quad::EKeyCode::eEnter)
    {
        EUITextOverflowMode overflowMode = mUITextCom->GetOverflowMode();

        if (overflowMode == EUITextOverflowMode::eMultiLine)
        {
            InsertChar('\n');
        }
        else
        {

            ReleaseKeyboardFocus();
            return;
        }
    }

    // 지금 영문자만들어가니깐 가능하지만, 한글이들어가면 유니코드로 글자 수를 파악해야함
    std::string text = mUITextCom->GetText();
    if (key == Quad::EKeyCode::eRight)
    {

        if (mIsSelected)
        {
            int startIndex = mCursorSelectionStartIndex;
            int endIndex = mCursorIndex;

            if (startIndex > endIndex)
            {
                endIndex = startIndex;
            }

            mCursorIndex = endIndex;

            ReleaseSelectedRegion();
        }
        else
        {

            if (mCursorIndex < text.size())
                mCursorIndex++;
        }

        UpdateCursorPosition();
        bConsume = true;
    }
    else if (key == Quad::EKeyCode::eLeft)
    {

        if (mIsSelected)
        {
            int startIndex = mCursorSelectionStartIndex;
            int endIndex = mCursorIndex;

            if (startIndex > endIndex)
            {
                endIndex = startIndex;
            }

            mCursorIndex = startIndex;

            ReleaseSelectedRegion();
        }
        else
        {
            if (mCursorIndex > 0)
                mCursorIndex--;
        }

        UpdateCursorPosition();
        bConsume = true;
    }

    ResetCursorBlinkTimer();

    // 키보드 포거스를 잡았으니 다 소비

    bConsume = true;
}

void UI::UITextInputComponent::OnChar(uint32_t ch, bool &bConsume)
{

    // TODO Unicode로 처리해야한다
    // 지금은 영문자만 다루었음으로 아스키코드로 처리되고있다.
    if (mIsFocused == false)
        return;

    ResetCursorBlinkTimer();

    // backSpace
    if (ch == 8)
    {
        std::string text = mUITextCom->GetText();

        if (mIsSelected)
        {
            RemoveSelectedRegion(text);

            mUITextCom->SetText(text);
            UpdateCursorPosition();
            bConsume = true;
            return;
        }

        if (mCursorIndex == 0)
            return;

        if (text.length() > 0)
        {

            text.erase(text.begin() + mCursorIndex - 1);
            mCursorIndex--;
            mUITextCom->SetText(text);
            UpdateCursorPosition();

            mOnTextChangedCallbackSystem.ExecuteCallbacks(mUITextCom->GetText());
        }
        bConsume = true;
        return;
    }

    // 제어 문자 필터링 (백스페이스(8), 엔터(13), ESC(27) 등은 제외)
    // 32번(Space) 이상만 실제 문자로 취급
    if (ch < 32)
        return;

    bool bFilterRet = CheckFilter(ch);
    if (bFilterRet != false)
    {
        InsertChar(ch);
    }

    bConsume = true;
}

bool UI::UITextInputComponent::IsHovered() const
{
    return mHover;
}

void UI::UITextInputComponent::SetCursorPos(float localX, float localY)
{
    SetCursorPos({localX, localY});
}

void UI::UITextInputComponent::SetCursorPos(const glm::vec2 &localPos)
{

    if (mUITextCom)
    {
        mCursorIndex = mUITextCom->GetCursorIndexFromPos(localPos);
    }

    UpdateCursorPosition();
}

void UI::UITextInputComponent::ResetCursorBlinkTimer()
{

    UIEditBox *editBox = static_cast<UI::UIEditBox *>(GetOwnerUIElement());

    UIElement *cursorImageElement = editBox->GetCursorImageElement();
    if (cursorImageElement)
    {
        cursorImageElement->SetActiveFlag(true);
    }

    mCursorBlinkTime = 0.0f;
}

void UI::UITextInputComponent::SetInputType(EUITextInputType inputType)
{

    mInputType = inputType;
}

UI::EUITextInputType UI::UITextInputComponent::GetTextInputType() const
{
    return mInputType;
}

void UI::UITextInputComponent::OnBegin()
{

    GetOwnerUIElement()->GetComponents<UI::UITextComponent>(&mUITextCom, 1);

    if (mUITextCom)
    {
        mUITextCom->SetInputEnable(false);
    }

    GetOwnerUIElement()->GetComponents<UI::UIMultiRectComponent>(&mMultiRectCom, 1);
    if (mMultiRectCom)
    {
        mMultiRectCom->SetColor(0, 0, 1.0f, 0.4f);
    }
}

void UI::UITextInputComponent::UpdateCursorPosition()
{

    UI::UIEditBox *editBox = static_cast<UI::UIEditBox *>(GetOwnerUIElement());
    auto cursorElemnet = editBox->GetCursorImageElement();
    if (mUITextCom && cursorElemnet)
    {
        glm::vec2 cursorPos = mUITextCom->GetCursorPos(mCursorIndex);

        if (mUITextCom->GetOverflowMode() == EUITextOverflowMode::eScrollHorizontal)
        {
            float actualLeft = mUITextCom->GetActualLeft();
            float actualRight = mUITextCom->GetActualRight();
            float currentScrollX = mUITextCom->GetScrollOffsetX();
            bool bScrollChanged = false;
            if (cursorPos.x > actualRight)
            {
                currentScrollX += (cursorPos.x - actualRight);
                bScrollChanged = true;
            }
            else if (cursorPos.x < actualLeft)
            {
                currentScrollX -= (actualLeft - cursorPos.x);
                bScrollChanged = true;
            }

            if (currentScrollX > 0.0f)
            {
                glm::vec2 endPos = mUITextCom->GetCursorPos(mUITextCom->GetText().length());

                if (actualRight > endPos.x)
                {
                    currentScrollX -= (actualRight - endPos.x);

                    if (currentScrollX < 0.0f)
                        currentScrollX = 0.0f;

                    bScrollChanged = true;
                }
            }

            if (bScrollChanged)
            {
                mUITextCom->SetScrollOffsetX(currentScrollX);
                cursorPos = mUITextCom->GetCursorPos(mCursorIndex);
            }
        }
        cursorElemnet->SetPositionLocal(cursorPos);

        cursorElemnet->SetSize(2.0f, mUITextCom->GetLineHeight());
    }
}

void UI::UITextInputComponent::InsertChar(char ch)
{

    std::string text = mUITextCom->GetText();
    // text += static_cast<char>(ch); // 영어숫자만 일단 생각하면 이코드는 가능함. 향후수정필요할수도
    if (mIsSelected)
    {
        RemoveSelectedRegion(text);
    }

    text.insert(text.begin() + mCursorIndex, ch);
    mCursorIndex++;

    mUITextCom->SetText(text);
    UpdateCursorPosition();

    mOnTextChangedCallbackSystem.ExecuteCallbacks(text);
}

void UI::UITextInputComponent::ReleaseSelectedRegion()
{

    mIsSelected = false;
    mCursorSelectionStartIndex = -1;

    if (mMultiRectCom->GetActiveState())
        mMultiRectCom->SetActiveState(false);
}

void UI::UITextInputComponent::RemoveSelectedRegion(std::string &text)
{
    // 선택되어있다면지우고 문자삽입.
    int startIndex = mCursorSelectionStartIndex;
    int endIndex = mCursorIndex;

    if (startIndex > endIndex)
        std::swap(startIndex, endIndex);

    text.erase(text.begin() + startIndex, text.begin() + endIndex);

    mCursorIndex = startIndex;
    ReleaseSelectedRegion();
}

bool UI::UITextInputComponent::CheckFilter(char ch)
{
    if (mInputType == EUITextInputType::eNumber)
    {
        bool isValidChar = false;

        if ('0' <= ch && ch <= '9')
            isValidChar = true;
        else if (ch == '-' || ch == '.')
            isValidChar = true;

        if (!isValidChar)
            return false;

        std::string currentText = mUITextCom->GetText();

        if (ch == '.')
        {
            //.은 한번만 입력가능
            if (currentText.find('.') != std::string::npos)
            {
                return false;
            }
        }
        else if (ch == '-')
        {
            if (mCursorIndex != 0)
                return false;

            if (currentText.find('-') != std::string::npos)
                return false;
        }
    }

    return true;
}

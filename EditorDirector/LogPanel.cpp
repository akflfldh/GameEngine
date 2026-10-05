#include "LogPanel.h"
#include <EditorDirector/EditorUIUtility.h>
#include <EditorDirector/UIScrollBox.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextComponent.h>
#include <algorithm>

LogPanel::LogPanel()
{
    SetStyleRole(UI::EUIStyleRole::eNone);
    SetColor(0.12f, 0.12f, 0.12f);
    SetUseScissorRect(true);
    // 활성 Canvas에서는 생성 즉시 Begin되므로 스크롤 테두리보다 큰 초기 표시 영역을 준비한다.
    SetSize(100.0f, 100.0f);
}

LogPanel::~LogPanel() {}

void LogPanel::OnBegin()
{
    UI::UIImage::OnBegin();
    if (mScrollBox)
        return;
    mScrollBox = EditorUIUtility::Create<UIScrollBox>(this, "LogScrollBox");
    mScrollBox->SetSize(std::max(1, GetWidth()), std::max(1, GetHeight()));
    mScrollBox->SetBackgrounColor(0.12f, 0.12f, 0.12f);
    mText = EditorUIUtility::CreateLabel(this, "LogText");
    EditorUIUtility::ApplyPreset(mText, UI::EUIStyleRole::eNone);
    // EditorUIUtility의 공통 폰트 규격 유지: mText->SetFontSize(20.0f);
    // EditorUIUtility의 기본 색상 유지: mText->SetTextColor(UI::UIColor::White);
    mText->SetHeight(28.0f);
    mText->SetOnlyVisible(true);
    mText->SetOverflowMode(UI::EUITextOverflowMode::eWordWrap);
    mScrollBox->AddItem(mText);
    RefreshText();
}

void LogPanel::OnTransformChanged(UI::ETransformChangeType type)
{
    UI::UIImage::OnTransformChanged(type);
    if (mScrollBox && (type == UI::ETransformChangeType::eSize || type == UI::ETransformChangeType::eAll))
    {
        mScrollBox->SetSize(std::max(1, GetWidth()), std::max(1, GetHeight()));
        if (mText)
        {
            // 같은 아이템을 다시 배치하면 기존 ScrollBox API가 현재 콘텐츠 너비를 적용한다.
            // SetParent는 같은 부모에 대해서는 아무 작업도 하지 않으므로 아이템을 중복 추가하지 않는다.
            mScrollBox->AddItem(mText);
            mText->GetTextComponent()->MarkDirty();
            mText->GetTextComponent()->GetVertexNum();
            mScrollBox->ForceUpdateLayout();
        }
    }
}

void LogPanel::AppendMessage(const std::string &message)
{
    if (message.empty())
        return;
    mMessages.push_back(message);
    while (mMessages.size() > mMaxMessageCount)
        mMessages.pop_front();
    RefreshText();
}

void LogPanel::ClearMessages()
{
    mMessages.clear();
    RefreshText();
}

void LogPanel::RefreshText()
{
    if (!mText)
        return;
    std::string text;
    for (const auto &message : mMessages)
    {
        text += message;
        if (text.back() != '\n')
            text += '\n';
    }
    if (text.empty())
        text = "로그 메시지 표시 영역";
    mText->SetText(text);

    // WordWrap은 텍스트 계산 시 높이를 결정한다. 계산을 먼저 끝내야 스크롤 범위가 새 기록과 일치한다.
    mText->GetTextComponent()->GetVertexNum();
    mScrollBox->ForceUpdateLayout();
}

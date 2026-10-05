#pragma once

#include <UiSystem/UIImage.h>
#include <deque>
#include <string>

#include "LogPanel.generated.h"

class UIScrollBox;
namespace UI
{
class UIText;
}

// 하단 Log 탭의 메시지 표시를 담당한다. Logger의 수집·구독은 이 표시 타입과 별도로 연결한다.
// 메시지 문자열은 패널이 보관하며, 실제 텍스트와 스크롤 UI의 수명은 UIManager가 관리한다.
class REFLECT_CLASS(EngineClass) LogPanel : public UI::UIImage
{
    GENERATED_BODY(LogPanel)
  public:
    LogPanel();
    virtual ~LogPanel() override;
    virtual void OnBegin() override;
    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

    // UI 스레드에서 호출한다. Logger/작업 스레드의 알림은 복사된 메시지를 UI 스레드로 전달한 뒤 표시한다.
    void AppendMessage(const std::string &message);
    void ClearMessages();

  private:
    void RefreshText();
    UIScrollBox *mScrollBox = nullptr;
    UI::UIText *mText = nullptr;
    std::deque<std::string> mMessages;
    // 한 번에 표시할 기록 수를 제한하여 오래 실행해도 텍스트 메시가 무한히 커지지 않도록 한다.
    size_t mMaxMessageCount = 500;
};

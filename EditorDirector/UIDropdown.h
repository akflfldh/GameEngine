#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>
#include <string>
#include <vector>

#include "UIDropdown.generated.h"

/*

하위 목록창을 펼지고 접을때마다  UIDropdown의 크기도 동적으로 변함.
항상 대표목록을 보여주는 자식 ui는 보여짐

//대표목록 ui(+펼치기버튼) , 하위 목록리스트 ui 로 구성됨



*/

// parameter : selected item index;
using OnSelectedItemChangedCallbackSystem = Core::MultiCallbackSystem<size_t>;

namespace UI
{
class UIImage;
class UIButton;
class UITextButton;
class UIText;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIDropdown : public UI::UIElement
{
    GENERATED_BODY(UIDropdown)
  public:
    UIDropdown();
    virtual ~UIDropdown();

    virtual void Update(float DeltaTime) override;
    // 리스트 판넬을 펼친다.
    void Open();
    // 리스트 판넬을 접는다.
    void Close();

    void SetItemList(const std::vector<std::string> &list);

    void SetItemHeight(float h);

    void SetSelectedIndex(size_t index, bool bNotify = true);
    void SetSelectedItem(UI::UITextButton *item);

    OnSelectedItemChangedCallbackSystem mOnSelectedItemChangedCallbackSystem;

    void SetHeaderButtonSize(float d);

    virtual void SetSize(float w, float h) override;
    virtual void SetWidth(float w) override;

    void SetHeaderHeight(float h);

    void ApplyOpenProgress();

  protected:
    virtual void OnBegin() override;

  private:
    UI::UIImage *CreateHeaderPanel();
    UI::UIImage *CreateListPanel();

    bool IsListOpened() const;

    UI::UITextButton *CreateItem(const std::string &text);

  private:
    UI::UIImage *mHeader = nullptr;
    UI::UIButton *mHeaderDropButton = nullptr;
    UI::UIText *mHeaderText = nullptr;
    UI::UIImage *mListPanel = nullptr;

    bool mIsListOpened = false;
    float mOpenProgress = 0.0f; // 0.0f :닫힘 ~ 1.0f :열림
    float mOpenSpeed = 8.0f;

    std::vector<std::string> mItemTextList;
    std::vector<UI::UITextButton *> mItemList;
    size_t mSelectedIndex = 0;

    float mItemHeight = 30.0f;
};

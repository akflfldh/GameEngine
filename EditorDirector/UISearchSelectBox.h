#pragma once

#include <UiSystem/UIImage.h>
#include <string>
#include <vector>

#include <CoreBase/CallbackSystem.h>

#include "UISearchSelectBox.generated.h"

using OnSearchTextChangedCallbackSystem = Core::MultiCallbackSystem<const std::string &>;
using OnSelectedItemChangedCallbackSystem = Core::MultiCallbackSystem<uint32_t>;

namespace UI
{
class UIEditBox;
class UITextButton;

} // namespace UI

struct SearchSelectItem
{
    std::string mLabel;
    uint32_t mUniqueNum = 0;
};

class REFLECT_CLASS(EngineClass) UISearchSelectBox : public UI::UIImage
{
    GENERATED_BODY(UISearchSelectBox)
  public:
    UISearchSelectBox();
    virtual ~UISearchSelectBox();

    virtual void Update(float deltaTime) override;

    void ClearItems();
    void SetItems(const std::vector<SearchSelectItem> &items);

    void SetListPanelHeight(float h);
    virtual void SetWidth(float w) override;

    OnSearchTextChangedCallbackSystem mOnSearchTextChangedCallbckSystem;
    OnSelectedItemChangedCallbackSystem mOnSelectedItemChangedCallbackSystem;

    void ClearSelectedText();
    void SetSelectedText(const std::string &text);

  protected:
    virtual void OnBegin() override;

  private:
    void Initialize();
    void CreateEditBox();
    void CreateListPanel();

    void RebuildItemList();

    UI::UITextButton *GetItemButton(const SearchSelectItem &item, size_t itemIndex);

    void OnSearchTextChanged(const std::string &text);

    void OnSelectedItem(size_t index);

    void ScrollListByWheel(int wheelDelta);

    void ScrollList(float shift);

    void OnFinishInput(const std::string &text);
    bool TrySelectItemByLabel(const std::string &label);

    void FilterItems(const std::string &text);

    // 현재 보이는 필터링된 아이템리스트 지우기
    void ClearVisibleitemsImmediately();
    void ClearItemButtons();

    // ClearVisibleItems 요청
    void RequestClearVisibleItems();

    void HideListPanel();
    void ShowListPanel();

    float GetEditBoxHeight() const;

  private:
    UI::UIEditBox *mEditBox = nullptr;

    UI::UIImage *mListViewportPanel = nullptr;
    UI::UIImage *mListContentPanel = nullptr;

    // 원본 전체 아이템 리스트
    std::vector<SearchSelectItem> mItems;
    // 현재 검색어에 매칭된 리스트
    std::vector<SearchSelectItem> mFilteredItems;

    // 검색어 매칭된 아이템 버튼들
    std::vector<UI::UITextButton *> mItemButtons;

    std::vector<UI::UITextButton *> mItemButtonPool;

    //// 다음프레임에 콜백을 해제할 이미 반납된 버튼들
    // std::vector<UI::UITextButton *> mPendingClearVisibleItems;

    // item선택시에 editbox의 finish 호출 중복 방지플래그
    bool mSelectingItem = false;

    bool mPendingClearVisibleItems = false;
};

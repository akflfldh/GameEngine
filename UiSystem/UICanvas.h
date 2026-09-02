#pragma once

#include "UiSystem/UIType.h"
#include <CoreMath/CoreMath.h>
#include <string>
#include <vector>

#include <UiSystem/IUIRenderProxyManager.h>

namespace UI
{
class UIRenderProxy;
class UIElement;

/*
    logical window- canvas는 1:1대응이되는게 맞는거같다.
    그래서 window size를 canvas가 유지하고
    uiElement들의 위치 pivot 설정에사용한다.

*/
class UISYSTEM_API UICanvas
{
    friend class UIManager;

  public:
    UICanvas(UICanvasID id, const std::string &name, ECanvasSizeMode sizeMode);
    ~UICanvas();

    UICanvasID GetID() const;

    void Begin();
    void Update(float deltaTime);

    void AddChild(UIElement *uiElement);

    void OnWindowResize(float w, float h);
    CoreMath::Vector2 GetWindowSize() const;

    void MarkDirty();

    void IncreaseRefCount();
    void DecreaseRefCount();

    uint32_t GetRefCount() const;

    ECanvasSizeMode GetSizeMode() const;

    void SetSize(CoreMath::Vector2 size);
    bool GetActiveFlag() const;
    const std::vector<UIElement *> &GetChildUIElementAll() const;
    const std::vector<UIElement *> &GetTopChildUIElement() const;

    void SetDepthValue(uint32_t value);
    uint32_t GetDepthValue() const;

    uint32_t GetTopUIElementDepthValue() const;

    void SetUIElementTopDepth(UIElement *uiElement);

    template <typename T> T *CreateUIElement(const char *instanceName);
    UIElement *CreateUIElement(const char *className, const char *instanceName);

    // Destory한다해서 바로 제거되면안됨 그럼 이 함수로 돌아왔을때 무슨일이 발생? -> 심각한 문제
    void DestroyUIElement(UIElement *uiElement);

    const std::vector<UI::UIRenderProxy *> &GetRenderProxyList();
    const std::vector<UI::UIRenderProxy *> &GetRenderProxyLists();

    void ProcessPendingElements();

    void SortTopChildElement();

    UI::UIElement *GetHittedElement(float x, float y) const;

  private:
    void AddChildInternal(UIElement *uiElement);

    void RebuildRenderProxyList();

    void RemoveUIElementFromList(UI::UIElement *uiElement);

    UI::UIElement *FindHittedElementRecursive(UI::UIElement *element, float x, float y) const;

  private:
    UICanvasID mID;
    std::string mName;
    bool mActiveFlag;
    bool mIsBegin;

    ECanvasSizeMode mCanvasSizeMode;

    std::vector<UIElement *> mTopChildUIElementList; // 최상위UIElement들
    std::vector<UIElement *> mChildUIElement;

    // 새로 생성되서 추가될 element 리스트
    std::vector<UIElement *> mPendingAddList;

    uint32_t mDepthValue;
    // 현재 가장 위에있는 요소깊이값. 다음번 새로운 요소가 가장위로올라와야한다. 이값 +1 을가지게될것이다.
    uint32_t mTopUIElementDepthValue;

    // 외부에서 접근하여 사용하고있다는것을 카운팅하는 도우미 변수.
    uint32_t mRefCount;

    // UIElement draw의 변화 여부
    bool mIsRenderableListDirty = true;
    std::vector<UI::UIRenderProxy *> mCachedRenderProxyList;

    CoreMath::Vector2 mWindowSize;
};
template <typename T> T *UI::UICanvas::CreateUIElement(const char *instanceName)
{

    return static_cast<T *>(CreateUIElement(T::GetStaticClassName(), instanceName));
}
} // namespace UI

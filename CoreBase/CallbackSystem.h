#pragma once
#pragma once
#include <functional>
#include <stdint.h>
#include <vector>

namespace Core
{

using CallbackID = uint64_t;
const CallbackID CallbackIDNone = 0;
// 정적 버전

template <typename T, typename Valid = void> struct CheckInstancePtr
{
    /*  특수화되는경우  ptr * GetPtr(T * instance) 시그니처의 정적 메서드를 정의해야한다 . */
};

// 이런 콜백들의 리턴은 무조건 void
// 생성자에서 바인딩하면 X
// Begin 이후부터 바인딩 해야한다 .

template <typename... Args> class MultiCallbackSystem
{
    struct Listener
    {
        CallbackID mCallbackID;
        std::function<void(Args...)> mCallback;
    };

  public:
    // 일반함수 등록
    CallbackID Register(const std::function<void(Args...)> &callback);

    // 오브젝트 메서드 등록
    template <typename T> CallbackID Register(T *Object, void (T::*callback)(Args...));

    // 등록해제 공통
    bool UnRegister(CallbackID id);

    // 모두 등록해제
    void UnRegisterAll();

    // 모든 리스너 호출
    void ExecuteCallbacks(Args... args);

    // 제거되어야할 등록된 콜백들을 모두 제거한다.
    void CleanUp();

  private:
    // 제거되어야할 콜백을 등록한다.
    void Clean(CallbackID id);

    bool IsMarkedForClean(CallbackID id);

  private:
    std::vector<MultiCallbackSystem::Listener> mListenerList;
    std::vector<CallbackID> mCleanList;
    CallbackID mNextCallback = 1;
};

template <typename... Args>
inline CallbackID MultiCallbackSystem<Args...>::Register(const std::function<void(Args...)> &callback)
{
    Listener listener;
    listener.mCallbackID = mNextCallback++;
    listener.mCallback = callback;

    mListenerList.push_back(std::move(listener));

    return listener.mCallbackID;
}

template <typename... Args> inline void MultiCallbackSystem<Args...>::ExecuteCallbacks(Args... args)
{

    for (size_t i = 0; i < mListenerList.size(); ++i)
    {
        mListenerList[i].mCallback(args...); // 일반함수들은 그냥 호출
        // 메서드는 검사후 호출될것이다.
        // 그 행동은 내부적으로 수행되니
        // , 여기서는 아무것도 알필요없이 호출하기만하면됨

        // TODO

        // 이전 콜백에서 이후 등록된 리스너를 해제한다면?또는 제거한다면  - >좀비 리스너  처리필요
        if (IsMarkedForClean(mListenerList[i].mCallbackID))
            continue;
    }
    // 한프레임 여러번 호출될수있고 그러면 clean이 여러번 중복해서 호출되서 동일한것이 여러개 들어갈수있음
    // 그러니 바로 CleanUp수행해서 깔끔하게 처리
    CleanUp();

    // 향후 멀티스레드 도입시 데이터경쟁을 위해서 동기화 구조 필요
    return;
}

template <typename... Args> inline void MultiCallbackSystem<Args...>::CleanUp()
{

    for (auto id : mCleanList)
    {
        UnRegister(id);
    }

    mCleanList.clear();
}

template <typename... Args> inline void MultiCallbackSystem<Args...>::Clean(CallbackID id)
{

    mCleanList.push_back(id);
}

template <typename... Args> inline bool MultiCallbackSystem<Args...>::UnRegister(CallbackID id)
{
    if (id == CallbackIDNone)
        return false;

    auto it = std::find_if(mListenerList.begin(), mListenerList.end(),
                           [id](const Listener &listener) { return listener.mCallbackID == id; });

    if (it == mListenerList.end())
        return false;

    mListenerList.erase(it);

    return true;
}

template <typename... Args> inline void MultiCallbackSystem<Args...>::UnRegisterAll()
{

    for (auto &listener : mListenerList)
    {
        Clean(listener.mCallbackID);
    }

    CleanUp();
}

template <typename... Args>
template <typename T>
inline CallbackID MultiCallbackSystem<Args...>::Register(T *instance, void (T::*method)(Args...))
{

    if (instance == nullptr)
        return CallbackIDNone;

    Listener listener;
    listener.mCallbackID = mNextCallback++;

    auto ptr = CheckInstancePtr<T>::GetPtr(instance);

    auto callback = [this, instance, method, ptr, callbackID = listener.mCallbackID](Args... args)
    {
        auto actualInstance = ptr.Get(); // 모든 ptr들이 동일한이름의 Get 메서드를 가져야한다.
        if (actualInstance == instance)
        {
            (instance->*method)(args...);
        }
        else
        {
            this->Clean(callbackID);
        }
    };
    listener.mCallback = callback;
    mListenerList.push_back(std::move(listener));

    return listener.mCallbackID;
}

template <typename... Args> inline bool MultiCallbackSystem<Args...>::IsMarkedForClean(CallbackID id)
{

    for (auto cleanId : mCleanList)
    {
        if (cleanId == id)
            return true;
    }
    return false;
}

} // namespace Core

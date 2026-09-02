#pragma once
// #include <Core/Object.h>
// #include <Core/ObjectPtr.h>
// #include <functional>
// #include <stdint.h>
// #include <vector>
// namespace Core
//{
//
// using CallbackID = uint64_t;
// const CallbackID CallbackIDNone = 0;
//// 정적 버전
//
//// 이런 콜백들의 리턴은 무조건 void
// template <typename... Args> class MultiCallbackSystem
//{
//     struct Listener
//     {
//         CallbackID mCallbackID;
//         std::function<void(Args...)> mCallback;
//     };
//
//   public:
//     // 일반함수 등록
//     CallbackID Register(const std::function<void(Args...)> &callback);
//
//     // 오브젝트 메서드 등록
//     template <typename T> CallbackID Register(T *Object, void (T::*callback)(Args...));
//
//     // 등록해제 공통
//     bool UnRegister(CallbackID id);
//
//     // 모든 리스너 호출
//     void ExecuteCallbacks(Args... args);
//
//     // 제거되어야할 등록된 콜백들을 모두 제거한다.
//     void CleanUp();
//
//   private:
//     // 제거되어야할 콜백을 등록한다.
//     void Clean(CallbackID id);
//
//   private:
//     std::vector<MultiCallbackSystem::Listener> mListenerList;
//     std::vector<CallbackID> mCleanList;
//     CallbackID mNextCallback = 0;
// };
//
// template <typename... Args>
// inline CallbackID MultiCallbackSystem<Args...>::Register(const std::function<void(Args...)> &callback)
//{
//     Listener listener;
//     listener.mCallbackID = mNextCallback++;
//     listener.mCallback = callback;
//
//     mListenerList.push_back(std::move(listener));
//
//     return listener.mCallbackID;
// }
//
// template <typename... Args> inline void MultiCallbackSystem<Args...>::ExecuteCallbacks(Args... args)
//{
//
//     for (auto &listener : mListenerList)
//     {
//         listener.mCallback(args...); // 일반함수들은 그냥 호출
//         // 메서드는 검사후 호출될것이다.
//         // 그 행동은 내부적으로 수행되니 , 여기서는 아무것도 알필요없이 호출하기만하면됨
//     }
//     // 한프레임 여러번 호출될수있고 그러면 clean이 여러번 중복해서 호출되서 동일한것이 여러개 들어갈수있음
//     // 그러니 바로 CleanUp수행해서 깔끔하게 처리
//     CleanUp();
//
//     // 향후 멀티스레드 도입시 데이터경쟁을 위해서 동기화 구조 필요
//     return;
// }
//
// template <typename... Args> inline void MultiCallbackSystem<Args...>::CleanUp()
//{
//
//     for (auto id : mCleanList)
//     {
//         UnRegister(id);
//     }
//
//     mCleanList.clear();
// }
//
// template <typename... Args> inline void MultiCallbackSystem<Args...>::Clean(CallbackID id)
//{
//
//     mCleanList.push_back(id);
// }
//
// template <typename... Args> inline bool MultiCallbackSystem<Args...>::UnRegister(CallbackID id)
//{
//     if (id == CallbackIDNone)
//         return false;
//
//     auto it = std::find_if(mListenerList.begin(), mListenerList.end(),
//                            [id](const Listener &listener) { return listener.mCallbackID == id; });
//
//     if (it == mListenerList.end())
//         return false;
//
//     mListenerList.erase(it);
//
//     return true;
// }
//
// template <typename... Args>
// template <typename T>
// inline CallbackID MultiCallbackSystem<Args...>::Register(T *instance, void (T::*method)(Args...))
//{
//
//     if (instance == nullptr)
//         return CallbackIDNone;
//
//     Listener listener;
//     listener.mCallbackID = mNextCallback++;
//     if constexpr (std::is_base_of_v<Object, T>)
//     {
//         ObjectPtr<T> objectPtr = instance;
//
//         auto callback = [objectPtr, method, this, callbackID = listener.mCallbackID](Args... args)
//         {
//             T *ob = objectPtr.Get(); // weak_ptr의 역할수행
//             if (ob != nullptr)
//             {
//                 (ob->*method)(args...);
//             }
//             else
//             {
//                 this->Clean(callbackID);
//             }
//         };
//         listener.mCallback = callback;
//     }
//     else
//     {
//         auto callback = [instance, method](Args... args) { (instance->*method)(args...); };
//         listener.mCallback = callback;
//     }
//
//     mListenerList.push_back(std::move(listener));
//
//     return listener.mCallbackID;
// }
//
// } // namespace Core
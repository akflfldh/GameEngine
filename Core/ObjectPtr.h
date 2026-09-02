#pragma once

#include <Core/Object.h>
#include <Core/ObjectManager.h>
#include <Core/ObjectTypeDef.h>
#include <CoreBase/CallbackSystem.h>

class Object;
namespace Core
{

// WeakPtr처럼 동작한다

template <typename T> class ObjectPtr
{
  public:
    ObjectPtr(T *object = nullptr);
    ObjectPtr(T &object);

    ObjectPtr(const ObjectPtr &ptr) = default;
    ObjectPtr &operator=(const ObjectPtr &ptr) = default;

    ObjectPtr(ObjectPtr &&ptr) noexcept;
    ObjectPtr &operator=(ObjectPtr &&ptr) noexcept;

    //    ObjectPtr &operator=(T *object);

    T *Get() const;

  private:
    mutable T *mObject;
    ObjectHandle mObjectHandle;
};

template <typename T> Core::ObjectPtr<T>::ObjectPtr(T *object)
{
    if constexpr (std::is_base_of_v<Object, T> == false)
    {
        static_assert(0);
    }

    mObject = object;

    if (mObject)
    {
        mObjectHandle = object->GetObjectHandle();
    }
}

template <typename T> Core::ObjectPtr<T>::ObjectPtr(T &object)
{
    if constexpr (std::is_base_of_v<Object, T> == false)
    {
        static_assert(0);
    }

    mObject = &object;
    mObjectHandle = object.GetObjectHandle();
}

template <typename T> Core::ObjectPtr<T>::ObjectPtr(ObjectPtr &&ptr) noexcept
{

    mObject = ptr.mObject;
    mObjectHandle = ptr.mObjectHandle;

    ptr.mObject = nullptr;
}

template <typename T> Core::ObjectPtr<T> &Core::ObjectPtr<T>::operator=(ObjectPtr<T> &&ptr) noexcept
{
    mObject = ptr.mObject;
    mObjectHandle = ptr.mObjectHandle;

    ptr.mObject = nullptr;
    return *this;
}

// template <typename T> ObjectPtr<T> &Core::ObjectPtr<T>::operator=(T *object) {
//
//
//
//
// }

template <typename T> T *Core::ObjectPtr<T>::Get() const
{

    if (mObject == nullptr)
        return mObject;

    ObjectManager *objectManager = ObjectManager::GetInstance();

    T *object = static_cast<T *>(objectManager->Get(mObjectHandle));
    if (object == nullptr)
    {
        mObject = nullptr;
    }

    return object;
}

template <typename T> struct CheckInstancePtr<T, typename std::enable_if<std::is_base_of<Object, T>::value>::type>
{
    static ObjectPtr<T> GetPtr(T *instance)
    {
        return ObjectPtr<T>(instance);
    }
};

} // namespace Core
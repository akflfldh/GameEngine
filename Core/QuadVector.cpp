#include "QuadVector.h"
#include <Core/Component.h>
#include <Core/QuadVectorImpl.h>
#include <Core/SceneComponent.h>
template <typename T> inline Core::QuadVector<T>::QuadVector()
{

    mImpl = std::make_unique<Core::QuadVectorImpl<T>>();
}

template <typename T> inline Core::QuadVector<T>::~QuadVector() {}

template <typename T> Core::QuadVector<T>::QuadVector(const QuadVector &lhs)
{

    mImpl = std::make_unique<Core::QuadVectorImpl<T>>(*(lhs.mImpl.get()));
}

template <typename T> Core::QuadVector<T> &Core::QuadVector<T>::operator=(const QuadVector &lhs)
{
    // TODO: 여기에 return 문을 삽입합니다.
    *(mImpl.get()) = *(lhs.mImpl.get());
    return *this;
}

template <typename T> Core::QuadVector<T>::QuadVector(QuadVector &&rhs)
{
    mImpl = std::make_unique<Core::QuadVectorImpl<T>>(std::move(*(rhs.mImpl.get())));
}

template <typename T> Core::QuadVector<T> &Core::QuadVector<T>::operator=(QuadVector &&rhs)
{
    // TODO: 여기에 return 문을 삽입합니다.
    *(mImpl.get()) = std::move(*(rhs.mImpl.get()));
    return *this;
}

template <typename T> void Core::QuadVector<T>::PushBack(const T &element)
{

    mImpl->PushBack(element);
}

template <typename T> void Core::QuadVector<T>::PushBack(T &&element)
{

    mImpl->PushBack(std::move(element));
}

template <typename T> void Core::QuadVector<T>::PopBack()
{

    mImpl->PopBack();
}

template <typename T> void Core::QuadVector<T>::Clear()
{

    mImpl->Clear();
}

template <typename T> T &Core::QuadVector<T>::Back()
{
    return mImpl->Back();
}

template <typename T> const T &Core::QuadVector<T>::Back() const
{
    return mImpl->Back();
}

template <typename T> size_t Core::QuadVector<T>::Find(const T &element) const
{
    return mImpl->Find(element);
}
template <typename T> size_t Core::QuadVector<T>::FindIf(bool (*pred)(const T &))
{
    return mImpl->FindIf(pred);
}
template <typename T> void Core::QuadVector<T>::Erase(size_t index)
{

    mImpl->Erase(index);
}
template <typename T> size_t Core::QuadVector<T>::End() const
{
    return mImpl->End();
}

template <typename T> void Core::QuadVector<T>::Reserve(size_t i)
{

    mImpl->Reserve(i);
}

template <typename T> bool Core::QuadVector<T>::Empty() const
{

    return mImpl->Empty();
}

template <typename T> size_t Core::QuadVector<T>::Size() const
{

    return mImpl->Size();
}

template <typename T> T &Core::QuadVector<T>::operator[](size_t i)
{

    return mImpl->operator[](i);
}
template <typename T> const T &Core::QuadVector<T>::operator[](size_t i) const
{

    return mImpl->operator[](i);
}

template <typename T> T *Core::QuadVector<T>::Data()
{

    return mImpl->Data();
}

template <typename T> const T *Core::QuadVector<T>::Data() const
{

    return mImpl->Data();
}

template class Core::QuadVector<int>;
template class Core::QuadVector<char>;
template class Core::QuadVector<float>;
template class Core::QuadVector<double>;
template class Core::QuadVector<unsigned char>;
template class Core::QuadVector<unsigned short>;
template class Core::QuadVector<short>;
template class Core::QuadVector<long>;
template class Core::QuadVector<long long>;
template class Core::QuadVector<unsigned long long>;
template class Core::QuadVector<unsigned long>;
template class Core::QuadVector<unsigned int>;
template class Core::QuadVector<Component *>;
template class Core::QuadVector<SceneComponent *>;

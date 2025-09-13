#pragma once

#include <algorithm>
#include <vector>
namespace Core
{

template <typename T> class QuadVectorImpl
{
  public:
    QuadVectorImpl();
    ~QuadVectorImpl();

    QuadVectorImpl(const QuadVectorImpl &lhs);
    QuadVectorImpl &operator=(const QuadVectorImpl &lhs);

    QuadVectorImpl(QuadVectorImpl &&rhs);
    QuadVectorImpl &operator=(QuadVectorImpl &&rhs);

    void PushBack(const T &element);
    void PushBack(T &&element);
    void PopBack();

    T &Back();
    const T &Back() const;

    void Clear();
    bool Empty() const;
    void Reserve(size_t i);
    void Erase(size_t i);

    size_t Find(const T &element) const;
    size_t FindIf(bool (*pred)(const T &)) const;
    size_t End() const;

    size_t Size() const;

    T &operator[](size_t i);
    const T &operator[](size_t i) const;

    T *Data();
    const T *Data() const;

  private:
    std::vector<T> mVector;
};

template <typename T> inline QuadVectorImpl<T>::QuadVectorImpl() {}

template <typename T> inline QuadVectorImpl<T>::~QuadVectorImpl() {}

template <typename T> inline QuadVectorImpl<T>::QuadVectorImpl(const QuadVectorImpl &lhs) : mVector(lhs.mVector) {}

template <typename T> inline QuadVectorImpl<T> &QuadVectorImpl<T>::operator=(const QuadVectorImpl &lhs)
{
    // TODO: 여기에 return 문을 삽입합니다.

    mVector = lhs.mVector;
    return *this;
}

template <typename T> inline QuadVectorImpl<T>::QuadVectorImpl(QuadVectorImpl &&rhs) : mVector(std::move(rhs.mVector))
{
}

template <typename T> inline QuadVectorImpl<T> &QuadVectorImpl<T>::operator=(QuadVectorImpl &&rhs)
{
    // TODO: 여기에 return 문을 삽입합니다.

    mVector = std::move(rhs.mVector);
    return *this;
}

template <typename T> void inline QuadVectorImpl<T>::PushBack(const T &element)
{
    mVector.push_back(element);
}

template <typename T> void inline QuadVectorImpl<T>::PushBack(T &&element)
{
    mVector.push_back(std::move(element));
}

template <typename T> void inline QuadVectorImpl<T>::PopBack()
{
    mVector.pop_back();
}

template <typename T> T &QuadVectorImpl<T>::Back()
{
    return mVector.back();
}

template <typename T> const T &QuadVectorImpl<T>::Back() const
{
    return mVector.back();
}

template <typename T> void inline QuadVectorImpl<T>::Clear()
{

    mVector.clear();
}

template <typename T> void Core::QuadVectorImpl<T>::Reserve(size_t i)
{
    mVector.reserve(i);
}
template <typename T> void Core::QuadVectorImpl<T>::Erase(size_t i)
{
    mVector.erase(mVector.begin() + i);
}

template <typename T> size_t inline QuadVectorImpl<T>::Size() const
{
    return mVector.size();
}

template <typename T> size_t QuadVectorImpl<T>::Find(const T &element) const
{

    typename std::vector<T>::const_iterator it = std::find(mVector.begin(), mVector.end(), element);

    if (it == mVector.end())
    {
        return -1;
    }
    else
    {
        return it - mVector.begin();
    }
}

template <typename T> inline size_t QuadVectorImpl<T>::FindIf(bool (*pred)(const T &)) const
{

    typename std::vector<T>::const_iterator it = std::find_if(mVector.begin(), mVector.end(), pred);

    if (it == mVector.cend())
    {
        return -1;
    }
    else
    {
        return it - mVector.cbegin();
    }
}

template <typename T> size_t QuadVectorImpl<T>::End() const
{

    return -1;
}

template <typename T> bool inline QuadVectorImpl<T>::Empty() const
{
    return mVector.empty();
}
template <typename T> T &QuadVectorImpl<T>::operator[](size_t i)
{
    return mVector[i];
}

template <typename T> const T &QuadVectorImpl<T>::operator[](size_t i) const
{

    return mVector[i];
}

template <typename T> T *QuadVectorImpl<T>::Data()
{

    return mVector.data();
}
template <typename T> inline const T *QuadVectorImpl<T>::Data() const
{
    return mVector.data();
}
} // namespace Core
#pragma once
#include <memory>
namespace Core
{

template <typename T> class QuadVectorImpl;

template <typename T> class QuadVector
{
  public:
    static const size_t npos = -1;

    QuadVector();
    ~QuadVector();

    QuadVector(const QuadVector &lhs);
    QuadVector &operator=(const QuadVector &lhs);

    QuadVector(QuadVector &&rhs);
    QuadVector &operator=(QuadVector &&rhs);

    void PushBack(const T &element);
    void PushBack(T &&element);
    void PopBack();
    void Clear();

    T &Back();
    const T &Back() const;

    size_t Find(const T &element) const;
    size_t FindIf(bool (*pred)(const T &));

    void Erase(size_t index);

    size_t End() const;

    void Reserve(size_t size);

    bool Empty() const;

    size_t Size() const;

    T &operator[](size_t i);
    const T &operator[](size_t i) const;

    T *Data();
    const T *Data() const;

  private:
    std::unique_ptr<QuadVectorImpl<T>> mImpl;
};

} // namespace Core
#pragma once

#include <CoreBase/CoreBaseDllMacro.h>
#include <string>

// 오 Arch 현재 상태를 유지하고 , get하는 코드가있으면 좋겠는데 ?
// 외부의 사용자가 필요할때마다 상태를 get해서 적절하게 처리하는거지 , 물론 내부적으로 상태를 점검하면서 안전하게
// 프로그램이 유지되도록할수있고

// 일단 상태 종류는 단순하게

// operator << buffer의 경우에 매크로를 지원해서 쉽게 작성할수있도록 하자
//  arch << QUAD_SERIALIZEBUFFER (pData, size)  - > arch<<Arch::Buffer(pData,size) 로 변환
#define QUAD_SERIALIZEBUFFER(pData, size) Arch::Buffer(pData, size)

class FString;

class COREBASE_API Arch
{
  public:
    // 이상태를 비트 필드로 관리하자
    enum EArchState : uint8_t
    {
        eGood = 1,          // 현재 Arch의 상태가 정상
        eFail = 1 << 1,     // 마지막 동작 실패
        eStart = 1 << 2,    // Start가 호출되었음
        eEnd = 1 << 3,      // End가 호출되었음
        eReadFail = 1 << 4, // 읽기 실패
        eEof = 1 << 5       // 파일끝
    };

    struct Buffer
    {
        void *mData;
        size_t mSize;
        Buffer(void *data, size_t size) : mData(data), mSize(size) {}
    };

    using ArchCallback = void (*)(Arch &arch, void *instance);

  public:
    Arch(bool mLoadingFlag = true);
    virtual ~Arch() = 0;

    // 시작과 끝을 알리는 메서드(구체적인 Arch마다 하고싶은 동작이있을수있기에)
    virtual void Start();
    virtual void End();

    // 템플릿을 제거하고 기본타입에대해서 다 가상함수로 정의하자
    // template <typename T> Arch &operator<<(T &value);

    // Boolean
    virtual Arch &operator<<(bool &value) = 0;

    // Characters (필요에 따라 wchar_t 등 추가 가능)
    virtual Arch &operator<<(char &value) = 0;

    // Signed Integers (플랫폼 독립적인 고정 크기 정수 사용을 권장)
    virtual Arch &operator<<(int8_t &value) = 0;
    virtual Arch &operator<<(int16_t &value) = 0;
    virtual Arch &operator<<(int32_t &value) = 0;
    virtual Arch &operator<<(int64_t &value) = 0;

    // Unsigned Integers
    virtual Arch &operator<<(uint8_t &value) = 0;
    virtual Arch &operator<<(uint16_t &value) = 0;
    virtual Arch &operator<<(uint32_t &value) = 0;
    virtual Arch &operator<<(uint64_t &value) = 0;

    // Floating Point
    virtual Arch &operator<<(float &value) = 0;
    virtual Arch &operator<<(double &value) = 0;

    Arch &operator<<(const Buffer &buffer);
    virtual Arch &operator<<(FString &fstring) = 0;

    // 외부 타입들은 operator<< 오버로딩하면서 내부적으로는 return arch.ProcessObject(ob, read, write)를
    // 호출해주면된다 이러면 상태판정, 상태에따른 read, writer 분기가 내부적으로 처리되기에 유저는 상위 부분(커스텀
    // read,write)에만 신경쓰면된다.
    template <typename T>
    Arch &ProcessObject(T &value, ArchCallback readCallback, ArchCallback writeCallback /*reader fun , wrtier fun*/);

    bool GetLoadingFlag() const;
    uint8_t GetState() const;
    bool IsState(EArchState state) const;

    // 추가적으로 상태 헬퍼메서드 제공가능
    bool IsGood() const;
    bool IsFail() const;
    bool IsFinished() const;
    bool IsEof() const;

    // Arch의 상태를 모두 초기화하고 Good 상태만 설정한다.
    void Reset();

  protected:
    Arch &SerializeInterface(void *data, size_t size);

    virtual void Serialize(const void *data, size_t size) = 0;
    virtual void DeSerialize(void *data, size_t size) = 0;

    void SetState(uint8_t state);
    void ReleaseState(uint8_t state);

    // 직접 SetState를 할수있지만 Good, Fail상태는 반대임으로 Good상태를 설정하면 Fail상태는 해제해야한다. 안전하게 이
    // 메서드들을 호출하자
    void SetGoodState();
    void SetFailState();
    virtual bool IsAvaliableSerialize() const;

  private:
  private:
    bool mIsLoading;
    uint8_t mState;
};

// template <typename T> inline Arch &Arch::operator<<(T &value)
//{
//
//     static_assert(std::is_fundamental_v<T>, "Arch에 전달되는 value의 타입이 기본타입이 아닙니다.");
//
//     return SerializeInterface(&value, sizeof(T));
// }

template <typename T> inline Arch &Arch::ProcessObject(T &value, ArchCallback readCallback, ArchCallback writeCallback)
{

    if (IsAvaliableSerialize() == false)
    {
        return *this;
    }

    if (mIsLoading)
    {
        readCallback(*this, (void *)&value);
    }
    else
    {
        writeCallback(*this, (void *)&value);
    }

    return *this;
    // TODO: 여기에 return 문을 삽입합니다.
}

inline uint8_t Arch::GetState() const
{
    return mState;
}

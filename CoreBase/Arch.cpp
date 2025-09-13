#include "Arch.h"

Arch::Arch(bool loadingFlag) : mIsLoading(loadingFlag), mState((uint8_t)EArchState::eGood) {}

Arch::~Arch() {}

bool Arch::GetLoadingFlag() const
{
    return mIsLoading;
}

void Arch::Start()
{

    SetState(EArchState::eStart);
}
void Arch::End()
{

    SetState(EArchState::eEnd);
}

void Arch::SetState(uint8_t state)
{
    mState |= state;
}

void Arch::ReleaseState(uint8_t state)
{
    mState &= ~state;
}

void Arch::SetGoodState()
{

    SetState(EArchState::eGood);
    ReleaseState(EArchState::eFail);
}

void Arch::SetFailState()
{

    SetState(EArchState::eFail);
    ReleaseState(EArchState::eGood);
}

bool Arch::IsAvaliableSerialize() const
{
    if (!IsGood() || IsState(EArchState::eStart) == false)
    {
        return false;
    }

    return true;
}
bool Arch::IsState(EArchState state) const
{
    return mState & (uint8_t)state;
}

bool Arch::IsGood() const
{
    return IsState(EArchState::eGood);
}

bool Arch::IsFail() const
{
    return IsState(EArchState::eFail);
}

bool Arch::IsFinished() const
{
    return IsState(EArchState::eEnd);
}
bool Arch::IsEof() const
{
    return IsState(EArchState::eEof);
}

void Arch::Reset()
{

    mState = 0;
    SetState(EArchState::eGood);
}

Arch &Arch::SerializeInterface(void *value, size_t size)
{

    if (IsAvaliableSerialize() == false)
    {
        return *this;
    }

    if (mIsLoading)
    {
        DeSerialize(value, size);
    }
    else
    {
        Serialize(value, size);
    }
    return *this;
}
Arch &Arch::operator<<(const Buffer &buffer)
{
    return SerializeInterface(buffer.mData, buffer.mSize);
}

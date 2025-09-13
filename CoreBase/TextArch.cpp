#include "TextArch.h"

TextArch::TextArch(bool loadingFlag) : Arch(loadingFlag) {}

TextArch::~TextArch() {}

void TextArch::Start()
{

    Super::Start();
    if (GetLoadingFlag())
    {

        // 읽기
        // 파일 오픈
        bool ret = mReader.StartRead(mFilePath.c_str());
        if (ret == false)
        {
            // 실패 , log
            SetFailState();
        }
    }
    else
    { // 쓰기
        mWriter.StartWrite();
    }
}

void TextArch::End()
{

    Super::End();

    if (GetLoadingFlag())
    {
    }
    else
    {
        // 쓰기
        bool ret = mWriter.Close(mFilePath.c_str());
        if (ret == false)
        {
            // 실패 log
            SetFailState();
        }
    }
}

void TextArch::SetFile(const char *file)
{
    mFilePath = file;
}

Arch &TextArch::operator<<(bool &value)
{ // TODO: 여기에 return 문을 삽입합니다.

    return *this;
}

Arch &TextArch::operator<<(char &value)
{
    // TODO: 여기에 return 문을 삽입합니다.
    return *this;
}

Arch &TextArch::operator<<(int8_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(int16_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(int32_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(int64_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(uint8_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(uint16_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(uint32_t &value)
{
    return *this;
}

Arch &TextArch::operator<<(uint64_t &value)
{
    return *this;
}
Arch &TextArch::operator<<(float &value)
{
    return *this;
}
Arch &TextArch::operator<<(double &value)
{
    return *this;
}

Arch &TextArch::operator<<(FString &fstring)
{
    // TODO: 여기에 return 문을 삽입합니다.

    if (IsAvaliableSerialize() == false)
    {
        return *this;
    }

    if (GetLoadingFlag())
    {

        // 읽기

        FBuffer buffer;
        // 개행문자까지가 어디인지알아야하는데 한번에 읽을수가없어
        // 하나씩읽으면서 개행문자인지 파악할수밖에
        FString str;
        while (1)
        {
            char c = 0;
            if (mReader.Read(&c, sizeof(c)) != sizeof(c))
            {
                // 실패
                break;
            }

            if (c == '\n')
            {
                // 개행문자 발견
                fstring = std::move(str);
                break;
            }

            str += c;
        }

        if (mReader.GetFileSize() == mReader.GetReadPointer())
        {
            // EOF 설정
            SetState(Arch::EArchState::eEof);
        }
    }
    else
    {
        // 쓰기
        // 문자열을 쓰고나서 개행문자삽입. (나중에 개행문자가 문자열에 들어가있는경우도 고려해야함)

        mWriter.Write(fstring.c_str());
        mWriter.Write('\n');
    }

    return *this;
}

void TextArch::Serialize(const void *data, size_t size)
{

    // 일반적인타입이 호출될텐데 무시 하자
}

void TextArch::DeSerialize(void *data, size_t size)
{

    // 기본타입에대해서 호출될텐데 무시하자 오로지 FSTRING만 처리한다.
}

bool TextArch::IsAvaliableSerialize() const
{
    bool ret = Super::IsAvaliableSerialize();

    if (ret == false)
        return false;

    if (IsEof())
        return false;

    return true;
}

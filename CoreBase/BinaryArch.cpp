#include "BinaryArch.h"
#include "Arch.h"

BinaryArch::BinaryArch(bool isLoadingFlag) : Arch(isLoadingFlag) {}

BinaryArch::~BinaryArch() {}

void BinaryArch::Start()
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
    {
        // 쓰기
        mWriter.StartWrite();
    }
}

void BinaryArch::End()
{

    Super::End();

    if (GetLoadingFlag())
    {
        // 읽기
        // binary rader는 end에서 할게없다
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

void BinaryArch::SetFile(const char *file)
{

    mFilePath = file;
}

Arch &BinaryArch::operator<<(bool &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(char &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(int8_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(int16_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(int32_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(int64_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(uint8_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(uint16_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(uint32_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(uint64_t &value)
{
    return SerializeInterface(&value, sizeof(value));
}



Arch &BinaryArch::operator<<(float &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(double &value)
{
    return SerializeInterface(&value, sizeof(value));
}

Arch &BinaryArch::operator<<(FString &fstring)
{
    return ProcessObject(
        fstring,
        [](Arch &arch, void *instance)
        {
            FString *str = (FString *)instance;

            size_t length = 0;
            arch << length;

            // 그후 문자열
            //  std::vector<uint8_t> buffer(length + 1);

            FBuffer buffer(length + 1);
            // uint8_t *buffer = new uint8_t[length + 1];

            arch << QUAD_SERIALIZEBUFFER(buffer.Data(), length);
            buffer[length] = '\0';

            *str = reinterpret_cast<const char *>(buffer.Data());
        },
        [](Arch &arch, void *instance)
        {
            // 쓰기
            FString *str = (FString *)instance;
            // 먼저 길이
            size_t length = str->size();
            arch << length;

            // 그후 문자열
            arch << QUAD_SERIALIZEBUFFER(const_cast<char *>(str->c_str()), length);
        }

    );
    return *this;
}

void BinaryArch::Serialize(const void *data, size_t size)
{
    mWriter.WriteRaw(data, size);
}

void BinaryArch::DeSerialize(void *data, size_t size)
{
    mReader.ReadRaw(data, size);

    if (mReader.GetFileSize() == mReader.GetReadPointer())
    {
        // EOF 설정
        SetState(Arch::EArchState::eEof);
    }
}

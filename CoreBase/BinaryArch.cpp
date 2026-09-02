#include "BinaryArch.h"
#include "Arch.h"

BinaryArch::BinaryArch(bool isLoadingFlag) : Arch(isLoadingFlag), mCurrPropertyValueSize(0) {}

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

void BinaryArch::Start(uint8_t *buffer, size_t size)
{
    if (GetLoadingFlag())
    {
        Super::Start();
        mReader.StartRead(buffer, size);
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
        bool ret = true;
        if (mFilePath == "")
        {
            mWriter.Close();
        }
        else
        {
            ret = mWriter.Close(mFilePath.c_str());
        }
        if (ret == false)
        {
            // 실패 log
            SetFailState();
        }
    }
}

void BinaryArch::SetFile(const std::filesystem::path &filepath)
{

    mFilePath = filepath;
}

uint8_t *BinaryArch::GetBufferFromMemory()
{

    if (GetLoadingFlag() == false)
    {
        return mWriter.GetBufferPointer();
    }

    return nullptr;
}

size_t BinaryArch::GetBufferSize() const
{

    if (GetLoadingFlag() == false)
    {

        return mWriter.GetBufferSize();
    }
    return 0;
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

Arch &BinaryArch::operator<<(long &value)
{

    return SerializeInterface(&value, sizeof(value));
}
Arch &BinaryArch::operator<<(unsigned long &value)
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

Arch &BinaryArch::operator<<(std::string &str)
{

    size_t size = 0;
    if (GetLoadingFlag())
    {
        operator<<(size);
        str.resize(size);
        DeSerialize(str.data(), size);
    }
    else
    {
        size = str.size();
        operator<<(size);
        Serialize(str.data(), size);
    }
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

void BinaryArch::StartTable(std::string &tableName)
{

    uint32_t tableSize = 0;
    if (GetLoadingFlag())
    {
        uint64_t tableStartPointPos = mReader.GetReadPointer();

        *this << tableSize;
        *this << tableName;

        // table끝 다음데이터로 정확히 이동하기위해 저장
        mRWPointerPosForTableStack.push(tableStartPointPos + tableSize);
    }
    else
    {

        // 저장
        uint64_t currWriterPointerPos = mWriter.GetCurrentWritePointer();
        // table 전체 사이즈
        *this << tableSize;

        mRWPointerPosForTableStack.push(currWriterPointerPos);
        // table name
        *this << tableName;
    }
}
void BinaryArch::EndTable()
{

    if (GetLoadingFlag())
    {
        uint64_t nextTableStartPos = mRWPointerPosForTableStack.top();
        mRWPointerPosForTableStack.pop();

        mReader.SetReadPointer(nextTableStartPos);
    }
    else
    {
        // 저장
        uint64_t currWritePointerPos = mWriter.GetCurrentWritePointer();
        uint64_t tableSizeStartPos = mRWPointerPosForTableStack.top();
        mRWPointerPosForTableStack.pop();
        uint32_t tableSize = currWritePointerPos - tableSizeStartPos;
        mWriter.WriteRaw(&tableSize, sizeof(uint32_t), tableSizeStartPos);
    }
}

void BinaryArch::ReadPropertyHeader(std::string &oPropertyName, std::string &oPropertyType,
                                    uint32_t &oPropertyValueSize)
{
    // propertyName string
    *this << oPropertyName;

    // propertyType string
    *this << oPropertyType;

    *this << oPropertyValueSize;
}

void BinaryArch::WritePropertyHeader(const std::string &propertyName, const std::string &propertyType)
{

    if (!GetLoadingFlag())
    {
        std::string name = propertyName;
        *this << name;

        std::string type = propertyType;
        *this << type;

        uint64_t sizepos = mWriter.GetCurrentWritePointer();
        mRWPointerPosForTableStack.push(sizepos);

        uint32_t dummySize = 0;
        *this << dummySize;
    }
}

void BinaryArch::SkipProperty(uint32_t propertyValueSize)
{
    mReader.SetReadPointer(mReader.GetReadPointer() + propertyValueSize);
}

bool BinaryArch::StartProperty(const std::string &propertyName, const std::string &propertyType)
{
    if (GetLoadingFlag())
    {

        //// 로드시에는
        //// PropertyMap에서 table을 가져와서 해당 propertyName, propertyType에 맞는것이있는지 확인한다.

        // 맞다면 해당 프로퍼티를 준비해둔다.

        ArchPropertyMap &currPropertyMap = mArchPropertyMapStack.top();

        ArchPropertyMap::iterator it = currPropertyMap.find(propertyName);
        if (it == currPropertyMap.end())
        {
            // name이 일치하지않는다.
            return false;
        }

        ArchProperty &archProperty = it->second;

        if (archProperty.mType != propertyType)
        {
            // type이 일치하지않는다.
            return false;
        }

        // name, type이 일치하니 , 준비한다.
        mCurrPropertyValueSize = archProperty.mValueSize;
        mReader.SetReadPointer(archProperty.pValuePointerPos);
    }
    else
    {
        uint64_t currWritePointerPos = mWriter.GetCurrentWritePointer();
        uint32_t propertySize = 0;
        mWriter.Write(propertySize);

        mRWPointerPosForTableStack.push(currWritePointerPos);

        mWriter.Write(propertyName);
        mWriter.Write(propertyType);
        // 이후에 이제 EndProperty호출전까지 외부에서 알아서 저장할것이다.
    }

    return true;
}

void BinaryArch::EndProperty()
{
    if (!GetLoadingFlag())
    {
        // 저장

        uint64_t propertyStartPointerPos = mRWPointerPosForTableStack.top();
        mRWPointerPosForTableStack.pop();
        uint64_t currWritePointerPos = mWriter.GetCurrentWritePointer();
        uint32_t propertySize = currWritePointerPos - propertyStartPointerPos - sizeof(uint32_t);

        mWriter.WriteRaw(&propertySize, sizeof(uint32_t), propertyStartPointerPos);
    }
}

uint32_t BinaryArch::GetPropertySize() const
{
    return mCurrPropertyValueSize;
}

// 다음 시작테이블의 이름을 엿본다.
void BinaryArch::peekTableName(std::string &oTableName)
{
    uint64_t tableStartReadPointerPos = mReader.GetReadPointer();
    uint32_t tableSize = 0;
    mReader.ReadRaw(&tableSize, sizeof(uint32_t));
    mReader.Read(oTableName);

    // 되돌린다.
    mReader.SetReadPointer(tableStartReadPointerPos);
}
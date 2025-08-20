#include "RapidJsonParserImpl.h"
#include "include/rapidjson/encodedstream.h"
#include "include/rapidjson/filereadstream.h"
#include "pch.h"
#include <stdio.h>

namespace JsonParser
{

RapidJsonParserImpl::RapidJsonParserImpl() : ParserValidFlag(false)
{
    pDocument = std::make_unique<rapidjson::Document>();
}

RapidJsonParserImpl::~RapidJsonParserImpl() {}

bool RapidJsonParserImpl::ReadFile(const std::string &jsonFile)
{
    FILE *fp = nullptr;
    if (fopen_s(&fp, jsonFile.c_str(), "rb") != 0)
    {
        SetLastErrorStr(jsonFile + " 파일읽기 실패\n");
        return false;
    }

    char buffer[1024];
    rapidjson::FileReadStream fileReadStream(fp, buffer, sizeof(buffer));

    rapidjson::AutoUTFInputStream<unsigned int, rapidjson::FileReadStream> autoUTFInputStream(fileReadStream);

    pDocument->ParseStream<rapidjson::kParseNoFlags, rapidjson::AutoUTF<unsigned int>>(autoUTFInputStream);

    std::fclose(fp);
    ParserValidFlag = !pDocument->HasParseError();

    if (ParserValidFlag == false)
    {
        // log기록
        SetLastErrorStr(jsonFile + "파일 파싱 실패\n");
        return false;
    }

    mReadPointer.Init(pDocument.get());

    return true;
}

bool RapidJsonParserImpl::IsVaild() const
{

    if (ParserValidFlag == false)
    {
        SetLastErrorStr("파싱이 유효하지않다\n ");
    }

    return ParserValidFlag;
}

bool RapidJsonParserImpl::HasValue(const JsonPath &keyVector) const
{
    return FindValueByKeyPath(keyVector);
}

EParserValueType RapidJsonParserImpl::GetValueType(const JsonPath &keyVector) const
{
    const rapidjson::Value *value = FindValueByKeyPath(keyVector);

    return GetValueType(value);
}

bool RapidJsonParserImpl::GetIntValue(const JsonPath &key, int &oValue) const
{
    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;

    if (!value->IsInt())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::Int));
        return false;
    }

    oValue = value->GetInt();
    return true;
}

bool RapidJsonParserImpl::GetUIntValue(const JsonPath &key, unsigned int &oValue) const
{
    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;
    if (!value->IsUint())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::UInt));
        return false;
    }

    oValue = value->GetUint();
    return true;
}

bool RapidJsonParserImpl::GetFloatValue(const JsonPath &key, float &oValue) const
{

    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;
    if (!value->IsFloat())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::Float));
        return false;
    }

    oValue = value->GetFloat();
    return true;
}

bool RapidJsonParserImpl::GetDoubleValue(const JsonPath &key, double &oValue) const
{
    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;
    if (!value->IsDouble())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::Double));
        return false;
    }

    oValue = value->GetDouble();
    return true;
}

bool RapidJsonParserImpl::GetLongLongValue(const JsonPath &key, long long &oValue) const
{

    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;
    if (!value->IsInt64())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::LongLong));
        return false;
    }

    oValue = value->GetInt64();
    return true;
}

bool RapidJsonParserImpl::GetULongLongValue(const JsonPath &key, unsigned long long &oValue) const
{
    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;

    if (!value->IsUint64())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::ULongLong));
        return false;
    }

    oValue = value->GetUint64();
    return true;
}

bool RapidJsonParserImpl::GetStringValue(const JsonPath &key, std::string &oValue) const
{
    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;

    if (!value->IsString())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::String));
        return false;
    }

    oValue = value->GetString();
    return true;
}

bool RapidJsonParserImpl::GetBoolValue(const JsonPath &key, bool &oValue) const
{

    const rapidjson::Value *value = FindValueByKeyPath(key);
    if (value == nullptr)
        return false;

    if (!value->IsBool())
    {
        // error log 기록
        SetGettingValueErrorLog(key, JsonParser::GetParserValueString(EParserValueType::Bool));
        return false;
    }

    oValue = value->GetBool();
    return true;
}

std::string RapidJsonParserImpl::GetLastError() const
{
    return LastErrorString;
}

bool RapidJsonParserImpl::NextReadPointer()
{

    return mReadPointer.Next();
}

bool RapidJsonParserImpl::BackReadPointer()
{
    return mReadPointer.Back();
}

bool RapidJsonParserImpl::IntoReadPointer()
{
    return mReadPointer.Into();
}

bool RapidJsonParserImpl::OutReadPointer()
{
    return mReadPointer.Out();
}

const char *RapidJsonParserImpl::GetKey() const
{
    return mReadPointer.GetKey();
}

EJsonType RapidJsonParserImpl::GetTypeFromReadPointer() const
{
    return mReadPointer.GetType();
}

bool RapidJsonParserImpl::GetIntValueFromReadPointer(int &oValue) const
{
    return mReadPointer.GetIntValue(oValue);
}

bool RapidJsonParserImpl::GetUIntValueFromReadPointer(unsigned int &oValue) const
{
    return mReadPointer.GetUIntValue(oValue);
}

bool RapidJsonParserImpl::GetFloatValueFromReadPointer(float &oValue) const
{
    return mReadPointer.GetFloatValue(oValue);
}

bool RapidJsonParserImpl::GetDoubleValueFromReadPointer(double &oValue) const
{
    return mReadPointer.GetDoubleValue(oValue);
}

bool RapidJsonParserImpl::GetLongLongValueFromReadPointer(long long &oValue) const
{
    return mReadPointer.GetLongLongValue(oValue);
}

bool RapidJsonParserImpl::GetULongLongValueFromReadPointer(unsigned long long &oValue) const
{
    return mReadPointer.GetULongLongValue(oValue);
}

bool RapidJsonParserImpl::GetStringValueFromReadPointer(std::string &oValue) const
{
    return mReadPointer.GetStringValue(oValue);
}

bool RapidJsonParserImpl::GetBoolValueFromReadPointer(bool &oValue) const
{
    return mReadPointer.GetBoolValue(oValue);
}

size_t RapidJsonParserImpl::GetElementNum() const
{

    return mReadPointer.GetElementNum();
}

EParserValueType RapidJsonParserImpl::GetValueType(const rapidjson::Value *value) const
{
    if (value == nullptr)
        return EParserValueType::None;

    if (value->IsInt())
    {
        return EParserValueType::Int;
    }
    else if (value->IsFloat())
    {
        return EParserValueType::Float;
    }
    else if (value->IsUint())
    {
        return EParserValueType::UInt;
    }
    else if (value->IsBool())
    {
        return EParserValueType::Bool;
    }
    else if (value->IsDouble())
    {
        return EParserValueType::Double;
    }
    else if (value->IsInt64())
    {
        return EParserValueType::LongLong;
    }
    else if (value->IsUint64())
    {
        return EParserValueType::ULongLong;
    }
    else if (value->IsString())
    {

        return EParserValueType::String;
    }
    else
    {
        return EParserValueType::None;
    }
}

const rapidjson::Value *RapidJsonParserImpl::FindValueByKeyPath(const JsonPath &keyPath) const
{
    if (IsVaild() == false)
    {
        return nullptr;
    }

    if (keyPath.size() == 0)
    {
        // error 기록
        return nullptr;
    }
    const rapidjson::Value *value = pDocument.get();

    for (const auto &keyElement : keyPath)
    {
        if (!(value->IsObject() || value->IsArray()))
        {
            // error 기록 object type, array type 아님

            return nullptr;
        }

        if (std::holds_alternative<int>(keyElement))
        {
            if (value->IsObject())
            {
                // error  log
                return nullptr;
            }

            // array
            unsigned int index = std::get<int>(keyElement);

            if (value->GetArray().Capacity() <= index)
            {
                // error log
                return nullptr;
            }

            value = &value->GetArray()[index];
        }
        else
        {
            // object

            std::string str = std::get<std::string>(keyElement);

            rapidjson::Value::ConstMemberIterator it = value->FindMember(str.c_str());

            if (it == value->MemberEnd())
            {
                // Error 기록	그런 멤버가없음
                SetLastErrorStr(str + "멤버가 없습니다.\n");
                return nullptr;
            }
            value = &it->value;
        }
    }

    return value;
}

void RapidJsonParserImpl::SetLastErrorStr(const std::string &log) const
{
    LastErrorString = log;
}

void RapidJsonParserImpl::SetGettingValueErrorLog(const JsonPath &keyVector, const std::string &type) const
{
    std::string keyStr = "";

    for (const auto &keyElement : keyVector)
    {
        if (std::holds_alternative<int>(keyElement))
        {
            int index = std::get<int>(keyElement);
            keyStr += "[" + std::to_string(index) + "]";
        }
        else
        {

            std::string str = std::get<std::string>(keyElement);

            keyStr += "{" + str + "}";
        }
    }

    SetLastErrorStr(keyStr + "의 value타입이 " + type + "이 아닙니다.\n");
}

RapidJsonParserReadPointer::RapidJsonParserReadPointer() : mCurrParentValue(nullptr), mCurrIndex(0) {}

RapidJsonParserReadPointer::~RapidJsonParserReadPointer() {}

void RapidJsonParserReadPointer::Init(rapidjson::Document *document)
{
    mDocument = document;
    mCurrParentValue = mDocument;

    std::stack<std::pair<rapidjson::Value *, int>> initStack;
    mParentStack.swap(initStack);
    mCurrIndex = 0;
}

const char *RapidJsonParserReadPointer::GetKey() const
{

    if (mCurrParentValue->IsObject() == false)
    {
        return "";
    }

    return mCurrParentValue->GetObject().MemberBegin()[mCurrIndex].name.GetString();
}

EJsonType RapidJsonParserReadPointer::GetType() const
{

    mCurrParentValue->GetType();

    rapidjson::Value *currValue = nullptr;
    if (mCurrParentValue->GetType() == rapidjson::kObjectType)
    {
        currValue = &mCurrParentValue->GetObject().MemberBegin()[mCurrIndex].value;
    }
    else if (mCurrParentValue->GetType() == rapidjson::kArrayType)
    {
        currValue = &mCurrParentValue->GetArray()[mCurrIndex];
    }

    if (currValue->IsInt())
    {
        return EJsonType::eInt;
    }

    if (currValue->IsUint())
    {

        return EJsonType::eUInt;
    }

    if (currValue->IsInt64())
    {
        return EJsonType::eULongLong;
    }

    if (currValue->IsUint64())
    {
        return EJsonType::eULongLong;
    }

    if (currValue->IsDouble())
    {
        return EJsonType::eDouble;
    }

    if (currValue->IsFloat())
    {
        return EJsonType::eFloat;
    }

    if (currValue->IsArray())
    {
        return EJsonType::eArray;
    }

    if (currValue->IsObject())
    {
        return EJsonType::eArray;
    }
}

bool RapidJsonParserReadPointer::GetIntValue(int &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsInt())
    {
        oValue = value->GetInt();
        return true;
    }

    return false;
}

bool RapidJsonParserReadPointer::GetUIntValue(unsigned int &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsUint())
    {
        oValue = value->GetUint();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetFloatValue(float &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsFloat())
    {
        oValue = value->GetFloat();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetDoubleValue(double &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsDouble())
    {
        oValue = value->GetDouble();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetLongLongValue(long long &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsInt64())
    {
        oValue = value->GetInt64();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetULongLongValue(unsigned long long &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsUint64())
    {
        oValue = value->GetUint64();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetStringValue(std::string &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsString())
    {
        oValue = value->GetString();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::GetBoolValue(bool &oValue) const
{
    rapidjson::Value *value = GetCurrentChildValue();

    if (value->IsBool())
    {
        oValue = value->GetBool();
        return true;
    }
    return false;
}

bool RapidJsonParserReadPointer::Next()
{

    if (mCurrParentValue->IsObject())
    {

        if (mCurrParentValue->GetObject().MemberCount() <= (mCurrIndex + 1))
        {
            // 더이상 형제가없다.
            return false;
        }
        ++mCurrIndex;
    }
    else if (mCurrParentValue->IsArray())
    {

        if (mCurrParentValue->GetArray().Size() <= (mCurrIndex + 1))
        {
            // 더이상 형제가없다.
            return false;
        }
        ++mCurrIndex;
    }

    return true;
}

bool RapidJsonParserReadPointer::Back()
{

    if (mCurrIndex == 0)
        return false;
    mCurrIndex--;

    return true;
}

bool RapidJsonParserReadPointer::Into()
{

    rapidjson::Value *newCurrParentValue = nullptr;
    if (mCurrParentValue->IsObject())
    {
        newCurrParentValue = &mCurrParentValue->GetObject().MemberBegin()[mCurrIndex].value;
    }
    else if (mCurrParentValue->IsArray())
    {
        newCurrParentValue = &mCurrParentValue->GetArray()[mCurrIndex];
    }

    if (!newCurrParentValue->IsArray() && !newCurrParentValue->IsObject())
        return false;

    // 이전부모보관
    mParentStack.push({mCurrParentValue, mCurrIndex});

    mCurrParentValue = newCurrParentValue;
    mCurrIndex = 0;

    return true;
}

bool RapidJsonParserReadPointer::Out()
{

    if (mParentStack.empty())
        return false;

    mCurrParentValue = mParentStack.top().first;
    mCurrIndex = mParentStack.top().second;

    mParentStack.pop();

    return true;
}

rapidjson::Value *RapidJsonParserReadPointer::GetCurrentChildValue() const
{

    if (mCurrParentValue->IsObject())
    {
        return &mCurrParentValue->GetObject().MemberBegin()[mCurrIndex].value;
    }
    else if (mCurrParentValue->IsArray())
    {

        return &mCurrParentValue->GetArray()[mCurrIndex];
    }
}

size_t RapidJsonParserReadPointer::GetElementNum() const
{

    rapidjson::Value *currentValue = nullptr;
    mCurrParentValue[mCurrIndex];
    if (mCurrParentValue->IsObject() == true)
    {
        currentValue = &mCurrParentValue->GetObject().MemberBegin()[mCurrIndex].value;
    }
    else if (mCurrParentValue->IsArray())
    {

        currentValue = &mCurrParentValue->GetArray()[mCurrIndex];
    }
    else
    {
        return 0;
    }

    if (currentValue->IsObject() == true)
    {

        return currentValue->MemberCount();
    }
    else if (currentValue->IsArray())
    {
        return currentValue->Size();
    }
}

} // namespace JsonParser
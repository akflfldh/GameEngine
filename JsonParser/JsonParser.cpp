
#include "Parser/JsonParser.h"
#include"rapidjson/stringbuffer.h"
#include"rapidjson/writer.h"
#include"rapidjson/filewritestream.h"
#include"rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h" // PrettyWriter 사용을 위한 헤더
//#include"ObjectSmartPointer.h"
//#include"TextureSmartPointer.h"
#include<fstream>


Quad::JsonParser::JsonParser()
{
    mDocument.SetArray();
}


Quad::JsonParser* Quad::JsonParser::GetInstance()
{

    static JsonParser jsonParser;

    return &jsonParser;
}

bool Quad::JsonParser::ReadFile(const std::string& fileName)
{
    FILE* fp;
    int ret =fopen_s(&fp,fileName.c_str(), "rb");
    if (ret != 0)
    {
        
        return false;
    }
    char buffer[65365];

    FileReadStream is(fp, buffer, sizeof(buffer));

    auto instance = GetInstance();
    instance->mDocument.ParseStream(is);
    fclose(fp);

    return true;
}

unsigned int Quad::JsonParser::GetObjectNum()
{
    auto instance = GetInstance();
    return instance->mDocument.Size();
}

void Quad::JsonParser::SetCurrentIndex(unsigned int index)
{
    auto instance = GetInstance();
    instance->mCurrentReadObjectIndex = index;
}

unsigned int Quad::JsonParser::GetCurrentIndex()
{
    auto instance = GetInstance();
    return instance->mCurrentReadObjectIndex;
    
}

void Quad::JsonParser::IncrementCurrentIndex()
{
    auto instance = GetInstance();
    instance->mCurrentReadObjectIndex++;

}

const std::string Quad::JsonParser::GetClassName(unsigned int index)
{
    auto instance = GetInstance();
    std::string className = instance->mDocument[index].FindMember("ClassName")->value.GetString();
    return className;
}

void Quad::JsonParser::ReadStart()
{
    auto instance = GetInstance();
    instance->mCurrentReadObjectIndex = 0;
}

void Quad::JsonParser::Read(const std::string& key, std::string& member)
{
    Value::MemberIterator  itr = GetMemberIterator(key);
    member = itr->value.GetString();
}

void Quad::JsonParser::Read(const std::string& key, int& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member = itr->value.GetInt();
}

void Quad::JsonParser::Read(const std::string& key, unsigned long long& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member = itr->value.GetUint64();
}

void Quad::JsonParser::Read(const std::string& key, unsigned int& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member = itr->value.GetUint();

}

void Quad::JsonParser::Read(const std::string& key, float& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member = itr->value.GetFloat();

}

//void Quad::JsonParser::Read(const std::string& key, ObjectSmartPointer& member)
//{
//    Value::MemberIterator itr = GetMemberIterator(key);
//    unsigned long long id =  itr->value.GetUint64();
//    member = id;
//}
//
//void Quad::JsonParser::Read(const std::string& key, TextureSmartPointer& member)
//{
//    Value::MemberIterator itr = GetMemberIterator(key);
//    unsigned long long id = itr->value.GetUint64();
//    member = id;
//
//}

void Quad::JsonParser::Read(const std::string& key, DirectX::XMFLOAT2& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member.x = itr->value[0].GetFloat();
    member.y = itr->value[1].GetFloat();
}

void Quad::JsonParser::Read(const std::string& key, DirectX::XMFLOAT3& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member.x = itr->value[0].GetFloat();
    member.y = itr->value[1].GetFloat();
    member.z = itr->value[2].GetFloat();


}

void Quad::JsonParser::Read(const std::string& key, DirectX::XMFLOAT4& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    member.x = itr->value[0].GetFloat();
    member.y = itr->value[1].GetFloat();
    member.z = itr->value[2].GetFloat();
    member.w = itr->value[3].GetFloat();

}

void Quad::JsonParser::Read(const std::string& key, DirectX::XMFLOAT4X4& member)
{
    Value::MemberIterator itr = GetMemberIterator(key);
   
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            member.m[row][col] = itr->value[4 * row + col].GetFloat();
        }
    }

}

bool Quad::JsonParser::ReadBool(const std::string& key)
{
    Value::MemberIterator itr = GetMemberIterator(key);
    return itr->value.GetBool();
}

rapidjson::Value::ConstArray Quad::JsonParser::ReadArray(const std::string& key)
{

    Value::ConstMemberIterator  itr = GetMemberIterator(key);
    return itr->value.GetArray();

}









void Quad::JsonParser::StartWrite()
{
    auto instance = GetInstance();
   // Document newDocument(kArrayType);
  //  instance->mDocument.Swap(newDocument);
    instance->mDocument.SetArray();
 /*   GenericMemberIterator v = newDocument[0].FindMember("as");
    v->value.IsArray();*/


}

void Quad::JsonParser::StartWriteObject()
{
    auto instance = GetInstance();    
    instance->mDocument.PushBack(Value(kObjectType), instance->mDocument.GetAllocator());

}

void Quad::JsonParser::Write(const std::string& key, const std::string& value)
{
    auto instance = GetInstance();
    Document& document = instance->mDocument;
    document[document.Size() - 1].AddMember(Value(key.c_str(), document.GetAllocator()), 
        Value(value.c_str(), document.GetAllocator()), document.GetAllocator());

    

}

//void Quad::JsonParser::Write(const std::string& key, int value)
//{
//    WriteMember(key, Value(value));
//}
//
//void Quad::JsonParser::Write(const std::string& key, double value)
//{
//    WriteMember(key, Value(value));
//}
//
//void Quad::JsonParser::Write(const std::string& key, unsigned long long value)
//{
//    WriteMember(key, Value(value));
//}
//
//
//void Quad::JsonParser::Write(const std::string& key, unsigned int value)
//{
//    WriteMember(key, Value(value));
//}
//
//void Quad::JsonParser::Write(const std::string& key, float value)
//{
//    WriteMember(key, Value(value));
//
//    
//}
//
//void Quad::JsonParser::Write(const std::string& key, bool value)
//{
//    WriteMember(key, Value(value));
//}

void Quad::JsonParser::Write(const std::string& key, const char* value)
{
    auto instance = GetInstance();
    Value va(value, instance->mDocument.GetAllocator());
    WriteMember(key, va);

}

//void Quad::JsonParser::Write(const std::string& key, const ObjectSmartPointer& member)
//{
//    unsigned long long id =  member.GetObjectID();
//    WriteMember(key, Value(id));
//}
//
//void Quad::JsonParser::Write(const std::string& key, const TextureSmartPointer& value)
//{
//    unsigned long long id = value.GetObjectID();
//    WriteMember(key, Value(id));
//
//}

void Quad::JsonParser::Write(const std::string& key, const DirectX::XMFLOAT3& value)
{
    auto instance = GetInstance();
    Document& document = instance->mDocument;
    Value float3(kArrayType);
    float3.PushBack(value.x, document.GetAllocator());
    float3.PushBack(value.y, document.GetAllocator());
    float3.PushBack(value.z, document.GetAllocator());

    WriteMember(key, float3);




}



void Quad::JsonParser::Write(const std::string& key, const DirectX::XMFLOAT4& value)
{
    auto instance = GetInstance();
    Document& document = instance->mDocument;
    Value float4(kArrayType);

    float4.PushBack(value.x, document.GetAllocator());
    float4.PushBack(value.y, document.GetAllocator());
    float4.PushBack(value.z, document.GetAllocator());
    float4.PushBack(value.w, document.GetAllocator());
    WriteMember(key, float4);
}

void Quad::JsonParser::Write(const std::string& key, const DirectX::XMFLOAT2& value)
{

    auto instance = GetInstance();
    Document& document = instance->mDocument;
    Value float2(kArrayType);

    float2.PushBack(value.x, document.GetAllocator());
    float2.PushBack(value.y, document.GetAllocator());

    WriteMember(key, float2);


    

}

void Quad::JsonParser::Write(const std::string& key, const DirectX::XMFLOAT4X4& value)
{
    auto instance = GetInstance();
    Document& document = instance->mDocument;
    Value float4x4(kArrayType);

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            float4x4.PushBack(value.m[row][col], document.GetAllocator());
        }
    }
    WriteMember(key, float4x4);
}

void Quad::JsonParser::Save(const std::string& fileName)
{
    FILE* fp;
    fopen_s(&fp,fileName.c_str(), "wb");
    char buffer[65356];
    FileWriteStream os(fp, buffer, sizeof(buffer));
    PrettyWriter<FileWriteStream> writer(os);

    auto instance = GetInstance();
    instance->mDocument.Accept(writer);

    fclose(fp);
}

Quad::Value::MemberIterator Quad::JsonParser::GetMemberIterator(const std::string& key)
{
    auto instance = GetInstance();
    Value::MemberIterator itr = instance->mDocument[instance->mCurrentReadObjectIndex].FindMember(key.c_str());
    return itr;
}

void Quad::JsonParser::WriteMember(const std::string& key, const Value &  value)
{
    auto instance = GetInstance();
    Document& document = instance->mDocument;
    document[document.Size() - 1].AddMember(Value(key.c_str(), document.GetAllocator()), const_cast<Value &>(value), document.GetAllocator());

}

int Quad::JsonParser::GetValue(Value& value, int)
{
    
    return value.GetInt();
}

unsigned int Quad::JsonParser::GetValue(Value& value, unsigned int)
{
    return value.GetUint();
}

double Quad::JsonParser::GetValue(Value& value, double)
{
    return value.GetDouble();
}

float Quad::JsonParser::GetValue(Value& value, float)
{
    return value.GetFloat();
}

unsigned long long Quad::JsonParser::GetValue(Value& value, unsigned long long)
{
    return value.GetUint64();
}



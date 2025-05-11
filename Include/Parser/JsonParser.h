#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include"rapidjson/document.h"
#include"rapidjson/stringbuffer.h"
#include"rapidjson/writer.h"
#include"rapidjson/filewritestream.h"
#include"rapidjson/filereadstream.h"
#include<string>
#include"DirectXMath.h"
#include <type_traits>




#ifdef JSONPARSER_LIB_EXPORT
#define JSONPARSER_LIB_API __declspec(dllexport)
#else
#define JSONPARSER_LIB_API __declspec(dllimport)
#endif





namespace Quad
{


	using namespace rapidjson;
//	class ObjectSmartPointer;
//	class TextureSmartPointer;
	
	class JSONPARSER_LIB_API JsonParser
	{

	public:

		~JsonParser()=default;

		static JsonParser* GetInstance();

		
		static bool ReadFile(const std::string& fileName);
		

		static unsigned int GetObjectNum() ;
		static void SetCurrentIndex(unsigned int index);
		static unsigned int GetCurrentIndex() ;
		static void IncrementCurrentIndex();
		static const std::string GetClassName(unsigned int index);


		static void ReadStart();

		static void Read(const std::string& key, std::string& member); 
		static void Read(const std::string& key, int& member); 
		static void Read(const std::string& key, unsigned long long& memeber);
		static void Read(const std::string& key, unsigned int& memeber);
		static void Read(const std::string& key, float & member);
	//	static void Read(const std::string& key, ObjectSmartPointer& member);
	//	static void Read(const std::string& key, TextureSmartPointer& member);
		static void Read(const std::string& key, DirectX::XMFLOAT2& member);
		static void Read(const std::string& key, DirectX::XMFLOAT3& member);
		static void Read(const std::string& key, DirectX::XMFLOAT4& member);
		static void Read(const std::string& key, DirectX::XMFLOAT4X4& member);

		//현재currentIndex의 해당하는 object의 속성중 array속성에값을 읽는것
		static rapidjson::Value::ConstArray ReadArray(const std::string& key);
		

		//static void ReadObjectProperty(const rapidjson::Value& object, const std::string& key);




		//컨테이너공간이존재해야한다.
		template<typename Iter>
		static void Read(const std::string& key, Iter begin, Iter end);

		static bool ReadBool(const std::string& key);


		//새로운 파일에 작성하기시작한다면 StartWrite를 먼저호출할것이다.
		static void StartWrite();
		static void StartWriteObject();
		static void Write(const std::string& key, const std::string& value);
		//static void Write(const std::string& key, int value);
		//static void Write(const std::string& key, double value);
		//static void Write(const std::string& key, unsigned long long value);
		//static void Write(const std::string& key, unsigned int value);
		//static void Write(const std::string& key, float value);
		//static void Write(const std::string& key, bool flag);

		template<typename T>
		static void Write(const std::string& key, T value);

		static void Write(const std::string& key, const char* value);
		//static void Write(const std::string& key, const ObjectSmartPointer& value);
		//static void Write(const std::string& key, const TextureSmartPointer& value);
		static void Write(const std::string& key, const DirectX::XMFLOAT3& value);
		static void Write(const std::string& key, const DirectX::XMFLOAT4& value);
		static void Write(const std::string& key, const DirectX::XMFLOAT2& value);
		static void Write(const std::string& key, const DirectX::XMFLOAT4X4& value);

		template<typename Iter>
		static void Write(const std::string& key,  Iter begin, Iter end);


		//현재까지의 내용을 파일에 저장
		static void Save(const std::string& fileName);





	private:


		JsonParser();
		static Value::MemberIterator  GetMemberIterator(const std::string& key);
		static void WriteMember(const std::string& key, const Value & value);

	
		static int GetValue(Value& value,int);
		static unsigned int GetValue(Value& value,unsigned int);
		static double GetValue(Value& value,double);
		static float GetValue(Value& value, float);
		static unsigned long long GetValue(Value& value, unsigned long long);


		Document mDocument;
		int mCurrentReadObjectIndex = 0;



	};

	template<typename Iter>
	inline void JsonParser::Read(const std::string& key, Iter begin, Iter end)
	{
		typedef typename Iter::value_type elementType;

		if (std::is_fundamental<elementType>::value)
		{
			auto instance = GetInstance();
			Document& document = instance->mDocument;
			

			GenericMemberIterator memberIt = document[instance->mCurrentReadObjectIndex].FindMember(key.c_str());
			if (!memberIt->value.IsArray())
				return;

			int index = 0;


			for (auto& it = begin; it != end; ++it)
			{
				
				Value& value = memberIt->value.GetArray()[index++];
				*it = GetValue(value, elementType{});

			}
		}
	}

	template<typename T>
	inline void JsonParser::Write(const std::string& key, T va)
	{

		if(std::is_fundamental<T>::value)
		{ 
			WriteMember(key, Value(va));
		}

	}

	template<typename Iter>
	inline void JsonParser::Write(const std::string& key, Iter begin, Iter end)
	{

		if (std::is_fundamental<typename Iter::value_type>::value)
		{
			auto instance = GetInstance();
			Document& document = instance->mDocument;
			Value arrayValue(kArrayType);


			for (auto& it = begin; it != end; ++it)
			{
				arrayValue.PushBack(*it,document.GetAllocator());
			}

			document[document.Size() - 1].AddMember(Value(key.c_str(), document.GetAllocator()), arrayValue, document.GetAllocator());
		}
	}

	

}

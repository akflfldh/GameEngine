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
#include<stack>




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
		
		//최상위 오브젝트들의 개수
		static unsigned int GetObjectNum() ;
		static void SetCurrentIndex(unsigned int index);
		static unsigned int GetCurrentIndex() ;
		static void IncrementCurrentIndex();



		static void ReadStart();


		//currElementNode의 value type이 object인경우에만 유효하다. 사용자가 올바르게호출할것
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

	
		
		//currElementNode value type이 array인경우에 유효
		//현재 index에해당하는 value값을 읽는다.
		static void Read(std::string & value);
		static void Read(int & value);
		static void Read(unsigned long long & value);
		static void Read(unsigned int & value);
		static void Read(float & value);
		


		//현재 ElementNode 타입이 Object일때 멤버개수리턴
		static size_t GetMemeberNum();

		//현재 오브젝트,또는 배열의 현재 Index에 해당하는 요소의 타입
		static rapidjson::Type GetCurrIndexValueType();
		
		//현재 ElementNode타입이 object일때 유효
		static std::string GetCurrMemberKey();


		//현재 object의 member or array의 value가 object,array 타입이면 안으로 들어간다. 
		static void DescendIntoObjectOrArray();
		//key에 해당하는 object memeber로 들어간다.
		static void DescendIntoObjectOrArray(const std::string & key);


		//이전 object or array로 되돌아간다.
		static void AscendOutofObjectOrArray();
		


		//컨테이너공간이존재해야한다.
		template<typename Iter>
		static void Read(const std::string& key, Iter begin, Iter end);

		static bool ReadBool(const std::string& key);




		//새로운 파일에 작성하기시작한다면 StartWrite를 먼저호출할것이다.
		static void StartWrite();	//object,array 로 시작하도록 설정할수있게 하자.


		//현재 ElementNode의 타입이 object인경우 object 타입의 새로운 member추가하는 함수 . 그 object로 들어간다. 따라서 
		static void StartWriteObject(const std::string& key); //빠져나올려면 AscendOutOfObejctOrArray호출

		//ElementNode 타입이 array의 경우에 object타입의 요소를 추가하는 함수 , 그 object로 들어간다. 따라서 
		static void StartWriteObject(); //빠져나올려면 AscendOutOfObejctOrArray호출



		//ElementNode의 타입이 object라고 가정하고 member를 추가한다.
		static void Write(const std::string& key, const std::string& value);

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
		static Value* GetCurrentArrayElementValue();
		static void WriteMember(const std::string& key, Value & value);

	
		static int GetValue(Value& value,int);
		static unsigned int GetValue(Value& value,unsigned int);
		static double GetValue(Value& value,double);
		static float GetValue(Value& value, float);
		static unsigned long long GetValue(Value& value, unsigned long long);


		Document mDocument;
		//최상위 오브젝트의 인덱스값
		//int mCurrentReadObjectIndex = 0;



		//array , object에대해서만 다룬다.계층구조처리를위해서
		struct ElementNode
		{
			rapidjson::Value * mCurrentElementValue;			//array 혹은 object이다.
			int mCurrentIndex;						//array의 index이거나, object의 memberIndex이다.
		};
		
		ElementNode mCurrentElementNode;
		std::stack<ElementNode> mElementNodeStack;


	};








	template<typename Iter>
	inline void JsonParser::Read(const std::string& key, Iter begin, Iter end)
	{
		typedef typename Iter::value_type elementType;

		if (std::is_fundamental<elementType>::value)
		{
			auto instance = GetInstance();
			Document& document = instance->mDocument;
			

			GenericMemberIterator memberIt = instance->mCurrentElementNode.mCurrentElementValue->FindMember(key.c_str());
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
			Value value(va);
			WriteMember(key, value);
		}

	}

	template<typename Iter>//현재 ElementNode타입이 object일때 유효
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



			if (instance->mCurrentElementNode.mCurrentElementValue->IsObject())
			{
				instance->mCurrentElementNode.mCurrentElementValue->AddMember(Value(key.c_str(), document.GetAllocator()), arrayValue, document.GetAllocator());
			}
		}
	}

	

}

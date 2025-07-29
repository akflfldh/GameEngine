#pragma once


#include"IParserImpl.h"

#include"include/rapidjson/document.h"
#include<memory>
#include<stack>
namespace JsonParser
{
	
	class RapidJsonParserReadPointer
	{
	public:
		RapidJsonParserReadPointer();
		~RapidJsonParserReadPointer();


		void Init(rapidjson::Document* document);


		//현재부모가 Object타입일때 올바르게 현재자식의 키값을 반환
		//그렇지않으면 "" 리턴
		const char* GetKey() const;
		

		//현재 가리키는것의 타입반환
		EJsonType GetType() const;

		
		bool GetIntValue(int& oValue) const ;
		bool GetUIntValue(unsigned int& oValue) const;
		bool GetFloatValue(float& oValue) const ;
		bool GetDoubleValue(double& oValue) const;
		bool GetLongLongValue(long long& oValue) const;
		bool GetULongLongValue(unsigned long long& oValue) const;
		bool GetStringValue(std::string& oValue) const;
		bool GetBoolValue(bool& oValue) const ;


		//다음 형제 value로 
		bool Next();
		//이전 형제value로
		bool Back();

		//자식 value로(자식이, array,object타입이어야한다)
		bool Into();

		//부모 value로
		bool Out();
	private:

		EJsonType ConvertRapidJsonType(rapidjson::Value* value);
		rapidjson::Value* GetCurrentChildValue() const ;

	private:
		rapidjson::Value* mCurrParentValue;
		int mCurrIndex;
		//현재 가리키는 자식의 인덱스(array, object)

		//object의 경우 currValue는 key,value중 value를 가리킬것이다.
		std::stack<std::pair<rapidjson::Value*,int>> mParentStack;

		rapidjson::Document* mDocument;





	};

	
	class RapidJsonParserImpl:public IParserImpl
	{
	public:
		RapidJsonParserImpl();
		virtual ~RapidJsonParserImpl();

		virtual bool ReadFile(const std::string& jsonFile) override;
		virtual bool IsVaild() const override; // 현재 파서가 유효하여 value를 얻는게 가능한지여부

		virtual bool HasValue(const JsonPath& path) const override;

		virtual EParserValueType GetValueType(const JsonPath& path) const override;

		virtual bool GetIntValue(const JsonPath& path, int& oValue) const override;
		virtual bool GetUIntValue(const JsonPath& path, unsigned int& oValue) const override;
		virtual bool GetFloatValue(const JsonPath& path, float& oValue) const override;
		virtual bool GetDoubleValue(const JsonPath& path, double& oValue) const override;
		virtual bool GetLongLongValue(const JsonPath& path, long long& oValue) const override;
		virtual bool GetULongLongValue(const JsonPath& path, unsigned long long& oValue) const override;
		virtual bool GetStringValue(const JsonPath& path, std::string& oValue) const override;
		virtual bool GetBoolValue(const JsonPath& path, bool& oValue) const override;
		// 바로 전동작이 실패했을경우 그 실패에대한 정보를 문자열로 리턴해준다.
		virtual std::string GetLastError() const override;








		//read pointer 

		//다음 형제 value로 
		virtual bool NextReadPointer() override;
		//이전 형제value로
		virtual bool BackReadPointer() override;

		//자식 value로(자식이, array,object타입이어야한다)
		virtual bool IntoReadPointer() override;

		//부모 value로
		virtual bool OutReadPointer() override;

		//현재부모가 Object타입일때 올바르게 현재자식의 키값을 반환
		//그렇지않으면 "" 리턴
		virtual const char* GetKey() const override;
		//현재 가리키는것의 타입반환
		virtual EJsonType GetTypeFromReadPointer() const override;
		virtual bool GetIntValueFromReadPointer(int& oValue) const override;
		virtual bool GetUIntValueFromReadPointer(unsigned int& oValue) const override;
		virtual bool GetFloatValueFromReadPointer(float& oValue) const override;
		virtual bool GetDoubleValueFromReadPointer(double& oValue) const override;
		virtual bool GetLongLongValueFromReadPointer(long long& oValue) const override;
		virtual bool GetULongLongValueFromReadPointer(unsigned long long& oValue) const override;
		virtual bool GetStringValueFromReadPointer(std::string& oValue) const override;
		virtual bool GetBoolValueFromReadPointer(bool& oValue) const override;





	private:

		EParserValueType		GetValueType(const rapidjson::Value* value) const;
		const rapidjson::Value* FindValueByKeyPath(const JsonPath& path) const;
		
		void SetLastErrorStr(const std::string& log) const;
		void SetGettingValueErrorLog(const JsonPath& path, const std::string& type) const;

	private:
		std::unique_ptr<rapidjson::Document> pDocument;
		mutable std::string					 LastErrorString;
		bool								 ParserValidFlag;

		RapidJsonParserReadPointer mReadPointer;


	};

} // namespace JsonParser
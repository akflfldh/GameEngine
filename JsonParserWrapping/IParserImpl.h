#pragma once


#include"JsonParserType.h"
#include<string>
#include<vector>
#include<unordered_map>
namespace JsonParser
{

	class IParserImpl
	{

	public:
		IParserImpl()=default;
		virtual ~IParserImpl() = 0;

		// 파일을 읽는다.
		virtual bool ReadFile(const std::string& jsonFile) = 0;
		virtual bool IsVaild() const = 0; // 현재 파서가 유효하여 value를 얻는게 가능한지여부

		virtual bool HasValue(const JsonPath& path) const = 0;

		virtual EParserValueType GetValueType(const JsonPath& path) const = 0;

		virtual bool GetIntValue(const JsonPath& key, int& oValue) const = 0;
		virtual bool GetUIntValue(const JsonPath& key, unsigned int& oValue) const = 0;
		virtual bool GetFloatValue(const JsonPath& key, float& oValue) const = 0;
		virtual bool GetDoubleValue(const JsonPath& key, double& oValue) const = 0;
		virtual bool GetLongLongValue(const JsonPath& key, long long& oValue) const = 0;
		virtual bool GetULongLongValue(const JsonPath& key, unsigned long long& oValue) const = 0;
		virtual bool GetStringValue(const JsonPath& key, std::string& oValue) const = 0;
		virtual bool GetBoolValue(const JsonPath& key, bool & oValue) const = 0;


		// 바로 전동작이 실패했을경우 그 실패에대한 정보를 문자열로 리턴해준다.
		virtual std::string GetLastError() const = 0;
		



		//read pointer 
	//다음 형제 value로 
		virtual bool NextReadPointer() =0;
		//이전 형제value로
		virtual bool BackReadPointer() =0;

		//자식 value로(자식이, array,object타입이어야한다)
		virtual bool IntoReadPointer() =0;

		//부모 value로
		virtual bool OutReadPointer() = 0;

		//현재부모가 Object타입일때 올바르게 현재자식의 키값을 반환
		//그렇지않으면 "" 리턴
		virtual const char* GetKey() const = 0;
		//현재 가리키는것의 타입반환
		virtual EJsonType GetTypeFromReadPointer() const = 0;
		virtual bool GetIntValueFromReadPointer(int& oValue) const = 0;
		virtual bool GetUIntValueFromReadPointer(unsigned int& oValue) const = 0;
		virtual bool GetFloatValueFromReadPointer(float& oValue) const = 0;
		virtual bool GetDoubleValueFromReadPointer(double& oValue) const = 0;
		virtual bool GetLongLongValueFromReadPointer(long long& oValue) const = 0;
		virtual bool GetULongLongValueFromReadPointer(unsigned long long& oValue) const = 0;
		virtual bool GetStringValueFromReadPointer(std::string& oValue) const = 0;
		virtual bool GetBoolValueFromReadPointer(bool& oValue) const = 0;

	private:

	};
} // namespace JsonParser

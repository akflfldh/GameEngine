#pragma once
#include <vector>
#include<variant>
#include<string>
namespace JsonParser
{
	enum class EParserValueType
	{
		None = 0,
		Int,
		UInt,
		Float,
		Double,
		LongLong,
		ULongLong,
		Bool,
		String,
	};

	

	enum class EJsonType
	{
		eInt,
		eUInt,
		eFloat,
		eDouble,
		eLongLong,
		eULongLong,
		eBool,
		eString,
		eArray,
		eObject
	};





	const char * GetParserValueString(EParserValueType type);

	//string은 object멤버, int를 array index이다 .
	// 둘중 하나를 가진다.
	using JsonKey = std::variant<std::string, int>;
	using JsonPath = std::vector<JsonKey>;

}
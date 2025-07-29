#include "pch.h"
#include "JsonParser.h"
#include "RapidJsonParserImpl.h"



namespace JsonParser
{

	Parser::Parser()
	{
		pImpl = std::make_unique<RapidJsonParserImpl>();
	}

	Parser::~Parser()
	{
	}

	bool Parser::ReadFile(const std::string& jsonFile)
	{
		return pImpl->ReadFile(jsonFile);
	}

	bool Parser::IsVaild() const
	{
		return pImpl->IsVaild();
	}

	bool Parser::HasValue(const JsonPath& key) const
	{
		return pImpl->HasValue(key);
	}

	EParserValueType Parser::GetValueType(const JsonPath& key) const
	{
		return pImpl->GetValueType(key);
	}

	std::string Parser::GetLastError() const
	{
		if (!pImpl)
		{
			LastError = "Parser에 올바른 Impl이 없습니다.";
			return LastError;
		}

		LastError = pImpl->GetLastError();
		return LastError;
	}


	template <typename T>
	inline bool Parser::GetValue(const JsonPath& key, T& oValue) const
	{
		
		if (!pImpl)
		{
			return false;
		}


		if constexpr (std::is_same_v<T, int>)
		{
			return pImpl->GetIntValue(key, oValue);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return pImpl->GetFloatValue(key,oValue);

		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return pImpl->GetDoubleValue(key, oValue);
		}
		else if constexpr (std::is_same_v<T, unsigned int>)
		{
			return  pImpl->GetUIntValue(key,oValue);
		}
		else if constexpr (std::is_same_v<T, unsigned long long>)
		{
			return pImpl->GetULongLongValue(key,oValue);
		}
		else if constexpr (std::is_same_v<T, long long>)
		{
			return pImpl->GetLongLongValue(key, oValue);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return pImpl->GetBoolValue(key, oValue);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return pImpl->GetStringValue(key, oValue);

		}else
		{	

			//아직 구현안한 타입
			return false;
		}


	}

	template<typename T>
	bool Parser::GetValueFromReadPointer(T& oValue) const
	{

		if constexpr (std::is_same_v<T, int>)
		{
			return pImpl->GetIntValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, unsigned int>)
		{
			return pImpl->GetUIntValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return pImpl->GetFloatValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return pImpl->GetDoubleValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, long long>)
		{
			return pImpl->GetLongLongValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, unsigned long long>)
		{
			return pImpl->GetULongLongValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return pImpl->GetStringValueFromReadPointer(oValue);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return pImpl->GetBoolValueFromReadPointer(oValue);
		}

		// 지원하지 않는 타입
		return false;

	}



	bool Parser::NextReadPointer()
	{
		return pImpl->NextReadPointer();
	}

	bool Parser::BackReadPointer()
	{
		return pImpl->BackReadPointer();
	}

	bool Parser::IntoReadPointer()
	{
		return pImpl->IntoReadPointer();
	}

	//부모 value로
	bool Parser::OutReadPointer()
	{
		return pImpl->OutReadPointer();
	}

	const char* Parser::GetKeyFromReadPointer() const
	{
		return pImpl->GetKey();
	}

	EJsonType Parser::GetTypeFromReadPointer() const
	{
		return pImpl->GetTypeFromReadPointer();
	}











	template bool Parser::GetValue<int>(const JsonPath& key, int & oValue) const;
	template bool Parser::GetValue<float>(const JsonPath& key, float& oValue) const;
	template bool Parser::GetValue<double>(const JsonPath& key, double& oValue) const;
	template bool Parser::GetValue<unsigned int>(const JsonPath& key, unsigned int& oValue) const;
	template bool Parser::GetValue<unsigned long long>(const JsonPath& key, unsigned long long& oValue) const;
	template bool Parser::GetValue<bool>(const JsonPath& key, bool& oValue) const;
	template bool Parser::GetValue<std::string>(const JsonPath& key, std::string& oValue) const;





	template bool Parser::GetValueFromReadPointer<int>(int& oValue) const;
	template bool Parser::GetValueFromReadPointer<unsigned int>(unsigned int& oValue) const;
	template bool Parser::GetValueFromReadPointer<float>(float& oValue) const;
	template bool Parser::GetValueFromReadPointer<double>(double& oValue) const;
	template bool Parser::GetValueFromReadPointer<long long>(long long& oValue) const;
	template bool Parser::GetValueFromReadPointer<unsigned long long>(unsigned long long& oValue) const;
	template bool Parser::GetValueFromReadPointer<std::string>(std::string& oValue) const;
	template bool Parser::GetValueFromReadPointer<bool>(bool& oValue) const;

} // namespace JsonParser
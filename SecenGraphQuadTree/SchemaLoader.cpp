//#include "SchemaLoader.h"
//#include<fstream>
//#include"RawResourceType.h">
//#include<regex>
//
//
//
//
////한 파일안에 하나의 타입만 있다고 가정하자 일단은
//bool Quad::SchemaLoader::Load(const std::wstring& filePath, RawSchemaData& oSchemaData)
//{
//
//
//	
//
//	//텍스트파일의 내용이 유니코드기반일것이다.
//
//	std::wifstream fin(filePath, std::ios::in);
//	if (!fin.is_open())
//	{
//		return false;
//	}
//
//	std::wstring str;
//
//
//
//
//	while (fin.good())
//	{
//
//		fin >> str;
//
//		if (str == L"type")
//		{
//			fin >> str;
//			if (str == L"UiEntity")
//			{
//				oSchemaData.mType = str;
//			}
//			else if (str == L"Entity")
//			{
//				oSchemaData.mType = str;
//			}
//			else
//			{
//				//실패
//				return false;
//			}
//
//			fin >> str;
//
//			if (str == L"{")
//			{
//				return LoadAttribute(fin, oSchemaData);
//			}
//
//		}
//
//	}
//
//
//
//
//
//
//
//
//
//
//}
//
//bool Quad::SchemaLoader::LoadAttribute(std::wifstream& fin, RawSchemaData& oSchemaData)
//{
//	//	String classId = "ButtonEntity";  mystring_classId = "ButtonEntity" ;		//o
//
//	std::wstring str;
//	std::wstring attributeType;
//	std::wstring attributeName;
//	std::wstring attributeValue;
//
//
//	while (fin.good())
//	{
//		std::getline(fin, str);
//
//		
//
//		std::wregex blank(L"[[:s:]]*");
//		std::wregex end(L"[[:s:]]*\\}[[:s:]]*");
//		std::wregex attr(L"[[:s:]]*([_[:alpha:]][[:w:]]*)[[:s:]]+([_[:alpha:]][[:w:]]*)[[:s:]]*=[[:s:]]*(.*)[[:s:]]*;[[:s:]]*");
//
//		std::wsmatch substr;
//
//		if (std::regex_match(str, blank))
//		{
//			continue;
//
//		}
//		else if (std::regex_match(str, end))
//		{
//			return true;
//		}
//		else if (std::regex_match(str, substr, attr))
//		{
//			attributeType = substr[1].str();
//			attributeName = substr[2].str();
//			attributeValue = substr[3].str();
//			if (!AttributeParse(attributeType, attributeName, attributeValue, oSchemaData))
//				return false;
//		}
//		else
//			break;
//	}
//
//	return false;
//
//}
//
//bool Quad::SchemaLoader::AttributeParse(const std::wstring &attributeType,
//	const std::wstring & attributeName, const std::wstring &attributeValue, RawSchemaData& oSchemaData)
//{
//
//	std::wstring stringValue;
//	DirectX::XMFLOAT3 vector3Value;
//	DirectX::XMFLOAT4 vector4Value;
//	bool boolValue;
//
//
//
//	if (attributeName == L"classId")
//	{
//		if (attributeType != L"String")
//			return false;
//
//		//여기서 다시 정규표현식사용을해야한다.
//		
//		if (ParseStringAttribute(attributeValue, stringValue))
//		{
//			oSchemaData.mClassId = stringValue;
//			return true;
//		}
//
//	}
//	else if (attributeName == L"pos")
//	{
//		if (attributeType != L"Vector3")
//			return false;
//		
//		if (ParseVector3Attribute(attributeValue, vector3Value))
//		{
//			oSchemaData.mPos = vector3Value;
//			return true;
//		}
//
//	}
//	else if (attributeName == L"scale")
//	{
//		if (attributeType != L"Vector3")
//			return false;
//
//
//		if (ParseVector3Attribute(attributeValue, vector3Value))
//		{
//			oSchemaData.mScale = vector3Value;
//			return true;
//		}
//
//	}
//	else if (attributeName == L"rotation")
//	{
//		if (attributeType != L"Quaternion")
//			return false;
//
//
//		if (ParseVector4Attribute(attributeValue, vector4Value))
//		{
//			oSchemaData.mRotate = vector4Value;
//			return true;
//		}
//	}
//	else if (attributeName == L"mesh")
//	{
//		if (attributeType != L"String")
//			return false;
//
//		if (ParseStringAttribute(attributeValue, stringValue))
//		{
//			oSchemaData.mMesh = stringValue;
//			return true;
//		}
//
//	}
//	else if (attributeName == L"boundingVolume")
//	{
//		if (attributeType != L"String")
//			return false;
//
//		if (ParseStringAttribute(attributeValue, stringValue))
//		{
//			oSchemaData.mBoundingVolume = stringValue;
//			return true;
//		}
//	}
//	else
//	{
//		//사용자가 정의한 타입
//
//	}
//
//
//
//
//	return false;
//}
//
//bool Quad::SchemaLoader::ParseStringAttribute(const std::wstring& attributeValue, std::wstring& oResult)
//{
//	std::wregex stringPattern(L"\"([_[:alpha:]][[:w:]]*)\"");
//	
//	std::wsmatch substr;
//	if (std::regex_match(attributeValue,substr, stringPattern))
//	{
//		oResult = substr[1];
//		return true;
//	}
//	return false;
//
//	// TODO: 여기에 return 문을 삽입합니다.
//}
//
//bool Quad::SchemaLoader::ParseVector3Attribute(const std::wstring& attributeValue, DirectX::XMFLOAT3& oResult)
//{
//
//	//{1.0f,1.0f,1.0f}
//	//{123  , 2 ,	3.523f}
//	std::wregex vector3Pattern(L"\\{[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*\\}");
//
//	std::wsmatch substr;
//	if (std::regex_match(attributeValue,substr, vector3Pattern))
//	{
//		oResult.x = std::stof(substr[1]);
//		oResult.y = std::stof(substr[2]);
//		oResult.z = std::stof(substr[3]);
//		return true;
//	}
//
//	return false;
//}
//
//bool Quad::SchemaLoader::ParseVector4Attribute(const std::wstring& attributeValue, DirectX::XMFLOAT4& oResult)
//{
//	std::wregex vector4Pattern(L"\\{[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([+-]?[[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*\\}");
////	std::regex vector3Pattern("\\{[[:s:]]*([[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*,[[:s:]]*([[:d:]]+(?:.[[:d:]]+)?)f?[[:s:]]*\\}");
//
//	std::wsmatch substr;
//	if (std::regex_match(attributeValue,substr, vector4Pattern))
//	{
//		oResult.x = std::stof(substr[1]);
//		oResult.y = std::stof(substr[2]);
//		oResult.z = std::stof(substr[3]);
//		oResult.w = std::stof(substr[4]);
//		return true;
//	}
//	return false;
//}
//
//bool Quad::SchemaLoader::ParseBoolAttribute(const std::wstring& attributeValue, bool& oResult)
//{
//
//	if (attributeValue == L"true")
//	{
//		oResult = true;
//	}
//	else if (attributeValue == L"false")
//	{
//		oResult = false;
//		
//	}
//	else
//	{
//		return false;
//	}
//
//	return true;
//}
//

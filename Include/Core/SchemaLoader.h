//#pragma once
//
//#include"Predeclare.h"
//#include"header.h"
//#include<string>
//#include<iostream>
//
//namespace Quad
//{
//	class SchemaLoader
//	{
//
//	public:
//		SchemaLoader() = default;
//		~SchemaLoader() = default;
//
//		bool Load(const std::wstring& filePath, RawSchemaData & oSchemaData);
//		
//
//
//	private:
//		bool LoadAttribute(std::wifstream& fin, RawSchemaData& oSchemaData);
//		bool AttributeParse(const std::wstring &attributeType, const std::wstring &attributeName,
//			const std::wstring & attributeValue, RawSchemaData& oSchemaData);
//
//
//		bool ParseStringAttribute(const std::wstring & attributeValue, std::wstring& oResult);
//		bool ParseVector3Attribute(const std::wstring& attributeValue,DirectX::XMFLOAT3& oResult);
//		bool ParseVector4Attribute(const std::wstring& attributeValue,DirectX::XMFLOAT4& oResult);
//		bool ParseBoolAttribute(const std::wstring& attributeValue, bool & oResult);
//
//
//	};
//
//}

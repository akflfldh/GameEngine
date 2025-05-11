//#pragma once
//
//
//
//#include"Predeclare.h"
//
//#include<unordered_map>
//#include<string>
//namespace Quad
//{
//
//
//	class EffectTable
//	{
//
//		friend class ResourceController;
//	public:
//		~EffectTable() = default;
//
//		
//
//		static Effect* GetEffect(const std::wstring& name);
//		static bool AddEffect(const std::wstring& name, Effect* effect);
//
//
//	private:
//		EffectTable() = delete;
//
//	private:
//		static std::unordered_map<std::wstring, Effect*> mEffectTable;
//
//	};
//
//}

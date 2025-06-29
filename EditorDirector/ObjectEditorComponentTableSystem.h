#pragma once

#include<unordered_map>
#include<string>


namespace Quad
{
	class BaseComponent;

	class ObjectEditorComponentTableSystem 
	{
	public:
		static ObjectEditorComponentTableSystem* GetInstance();
		~ObjectEditorComponentTableSystem();

		template<typename T>
		T* GetComponent(unsigned long long objectID);

		template<typename T>
		T * AddComponent(unsigned long long objectID);

		//object가 제거되었기에 관련된 모든 에디터 컴포넌트들을 제거
		void RemoveObject(unsigned long long objectID);

	

	private:
		ObjectEditorComponentTableSystem();

		BaseComponent* GetComponent(const char* componentTypeName, unsigned long long objectID);
		BaseComponent* AddComponent(const char* componentTypeName, unsigned long long objectID);


	private:

		using ComTableType = std::unordered_map<unsigned long long, std::unordered_map<std::string, BaseComponent*>>;
					//<object id , objectEditorComponentTable(componentTypeName,component)>
		ComTableType mComponentTable;

	};


	template<typename T>
	inline T* ObjectEditorComponentTableSystem::GetComponent(unsigned long long objectID)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "엔진 올바르지못한 에디터컴포넌트");

		return static_cast<T*>(GetComponent(T::GetClassTypeNameStatic(),objectID));
	}

	template<typename T>
	inline T * ObjectEditorComponentTableSystem::AddComponent(unsigned long long objectID)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value, "엔진 올바르지못한 에디터컴포넌트");

		return static_cast<T*>(AddComponent(T::GetClassTypeNameStatic(), objectID));
	}

}

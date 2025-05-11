#pragma once

#include"Core/Event.h"
#include"Object/Object.h"
#include<list>
#include<vector>
#include<unordered_map>


#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB EventTable
	{
	public:
		EventTable();
		~EventTable() = default;
		//string std::vector<object> or list 

		 bool RegisterEvent(const std::string& eventName);
		  void AddObject(const std::string& eventName, Object* object);
		  void RemoveObject(const std::string& eventName, Object* object);


		  void GetObjectList(const std::string& eventName, std::list<Object*>& oObjectList) const ;
		  void GetObjectVector(const std::string& eventName, std::vector<Object*>& oObjectVector)const;

	private:
		 std::unordered_map<std::string, std::vector<Object*>> mEventTable;
		// std::unordered_map<std::string, std::vector<Object*>> mEventTable;
		//보통 이벤트에 등록하면 계속 그 이벤트를 받을 가능성이 크지 
		//즉 추가 삭제보다는 이벤트에등록된 오브젝트들을 순회할가능성이 더 크다.
		//그래서 list보다는 다른 자료구조를 사용하는게 좋겠다.
		// 벡터도 좋을듯하다.

	};

}
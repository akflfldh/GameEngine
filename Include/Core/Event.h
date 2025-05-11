#pragma once

#include"Predeclare.h"
#include<string>
#include<windows.h>
#include"Core/CoreDllExport.h"

//이벤트는 클래스도 좋지만, 구조체로 정의하는것도고려하자, 딱히 메서드가 필요없는경우가 많을거같다.


//이벤트를 구분짓는것은 문자열 id로 

//이벤트는 단지 어떤 사건이 발생했다라는 정보만 알려주는역할이다.
//각 시스템의 이벤트핸들러나 오브젝트들의 이벤트핸들러가 그러한 이벤트들에대해 적절한 작업을 수행할것이다.


namespace Quad
{

	enum class EEventID
	{
		eSelectEvent=1
		
	};



	class CORE_API_LIB Event
	{
	public:

		Event(const std::string& name);
		virtual ~Event() = 0;
		const std::string& GetName() const;

		void SetSystemID(ESystemID systemID);
		ESystemID GetSystemID() const;


		bool GetImmediateEventFlag() const;
			
		void SetEventID(EEventID id);
		EEventID GetEventID() const;

	protected:
		void SetName(const std::string& name);
		void SetImmediateEventFlag(bool flag);

	private:
		std::string mName;

		//이벤트에 등록된 오브젝트를 관리하는것을 만들자.
		ESystemID mSystemID = ESystemID::eNone;
		bool mImmediateEventFlag;
		EEventID mEventID ;

	};


	class DragAndDropEvent :public Event
	{

	public:
		DragAndDropEvent(const std::string& name);
		~DragAndDropEvent();

		void SetFilePath(const std::wstring& filePath);
		std::wstring GetFilePath() const;

	private:
		std::wstring mFilePath;

	};

}


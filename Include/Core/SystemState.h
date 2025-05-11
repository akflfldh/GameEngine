#pragma once



#include"Core/Event.h"
#include"Predeclare.h"
#include"Core/CoreDllExport.h"



namespace Quad
{

	class CORE_API_LIB SystemState
	{
	public:
		SystemState(const std::string& name);
		~SystemState() = default;


		virtual void EnterState(System& system) = 0;
		virtual void RunState(System& system) = 0;
		virtual void ExitState(System& syetem) = 0;

		virtual SystemState* HandleEvent(System& syetem, Event* event)=0;		//이벤트를 처리하고나서
		//상태가 바뀔수있어 리턴된상태로 바꾼다 만약 리턴된상태가 동일하다면 상태가바뀌지않은것
		const std::string& GetName() const;

		bool GetEventFocusFlag() const;
		void SetEventFocusFlag(bool flag);

		virtual void Reset() = 0;

	private:



	private:
		const std::string mName;
		bool mEventFocusFlag;

	};

}
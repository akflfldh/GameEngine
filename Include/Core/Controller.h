#pragma once

#include"Predeclare.h"
#include<string>
#include<unordered_map>

#include"Utility/CircleQueue.h"
#include"Utility/ReaderWriter.h"


#include"Core/CoreDllExport.h"

namespace Quad
{

	class CORE_API_LIB Controller
	{
	public:

		Controller() = default;
		~Controller() = default;

		virtual void Initialize();

		static bool AddController(const  std::string& name, Controller* controller);
		static void AddEvent(const std::string& controllerName, Event* event);

		const std::string& GetName() const;
		void SetName(const std::string& name);

		void AddEvent(Event* event);
		Event* GetEvent();

	protected:

	


	private:
	

	private:
		static std::unordered_map<std::string, Controller*> mControllerUnMap;
		std::string mName;

		CircleQueue<Event*> mEventQueue;
		ReaderWriter mEventQueueSynObject;
	};

}
#pragma once



#include"Predeclare.h"

#include"Core/Event.h"
#include"Core/Ray.h"


#include"Core/CoreDllExport.h"

namespace Quad
{
	//"Select" ,"ReleaseSelect"
	class CORE_API_LIB MouseClickSelectEvent:public Event
	{
	public:
		MouseClickSelectEvent(const std::string & name);
		~MouseClickSelectEvent() = default;
		Ray GetRay() const;
		void SetRay(const Ray& ray);

	private:
		std::pair<UINT, UINT> mScreenPos;
		Ray mRay;


	};


}
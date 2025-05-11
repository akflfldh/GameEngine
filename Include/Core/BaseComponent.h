#pragma once

#include"Predeclare.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB BaseComponent
	{
	public:
		BaseComponent()=default;
		virtual ~BaseComponent() = 0 ;

		virtual void Update(float delta) = 0;

		virtual int GetID() = 0;
		
	private:



	};

}

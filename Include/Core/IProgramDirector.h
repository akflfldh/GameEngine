#pragma once


#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB IProgramDirector
	{

	public:		
		virtual void Initialize() = 0;
		
		virtual void PreUpdate(float deltaTime) = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void EndUpdate(float deltaTime) = 0;
		virtual void Draw() = 0;

	private:




	};

}

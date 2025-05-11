#pragma once


#include"Predeclare.h"
#include"ComponentUpdateSystem.h"

#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB AnimationUpdateSystem:public ComponentUpdateSystem
	{
	public:

		AnimationUpdateSystem() = default;
		virtual ~AnimationUpdateSystem();

		void Update(float deltaTime);


	private:

	};

}

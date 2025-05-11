#pragma once

#include"Component.h"
#include"Core/CoreDllExport.h"


namespace Quad
{
	class Object;
	class CORE_API_LIB MoveComponent:public Component<MoveComponent>
	{
	public:
		MoveComponent(Object * destObject);
		virtual ~MoveComponent();

		virtual void Update(float deltaTime);
		
		void SetSpeed(float speed);

		void MoveForwardInput(float axis);
		void MoveSideInput(float axis);

	private:

		void MoveForward(float deltaTime);
		void MoveSide(float deltaTime);
		void Move(float deltaTime);

	private: 
		Object* mDestObject;
		
		float mSpeed;//초 단위


		float mForwardDir;
		float mSideDir;



	};

}

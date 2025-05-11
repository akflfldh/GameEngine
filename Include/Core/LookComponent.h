#pragma once

#include"Component.h"
#include"Core/CoreDllExport.h"

namespace Quad
{

	class Object;
	class CORE_API_LIB LookComponent :public Component<LookComponent>
	{
	public:
		LookComponent(Object * destObject);
		virtual ~LookComponent();

		virtual void Update(float delta);

		void TurnPitchInput(float value);
		void TurnYawInput(float value);

	private:

		void TurnPitch(float deltaTime);
		void TurnYaw(float deltaTime);

		void Turn(float deltaTime);


		Object* mDestObject;

		float mDeltaX;
		float mDeltaY;
		float mRotationScreenX;
		float mRotationScreenY;
	};
}
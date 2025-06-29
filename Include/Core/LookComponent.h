#pragma once

#include"BaseComponent.h"
#include"Core/CoreDllExport.h"

namespace Quad
{

	class Object;
	class CORE_API_LIB LookComponent :public BaseComponent
	{
	public:
		LookComponent();
		virtual ~LookComponent();
		
		virtual void Start() override;
		virtual void Update(float delta) override;

		void TurnPitchInput(float value);
		void TurnYawInput(float value);

		CLASSNAME(LookComponent)
		CLASSNAMESTATIC(LookComponent)

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
	REGISTERCOMPONENTCLASS(LookComponent)
}
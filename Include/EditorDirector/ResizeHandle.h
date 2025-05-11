#pragma once

#include"Predeclare.h"
#include"UiEntity.h"

namespace Quad
{

	class ResizeHandle:public UiEntity
	{
	public:
		ResizeHandle(const std::string & name);
		~ResizeHandle() = default;

		void Initialize(int direction);
		void Update(float deltaTime) override ;
		void OnEvent(Event* event) override;

		void SetDstEntity(UiEntity* entity);
	private:

		void Initialize() override;
	private:
		float mWidth;
		float mHeight;

		UiEntity* mDstEntity;
		int mDirection;

	};

}


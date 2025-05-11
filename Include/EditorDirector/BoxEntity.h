#pragma once

#include"Predeclare.h"
#include"UiEntity.h"
#include"EditorMacro.h"


namespace Quad
{

	class BoxEntity:public UiEntity
	{
	public:
		BoxEntity(const std::string & name="");
		~BoxEntity() = default;
		virtual void Initialize() override;

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

	private:


	};
	REGISTEREDITORCLASS(BoxEntity)

}

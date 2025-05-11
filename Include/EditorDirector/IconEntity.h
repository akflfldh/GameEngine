#pragma once


#include"Predeclare.h"
#include"UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class IconEntity:public UiEntity
	{
	public:
		IconEntity(const std::string& name="");
		virtual ~IconEntity();

		virtual void Initialize() override;

		static IconEntity* Create(Map* map, int mapLayerIndex = 0);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(IconEntity)
		CLASSSIZE(IconEntity)



	protected:
		void InitCreating();

	private:


	};
	REGISTEREDITORCLASS(IconEntity)

}

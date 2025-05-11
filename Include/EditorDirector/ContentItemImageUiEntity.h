#pragma once


#include"Predeclare.h"

#include"UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{

	class ContentItemImageUiEntity:public UiEntity
	{
	public:
		ContentItemImageUiEntity(const std::string& name="");
		virtual ~ContentItemImageUiEntity()=default;

		virtual void Initialize() override;

		static ContentItemImageUiEntity* Create(Map* map, int mapLayerIndex, ContentItem* contentItem);

		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;


		CLASSNAME(ContentItemImageUiEntity)
		CLASSSIZE(ContentItemImageUiEntity)
		void SetTexture(ContentItem* contentItem);
	protected:

		void InitCreating(ContentItem * contentItem);


	private:


	};
	REGISTEREDITORCLASS(ContentItemImageUiEntity)

}


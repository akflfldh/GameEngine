#pragma once





#include"Predeclare.h"
#include"ContentItemBaseUiEntity.h"
#include<string>
#include"ContentItemUiEntityFactory.h"

//객체풀을 사용하면 좋을거같은데
//그러면 미리객체들이 생성되어져야되고
//그때이름은 가지지않는다. 
//사용할려고하면 setName호출하면된다.
//


#include"EditorMacro.h"

namespace Quad
{
	//AssetUiEntity는 참조하고있는 asset과 동일한이름을 가진다.
	//그 asset과 이름이 동일한데 문제없는이유는 다른시스템에서 존재할것이기에 
	class ContentItemUiEntity :public ContentItemBaseUiEntity
	{
		friend class ContentItemUiEntityFactory;

	public:

		//enum class EType
		//{
		//	//어디에서 사용하는 것인가 file ui window에서 사용하는 것
		//	eFileUiVersion = 0,

		//	//drag and drop 윈도우에서 사용하는것
		//	eDragAndDropVersion,
		//};

	public:
		ContentItemUiEntity();

	//	void ChangeContentItem(ContentItem* item);
		virtual ~ContentItemUiEntity()=default;

		virtual void Initialize() override;

		static ContentItemUiEntity* Create(Map* map, int mapLayerIndex, ContentItem* item, EType type);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(ContentItemUiEntity)
		CLASSSIZE(ContentItemUiEntity)
	//	ContentItem* GetAsset() const;
	
	//	void SetOnOff(bool state);

	private:
		
		//ContentItem* GetContentItem() const;
	private:
		//handle event
		
		virtual void HandleHover(Event* pEvent) override;
		virtual void HandleMouseMove(Event* pEvnet) override;
		virtual void HandleLButtonDown(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;
		void HandleLButtonDownDouble(Event* pEvent);

		
		void InitCreating(ContentItem* item, EType type);

	private:
		//ContentItem* mContentItem;
		//ContentItemImageUiEntity* mContentItemImageUiEntity;
		////Text* mContentItemNameText;
		//TextBox* mContentItemNameText;

		//EType mUiEntityType;

		//Texture* mDefaultStateTexture;
		//Texture* mHoverStateTexture;
		//Texture* mSelectStateTexture;




		bool mContentItemNameTextSelectState = false;

	};
	REGISTEREDITORCLASS(ContentItemUiEntity)

}


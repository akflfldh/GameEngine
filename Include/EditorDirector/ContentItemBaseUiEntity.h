#pragma once

#include"PanelUiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class ContentItemBaseUiEntity :public PanelUiEntity
	{
		friend class ContentItemUiEntityFactory;

	public:

		enum class EType
		{
			//어디에서 사용하는 것인가 file ui window에서 사용하는 것
			eFileUiVersion = 0,

			//drag and drop 윈도우에서 사용하는것
			eDragAndDropVersion,
		};

	public:
		ContentItemBaseUiEntity();

		void ChangeContentItem(ContentItem* item);
		virtual ~ContentItemBaseUiEntity() = default;

		virtual void Initialize() override;

		static ContentItemBaseUiEntity* Create(Map* map, int mapLayerIndex, ContentItem* item,EType type);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(ContentItemBaseUiEntity)
			CLASSSIZE(ContentItemBaseUiEntity)
		

		void SetOnOff(bool state);

		ContentItem* GetAsset() const;
	protected:

		ContentItem* GetContentItem() const;
		void InitCreating(ContentItem* item, EType type);


		TextBox* GetContentItemNameText() const;
		ContentItemImageUiEntity* GetContentItemImageUiEntity() const;

		Texture* mDefaultStateTexture;
		Texture* mHoverStateTexture;
		Texture* mSelectStateTexture;

	private:
		//handle event

		//virtual void HandleHover(Event* pEvent) override;
		//virtual void HandleMouseMove(Event* pEvnet) override;
		//virtual void HandleLButtonDown(Event* pEvent) override;
	//	virtual void HandleLButtonUp(Event* pEvent) override;
		//void HandleLButtonDownDouble(Event* pEvent);


		

	private:
		ContentItem* mContentItem;
		ContentItemImageUiEntity* mContentItemImageUiEntity;
		//Text* mContentItemNameText;
		TextBox* mContentItemNameText;

		EType mUiEntityType;





		bool mContentItemNameTextSelectState = false;
	};
	REGISTEREDITORCLASS(ContentItemBaseUiEntity)
}


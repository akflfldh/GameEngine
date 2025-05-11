#pragma once
#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"ScrollBarEntity.h"
#include<unordered_map>
#include<functional>

#include"EditorMacro.h"
namespace Quad
{

	class ContentItemUiPanelEntity:public PanelUiEntity
	{

	public:
		ContentItemUiPanelEntity(const std::string& name="");
		virtual ~ContentItemUiPanelEntity();

		virtual void Initialize() override;

		static ContentItemUiPanelEntity* Create(Map* map, int mapLayerIndex, float width, float height, float startOffsetX, float startOffsetY, float z, float clientWidth, float clientHeight);

		virtual void Start() override;
		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(ContentItemUiPanelEntity)
		CLASSSIZE(ContentItemUiPanelEntity)
		void AddContentItemUiEntity(ContentItemBaseUiEntity* entity);

		void ActivatePanel(bool flag);


		void OnResize(float clientWidth, float clientHeight);

		float GetPanelPerScrollBarShiftFactor() const;

		float GetStartX() const;
		float GetStartY() const;


		const std::vector< ContentItemBaseUiEntity*>& GetContentItemBaseUiEntityVector() const;

	protected:
		void InitCreating(float width, float height, float startOffsetX, float startOffsetY, float z, float clientWidth, float clientHeight);

	private:

		void CaculateContentItemUiEntityLocalPosition(int rowNum, int columnNum, DirectX::XMFLOAT3& oPos);

		void AdjustScrollbarPosition();
		

		void AdjustPanelPerScrollBarShiftFactor();
		void AdjustPanelAndScrollBarPosition();


	private:



		void HandleWindowResize(Event* pEvent);
		//virtual void HandleLButtonDown(Event* pEvent) override ;
		//void HandleLButtonUp(Event* pEvent);
	//	virtual void HandleMouseMove(Event* pEvent)override;
	

		//void HandleHover(Event* pEvent);
		//void HandleRButtonDown(Event* pEvent);
		//void HandleRButtonUp(Event* pEvent);

		void HandleWheel(Event* pEvent);
		void HandleLButtonDownDouble(Event* pEvent);



	private:
		

		float mStartX;
		float mStartY;
		float mVisibleWidth;	//실제보여지는 영역의너비,(이정보를바탕으로, contentItem들의 위치를 조정한다)
		////(이값은 창의크기가변할때마다 갱신된다)
		float mVisibleHeight;


		static float mContentItemUiEntityStartOffsetX;
		static float mContentItemUiEntityStartOffsetY;
		std::vector<ContentItemBaseUiEntity*> mContentItemUiEntityVector;

		int mColumnIndex;
		int mRowIndex;
		static float mContentItemUiEntityDistance;
		

		ScrollBarEntity* mScrollBar;
	
		//std::unordered_map < std::string, std::function<void(Event*)>> mEventHandlerUnMap;


		//스크롤바의이동값에대한 판넬의 이동값 비율인자. ex)  0.5이면 스크롤바가 1 만큼이동하면 contentItemPanel이 0.5만큼 이동 
		float mPanelPerScrollBarShiftFactor;

		



















	};
	REGISTEREDITORCLASS(ContentItemUiPanelEntity)
}

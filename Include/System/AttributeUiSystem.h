#pragma once

#include"Predeclare.h"
#include"UiSystem.h"
#include"Object/Object.h"
#include"Singleton.h"
namespace Quad
{
	class AttributeUiSystem:public UiSystem,public Singleton<AttributeUiSystem>
	{
	public:
		AttributeUiSystem();
		virtual ~AttributeUiSystem()=default;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime,bool playMode);
		virtual void OnResize(UINT clientWidth, UINT clientHeight);

		virtual void AddEntity(Object* entity, int mapLayerID = 0 ,bool egineEditObject = false) ;
		//virtual void GetEntity(std::vector<Object*>& oUiEntityVector) ;
		//virtual const std::vector<MapLayer>& GetEntity();

		//새클래스생성,프로젝트로드시 클래스를 리스트에 추가
		void AddClassListItem(const std::string& className);

		void SetSelectedObject(Object* object);
		Object* GetSelectedObject() const;

	private:
		void CreateGenerateEntityPanel();
		void CreateTransformPanel();
		void CreateMaterialPanel();
		
		void CreateMaterialSubPanel2(PanelUiEntity * subPanel2);

	


		//assetPanel의 자식인 assetImageFrameEntity포인터를 돌려받고 콜백을 등록하고 부모panel의 위치를조정하자.
		void CreateMaterialAssetPanel(PanelUiEntity* parentPanel,const std::string& panelName, const std::string& text,
			PanelUiEntity*& oParentPanel, IconEntity*& oAssetImageFramePanel);
	



		void CreateAnimationPanel();



		//helper
		void SetDefaultInitSetting(UiEntity* entity);



	private:
		//property라고되어있지만 이름만이런것이고 x,y,z 각 입력을받을수있는 판넬을 의미하는것
		TransformPropertyPanel* CreatePropertyPanel(const std::string& panelName, const std::string& tagName, float localWidth, float localHeight, int textBoxNum , const std::string& texturName);

		PanelUiEntity* CreateDefaultPanel(const std::string& panelName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition,float width ,float height);

		TextBoxPanel* CreateDefaultTextBoxPanel(const std::string& panelName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition, const std::string& text, float width, float height, const DirectX::XMFLOAT3& textColor = { 1.0f,1.0f,1.0f }, ETextAlignmentDirection textAlignmentDirection = ETextAlignmentDirection::eLeft);

		TextBox* CreateDefaultTextBox(int maxCharacterNum,const std::string& textBoxName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition, const std::string& text, float width, float height, const DirectX::XMFLOAT3& textColor = { 1.0f,1.0f,1.0f });



		//PanelUiEntity* mRootPanel;
		ExpandablePanel* mRootPanel;
		TransformPropertyPanel* mPositionPanel;
		TransformPropertyPanel* mScalePanel;
		TransformPropertyPanel* mRotationPanel;
	
		PanelUiEntity* mMaterialPanel;


		ScrollListPanel* mScrollClassListPanel;



		Object* mSelectedObject;

	};



}

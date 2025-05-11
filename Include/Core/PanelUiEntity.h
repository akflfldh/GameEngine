#pragma once

#include"Predeclare.h"
#include"Object/UiEntity.h"

#include"Core/CoreDllExport.h"

#include<vector>
namespace Quad
{
	class CORE_API_LIB PanelUiEntity:public UiEntity
	{
	public:
		PanelUiEntity(const std::string& name="");
		virtual ~PanelUiEntity();

		virtual void Initialize() override;

		static PanelUiEntity* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		virtual void AddChildPanelUiEntity(UiEntity* childPanel);


		virtual void RemoveChildObject(Object* childObject) override;
		virtual bool RemoveChildObjectInVector(Object* childObject) override;
		CLASSNAME(PanelUiEntity)
		CLASSSIZE(PanelUiEntity)
		const std::vector<UiEntity*> & GetChildPanelUiEntityVector() const;
		std::vector<UiEntity*> & GetChildPanelUiEntityVector();

		//자신과 자식에대한 draw flag 설정
		virtual void SetDrawFlag(bool flag)override;
	
	protected:
		Object* GetChildHoverObject() const;
		void SetChildHoverObject(Object* childObject);



		virtual void HandleLButtonDown(Event* pEvent);
		virtual void HandleLButtonUp(Event* pEvent);
		virtual void HandleRButtonDown(Event* pEvent);
		virtual void HandleRButtonUp(Event* pEvent);
		virtual void HandleMouseMove(Event* pEvent);
		virtual void HandleHover(Event* pEvent);


		void InitCreating();

	private:
		std::vector<UiEntity*> mChildPanelUiEntityVector;
		

		//void HandleDefault(Event* pEvent);

		Object* mHoverChildObject = nullptr;








	};
	REGISTERCLASS(PanelUiEntity)

}

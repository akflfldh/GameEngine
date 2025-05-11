#pragma once

#include"Predeclare.h"

#include"ButtonUiEntitiy.h"

#include<functional>
#include"EditorMacro.h"

namespace Quad
{
	class ListExpandButton:public ButtonUiEntity
	{
	public:
		ListExpandButton(const std::string& name="");
		~ListExpandButton() = default;

		void Initialize() override;

		static ListExpandButton* Create(Map* map, int mapLayerIndex = 0);


		void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;
		

		CLASSNAME(ListExpandButton)
		CLASSSIZE(ListExpandButton)
		int GetExpandFlag() const;
		

		void LogicalButtonClick();
	protected:

		void InitCreating();
	
	private:
		void HandleHover(Event* pEvent);
		void HandleMouseMove(Event* pEvent);
		void HandleLButtonDown(Event* pEvent);
		void HandleLButtonUp(Event* pEvent);

		DirectX::XMFLOAT4X4 mExpandTransformTexMatrixF;


		int mExpandFlag;

	};
	REGISTEREDITORCLASS(ListExpandButton)
}


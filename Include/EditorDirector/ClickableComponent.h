#pragma once




#include"Component.h"
#include"UiComponent.h"

namespace Quad
{


	class ClickableComponent :public Component<ClickableComponent>
	{
	public:

		ClickableComponent();
		virtual ~ClickableComponent() = default;

		void Update(float delta= 0) override;


		//이런 메서드는 드러나지않는다 더이상

		void ExecuteClickDown();
		void ExecuteClickUp();

		//상태를 변경하는 메서드만 드러날것이고
		//update에서 상태에따라 수행할것이다.


	protected:
		virtual void ClickDown();
		virtual void ClickUp();

		//texture
	private:



	};

}
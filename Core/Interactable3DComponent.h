#pragma once


#include"BaseComponent.h"

namespace Quad
{


		//3d 오브젝트들에대한 상호작용 컴포넌트 (ray, 피킹 등등 )
	class Interactable3DComponent:public BaseComponent
	{
	public:
		Interactable3DComponent();
		virtual ~Interactable3DComponent();

		virtual void Start() override;
		virtual void Update(float delta) override ;

	private:

	};

}

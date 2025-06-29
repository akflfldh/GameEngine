#pragma once



#include"Component/UiColliderComponent.h"
#include"Collision/RectCollider.h"

namespace Quad
{
	class RectColliderComponent:public UiColliderComponent
	{
	public:
		RectColliderComponent();
		virtual ~RectColliderComponent();
		virtual void Update() override;

		virtual void Serialize(const std::string& tag);
		virtual void DeSerialize(const std::string& tag);

		CLASSSIZE(ColliderBaseComponent)

		virtual RectCollider* GetCollider() const override;

	private:
		RectCollider* mRectCollider;
	};

}

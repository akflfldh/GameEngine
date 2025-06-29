//#pragma once
//
//
//#include<Component/SceneComponent.h>
//namespace Quad
//{
//	class BaseCollider;
//
//	class PickingColliderComponent:public SceneComponent
//	{
//	private:
//		PickingColliderComponent();
//		virtual ~PickingColliderComponent() ;
//
//		//void SetColliderType(); 새롭게 충돌체를변경
//
//		BaseCollider* GetCollider() const ;		//가져올떄마다 내부적으로는 collider를 update(worldTransform을통해)
//
//	public:
//
//		BaseCollider* mBaseCollider;
//
//	};
//
//}

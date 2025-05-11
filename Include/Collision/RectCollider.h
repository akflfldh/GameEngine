#pragma once

#include"Collision/UiCollider.h"
#include<DirectXMath.h>
namespace Quad
{
	class RectCollider:public UiCollider
	{
	public:
		RectCollider();
		virtual ~RectCollider();

		//void SetDestUiEntity(UiEntity* destUiEntity);
		//UiEntity* GetDestUiEntity() const;

		virtual void Update(float deltaTime) override;


		const DirectX::XMFLOAT3& GetTopLeftPoint() const;
		const DirectX::XMFLOAT3& GetTopRightPoint() const;
		const DirectX::XMFLOAT3& GetBottomLeftPoint() const;
		const DirectX::XMFLOAT3& GetBottomRightPoint() const;


	private:

		DirectX::XMFLOAT3 mTopLeftPoint;
		DirectX::XMFLOAT3 mTopRightPoint;
		DirectX::XMFLOAT3 mBottomLeftPoint;
		DirectX::XMFLOAT3 mBottomRightPoint;

	};

}

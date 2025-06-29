#pragma once

#include"Core/CoreDllExport.h"

#include<DirectXMath.h>

namespace Quad
{
	class Event;
	class SceneComponent;

	enum class EBaseColliderType
	{
		eCollider=0,
		eUiCollider
	};
	
	class CORE_API_LIB BaseCollider
	{
	public:
		BaseCollider(EBaseColliderType mBaseColliderType = EBaseColliderType::eCollider);
		virtual ~BaseCollider() = 0;

		virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix) = 0;


		void SetParentSceneComponent(SceneComponent* parentComponent);
		SceneComponent* GetParentComponent() const;
		EBaseColliderType GetBaseColliderType() const;
	private:

		EBaseColliderType mBaseColliderType;

		SceneComponent* mParentSceneComponent;

	};

}

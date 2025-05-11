#pragma once

#include"Predeclare.h"

#include"ModelBaseComponent.h"
#include"Core/CoreDllExport.h"

namespace Quad
{


	class CORE_API_LIB UiColliderComponent:public ModelBaseComponent
	{
	public:
		UiColliderComponent(Object* uiEntity);
		virtual ~UiColliderComponent();

		virtual void Update(float deltaTime) override;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;

		UiCollider* GetCollider() const;

	private:
		UiCollider* mCollider;

	};

}

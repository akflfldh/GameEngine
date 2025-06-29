#pragma once

#include"Predeclare.h"

#include"Component/ColliderBaseComponent.h"
#include"Core/CoreDllExport.h"



namespace Quad
{


	class CORE_API_LIB UiColliderComponent:public ColliderBaseComponent
	{
	public:
		UiColliderComponent();
		virtual ~UiColliderComponent() = 0;

		virtual void Update(float deltaTime) override;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;

	private:

	};

}

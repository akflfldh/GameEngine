#pragma once


#include"Core/CoreDllExport.h"

#include"Object/Macro.h"
#include"SceneComponent.h"
#include<string>

namespace Quad
{

	class BaseCollider;
	class CORE_API_LIB ColliderBaseComponent:public SceneComponent
	{

	public:
		ColliderBaseComponent();
		virtual ~ColliderBaseComponent() = 0;

		
		virtual void Update()= 0;

		//기존 콜라이더는 맵에서 제거+완전히 삭제
		void SetCollider(BaseCollider* collider);


		virtual void Serialize(const std::string& tag) ;
		virtual void DeSerialize(const std::string& tag) ;

		CLASSSIZE(ColliderBaseComponent)

		virtual BaseCollider* GetCollider() const = 0;

	private:



	};



}


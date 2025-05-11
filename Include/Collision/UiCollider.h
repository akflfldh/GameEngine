#pragma once
#include"Predeclare.h"
namespace Quad
{
	
	enum class EUiColliderType :int
	{
		eRect=0,
		eSpline
	};
	class UiCollider
	{
	public:
		UiCollider(EUiColliderType colliderType);
		virtual ~UiCollider() = 0;

		virtual void Update(float deltaTime) = 0;
		void SetDestObject(Object* object);
		Object* GetDestObject()const;

		EUiColliderType  GetColliderType() const;


	private:
		EUiColliderType	mColliderType;
		Object* mDestObject;

	};

}

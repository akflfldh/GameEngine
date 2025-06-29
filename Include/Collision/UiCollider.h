#pragma once
#include"Predeclare.h"

#include"BaseCollider.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	
	enum class EUiColliderType :int
	{
		eRect=0,
		eSpline
	};
	class CORE_API_LIB UiCollider:public BaseCollider
	{
	public:
		UiCollider(EUiColliderType colliderType);
		virtual ~UiCollider() = 0;

		virtual void Update(const DirectX::XMFLOAT4X4 & transformMatrix) = 0;
		void SetDestObject(Object* object);
		Object* GetDestObject()const;

		EUiColliderType  GetColliderType() const;


	private:
		EUiColliderType	mColliderType;
		Object* mDestObject;

	};

}

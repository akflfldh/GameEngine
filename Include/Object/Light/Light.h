#pragma once




#include"Predeclare.h"

#include"Object/Object.h"
#include"Utility/MathHelper.h"
#include"Object/ObjectType.h"

#include"Core/CoreDllExport.h"

namespace Quad
{


	class CORE_API_LIB Light:public Object
	{
	public:
		Light(const std::string& name = "", ELightType type = ELightType::eDirect);
		virtual ~Light() = default;
		virtual void OnEvent(Event * event) override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;
		virtual void Initialize() override;
		virtual void DefaultCreatingInitialize() override;


		CLASSNAME(Light)
		CLASSSIZE(Light)
		void SetLightType(ELightType lightType);
		ELightType GetLightType()const;

		DirectX::XMFLOAT3 GetLightDirect() const;
		DirectX::XMFLOAT3 GetLight() const;
		DirectX::XMFLOAT3 GetPosition() const;
		float GetSpotDegree() const;
		
		void SetLight(float r, float g, float b);

		//이 메서드는 아무것도하지않고 인터페이스역할만 한다.
		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) override;
		virtual bool InnerDetectRayHit( const Ray& ray) override;

		virtual void Serialize() override;
		virtual void DeSerialize() override;


	private:
		ELightType mLightType;
		DirectX::XMFLOAT3 mLight;
		float mSpotDegreeRange;  //라디안

	};
	REGISTERCLASS(Light)
	
}


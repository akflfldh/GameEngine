#include "Object/Light/Light.h"
#include"DirectXMath.h"
#include"Parser/JsonParser.h"

Quad::Light::Light(const std::string& name, ELightType type)
	:Object(name,EObjectType::eLight),mLightType(type), mLight({ 1.0f,1.0f,1.0f }), mSpotDegreeRange(DirectX::XM_PI / 4.0f)
{
}

void Quad::Light::OnEvent(Event* event)
{
}

void Quad::Light::Update(float deltaTime)
{
	Object::Update(deltaTime);
}

void Quad::Light::EndUpdate(float deltaTime)
{
}

void Quad::Light::Initialize()
{
	
}

void Quad::Light::DefaultCreatingInitialize()
{


}

void Quad::Light::SetLightType(ELightType lightType)
{
	mLightType = lightType;
}

ELightType Quad::Light::GetLightType() const
{
	return mLightType;
}

DirectX::XMFLOAT3 Quad::Light::GetLightDirect() const
{
	return GetTransform().GetLookWorld();
}

DirectX::XMFLOAT3 Quad::Light::GetLight() const
{
	return mLight;
}

DirectX::XMFLOAT3 Quad::Light::GetPosition() const
{
	return GetTransform().GetPositionWorld();
}

float Quad::Light::GetSpotDegree() const
{
	return mSpotDegreeRange;
}

void Quad::Light::SetLight(float r, float g, float b)
{
	mLight.x = r;
	mLight.y = g;
	mLight.z = b;
}

bool Quad::Light::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
{
	return true;
}

bool Quad::Light::InnerDetectRayHit( const Ray& ray)
{
	return true;
}

void Quad::Light::Serialize()
{
	Object::Serialize();
	
	JsonParser::Write("Light_LightType", (int)mLightType);
	JsonParser::Write("Light_Light", mLight);
	JsonParser::Write("Light_SpotDegreeRange", mSpotDegreeRange);


}

void Quad::Light::DeSerialize()
{
	Object::DeSerialize();

	int lightType = 0;
	JsonParser::Read("Light_LightType", lightType);
	mLightType =(ELightType)lightType;

	JsonParser::Read("Light_Light", mLight);
	JsonParser::Read("Light_SpotDegreeRange", mSpotDegreeRange);




}

#include "Core/BaseComponent.h"
#include"Parser/JsonParser.h"

Quad::BaseComponent::BaseComponent(EComponentType componentType)
    :mDestObject(nullptr), mActiveState(false),mEComponentType(componentType)
{
}

Quad::BaseComponent::~BaseComponent()
{

}

void Quad::BaseComponent::Initialize(Object* destObject)
{
    mDestObject = destObject;
}

void Quad::BaseComponent::SetDestObject(Object* object)
{
    mDestObject = object;
}

Quad::Object* Quad::BaseComponent::GetDestObject() const
{
    return mDestObject;
}

void Quad::BaseComponent::OnActive()
{

}

void Quad::BaseComponent::DeActive()
{
}

Quad::EComponentType Quad::BaseComponent::GetComponentType() const
{
    return mEComponentType;
}

void Quad::BaseComponent::SetActiveState(bool activeState)
{
    mActiveState = activeState;
}

bool Quad::BaseComponent::GetActiveState() const
{
    return mActiveState;
}

void Quad::BaseComponent::Serialize(const std::string& tag)
{
    
    JsonParser::Write("BaseComponent_ActiveState", mActiveState);


}

void Quad::BaseComponent::DeSerialize(const std::string& tag)
{
    mActiveState = JsonParser::ReadBool("BaseComponent_ActiveState");

}

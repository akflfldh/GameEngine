#include "EditorCoreComponent.h"

Quad::EditorCoreComponent::EditorCoreComponent()
	:mEngineObjectFlag(false)
{
}

Quad::EditorCoreComponent::~EditorCoreComponent()
{
}
void Quad::EditorCoreComponent::Initialize(Object* destObject)
{


}
void Quad::EditorCoreComponent::Start()
{

}

void Quad::EditorCoreComponent::Update(float delta)
{

}

void Quad::EditorCoreComponent::SetEngineObjectFlag(bool flag)
{
	mEngineObjectFlag = flag;
}

bool Quad::EditorCoreComponent::GetEngineObjectFlag() const
{
	return mEngineObjectFlag;
}

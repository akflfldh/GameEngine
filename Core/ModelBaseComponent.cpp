#include "Component/ModelBaseComponent.h"




Quad::ModelBaseComponent::ModelBaseComponent(SceneElement* sceneElement)
	:mDestSceneElement(sceneElement)
{
}

Quad::ModelBaseComponent::~ModelBaseComponent()
{

}

Quad::SceneElement* Quad::ModelBaseComponent::GetDestSceneElement() const
{
	return mDestSceneElement;
}

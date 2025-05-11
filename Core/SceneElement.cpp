#include "Core/SceneElement.h"



Quad::SceneElement::SceneElement(ESceneElementType sceneElementType)
	:mModel(this),mSceneElementType(sceneElementType)
{
	
}

Quad::SceneElement::~SceneElement()
{

}

const Quad::Model * Quad::SceneElement::GetModel() const
{
	return &mModel;
}

Quad::Model* Quad::SceneElement::GetModel()
{
	return &mModel;
}

Quad::ESceneElementType Quad::SceneElement::GetSceneElementType() const
{
	return mSceneElementType;
}

void Quad::SceneElement::Serialize(const std::string & tag)
{
	mModel.Serialize(tag);
	


}

void Quad::SceneElement::DeSerialize(const std::string& tag)
{
	mModel.DeSerialize(tag);

}

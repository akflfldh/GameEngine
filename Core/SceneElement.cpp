#include "Core/SceneElement.h"
#include"Parser/JsonParser.h"


Quad::SceneElement::SceneElement(std::string name,ESceneElementType sceneElementType)
	:mName(name),mSceneElementType(sceneElementType)
{
	
}

Quad::SceneElement::~SceneElement()
{

}

//const Quad::Model * Quad::SceneElement::GetModel() const
//{
//	return &mModel;
//}
//
//Quad::Model* Quad::SceneElement::GetModel()
//{
//	return &mModel;
//}

const std::string Quad::SceneElement::GetName() const
{
	return mName;
}

void Quad::SceneElement::SetName(const std::string& name)
{
	mName = name;

	//manager에게 조정필요?  내부에서 할것인가 
	//아니면 이름은 manager를 통해서 바꾸도록만 허용할것인가


}

Quad::ESceneElementType Quad::SceneElement::GetSceneElementType() const
{
	return mSceneElementType;
}

void Quad::SceneElement::Serialize(const std::string & tag)
{
//	mModel.Serialize(tag);
	JsonParser::Write("SceneElement_Name", mName);


}

void Quad::SceneElement::DeSerialize(const std::string& tag)
{
	//mModel.DeSerialize(tag);
	JsonParser::Read("SceneElement_Name", mName);
	JsonParser::IncrementCurrentIndex();

}

#include "EditorDirector/AttrUiSystemSelectObjectComponent.h"

void Quad::AttrUiSystemSelectObjectComponent::Update(float delta)
{

}

void Quad::AttrUiSystemSelectObjectComponent::SetSelectedObject(Object* object)
{
	mSelectedObject = object;
}

Quad::Object* Quad::AttrUiSystemSelectObjectComponent::GetSelectedObject() const
{
	return mSelectedObject;
}

void Quad::AttrUiSystemSelectObjectComponent::SetSubMeshIndex(int index)
{

	mSubMeshIndex = index;
}

int Quad::AttrUiSystemSelectObjectComponent::GetSubMeshIndex() const
{
	return mSubMeshIndex;
}

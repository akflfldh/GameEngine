#include "UIElement.h"
#include"UIManager.h"


UI::UIElement::UIElement(UIElementID id, const std::string& name)
	:mName(name),mID(id),mParent(nullptr),mActiveFlag(true), mTransform(std::make_unique<UIRectTransform>(this))
	,mMaterialID(0),mTextureID(0)
{



}

UI::UIElement::~UIElement()
{
}


void UI::UIElement::Update(float deltaTime)
{
	if (!mActiveFlag)
		return;

	//mTransform->Update() // RectTransform 업데이트 (월드 변환 계산 등)

	for (auto& child : mChildVector)
	{
		child->Update(deltaTime);
	}



}

UI::UIElementID  UI::UIElement::GetID() const
{
	return mID;

}

UI::UIElement* UI::UIElement::GetParent() const
{
	return mParent;
}


UI::UIRectTransform* UI::UIElement::GetRectTransform() const
{
	return mTransform.get();
}

void UI::UIElement::AddChildInternal(UIElement* child)
{
	if (!child)
		return;
	child->mParent = this;
	mChildVector.push_back(child);


}

void UI::UIElement::RemoveChildInternal(UIElement* child)
{
	if (child == nullptr)
		return;


	mChildVector.erase(std::find(mChildVector.begin(), mChildVector.end(), child));
	child->mParent = nullptr;


}

void UI::UIElement::SetCanvasInternal(UICanvas* canvas)
{
	mDestCanvas = canvas;
}

void UI::UIElement::SetActiveFlag(bool flag)
{
	mActiveFlag = flag;

}

bool UI::UIElement::GetActiveFlag() const
{

	return mActiveFlag;

}
const std::vector<UI::UIElement*> UI::UIElement::GetChildVector() const
{
	return mChildVector;
}


void UI::UIElement::SetParent(UIElement* parent)
{
	auto manager =	UIManager::GetInstance();
	manager->SetParent(parent, this);
	
}



void UI::UIElement::SetMaterialID(uint32_t materialID)
{
	mMaterialID = materialID;


}
void UI::UIElement::SetTextureID(uint32_t textureID)
{
	mTextureID = textureID;
}
uint32_t UI::UIElement::GetMaterialID() const
{
	return mMaterialID;
}
uint32_t UI::UIElement::GetTextureID() const
{
	return mTextureID;

}



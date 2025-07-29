#include "UIManager.h"
#include"UIElement.h"
#include"UICanvas.h"


UI::UIManager* UI::UIManager::GetInstance()
{ 
	static UIManager manager;
	return &manager;
}


UI::UIManager::UIManager()
	:mNextUICanvasID(0), mNextUIElementID(0)
{


}

UI::UIManager::~UIManager()
{
}

UI::UICanvasID UI::UIManager::CreateCanvas(const std::string& name, ECanvasSizeMode sizeMode)
{
	
	UICanvasID newID = mNextUICanvasID;
	if (!mFreeCanvasIDVector.empty())
	{
		newID = mFreeCanvasIDVector.back();
		mFreeCanvasIDVector.pop_back();
	}
	else
		mNextUICanvasID++;



	UICanvas* canvas = new UICanvas(newID, name, sizeMode);	///할당자를 사용하자
	mUICanvasTable[newID] = canvas;
	return newID;
}

void UI::UIManager::DestroyCanvas(UICanvasID id)
{
	auto it =	mUICanvasTable.find(id);
	if (it == mUICanvasTable.end())
		return;


	delete it->second;
	mUICanvasTable.erase(id);
	mFreeCanvasIDVector.push_back(id);



}


UI::UICanvas* UI::UIManager::GetCanvas(UICanvasID id) const
{
	
	auto it=	mUICanvasTable.find(id);
	
	return it == mUICanvasTable.cend() ? nullptr : it->second;


}

UI::UIElement* UI::UIManager::CreateUIElement(const std::string & name)
{
	
	UIElementID newID = mNextUIElementID;
	if (!mFreeUIElementIDVector.empty())
	{
		newID = mFreeUIElementIDVector.back();
		mFreeUIElementIDVector.pop_back();
	}
	else
		mNextUIElementID++;
	

	mUIElementTable[newID] = new UIElement(newID, name);
	return mUIElementTable[newID];

}

void UI::UIManager::DestoryUIElement(UIElementID uiElementID)
{

	auto it = mUIElementTable.find(uiElementID);

	if (it == mUIElementTable.end())
		return;


	UIElement* uiElement = it->second;
	delete uiElement;

	mUIElementTable.erase(uiElementID);
	mFreeUIElementIDVector.push_back(uiElementID);

}

UI::UIElement* UI::UIManager::GetUIElement(UIElementID uiElementID) const
{
	auto it =	mUIElementTable.find(uiElementID);
	

	return it == mUIElementTable.end() ? nullptr : it->second;

}

void UI::UIManager::AddUIElement(UICanvasID canvasID, UIElementID uiElementID)
{
	UICanvas* uiCanvas = GetCanvas(canvasID);
	UIElement * uiElement =	GetUIElement(uiElementID);

	AddUIElement(uiCanvas, uiElement);

}

void UI::UIManager::AddUIElement(UICanvas* canvas, UIElement* uiElement)
{
	uiElement->mParent->RemoveChildInternal(uiElement);
	uiElement->mParent = nullptr;

	canvas->AddChildInternal(uiElement);

}

void UI::UIManager::SetParent(UIElement* parent, UIElement* child)
{
	if (child->mParent != nullptr)
	{
		child->mParent->RemoveChildInternal(child);
	}
	else
	{
		auto & elementVector =	child->mDestCanvas->mChildUIElement;

		elementVector.erase(std::find(elementVector.begin(), elementVector.end(), child));

	}
	parent->AddChildInternal(child);
}

const std::unordered_map<UI::UICanvasID, UI::UICanvas*>& UI::UIManager::GetUICanvasAll() const
{

	return mUICanvasTable;

	// TODO: 여기에 return 문을 삽입합니다.
}

#include "UICanvas.h"

#include"UIElement.h"
#include"UIManager.h"
UI::UICanvas::UICanvas(UICanvasID id, const std::string& name, ECanvasSizeMode sizeMode)
	:mID(id),mName(name), mActiveFlag(true),mCanvasSizeMode(sizeMode)
{
}

UI::UICanvas::~UICanvas()
{
}




void UI::UICanvas::Update(float deltaTime)
{
	for (auto childUIElement : mChildUIElement)
	{
		childUIElement->Update(deltaTime);
	}




}




void UI::UICanvas::AddChild(UIElement* uiElement)
{
	auto manager = UIManager::GetInstance();

	manager->AddUIElement(this, uiElement);

}

UI::ECanvasSizeMode UI::UICanvas::GetSizeMode() const
{
	return mCanvasSizeMode;
}

void UI::UICanvas::SetSize(CoreMath::Vector2 size)
{
	if (mCanvasSizeMode == ECanvasSizeMode::eScreenSize)
	{
		mSize = size;	//항상 스크린영역의 크기와 동일
	}

}

bool UI::UICanvas::GetActiveFlag()const
{
	return mActiveFlag;
}

void UI::UICanvas::AddChildInternal(UIElement* uiElement)
{
	mChildUIElement.push_back(uiElement);

}


const std::vector<UI::UIElement*>& UI::UICanvas::GetChildUIElementAll() const
{
	return mChildUIElement;
}
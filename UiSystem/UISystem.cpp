#include "UISystem.h"
#include"UIManager.h"
#include"UICanvas.h"
#include<assert.h>
UI::UISystem* mInstance = nullptr;

UI::UISystem* GetInstance()
{
	if (mInstance == nullptr)
	{
		
		//log
		assert(0);
	}

	return mInstance;
}

UI::UISystem::UISystem(UIManager* uiManager)
	:mUIManager(uiManager), mFreeUIChannelID(1)
{
	if (mInstance != nullptr)
	{
		//log 
		assert(0);
	}

	mInstance = this;

}

UI::UISystem::~UISystem()
{
}

void UI::UISystem::Update(float deltaTime)
{
	const auto & uiCanvasTable =	mUIManager->GetUICanvasAll();


	for (const auto &canvasElement : uiCanvasTable)
	{
		UICanvas* canvas = canvasElement.second;
		if (canvas->GetActiveFlag())
		{
			//canvas->
		}
	}



}





UI::UIChannelID UI::UISystem::ReigsterChannel(UICanvasID canvasID,uint32_t clientWidth, uint32_t clientHeight)
{
	UICanvas * canvas = mUIManager->GetCanvas(canvasID);
	if (canvas == nullptr)
		return InvaildUIChannelID;

	ECanvasSizeMode canvasSizeMode = canvas->GetSizeMode();

	if (canvasSizeMode == ECanvasSizeMode::eScreenSize)
	{
		//중복검사
		for (const auto& channelContext : mUIChannelContextTable)
		{
			if (channelContext.second->mCanvasID == canvasID)
			{
				return InvaildUIChannelID;
			}
		}	
	}


	std::unique_ptr<UIChannelContext> channelContext = std::make_unique<UIChannelContext>();
	channelContext->mCanvasID = canvasID;
	channelContext->mClientWidth = clientWidth;
	channelContext->mClientHeight = clientHeight;

	mUIChannelContextTable[mFreeUIChannelID] = std::move(channelContext);

	return mFreeUIChannelID++;
}

void UI::UISystem::ReSize(UIChannelID channelID, uint32_t clientWidth, uint32_t clientHeight)
{
	
	auto channelContextIt = mUIChannelContextTable.find(channelID);

	if (channelContextIt == mUIChannelContextTable.end())
		return;

	UICanvasID canvasID = channelContextIt->second->mCanvasID;
	UICanvas * canvas =	mUIManager->GetCanvas(canvasID);
	if (canvas == nullptr)
		return;


	if (canvas->GetSizeMode() == ECanvasSizeMode::eScreenSize)
	{
	
		channelContextIt->second->mClientWidth = clientWidth;
		channelContextIt->second->mClientHeight = clientHeight;

		canvas->SetSize({ (float)clientWidth ,(float)clientHeight });
	}




}

bool UI::UISystem::GetChannelContext(UIChannelID channelID, UI::UIChannelContext & oChannelContext) const
{
	auto it =	mUIChannelContextTable.find(channelID);
	
	if (it == mUIChannelContextTable.end())
		return false;



	oChannelContext = *it->second;
	return true;

}

//void UI::UISystem::SubmitRenderItem()
//{
//
//
//}
//

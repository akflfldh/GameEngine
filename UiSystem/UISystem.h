#pragma once



#include<unordered_map>
#include<memory>

#include"UIType.h"

namespace UI
{
	class UICanvas;
	class UIManager;




	struct UIChannelContext
	{
		UICanvasID mCanvasID;
		
		uint32_t mClientWidth;
		uint32_t mClientHeight;

	
	};
	
	class UISystem
	{
	public:
		static UISystem* GetInstance();
		UISystem(UIManager* uiManager);
		~UISystem();


		//스크린사이즈캔버스는 단 하나의 채널과 연결가능, 
		//FIX사이즈 캔버스는 여러개의 채널과 연결가능 
		//만약 스크린사이즈캔버스일경우 이미 연결이 존재한다면 이함수는 실패  UIChannelID는 0을리턴
		UIChannelID ReigsterChannel(UICanvasID canvasID, uint32_t clientWidth, uint32_t clientHeight);
		void ReSize(UICanvasID mCanvas,uint32_t mClientWidth,uint32_t mClientHeight);

		void Update(float deltaTime);

		bool GetChannelContext(UIChannelID channelID, UI::UIChannelContext& oChannelContext) const;

		
	private:
		static UISystem* mInstance;

	private:
		std::unordered_map< UIChannelID, std::unique_ptr<UIChannelContext>> mUIChannelContextTable;
		UIManager* mUIManager;



		UIChannelID mFreeUIChannelID;


	};
}
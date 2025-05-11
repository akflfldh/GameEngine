#pragma once


#include"Predeclare.h"

#include"Core/Event.h"

#include"Ray.h"
namespace Quad
{
	class NotifyDragAndDropEvent:public Event
	{
	public:
		NotifyDragAndDropEvent();
		~NotifyDragAndDropEvent() = default;
		
		void SetContentItem(ContentItem * contentItem); 
		ContentItem* GetContentItem() const;

		void SetScreenPosX(int x);
		void SetScreenPosY(int y);

		int GetScreenPosX() const;
		int GetScreenPosY() const;

		void SetLButtonUpState(bool state);
		bool GetLButtonUpState()const;

		//각 시스템마다,맵마다 설정할수있다.(한맵에 여러엔티티들이 여러번ray계산을 하는것을방지할수있다)
		void SetRay(const Ray& ray);
		const Ray& GetRay() const;

	private:
		ContentItem* mContentItem;

		int mScreenPosX;
		int mScreenPosY;
		
		Ray mRay;
		bool mLButtonUp = false;

	};

}

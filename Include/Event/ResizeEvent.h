#pragma once


#include"Predeclare.h"
#include"header.h"
#include"Core/Event.h"



namespace Quad
{
	class ResizeHandleEvent : public Event
	{
	public:
		ResizeHandleEvent(const std::string& name);
		~ResizeHandleEvent() =default;

		void SetNewSize(DirectX::XMFLOAT3 size);
		DirectX::XMFLOAT3 GetNewSize()const;

		void SetDirection(int dir);
		int GetDirection() const;

	private:
		DirectX::XMFLOAT3 mNewSize;
		int mDirection;			

	};
}

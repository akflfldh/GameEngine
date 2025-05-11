#include "WindowControlEntity.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"System.h"



void Quad::WindowControlEntity::Initialize()
{
	UiEntity::Initialize();
	System* system = GetSystem();
	system->RegisterEntityToEvent("WindowResizeEvent", this);
}

Quad::WindowControlEntity* Quad::WindowControlEntity::Create(Map* map, int mapLayerIndex)
{

	WindowControlEntity* ob = static_cast<WindowControlEntity*>(map->CreateObject("WindowControlEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();


	return nullptr;
}


Quad::WindowControlEntity::WindowControlEntity(const std::string& name)
	:UiEntity(name), mWidth(1), mHeight(1), mWindowWidth(DEFAULT_WINDOW_WIDTH), mWindowHeight(DEFAULT_WINDOW_HEIGHT)
{

}



void Quad::WindowControlEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::WindowControlEntity::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);






}

//void Quad::WindowControlEntity::SetWidthHeight(float width, float height)
//{
//	mWidth = width;
//	mHeight = height;
//
//	GetTransform().SetScaleLocal({ width ,height,1.0f });
//}
//
//float Quad::WindowControlEntity::GetWidth() const
//{
//	return mWidth;
//}
//
//float Quad::WindowControlEntity::GetHeight() const
//{
//	return mHeight;
//}
//


UINT Quad::WindowControlEntity::GetWindowWidth() const
{
	return mWindowWidth;
}

UINT Quad::WindowControlEntity::GetWindowHeight() const
{
	return mWindowHeight;
}

void Quad::WindowControlEntity::SetKeepVisibleFlag(bool flag)
{
	mKeepVisibleFlag = flag;
}

bool Quad::WindowControlEntity::GetKeepVisibleFlag() const
{
	return mKeepVisibleFlag;
}

int Quad::WindowControlEntity::GetKeepVisibleBaseVerticalLine() const
{
	return mKeepVisibleBaseVerticalLine;
}

int Quad::WindowControlEntity::GetKeepVisibleBaseHorizontalLine() const
{
	return mKeepVisibleBaseHorizontalLine;
}

int Quad::WindowControlEntity::GetKeepVisibleBaseVerticalLineOffset() const
{
	return mKeepVisibleBaseVerticalOffset;
}

int Quad::WindowControlEntity::GetKeepVisibleBaseHorizontalLineOffset() const
{
	return mKeepVisibleBaseHorizontalOffset;
}


void Quad::WindowControlEntity::SetPosition(float x, float y)
{
	if (!GetKeepVisibleFlag())
	{
		//KEEP Visible이 켜져있으면 이 좌표설정이 의미가없다.
		//항상고정되게 보여지니깐

		GetTransform().SetPositionLocal({ x,y,5.0f });


	}


}

void Quad::WindowControlEntity::InitCreating()
{
	UiEntity::InitCreating();

}

void Quad::WindowControlEntity::SetKeepVisibleBaseVerticalLine(int lineNum)
{
	mKeepVisibleBaseVerticalLine = lineNum;
}

void Quad::WindowControlEntity::SetKeepVisibleBaseHorizontalLine(int lineNum)
{
	mKeepVisibleBaseHorizontalLine = lineNum;
}

void Quad::WindowControlEntity::SetKeepVisibleBaseVerticalLineOffset(int offset)
{
	mKeepVisibleBaseVerticalOffset = offset;
}

void Quad::WindowControlEntity::SetKeepVisibleBaseHorizontalLineOffset(int offset)
{
	mKeepVisibleBaseHorizontalOffset = offset;
}

void Quad::WindowControlEntity::ChangeWindowSize(UINT width, UINT height)
{

	mWindowWidth = width;
	mWindowHeight = height;


	if (GetKeepVisibleFlag())
	{
		float positionX = 0;
		float positionY = 0;
		//창의 사이즈가 변해도 계속 보인다.
		switch (GetKeepVisibleBaseVerticalLine())
		{
		case 0:	//left
			positionX = GetKeepVisibleBaseVerticalLineOffset();
			break;

		case 1://right
			positionX = width - GetKeepVisibleBaseVerticalLineOffset();
			break;
		}

		switch (GetKeepVisibleBaseHorizontalLine())
		{
		case 0:	//top
			positionY = 3000 - GetKeepVisibleBaseHorizontalLineOffset();
			break;

		case 1: //bottom
			positionY = (3000 - height) + GetKeepVisibleBaseHorizontalLineOffset();
			break;

		}

		GetTransform().SetPositionLocal({ positionX,positionY,GetDepth() });

	}
	//GetTransform().SetScaleLocal({ 2.0f, ((float)mHeight / (float)mWindowHeight),1.0f });
	//GetTransform().SetPositionWorld({ 0.0f,1.0f - ((mHeight / mWindowHeight)) / 2.0f, 0.0f });

}

void Quad::WindowControlEntity::SetDepth(float depth)
{
	mDepth = depth;
}

float Quad::WindowControlEntity::GetDepth() const
{
	return mDepth;
}


#include "FrameWindowMainSceneDirector.h"

#include"PanelUiEntity.h"
#include"System/FrameWindowUiSystem.h"
#include"ContentItemUiPanelEntity.h"
#include"ContentIProjecttemUiEntity.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ButtonUiEntitIy.h"
#include"TextBox.h"
#include"TextBoxPanel.h"

#include"ProjectDirector.h"


#include"ScrollListPanel.h"


Quad::FrameWindowMainSceneDirector::FrameWindowMainSceneDirector()
	: mCurrSceneDirector(nullptr)
	
{
}

void Quad::FrameWindowMainSceneDirector::Initialize(FrameWindowUiSystem* frameWindowUiSystem)
{
	mFrameWindowUiSystem = frameWindowUiSystem;
	mProjectSelectSceneDirector.Initialize(frameWindowUiSystem);
	mAnimStateEditDirector.Initialize(frameWindowUiSystem);
	


	//mCurrSceneDirector = &mProjectSelectSceneDirector;
	SetProjectSelectSecene();
	//mCurrSceneDirector = &mProjectSelectSceneDirector;




}






void Quad::FrameWindowMainSceneDirector::SetNullScene()
{
	if (mCurrSceneDirector != nullptr)
		mCurrSceneDirector->InActiveScene();
	
	mCurrSceneDirector = nullptr;
}

void Quad::FrameWindowMainSceneDirector::SetProjectSelectSecene()
{

	if(mCurrSceneDirector!=nullptr)
		mCurrSceneDirector->InActiveScene();

	mProjectSelectSceneDirector.ActiveScene();
	mCurrSceneDirector = &mProjectSelectSceneDirector;




}

void Quad::FrameWindowMainSceneDirector::SetAnimStateEditScene(Object* destObject)
{
	if (mCurrSceneDirector != nullptr)
		mCurrSceneDirector->InActiveScene();


	mAnimStateEditDirector.ActiveScene(destObject);

	mCurrSceneDirector = &mAnimStateEditDirector;
}



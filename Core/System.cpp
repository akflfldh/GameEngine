#include "Core/System.h"
#include"RenderSystem.h"
#include"Core/WindowResizeEvent.h"
#include"Core/SystemState.h"
#include"BaseWindowController.h"
//#include"Window.h"
//#include"TextCharacterEntity.h"
//#include"EditorSystem.h"


#include"GameMapSystem.h"



namespace Quad
{



	System::System(ESystemType systemType, ESystemID systemID, const std::string& name, Quad::ESystemMode systemMode)
		: mName(name),mEventFocusFlag(false),mSystemType(systemType),mSystemID(systemID),mState(nullptr),
		mPlayModeState(false),mActiveState(false),mSystemMode(systemMode)
	{



	}

	System::~System()
	{
		delete mState;
	}

	void System::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
	{
		mClientRect.left = mClientRect.top = 0;
		SetClientWidth(clientWidth);
		SetClientHeight(clientHeight);


		ESystemMode systemMode = GetSystemMode();

		switch (systemMode)
		{
		case ESystemMode::eSwitchMode:

		{
			mEditMap = map;
			mCurrentMap = mEditMap;

			mRuntimeMap = GameMapSystem::CreateMap(this, "", GetIsUserGameSystem());
			//mRuntimeMap.Initialize(this, { 0,1.0f,0 });

			//SetViewPort(0, 0, mClientWidth, clientHeight, 0.0f, 1.0f);
		}
		break;
		case ESystemMode::eRunTimeMode:
		{
			mRuntimeModeState = true;
			mEditMap = nullptr;
			mRuntimeMap = map;
			mCurrentMap = mRuntimeMap;
		}
		break;
		}


		SetViewPort(0, 0, mClientWidth, clientHeight, 0.0f, 1.0f);




	
		//MAP의 초기 VIEWPORT는 기본설정 (화면전체)
		//또 창의 크기가변하면 따라 변하든지 조정이 필요할텐데.



	}

	void System::Update(float deltaTime, bool playMode)
	{
		mCurrentMap->Update(deltaTime);
	}

	void System::EndUpdate(float deltaTime)
	{
		if (mTransformDirtyObjectVector.size() != 0)
		{
			int a = 2;
		}

		for (int i = 0; i < mTransformDirtyObjectVector.size(); ++i)
		{


			SceneElement* object = mTransformDirtyObjectVector[i];
		//	if (object == nullptr)
			//	continue;

	/*		if (object->GetUniqueID() == 0)
				continue;*/

			object->GetTransform().SetDirtyFlag(false);
			
		}


		mTransformDirtyObjectVector.clear();
		

		mCurrentMap->EndUpdate(deltaTime);
		//mTrnasformDirtyObjectVector가 다시 채워질수있다.
		//그들은 프레임시작앞부분에 world Matrix를 자식들에게 전파한다.
		//혹시나 EndUpdate에서 죽은 object들이있다면 적절히 vector에서 걸러내야한다.

		for(int i=0; i<mTransformDirtyObjectVector.size();)
		{
			SceneElement* element = mTransformDirtyObjectVector[i];

			if (element->GetSceneElementType() == ESceneElementType::eObject)
			{
				Object* object = static_cast<Object*>(element);
				if (object->GetKilledState())
				{
					mTransformDirtyObjectVector[i] = mTransformDirtyObjectVector.back();
					mTransformDirtyObjectVector.pop_back();
					continue;
				}
			}

			i++;

		}


	}

	void System::OnResize(UINT clientWidth, UINT clientHeight)
	{
		UINT preClientWidth = GetClientWidth();
		UINT preClientHeight = GetClientHeight();
		SetClientWidth(clientWidth);
		SetClientHeight(clientHeight);

		//mViewPort.TopLeftY = ((WindowController*)GetController())->GetTitleBarHeight();
		mViewPort.Width = GetClientWidth();
		mViewPort.Height = GetClientHeight();

		if (mCurrentMap != nullptr)
		{
			//mCurrentMap->OnResize(clientWidth, clientHeight);

			if (mCurrentMap->GetViewportAutoFlag(0))
			{
				mCurrentMap->SetViewPort(mViewPort.TopLeftX, mViewPort.TopLeftY, mViewPort.Width, mViewPort.Height, mViewPort.MinDepth,
					mViewPort.MaxDepth);
			}

			if (mCurrentMap->GetViewportAutoFlag(1))
			{
				mCurrentMap->SetViewPort(mViewPort.TopLeftX, mViewPort.TopLeftY, mViewPort.Width, mViewPort.Height, mViewPort.MinDepth,
					mViewPort.MaxDepth, 1);
			}
		}


		std::vector<Object*> objectVector;
		mEventTable.GetObjectVector("WindowResizeEvent", objectVector);
		WindowResizeEvent windowResizeEvent;
		windowResizeEvent.SetWindowSize(clientWidth, clientHeight);
		windowResizeEvent.SetPreWindowSize(preClientWidth, preClientHeight);
		for (int i = 0; i < objectVector.size(); ++i)
		{
			objectVector[i]->OnEvent(&windowResizeEvent);
		}


	}

	void System::SetDefaultController(const std::string& controllerName)
	{
		mDefaultControllerName = controllerName;
	}

	const std::string& System::GetDefaultControllerName() const
	{
		return mDefaultControllerName;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	void System::SetClientWidth(UINT clientWidth)
	{
		mClientWidth = clientWidth;
		mClientRect.right = clientWidth;
	}

	void System::SetClientHeight(UINT clientHeight)
	{

		if (mClientHeight != clientHeight)
		{
			int a = 2;
			OutputDebugString(L"kk");
		}
		mClientHeight = clientHeight;
		mClientRect.bottom = clientHeight;
	}

	UINT System::GetClientWidth() const
	{

		return mClientWidth;
	}

	UINT System::GetClientHeight() const
	{
		return mClientHeight;
	}

	RECT System::GetClientRect() const
	{
		return mClientRect;
	}

	void System::SetViewPort(FLOAT topLeftX, FLOAT topLeftY, FLOAT width, FLOAT height, FLOAT minDepth, FLOAT maxDepth)
	{
		mViewPort.TopLeftX = topLeftX;
		mViewPort.TopLeftY = topLeftY;
		mViewPort.Width = width;
		mViewPort.Height = height;
		mViewPort.MinDepth = minDepth;
		mViewPort.MaxDepth = maxDepth;

		if(mCurrentMap!=nullptr)
		mCurrentMap->SetViewPort(mViewPort.TopLeftX, mViewPort.TopLeftY, mViewPort.Width, mViewPort.Height, mViewPort.MinDepth,
			mViewPort.MaxDepth);
	}

	void System::SetViewPort(const D3D12_VIEWPORT& viewport)
	{
		mViewPort = viewport;
		mCurrentMap->SetViewPort(mViewPort.TopLeftX, mViewPort.TopLeftY, mViewPort.Width, mViewPort.Height, mViewPort.MinDepth,
			mViewPort.MaxDepth);
	}

	D3D12_VIEWPORT System::GetViewPort() const
	{
		return mViewPort;
	}

	bool System::GetEventFocusFlag() const
	{
		return mEventFocusFlag;
	}

	void System::SetEventFocusFlag(bool flag)
	{
		mEventFocusFlag = flag;
	}

	Quad::Map* System::GetMap() const
	{
		return mCurrentMap;
	}

	void System::SetMap(Map* map)
	{
		if (mCurrentMap == mEditMap)
		{
			mEditMap = map;
		}
		else
		{
			mRuntimeMap = map;
		}
		mCurrentMap = map;
		
	}

	void System::GetRenderSettingItem(RenderSettingItem& oRenderSettingItem) const
	{


		Quad::Map* map = GetMap();
		//map->GetCamera

		oRenderSettingItem.mCamera = map->GetMainCamera();
		oRenderSettingItem.mViewPort = GetViewPort();

	}

	bool System::RemoveObject(Object* object)
	{		
		return mCurrentMap->RemoveObject(object);
	}

	//void System::GetEntity(std::vector<Object*>& oEntityVector)
	//{
	//}

	const std::vector<MapLayer> System::GetEntity()
	{

		Map* map = GetMap();
		return map->GetObjectsInViewFrustum();

		// TODO: 여기에 return 문을 삽입합니다.
	}

	void System::HandleEvent(Event* event)
	{
		if (!GetActiveState())
			return;

		//문자열 비교는 비용이 많이들어간다.
		const std::string& eventName = event->GetName();

		SystemState* currState = GetSystemState();

		SystemState* newState = currState->HandleEvent(*this, event);
		if (newState != nullptr)
		{
			currState->ExitState(*this);
			delete currState;

			newState->EnterState(*this);
			SetSystemState(newState);
		}

	}

	ESystemType System::GetSystemType() const
	{
		return mSystemType;
	}

	ESystemID System::GetSystemID() const
	{
		return mSystemID;
	}

	void System::SetController(BaseWindowController* controller)
	{
		mController = controller;
	}

	Controller* System::GetController() const
	{
		return mController;
	}

	void System::RegisterEntityToEvent(const std::string& eventName, Object* object)
	{
		mEventTable.AddObject(eventName, object);
	}

	void System::RemoveEntityInEventTable(const std::string& eventName, Object* object)
	{

		mEventTable.RemoveObject(eventName, object);

	}

	const EventTable* System::GetEventTable() const
	{
		return &mEventTable;
	}

	EventTable* System::GetEventTable()
	{
		return &mEventTable;
	}

	BaseWindow* System::GetWindow() const
	{
		if (mController == nullptr)
		{
			//MessageBox(nullptr, L"컨트롤러가 null이다", L"error", 0);
			return nullptr;
		}
		return mController->GetWindow();




	}


	void System::OnRuntimeMode()
	{
		if (GetSystemMode() == ESystemMode::eRunTimeMode)
			return;



		if (mRuntimeModeState == true)
			return;


		if (mEditMap != nullptr)
		{
			//mEditMap->Seralize();


			//유저의카메라로 main카메라설정
			//유저의카메라가 없으면 아무것도안한다(다른 editor시스템에 해당)
#ifdef EditorMode
		//SetMainUserCamera();
			//mEditMap->SetMainUserCamera();
#endif		
			
			mEditMap->Serialize();

			//오브젝트 직렬화
			//RunTimeMap 구현

			mRuntimeMap->DeSerialize();
			//	mRuntimeMap->SetPauseState(true);
		
		}
		//currentmap =runtimemap
		mRuntimeModeState = true;
		auto systemState = GetSystemState();
		if (systemState)
			systemState->Reset();

		mCurrentMap = mRuntimeMap;
		mCurrentMap->Start();
	}

	void System::OffRunTimeMode()
	{
		if (GetSystemMode() == ESystemMode::eRunTimeMode)
			return;


		if (mRuntimeModeState == false)
			return;

		mRuntimeMap->PrintObjectName();

		//runtimeMap reset		
		mRuntimeMap->Reset();
		//runtime object table리셋(정확히는 그 맵에있는 오브젝트들을 테이블에서 제거)



	


		//currentamap= editmap;
		mCurrentMap = mEditMap;

		if (mEditMap != nullptr)
		{

#ifdef EditorMode
			//다시 Editor용 카메라로 설정해준다
		//	mEditMap->SetMainEngineCamera();
#endif

		}
		GetSystemState()->Reset();

		mRuntimeModeState = false;
	}

	bool System::GetRuntimeModeState() const
	{
		return mRuntimeModeState;
	}



	void System::SetPause(bool state)
	{

		mCurrentMap->SetPauseState(state);
	}

	bool System::GetPause() const
	{
		return mCurrentMap->GetPauseState();
	}

	void System::SetSelectObject(Object* object)
	{
		mSelectObject = object;
	}

	Object* System::GetSelectObject() const
	{
		return mSelectObject;
	}

	//void System::OnGizmo(Object * object)
	//{

	//	//내가볼땐 자식으로 묶어야돼

	//	//부모자식관계를 다시 맺는것도 비용이들고 어차피 다른씬그래프에있으니
	//	//Gizmo가 움직일때 그 선택된 object도 같은 위치로 움직이면된다.




	//	//GetMap()->ChangeParentObject(object, &mGizmo);

	//	//mGizmo.GetTransform().SetPositionLocal({ 0,0,0 });

	//	//mGizmo.GetTransform().SetPositionWorld(object->GetTransform().GetPositionWorld());


	//	
	//	/*mGizmo->SetDestObject(object);

	//	mGizmo->OnGizmo();*/

	//}

	//void System::OffGizmo()
	//{
	//	/*mGizmo->SetDestObject(nullptr);

	//	mGizmo->OffGizmo();*/

	//	/*mGizmo.SetDrawFlag(false);
	//	mGizmo.SetActiveFlag(false);
	//	mGizmo.SetEntireDrawFlag(false);
	//	mGizmo.SetEntireSelectAvailableFlag(false);*/
	//}

	//const Gizmo* System::GetGizmo() const
	//{
	//	return mGizmo;
	//}

	void System::AddTransformDirtyObject(SceneElement* object)
	{
		mTransformDirtyObjectVector.push_back(object);





	}

	// std::vector<Object*>& System::GetTransformDirtyObjectVector() 
	//{

	//	return mTransformDirtyObjectVector;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}

	void System::SetActiveState(bool state)
	{
		mActiveState = state;
	}

	bool System::GetActiveState() const
	{
		return mActiveState;
	}

	const std::string& System::GetName() const
	{
		return mName;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	ESystemMode System::GetSystemMode() const
	{
		return mSystemMode;
	}

	SystemState* System::GetSystemState() const
	{

		return mState;
	}

	void System::SetSystemState(SystemState* systemState)
	{
		mState = systemState;
	}

	void System::SetInitSetting(bool isUserGameSystem)
	{

		mIsUserGameSystem = isUserGameSystem;

	}

	bool System::GetIsUserGameSystem() const
	{
		return mIsUserGameSystem;
	}




}
#pragma once



#include"header.h"

#include"Map/Map.h"
#include"Event/EventTable.h"
//#include"Gizmo.h"

#include"Core/CoreDllExport.h"




namespace Quad
{

	enum class ESystemMode
	{ 
		eSwitchMode=0,	//edit <-> runtime 왔다갔다 할수있는 모드
		eRunTimeMode	//오직 Runtime으로만 동작하는 모드	
	};




	class CORE_API_LIB System
	{
	public:
		System(ESystemType systemType, ESystemID systemID,const std::string & name ,Quad::ESystemMode systemMode  =Quad::ESystemMode::eRunTimeMode);


		virtual ~System() = 0;

		virtual void Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map);
		virtual void Update(float deltaTime,bool playMode= true);
		virtual void EndUpdate(float deltaTime) ;
		virtual void OnResize(UINT clientWidth, UINT clientHeight);
		 

		void SetDefaultController(const std::string& controllerName);
		const std::string& GetDefaultControllerName()const;


		void SetClientWidth(UINT clientWidth);
		void SetClientHeight(UINT clientHeight);
		UINT GetClientWidth()const;
		UINT GetClientHeight()const;

		RECT GetClientRect()const;

		void SetViewPort(FLOAT topLeftX, FLOAT topLeftY, FLOAT width,
			FLOAT height, FLOAT minDepth, FLOAT maxDepth);
		void SetViewPort(const D3D12_VIEWPORT & viewport) ;


		D3D12_VIEWPORT GetViewPort() const;

		bool GetEventFocusFlag() const;
		void SetEventFocusFlag(bool flag);

		Quad::Map* GetMap() const;
		void SetMap(Map* map);
		virtual void GetRenderSettingItem(RenderSettingItem& oRenderSettingItem) const;

		virtual void AddEntity(Object* entity,int mapLayer = 0 , bool egineEditObject=false) = 0;

		//시스템에따라 오버라이딩하여 다르게 처리할수도있다.
		//디폴트는 현재map의 RemoveObject호출
		virtual bool RemoveObject(Object* object);
		//virtual void GetEntity(std::vector<Object*>& oEntityVector);
		virtual const std::vector<MapLayer> GetEntity()final;
		virtual void HandleEvent(Event* event);


		ESystemType GetSystemType()const;
		ESystemID GetSystemID() const;


		void SetController(BaseWindowController* controller);
		Controller* GetController() const;

		void RegisterEntityToEvent(const std::string& eventName, Object* object);
		void RemoveEntityInEventTable(const std::string& eventName, Object* object);

		const EventTable* GetEventTable() const;
		EventTable* GetEventTable();


		BaseWindow* GetWindow()const;




		void OnRuntimeMode();
		void OffRunTimeMode();

		bool GetRuntimeModeState() const;





		
		void SetPause(bool state);
		bool GetPause()const;




		void SetSelectObject(Object* object);
		Object* GetSelectObject()const;


	//	void OnGizmo(Object* object);
	///	void OffGizmo();
		//const Gizmo* GetGizmo() const;

		
		
		void AddTransformDirtyObject(SceneElement* object);
		//void AddMaterialDirtyObject(Object* object);
		
		//const std::vector<Object*>& GetTransformDirtyObjectVector() const;



		void SetActiveState(bool flag);
		bool GetActiveState() const;



		
		const std::string& GetName() const;

		ESystemMode GetSystemMode() const;


	protected:
		SystemState* GetSystemState() const;
		void SetSystemState(SystemState* systemState);

		


		void SetInitSetting(bool isUserGameSystem);
		bool GetIsUserGameSystem() const;


	private:
		std::string mName;
		ESystemType mSystemType;
		ESystemID mSystemID;
		SystemState* mState;
		EventTable mEventTable;
		UINT mClientWidth;
		UINT mClientHeight;
		RECT mClientRect;
		D3D12_VIEWPORT mViewPort;
		Quad::Map* mEditMap;
		Quad::Map* mRuntimeMap;
		Quad::Map* mCurrentMap;


		bool mEventFocusFlag;
		std::string mDefaultControllerName;
		BaseWindowController* mController;

		bool mPlayModeState = mPlayModeState;
		bool mPuaseState = false;

		//systemMode가 switchMode일때 runtime임을 나타내는 상태변수
		bool mRuntimeModeState = false;

		Object* mSelectObject =nullptr;
		//Gizmo* mGizmo;//모든 맵에서 사용한다.




		//더티플래그가 켜진 오브젝트들에대한 리스트 (매프레임마지막에 이리스트의 오브젝트들의 더티플래그가 초기화될것이다(그런용도))
							//<object handle, ,object unique id >
	//	std::vector<std::pair<QOBHANDLE, unsigned long long>> mTransformDirtyObjectVector;
	//	std::vector<ObjectSmartPointer> mTransformDirtyObjectVector;
		std::vector<SceneElement*> mTransformDirtyObjectVector;
		std::vector<Object*> mMaterialDirtyObjectVector;


		bool mIsUserGameSystem = false;


		bool mActiveState;


		ESystemMode mSystemMode;

	};



}
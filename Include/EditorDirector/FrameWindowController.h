#pragma once

#include"Predeclare.h"


#include"DockingWindowController.h"
#include"Singleton.h"


namespace Quad
{
	class FrameWindowController:public DockingWindowController,public Singleton<FrameWindowController>
	{
		friend class ProjectDirector;

		//leaf 윈도우노드는 가장작은단위 창
		//leaf노드가 아닌노드는 분할된노드를 가지는 영역 
		//left,right,top,bottom 은 창의 테두리가 분할전 어떤영역에 속했는지 나타내고
		//움직이거나 사이즈가변할때 그 테두리가속한 영역 안에있는 모든 창들이 영향을 받는다.
		//이진트리 계층구조로 표현한다.
		//항상 자식윈도우가 도킹될때(추가될때)는 기존의 창(leaf노드 )에 추가되는것
		//즉 분할되는것이다. leaf노드가아닌 노드 즉 영역에 추가되는것이 아니다.

		struct WindowNode
		{

			//type :region, window 사실 window면 leaf node
			TaskWindow* mWindow=nullptr;
			WindowNode* mParentWindowNode = nullptr;
			WindowNode* mLeftChildWindowNode=nullptr;		//left, top
			WindowNode* mRightChildWindowNode =nullptr;		//right ,bottom
			//막대인스턴스 포인터 
			//막대에 hover하면 화살표표시로 , 클릭해서 드래그하면 이 막대인스턴스를 가지는 윈도우노드에
			//영향을준다.각각의 창 size조절 각 창은 자신의 창사이즈만 조절하고끝나니 문제발생여지없음.
			SizeControllerStickEntity* mSizeControllerStick=nullptr;		//region windownode일 경우에만 가질것이다.
			


			float mWidth;
			float mHeight;
			float mDeltaX;	//사이즈에대한
			float mDeltaY;

			//당연히 도킹되었으니 프레임윈도우영역에서의 좌표
			float mPositionX;
			float mPositionY;
			float mShiftX;	//위치에대한
			float mShiftY;

		};

	public:
		FrameWindowController(RenderSystem * renderSystem);
		~FrameWindowController() = default;

		virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList,
			MeshManager* meshManager, MaterialManager* materialManager, TextureManager* textureManager,
			MapManager* mapManager, TaskWindow* window,
			 System* gameWorldSystem, WindowChromeSystem* windowLayoutSystem,FrameWindowDockingSystem*
			frameWindowDockingSystem,
			DescriptorHeapManagerMaster* descriptorHeapManagerMaster);


		virtual void Update(float deltaTime)override;
		virtual void Draw()override;

		virtual void OnResize(int clientWidth, int clientHeight, int direction) override;

		//virtual void AddEvent(Event* pEvent) override;
		void InitChildWindowSetting(TaskWindow* gameWindow, TaskWindow* attributeWindow, TaskWindow * fileUiWindow);
		//srcWindow  새롭게 추가하는윈도우, destWindow는 이미 도킹되어있어서 분할될 윈도우
		void AddChildWindow(TaskWindow* srcWindow, TaskWindow* destWindow, EDockingDirection direction);
		virtual void WindowActivate(int wmActivateState)override;
		virtual void WindowMove(int preWindowPosX, int preWindowPosY, int currWindowPosX, int currWindowPosY) override;
		virtual void HandleWindowReStore();
		virtual void WindowMinimize() override;





		//도킹된자식윈도우가 빠져나가서 새로자식윈도우들을 조정할떄
		static void ReleaseDockingChildWindow(TaskWindow * window);

		//현재 moving window가 다음 매개변수 window에 도킹된다.
		static void SetDockingChildWindow(TaskWindow* window, EDockingDirection direction);
		

		//도킹되지않은 자식윈도우가 타이틀바를 클릭해서이동할떄 
		//마우스가 어떤 도킹된윈도우 위로 올라왔는지확인하고 그 윈도우의 도킹시스템을 작동시킨다.
		//벗어나면 도킹시스템종료 도킹처리수행은 없음 여전히 마우스가 클릭되어 윈도우가 이동중
		static void OnChildWindowDockingSystem(int screenPositionX, int screenPositionY);
		// 도킹 시스템 종료,//선택박스위에서 마우스가 LButtonUp이 되었느지 확인하여 도킹처리수행
		static void OffChildWindowDockingSystem(int screenPositionX, int screenPositionY);
		
		static void SetMovingWindow(TaskWindow* window);
		static TaskWindow* GetMovingWindow();


		void MoveSizeControllerStick(SizeControllerStickEntity * stick, float deltaX, float deltaY);

	private:
		void HandleEvent(Event* pEvent) override;
		void HandleWindowRequestEvent(Event* pEvent);


		WindowNode* FindWindowNode(TaskWindow* window);

		//이 마지막매개변수 옵션은 자식윈도우를 resize할때 재귀적으로 resize가호출되는것을 막기위해서
		void ModifyChildWindowSizePos(WindowNode* parentNode, int direction);



		static void ReleaseDockingChildWindowRestructRegionNode(WindowNode* parentWindowNode, WindowNode* otherChildWindowNode
			, WindowNode* windowNode,int direction);

		private:
			//event handle 

			virtual void MouseMove(Event* event) override;


	private:
	
		//여기에있는 windowNode가 삭제되지않는다. 초기화시점에 모든윈도우에대해서 가지는거지
		//윈도우가 도킹상태인가 아닌가, 꺼진상태이다 등등으로 동작의 변화가있는거고
		std::vector<TaskWindow*> mChildWindowVector;

		
		//계층구조
		WindowNode* mRootWindowNode;	//전체 영역을 나타낸다. //즉 프레임윈도우가 된다라고 볼수있다.
		//정렬해야한다면 할당, 그렇지않으면 그냥 정적변수로
		//이진트리에서 항상 리프노드는 두개씩존재함으로, 벡터를 사용해서 트리를표현하는것도 고려해보자


		//도킹되지않고 외부에서움직이는 윈도우
		//도킹될가능성이있는 윈도우이다.
		static TaskWindow* mMovingWindow ;



		FrameWindowDockingSystem* mFrameWindowDockingSystem;


		int margin = 5;

	};
}


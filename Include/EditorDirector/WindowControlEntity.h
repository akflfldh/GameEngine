#pragma once

#include"Predeclare.h"
#include"Object/UiEntity.h"

#include"EditorMacro.h"

namespace Quad
{
	//windowControlEntity는 ndc공간으로 변환해서 transform위치,스케일값,등을 정한다. 

	//메리트가있는가?

	//일단 어떻게 될지몰르니. 

	//창컨트롤유닛은 무조건 2차원 직사각형
	//그럼 초기화에서 애초에 mesh는 Rectangle 확정시키자.
	//그리고 width와 heigh를 계산한다. 디폴트 윈도우크기를 600x400을 기준으로하고
	//그상태에서 먼저초기 설정하도록한다. 
	//그리고 자동으로 ndc공간으로 변환해서 처리되도록한다. 
	//그리고 윈도우 컨트롤 엔티티들은 창의 크기가 변해도  시각적으로 일정한 크기를 유지함으로
	//  그렇게 처리되도록 이베이스클래스에서변환해서설정하는부분을
	//넣는다.


	//NDC공간에서의 좌표를가진다.(위치,너비,높이)

	//위치는 어떻게해야할까 위치가 고정되기원할수도있고 크기에따라 비례해서 변하는것을 원할수도있을거같은데.
	//타이틀바는 고정되는거고 
	// ndc공간에서의 스케일값도 너비가 계속 윈도우사이즈와동일한것을 원할수도있고 높이는 60으로 계속유지되어서 보여지도록하고싶을수있고
	// 결국은 이거는 엔티티마다 다르다.
	// 
	//안그러면 change window size는 오버라이딩 하던가
	//일단 오버라이딩해보자.
	// 
	//맨초기 윈도우 기본사이즈를 기준으로  엔티티의 너비와 높이를 초기설정(모든 윈도우 컨트롤 엔티티들에대해서 적용되는)


	class WindowControlEntity :public UiEntity
	{
	public:
		WindowControlEntity(const std::string & name="");
		virtual ~WindowControlEntity() = default;

		virtual void Initialize() override ;


		static WindowControlEntity* Create(Map* map, int mapLayerIndex = 0);



		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;

		
	//	virtual void SetWidthHeight(float width, float height);
		virtual void ChangeWindowSize(UINT width, UINT height);
		void SetDepth(float depth);
		float GetDepth()const;

		//float GetWidth() const;
		//float GetHeight() const;

		UINT GetWindowWidth() const;
		UINT GetWindowHeight() const;
		

		void SetKeepVisibleFlag(bool flag);
		void SetKeepVisibleBaseVerticalLine(int lineNum);//left: 0 ,right :1
		void SetKeepVisibleBaseHorizontalLine(int lineNum);//top :0 ,bottom:1
		void SetKeepVisibleBaseVerticalLineOffset(int offset);
		void SetKeepVisibleBaseHorizontalLineOffset(int offset);

		bool GetKeepVisibleFlag() const;
		int GetKeepVisibleBaseVerticalLine() const;
		int GetKeepVisibleBaseHorizontalLine() const;
		int GetKeepVisibleBaseVerticalLineOffset() const;
		int GetKeepVisibleBaseHorizontalLineOffset() const;


		//void SetTexture(const std::wstring& name);
		virtual void SetPosition(float x, float y) ;


		CLASSSIZE(WindowControlEntity)
		CLASSNAME(WindowControlEntity)
	private:

	protected:

		void InitCreating();
	


	private:
	
		//ndc공간을 기준으로하려면 당연히 window의 크기를 알아야하는거고.
		float mWidth;
		float mHeight;
		float mPositionXNdc;
		float mPositionYNdc;



		//맨처음 기본 창의크기 //이크기를 기준으로 엔티티들의 크기를 설정하면 창의크기가변해도
		//항상 시각적으로 일정한크기가되도록 너비와높이가 유지된다.
		UINT mWindowWidth= DEFAULT_WINDOW_WIDTH;		
		UINT mWindowHeight= DEFAULT_WINDOW_HEIGHT;

		bool mKeepVisibleFlag= false;
		int mKeepVisibleBaseVerticalLine = 0;	//left : 0, right : 1
		int mKeepVisibleBaseHorizontalLine = 0; //top : 0 , bottom : 1
		int mKeepVisibleBaseVerticalOffset = 0;	//그 base라인으로부터 어느정도값으로 떨어져있는가.
		int mKeepVisibleBaseHorizontalOffset = 0;	//그 base라인으로부터 어느정도값으로 떨어져있는가.

		float mDepth=5.0F; 




	};
	REGISTEREDITORCLASS(WindowControlEntity)


}

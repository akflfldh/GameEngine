#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"EditorMacro.h"

namespace Quad
{
	enum class EScrollDirection
	{
		eVertical = 0,
		eHorizontal
	};

	class ScrollBarEntity :public PanelUiEntity
	{
	public:
		ScrollBarEntity(const std::string& name="");
		virtual ~ScrollBarEntity() = default;

		
		virtual void Initialize() override;

		static ScrollBarEntity* Create(Map* map, int mapLayerIndex, EScrollDirection direction, float width, float height, float maxAvailableShiftValue = 100);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(ScrollBarEntity)
		CLASSSIZE(ScrollBarEntity)
		EScrollDirection GetScrollDirection() const;

		void SetMaxAvailableShiftValue(float max);
		float GetMaxAvailableShiftValue()const;

		//현재까지 디폴트위치에서 이동한값
		float GetCurrentShiftValue()const;

		//defaultPositionLocal이 기준위치가된다.(default위치에서부터시작해서 움직인다)(그리고 현재위치를 default위치로 설정한다)
		void SetDefaultPositionLocal(const  DirectX::XMFLOAT3& defaultPositionLocal);

		//스크롤바는 디폴트위치로, 이동거리도 0으로 리셋한다.
		void Reset();

		//현재위치에서 shift만큼 이동한다..(자동으로 최대이동거리보다 더움직일수없고 ,디폴트위보다 작은위치로 움직일수없다)
		void Shift(float shift);

		//shift값을 설정한다 (즉 스크르롤바가 shiftvalue만큼이동했다고 설정한다)
		void SetShiftValue(float shiftValue);

		void SetWheelShiftFactor(float wheelShiftFactor);
	protected:
		void InitCreating(EScrollDirection direction, float width, float height, float maxAvailableShiftValue);



	private:





	private:

		void HandleLButtonDown(Event* pEvent)override;
		void HandleMouseMove(Event *pEvent) override;
		void HandleLButtonUp(Event* pEvent) override;
		void HandleWheel(Event* pEvent);



		EScrollDirection mScrollDirection;

		//총이동할수있는거리
		float mMaxAvailableShiftValue;
		//현재까지이동한거리
		float mCurrentShiftValue;


		//디폴트기준위치
		DirectX::XMFLOAT3 mDefaultPositionLocal;
		

		//휠 하나에 이동하는값 (마우스 휠 한번움직일떄 스크롤바가 이동하는값)
		float mWheelShiftFactor; 

	};
	REGISTEREDITORCLASS(ScrollBarEntity)

}

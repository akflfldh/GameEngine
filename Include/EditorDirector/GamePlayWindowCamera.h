#pragma once


#include"Predeclare.h"
#include"Camera/FrustumCamera.h"
#include"EditorMacro.h"
namespace Quad
{
	class GamePlayWindowCamera:public FrustumCamera
	{
	public:
		GamePlayWindowCamera(const std::string& name="");
		~GamePlayWindowCamera() = default;
		virtual void Initialize() override;

		static GamePlayWindowCamera* Create(Map* map, int mapLayerIndex, float fovY, float aspect);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(GamePlayWindowCamera)
		CLASSSIZE(GamePlayWindowCamera)
	protected:
		void InitCreating(float fovY, float aspect);

	private:

		void HandleRButtonDown(Event* pEvent);
		void HandleRButtonUp(Event* pEvent);
		void HandleMouseMove(Event* pEvent);
		void HandleKeyDown(Event* pEvent);
		void HandleKeyUp(Event* pEvent);

	private:

		bool mKeyADownState;
		bool mKeyWDownState;
		bool mKeyDDownState;
		bool mKeySDownState;

		float mRotationScreenX;
		float mRotationScreenY;

		//마우스 오른쪽버튼을 눌를떄의 마우스의좌표 
		int mRButtonDownClientPosX;
		int mRButtonDownClientPosY;
		int mRButtonDownScreenPosX;
		int mRButtonDownScreenPosY;

		float mASpeed = 1.0f;
		float mDSpeed = 1.0f;
		float mWSpeed = 1.0f;
		float mSSpeed = 1.0f;


	};
	REGISTEREDITORCLASS(GamePlayWindowCamera)

}

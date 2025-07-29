#pragma once

namespace Quad
{
	class BaseWindow;

	class SuperController
	{
	public:
		SuperController();
		virtual ~SuperController() = 0;
		virtual void Initialize()= 0;


		virtual void PreUpdate() = 0;


		virtual void Update(float deltaTime) = 0;


		//여기서는 렌더링 위한 사전작업등을한다(렌더아이템구축,제출)
		virtual void EndUpdate() = 0;
		virtual void Draw() = 0;

		virtual BaseWindow* GetWindow() = 0;

	private:



	};

}

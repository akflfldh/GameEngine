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

		virtual BaseWindow* GetWindow() = 0;

	private:



	};

}

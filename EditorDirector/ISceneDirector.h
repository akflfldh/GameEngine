#pragma once

namespace Quad
{

	class ISceneDirector
	{
	public:
		ISceneDirector() = default;
		virtual ~ISceneDirector() = 0;



		virtual void InActiveScene() = 0;

	private:


	};
}


#pragma once

#include"Predeclare.h"
#include"Core/Event.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	//"DeleteTeture"
	class CORE_API_LIB DeleteTextureEvent:public Event
	{
	public:
		DeleteTextureEvent();
		virtual ~DeleteTextureEvent();


		void SetDiffuseMap(Texture* texture);
		Texture* GetTexture() const;

	private:
		Texture* mTexture;

	};
}

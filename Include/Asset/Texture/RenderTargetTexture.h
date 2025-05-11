#pragma once




#include"Predeclare.h"
#include"Asset/Texture/Texture.h"


namespace Quad
{

	class  RenderTargetTexture:public Texture
	{
	public:

		RenderTargetTexture(const std::string& name = "");
		~RenderTargetTexture() = default;

		void SetRenderTargetViewIndex(ViewIndex index);
		ViewIndex GetRendedrTargetViewIndex() const;

		//CLASSNAME(RenderTargetTexture)
		const char* GetClassTypeName() const { return "RenderTargetTexture"; };

	private:
		ViewIndex mRenderTargetViewIndex = -1;




	};

}

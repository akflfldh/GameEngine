#include "DeleteTextureEvent.h"

namespace Quad
{


	

	DeleteTextureEvent::DeleteTextureEvent():
		Event("DeleteTexture")
	{
	}

	Quad::DeleteTextureEvent::~DeleteTextureEvent()
	{
	}

	void DeleteTextureEvent::SetDiffuseMap(Texture* texture)
	{
		mTexture = texture;
	}

	Texture* Quad::DeleteTextureEvent::GetTexture() const
	{
		return mTexture;
	}

}
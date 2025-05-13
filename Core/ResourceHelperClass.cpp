#include"Utility/Utility.h"
#include "ResourceHelperClass.h"
#include"Asset/Texture/Texture.h"
#include<sstream>


Quad::ResourceHelperClass::~ResourceHelperClass()
{
}

const std::string Quad::ResourceHelperClass::GetTextureFileName(Texture* texture)
{

	std::stringstream ss;


	std::string assetRawName = Utility::GetFileNameFromPath(texture->GetName());

	ss << std::hex;
	ss << texture->GetUniqueID() << "_" << assetRawName;

	return ss.str();

}

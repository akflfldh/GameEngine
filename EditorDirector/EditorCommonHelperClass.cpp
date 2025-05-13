#include<sstream>
#include"Utility/Utility.h"
#include "EditorCommonHelperClass.h"

#include"Object.h"
#include"Asset/Texture/Texture.h"

void Quad::EditorCommonHelperClass::SetObjectDefaultInit(Object* ob)
{
	Transform & transform =	ob->GetTransform();

	transform.SetIndependentRotationFlag(true);
	transform.SetIndependentScaleFlag(true);

	ob->SetSelectAvailableFlag(false);


}

//const std::string Quad::EditorCommonHelperClass::GetTextureFileName(Texture* texture)
//{
//
//
//	std::stringstream ss;
//
//	
//	std::string assetRawName = Utility::GetFileNameFromPath(texture->GetName());
//
//	std::stringstream ss;
//	ss << std::hex;
//	ss << texture->GetUniqueID() << "_" << assetRawName;
//
//	return ss.str();
//}

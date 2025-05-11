#include "EditorCommonHelperClass.h"
#include"Object.h"
void Quad::EditorCommonHelperClass::SetObjectDefaultInit(Object* ob)
{
	Transform & transform =	ob->GetTransform();

	transform.SetIndependentRotationFlag(true);
	transform.SetIndependentScaleFlag(true);

	ob->SetSelectAvailableFlag(false);


}

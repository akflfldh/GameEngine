#include "EditorUiSpacePartitioningStructure.h"

Quad::EditorUiSpacePartitioningStructure::EditorUiSpacePartitioningStructure(SpacePartitioningStructure* implSpaceStructure)
	:mImplSpaceStructure(implSpaceStructure)
{



}

const char* Quad::EditorUiSpacePartitioningStructure::GetClassTypeName() const
{
	return mImplSpaceStructure->GetClassTypeName();
}

void Quad::EditorUiSpacePartitioningStructure::AddCollider(UiCollider* collider)
{

	mImplSpaceStructure->AddCollider(collider);

}

void Quad::EditorUiSpacePartitioningStructure::RemoveCollider(UiCollider* collider)
{
	mImplSpaceStructure->RemoveCollider(collider);

}

void Quad::EditorUiSpacePartitioningStructure::DeleteCollider(UiCollider* collider)
{
	mImplSpaceStructure->DeleteCollider(collider);

}

void Quad::EditorUiSpacePartitioningStructure::ReBuild()
{
	mImplSpaceStructure->ReBuild();

}

void Quad::EditorUiSpacePartitioningStructure::FrsutumCullingCollider(std::vector<UiCollider*>& oColliderVector, Camera* camera)
{
	mImplSpaceStructure->FrsutumCullingCollider(oColliderVector, camera);


}

bool Quad::EditorUiSpacePartitioningStructure::RayCasting(std::vector<UiCollider*>& oColliderVector, const Ray& ray)
{
	return mImplSpaceStructure->RayCasting(oColliderVector, ray);
}

bool Quad::EditorUiSpacePartitioningStructure::RayCastingFirst(UiCollider*& oCollider, Ray& ray)
{
	return mImplSpaceStructure->RayCastingFirst(oCollider, ray);
}

void Quad::EditorUiSpacePartitioningStructure::GetAllCollider(std::vector<UiCollider*>& oColliderVector)
{
	mImplSpaceStructure->GetAllCollider(oColliderVector);



}

bool Quad::EditorUiSpacePartitioningStructure::RayCastingFirstNotIncludeEditorObject(UiCollider*& oCollider, Ray& ray) const
{
	return mImplSpaceStructure->RayCastingFirst(oCollider, ray);
}

void Quad::EditorUiSpacePartitioningStructure::AddGizmoCollider(UiCollider* collider)
{


}

//const std::vector<UiCollider*>& Quad::EditorUiSpacePartitioningStructure::GetGizmoColliderVector() const
//{
//	// TODO: 여기에 return 문을 삽입합니다.
//}

void Quad::EditorUiSpacePartitioningStructure::Reset()
{ 
	mImplSpaceStructure->Reset();


}

Quad::SpacePartitioningStructure<Quad::UiCollider>* Quad::EditorUiSpacePartitioningStructure::GetImplSpaceStructure()
{
	return mImplSpaceStructure;
}

#pragma once
#include"Collision/UiCollisionWorld.h"


namespace Quad
{
	class EditorUiSpacePartitioningStructure;

	class EditorUiCollisionWorld:public UiCollisionWorld
	{
	public:
		EditorUiCollisionWorld(EditorUiSpacePartitioningStructure* partitioingStructure);
		EditorUiCollisionWorld(const EditorUiCollisionWorld& world) = delete;
		EditorUiCollisionWorld(EditorUiCollisionWorld&& world) = delete;


	//	void AddGizmoCollider(UiCollider* collider);


		bool RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray);

	private:



	};

}

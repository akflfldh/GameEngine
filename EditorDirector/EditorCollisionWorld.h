#pragma once
#include"Predeclare.h"
#include"Collision/CollisionWorld.h"

namespace Quad
{
	class EditorSpacePartitioningStructure;
	class EditorCollisionWorld: public CollisionWorld
	{
	public:
		EditorCollisionWorld(EditorSpacePartitioningStructure* partitioingStructure);
		EditorCollisionWorld(const EditorCollisionWorld& world) = delete;
		EditorCollisionWorld(EditorCollisionWorld&& world) = delete;


		void AddGizmoCollider(Collider* collider);

		
		bool RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray);
	private:

	
		

	};

}

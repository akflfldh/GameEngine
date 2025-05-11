#pragma once

#include"Map/Map.h"


namespace Quad
{
	class EditorMap:public Map
	{
	public:
		EditorMap();
		~EditorMap();


		virtual void Update(float deltaTime) override;

		void AddGizmo(Gizmo* gizmo, int mapLayerIndex =1 );
		Gizmo* GetGizmo() const;


		virtual void Serialize() override;
		virtual void DeSerialize() override;

		void SetMainUserCamera();
		void SetMainEngineCamera();

	
		virtual void SetMainCamera(Camera* camera, int mapLayerIndex = 0) override;
		virtual void SetMainCamera(unsigned long long id, int mapLayerIndex = 0) override;


		//void SetEditorMainCamera(Camera* camera, int mapLayerIndex = 0);
		void AddEditorMainCamera(Camera* camera, int mapLayerIndex);

		bool RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray);
		bool RayCastingFirstNotIncludeEditorObject(const DirectX::XMFLOAT2 & screenPos , Object*& oObject );

		//void SetEditorMainCamera(, int mapLayerIndex = 0)


		virtual void ObjectUpdate(float deltaTime) override;


	private:

		Gizmo* mGizmo;


		std::vector<Camera*> mUserMainCamera;
		std::vector<Camera*> mEngineMainCamera;

		std::vector<Object*> mEditUpdateObjectVector;



	};

}

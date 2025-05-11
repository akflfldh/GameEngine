#pragma once



#include"Predeclare.h"
#include"Object/Entity.h"
#include"EditorMacro.h"
namespace Quad

{
	class GizmoScaleHandle:public Entity
	{
	public:
		GizmoScaleHandle(const std::string & name="");
		~GizmoScaleHandle() = default;


		virtual void Initialize() override;

		static GizmoScaleHandle* Create(Map* map, int mapLayerIndex, EAxisType axisType, Gizmo* gizmo);

		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;
		virtual void UpdateCollider(float deltaTime) override;

		CLASSNAME(GizmoScaleHandle)

		void SetColor(const DirectX::XMFLOAT3& color);

		EAxisType GetAxisType()const;

	private:
		void HandleHover(Event* pEvent);
		void HandleLButtonDown(Event* pEvent);
		void HandleLButtonUp(Event* pEvent);
		void HandleMouseMove(Event* pEvent);


		void InitCreating(EAxisType axisType, Gizmo* gizmo, Map* map);


	private:


		Line* mLine;
		Entity* mBox;

		EAxisType mAxisType;
		Gizmo* mGizmo;


		DirectX::XMFLOAT3 mDefaultColor;
		DirectX::XMFLOAT3 mHoverColor;




	};
	REGISTEREDITORCLASS(GizmoScaleHandle)
}

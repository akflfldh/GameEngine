#pragma once
#include"Predeclare.h"
#include"Object/Entity.h"
#include"Object/Line/Line.h"
#include"EditorMacro.h"
namespace Quad

{
	
	

	class GizmoHandle :public Entity
	{
	public:
		GizmoHandle(const std::string & name="");
		~GizmoHandle() = default;

		void Initialize();
		
		static GizmoHandle* Create(Map* map, int mapLayeterIndex , EAxisType axisType, Gizmo* gizmo);
		
		void SetColor(const DirectX::XMFLOAT3& color);

		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event* event) override;
		
		CLASSNAME(GizmoHandle)

		EAxisType GetAxisType() const;
		

	private:
		void HandleLButtonDown(Event * pEvent);
		void HandleLButtonUp(Event * pEvent);
		void HandleHover(Event * pEvent);
		void HandleMouseMove(Event* pEvent);


		void InitCreating(Map* map, int mapLayerIndex ,EAxisType axisType, Gizmo* gizmo);


	private:
	
		Line* mLine;
		//Entity* mLine;
		Entity* mCone;
		EAxisType mAxisType;
		Gizmo* mGizmo;

		DirectX::XMFLOAT3 mDefaultColor;
		DirectX::XMFLOAT3 mHoverColor;

	};
	REGISTEREDITORCLASS(GizmoHandle)
}

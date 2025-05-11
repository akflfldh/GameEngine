#pragma once

#include"Predeclare.h"
#include"UiEntity.h"
#include <DirectXMath.h>
#include"EditorMacro.h"

namespace Quad
{


	class BoundaryRect :public UiEntity
	{
	public:
		BoundaryRect(const std::string& name="");
		~BoundaryRect() = default;

		virtual void Initialize() override;

		static BoundaryRect* Create(Map* map, int mapLayerIndex, float width, float height);

		virtual void Update(float deltaTime);
		virtual void OnEvent(Event* event) override;	


		CLASSNAME(BoundaryRect)
		CLASSSIZE(BoundaryRect)

		void SetPositionLocal(DirectX::XMFLOAT3 pos);

		void SetWidth(float width);
		void SetHeight(float height);
		float GetWidth()const;
		float GetHeight()const;



		void SetDstEntity(UiEntity* entity);

	private:
	
		void InitCreating(float width, float height);
			

	private:
		float mWidth;
		float mHeight;
		UiEntity* mDstEntity=nullptr;
		BoundaryStick* mBoundaryStick[4];

	};
	REGISTEREDITORCLASS(BoundaryRect)
	

}


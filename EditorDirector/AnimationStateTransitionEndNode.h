#pragma once


#include"UiEntity.h"
#include"EditorMacro.h"
#include"PanelUiEntity.h"
namespace Quad
{
	class Spline;
	class AnimationStateTransitionEndNode:public PanelUiEntity
	{
	public:
		AnimationStateTransitionEndNode(const std::string& name = "");
		virtual ~AnimationStateTransitionEndNode();

		virtual void Initialize() override;

		static AnimationStateTransitionEndNode* Create(Map* map, int mapLayerIndex);

		virtual void Update(float deltaTime)override;
		virtual void EndUpdate(float deltaTime)override;
		virtual void OnEvent(Event* event) override;




		virtual void Serialize() override;
		virtual void DeSerialize() override;


		void AddLinkedSpline(Spline* spline); 
		void SetUpdateSplineFlag();
		CLASSSIZE(AnimationStateTransitionEndNode)




	protected:
		void InitCreating();

	private:
		bool mSplineUpdateFlag;
		std::vector<Spline*> mLinkedSplineVector;
	
	};
	REGISTEREDITORCLASS(AnimationStateTransitionEndNode)

}

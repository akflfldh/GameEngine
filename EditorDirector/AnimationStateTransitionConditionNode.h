#pragma once

#include"AnimationStateTransitionBaseNode.h"


#include"EditorMacro.h"
namespace Quad
{
	class AnimationStateTransitionConditionNode:public AnimationStateTransitionBaseNode
	{
	public:
		AnimationStateTransitionConditionNode(const std::string& name = "");
		virtual ~AnimationStateTransitionConditionNode();



		virtual void Initialize() override;
		static AnimationStateTransitionConditionNode * Create(Map* map, int mapLayerIndex);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(AnimationStateTransitionConditionNode)

	protected:
		void InitCreating();


	private:


	};
	REGISTEREDITORCLASS(AnimationStateTransitionConditionNode)


}
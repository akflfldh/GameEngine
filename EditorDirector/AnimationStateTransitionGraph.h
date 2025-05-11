#pragma once


#include"PanelUiEntity.h"
#include<unordered_map>
#include<vector>
#include"EditorMacro.h"
namespace Quad
{
	class AnimationStateNode;
	class AnimationStateTransitionBaseNode;
	class AnimationStateTransitionBlendNode;
	class AnimationStateTransitionGraph:public PanelUiEntity
	{
	public:
		AnimationStateTransitionGraph(const std::string& name = "");
		virtual ~AnimationStateTransitionGraph();

		virtual void Initialize() override;

		static AnimationStateTransitionGraph* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;


		bool CreateStateNode(const std::string& stateName);
		bool CreateBlendNode();
		bool CreateConditionNode();

		bool RemoveStateNode(const std::string& stateName);
		AnimationStateNode* GetStateNode(const std::string& stateNode);
		const std::unordered_map<std::string, AnimationStateNode*>& GetStateNodeTable() const;

		const std::vector<AnimationStateTransitionBaseNode*>& GetAnimationStateTransitionNodeVector() const;
		AnimationStateTransitionBaseNode* GetNode(unsigned long long nodeID);




		virtual void Serialize() override;
		virtual void DeSerialize() override;


		CLASSNAME(AnimationStateTransitionGraph)
		CLASSSIZE(AnimationStateTransitionGraph)

	protected:
		void InitCreating();
		void SetNodeID(AnimationStateTransitionBaseNode* node);


		void InitStateNodeDefaultSetting(AnimationStateNode* node);
		void InitBlendNodeDefaultSetting(AnimationStateTransitionBlendNode* node);


		//<state Name , StateNode>
		std::unordered_map<std::string, AnimationStateNode*> mStateNodeTable;
		std::vector<AnimationStateTransitionBaseNode*> mAnimationStateTransitionNodeVector;


		unsigned long long mNextNodeID;
	};
	REGISTEREDITORCLASS(AnimationStateTransitionGraph)
}

#include "AnimationStateTransitionConditionNode.h"
#include"Map/Map.h"
Quad::AnimationStateTransitionConditionNode::AnimationStateTransitionConditionNode(const std::string& name)
	:AnimationStateTransitionBaseNode(Quad::EAnimationNodeType::eConditionNode,name)
{
}

Quad::AnimationStateTransitionConditionNode::~AnimationStateTransitionConditionNode()
{
}

void Quad::AnimationStateTransitionConditionNode::Initialize()
{
	AnimationStateTransitionBaseNode::Initialize();
}

Quad::AnimationStateTransitionConditionNode* Quad::AnimationStateTransitionConditionNode::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionConditionNode* ob = static_cast<AnimationStateTransitionConditionNode*>(map->CreateObject("AnimationStateTransitionConditionNode", mapLayerIndex));


	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionConditionNode::Update(float deltaTime)
{
	AnimationStateTransitionBaseNode::Update(deltaTime);

}

void Quad::AnimationStateTransitionConditionNode::OnEvent(Event* event)
{
	AnimationStateTransitionBaseNode::OnEvent(event);

}

void Quad::AnimationStateTransitionConditionNode::InitCreating()
{
	AnimationStateTransitionBaseNode::InitCreating();

	Transform& transform = GetTransform();
	transform.SetIndependentRotationFlag(true);
	transform.SetIndependentScaleFlag(true);
	SetSelectAvailableFlag(false);
	
	SetTexture("Blue.png");






}

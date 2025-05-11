#include "AnimationStateTransitionGraph.h"
#include"Map/Map.h"


#include"AnimationStateNode.h"
#include"AnimationStateTransitionBlendNode.h"
#include"AnimationStateTransitionConditionNode.h"

#include"EditorCommonHelperClass.h"

Quad::AnimationStateTransitionGraph::AnimationStateTransitionGraph(const std::string& name)
	:PanelUiEntity(name), mNextNodeID(1)
{
}

Quad::AnimationStateTransitionGraph::~AnimationStateTransitionGraph()
{
}

void Quad::AnimationStateTransitionGraph::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateTransitionGraph* Quad::AnimationStateTransitionGraph::Create(Map* map, int mapLayerIndex)
{

	AnimationStateTransitionGraph* ob = static_cast<AnimationStateTransitionGraph*>(map->CreateObject("AnimationStateTransitionGraph", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionGraph::Update(float deltaTime)
{

	PanelUiEntity::Update(deltaTime);

}

void Quad::AnimationStateTransitionGraph::OnEvent(Event* event)
{

	PanelUiEntity::OnEvent(event);



}

bool Quad::AnimationStateTransitionGraph::CreateStateNode(const std::string& stateName)
{

	auto it = mStateNodeTable.find(stateName);
	if (it != mStateNodeTable.end())
		return false;
	AnimationStateNode* newStateNode = AnimationStateNode::Create(GetMap(), GetMapLayerID());
	InitStateNodeDefaultSetting(newStateNode);
	AddChildPanelUiEntity(newStateNode);
	newStateNode->SetPosition(0, 0, -1);
	newStateNode->SetStateName(stateName);


	mStateNodeTable.insert({ stateName,newStateNode });
	mAnimationStateTransitionNodeVector.push_back(newStateNode);

	SetNodeID(newStateNode);

	return true;
}

bool Quad::AnimationStateTransitionGraph::CreateBlendNode()
{

	AnimationStateTransitionBlendNode* blendNode = AnimationStateTransitionBlendNode::Create(GetMap(), GetMapLayerID());

	InitBlendNodeDefaultSetting(blendNode);
	
	AddChildPanelUiEntity(blendNode);

	blendNode->SetPosition(0, 0, -1);

	mAnimationStateTransitionNodeVector.push_back(blendNode);

	SetNodeID(blendNode);

	return true;
}

bool Quad::AnimationStateTransitionGraph::CreateConditionNode()
{
	AnimationStateTransitionConditionNode* conditionNode = AnimationStateTransitionConditionNode::Create(GetMap(), GetMapLayerID());
	conditionNode->SetSize(100, 100);
	conditionNode->SetDrawFlag(GetDrawFlag());

	AddChildPanelUiEntity(conditionNode);

	conditionNode->SetPosition(0, 0, -1);

	mAnimationStateTransitionNodeVector.push_back(conditionNode);
	return true;

}

bool Quad::AnimationStateTransitionGraph::RemoveStateNode(const std::string& stateName)
{
	auto it = mStateNodeTable.find(stateName);
	if (it == mStateNodeTable.end())
		return false;

	AnimationStateNode* node = it->second;

	mStateNodeTable.erase(it);
	
	Map * map =	node->GetMap();
	map->RequestRemoveObject(node);



	std::vector<AnimationStateTransitionBaseNode*>::iterator stateNodeVectorIt = std::find(mAnimationStateTransitionNodeVector.begin(), mAnimationStateTransitionNodeVector.end(), node);

	*stateNodeVectorIt = mAnimationStateTransitionNodeVector.back();
	mAnimationStateTransitionNodeVector.pop_back();



	//관련된 상태전이노드들도 모두제거
	



	return true;
}

Quad::AnimationStateNode* Quad::AnimationStateTransitionGraph::GetStateNode(const std::string& stateNode)
{

	auto it = mStateNodeTable.find(stateNode);
	if (it == mStateNodeTable.end())
		return nullptr;

	return it->second;


	return nullptr;
}

const std::unordered_map<std::string, Quad::AnimationStateNode*>& Quad::AnimationStateTransitionGraph::GetStateNodeTable() const
{
	return mStateNodeTable;

	// TODO: 여기에 return 문을 삽입합니다.
}

const std::vector<Quad::AnimationStateTransitionBaseNode*>& Quad::AnimationStateTransitionGraph::GetAnimationStateTransitionNodeVector() const
{

	return mAnimationStateTransitionNodeVector;

	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::AnimationStateTransitionBaseNode* Quad::AnimationStateTransitionGraph::GetNode(unsigned long long nodeID)
{
	std::vector<AnimationStateTransitionBaseNode*>::iterator it = std::find_if(mAnimationStateTransitionNodeVector.begin(), mAnimationStateTransitionNodeVector.end(), [nodeID](AnimationStateTransitionBaseNode * node) {
		if (node->GetNodeID() == nodeID)
			return true;
		
		return false;
		});

	if (it == mAnimationStateTransitionNodeVector.end())
		return nullptr;

	return *it;
}

void Quad::AnimationStateTransitionGraph::Serialize()
{
	//PanelUiEntity::Serialize();

	//에디터에서만 사용하는것이기에 에니메이션노드들에 대한 데이터들에대해서만 직렬화한다
	
	JsonParser::StartWriteObject();
	JsonParser::Write("AnimationStateTransitionGraph_NodeNextID", mNextNodeID);

	JsonParser::Write("AnimationStateTransitionGraph_NodeNum", mAnimationStateTransitionNodeVector.size());

	for( auto & nodeElement :mAnimationStateTransitionNodeVector  )
	{
		JsonParser::StartWriteObject();

		JsonParser::Write("NodeType", (int)nodeElement->GetAnimationNodeType());
		JsonParser::Write("NodeID", (int)nodeElement->GetNodeID());
		nodeElement->Serialize();
	}


}

void Quad::AnimationStateTransitionGraph::DeSerialize()
{
	//PanelUiEntity::DeSerialize();
	JsonParser::IncrementCurrentIndex();
	size_t nodeNum = 0;
	int nodeType = 0;
	unsigned long long nodeID = 0;
	AnimationStateTransitionBaseNode* node = nullptr;

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();

	JsonParser::Read("AnimationStateTransitionGraph_NodeNextID", mNextNodeID);

	JsonParser::Read("AnimationStateTransitionGraph_NodeNum", nodeNum);
	mAnimationStateTransitionNodeVector.resize(nodeNum);

	unsigned int startNodeReadIndex =  JsonParser::GetCurrentIndex();
	for (size_t i = 0; i < nodeNum; ++i)
	{
		JsonParser::IncrementCurrentIndex();
		JsonParser::Read("NodeType", nodeType);
		EAnimationNodeType animNodeType = (EAnimationNodeType)nodeType;
		switch (animNodeType)
		{
		case EAnimationNodeType::eStateNode:

			node = AnimationStateNode::Create(map, mapLayerIndex);
		
			InitStateNodeDefaultSetting(static_cast<AnimationStateNode*>(node));

			break;
		case EAnimationNodeType::eBlendNode:
			node = AnimationStateTransitionBlendNode::Create(map, mapLayerIndex);
			InitBlendNodeDefaultSetting(static_cast<AnimationStateTransitionBlendNode*>(node));
			break;
		}
		EditorCommonHelperClass::SetObjectDefaultInit(node);

		JsonParser::Read("NodeID", nodeID);
		node->SetNodeID(nodeID);

		mAnimationStateTransitionNodeVector[i] = node;



	}


	JsonParser::SetCurrentIndex(startNodeReadIndex);
	for (auto& nodeElement : mAnimationStateTransitionNodeVector)
	{
		JsonParser::IncrementCurrentIndex();
		nodeElement->DeSerialize();

		if (nodeElement->GetAnimationNodeType()==EAnimationNodeType::eStateNode)
		{
			AnimationStateNode* stateNode = static_cast<AnimationStateNode*>(nodeElement);
			mStateNodeTable.emplace(stateNode->GetStateName(), stateNode);

		}

		AddChildPanelUiEntity(nodeElement);

	}



}








void Quad::AnimationStateTransitionGraph::InitCreating()
{

	PanelUiEntity::InitCreating();
	SetSize(1000, 1000);
	GetTransform().SetIndependentRotationFlag(true);
	GetTransform().SetIndependentScaleFlag(true);




}

void Quad::AnimationStateTransitionGraph::SetNodeID(AnimationStateTransitionBaseNode* node)
{
	node->SetNodeID(mNextNodeID);
	mNextNodeID++;
}

void Quad::AnimationStateTransitionGraph::InitStateNodeDefaultSetting(AnimationStateNode* node)
{
	node->GetTransform().SetIndependentRotationFlag(true);
	node->GetTransform().SetIndependentScaleFlag(true);
	node->SetSelectAvailableFlag(false);
	//newStateNode->SetActiveFlag(true);
	node->SetDrawFlag(GetDrawFlag());
	node->SetSize(150, 150);



}

void Quad::AnimationStateTransitionGraph::InitBlendNodeDefaultSetting(AnimationStateTransitionBlendNode* node)
{
	node->GetTransform().SetIndependentRotationFlag(true);
	node->GetTransform().SetIndependentScaleFlag(true);
	node->SetSelectAvailableFlag(false);
	node->SetSize(100, 100);
	node->SetDrawFlag(GetDrawFlag());


}

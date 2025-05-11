#include "AnimationStateTransitionGraphBasePanel.h"
#include"Map/Map.h"


#include"AnimationStateTransitionGraph.h"

#include"System/PopupWindowUiSystem.h"
#include"PopupWindow.h"
#include"PopupWindowController.h"
#include"MouseEvent.h"

#include"ObjectManager/EditGameObjectManager.h"

Quad::AnimationStateTransitionGraphBasePanel::AnimationStateTransitionGraphBasePanel(const std::string& name)
	:PanelUiEntity(name), mDestObject(nullptr), mDestObjectGraph(nullptr)
{
}

Quad::AnimationStateTransitionGraphBasePanel::~AnimationStateTransitionGraphBasePanel()
{
}

void Quad::AnimationStateTransitionGraphBasePanel::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateTransitionGraphBasePanel* Quad::AnimationStateTransitionGraphBasePanel::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionGraphBasePanel* ob = static_cast<AnimationStateTransitionGraphBasePanel*>(map->CreateObject("AnimationStateTransitionGraphBasePanel", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionGraphBasePanel::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::AnimationStateTransitionGraphBasePanel::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

}

void Quad::AnimationStateTransitionGraphBasePanel::SetDrawFlag(bool flag)
{
	Object::SetDrawFlag(flag);

	if (mDestObject != nullptr)
	{
		unsigned long long id = mDestObject->GetUniqueID();
		auto it = mAnimationStateTransitionGraphTable.find(id);
		AnimationStateTransitionGraph* graph = it->second;
		
		graph->SetDrawFlag(flag);
		//mDestObject->SetDrawFlag(flag);
	}




}

void Quad::AnimationStateTransitionGraphBasePanel::SetDestObject(Object* object)
{
	mDestObject = object;
	if (mDestObject == nullptr)
		return;



	unsigned long long id = mDestObject->GetUniqueID();
	auto it = mAnimationStateTransitionGraphTable.find(id);


	if (it == mAnimationStateTransitionGraphTable.end()) 
	{

		AnimationStateTransitionGraph* graph = AnimationStateTransitionGraph::Create(GetMap(), GetMapLayerID());
		//graph->GetTransform().SetIndependentRotationFlag(true);
		InitGraphDefaultSetting(graph);

		AnimationComponent* animComponent = object->GetModel()->GetAnimationComponent();

		const std::unordered_map<std::string,AnimationState*> & animStateTable = animComponent->GetAnimationStateTable();
		for (auto& stateElement : animStateTable)
		{

			const std::string& stateName = stateElement.first;
			graph->CreateStateNode(stateName);
		}

		mAnimationStateTransitionGraphTable.insert({ id,graph });
		//생성

		

	}
	else
	{
		//graph가 이미존재 

		//하지만 새로운상태들이 추가되었을수도있기에, 혹은 기존상태가 제거되었을수도있다.

		//기존의 상태중 제거된것들이 있다면 그상태노드제거,
		//그상태노드를 출발,도착으로 사용하는 전이노드들도 모두제거

		//새로운상태들에 대해서 상태노드생성
		AnimationStateTransitionGraph* graph =  it->second;
		

		AnimationComponent* animComponent = object->GetModel()->GetAnimationComponent();

		const std::unordered_map<std::string, AnimationState*>& animStateTable = animComponent->GetAnimationStateTable();
		for (auto& stateElement : animStateTable)
		{

		
		}


	}




}

Quad::Object* Quad::AnimationStateTransitionGraphBasePanel::GetDestObject() const
{
	return mDestObject;
}

void Quad::AnimationStateTransitionGraphBasePanel::InActive()
{
	//mDestObject = nullptr;
}

bool Quad::AnimationStateTransitionGraphBasePanel::CreateStateNode(const std::string& stateName)
{
	if (mDestObject == nullptr)
		return false;


	unsigned long long id = mDestObject->GetUniqueID();
	auto it = mAnimationStateTransitionGraphTable.find(id);
	
	AnimationStateTransitionGraph* graph = it->second;

	return graph->CreateStateNode(stateName);

}

bool Quad::AnimationStateTransitionGraphBasePanel::RemoveStateNode(const std::string& stateName)
{

	if (mDestObject == nullptr)
		return false;

	unsigned long long id = mDestObject->GetUniqueID();
	auto it = mAnimationStateTransitionGraphTable.find(id);
	AnimationStateTransitionGraph* graph = it->second;



	return graph->RemoveStateNode(stateName);
}

Quad::AnimationStateNode* Quad::AnimationStateTransitionGraphBasePanel::GetStateNode(const std::string& stateNode)
{
	if (mDestObject == nullptr)
		return nullptr;

	unsigned long long id = mDestObject->GetUniqueID();
	auto it = mAnimationStateTransitionGraphTable.find(id);
	AnimationStateTransitionGraph* graph = it->second;

	return graph->GetStateNode(stateNode);


}

Quad::AnimationStateTransitionGraph* Quad::AnimationStateTransitionGraphBasePanel::GetCurrentAnimStateTransitionGraph()
{
	if (mDestObject == nullptr)
		return nullptr;


	auto it = mAnimationStateTransitionGraphTable.find(mDestObject->GetUniqueID());
	return it->second;



}

void Quad::AnimationStateTransitionGraphBasePanel::Serialize()
{
	unsigned long long objectUniqueID = 0;
	AnimationStateTransitionGraph* graph = nullptr;

	JsonParser::StartWriteObject();
	JsonParser::Write("AnimationStateTransitionGraphBasePanel_GraphNum", mAnimationStateTransitionGraphTable.size());

	for (auto& graphElement : mAnimationStateTransitionGraphTable)
	{

		objectUniqueID =  graphElement.first;
		graph= graphElement.second;
		JsonParser::StartWriteObject();
		JsonParser::Write("AnimationStateTransitionGraphBasePanel_ObjectUniqueID", objectUniqueID);
		graph->Serialize();

	}

}

void Quad::AnimationStateTransitionGraphBasePanel::DeSerialize()
{
	unsigned long long objectUniqueID = 0;
	AnimationStateTransitionGraph* graph = nullptr;

	JsonParser::IncrementCurrentIndex();
	size_t graphNum = 0;
	JsonParser::Read("AnimationStateTransitionGraphBasePanel_GraphNum", graphNum);

	Map * map =	GetMap();
	int mapLayerIndex = GetMapLayerID();
	EditGameObjectManager* objectManger = EditGameObjectManager::GetInstance();

	for (size_t i = 0; i < graphNum; ++i)
	{
		JsonParser::IncrementCurrentIndex();
		AnimationStateTransitionGraph * graph =	AnimationStateTransitionGraph::Create(map, mapLayerIndex);
		JsonParser::Read("AnimationStateTransitionGraphBasePanel_ObjectUniqueID", objectUniqueID);
		InitGraphDefaultSetting(graph);

		mAnimationStateTransitionGraphTable.insert({ objectUniqueID,graph });
		mDestObject = objectManger->GetObjectFromID(objectUniqueID);

		graph->DeSerialize();

		//AddChildPanelUiEntity(graph);

	}





}

 



void Quad::AnimationStateTransitionGraphBasePanel::InitCreating()
{

	PanelUiEntity::InitCreating();

}

void Quad::AnimationStateTransitionGraphBasePanel::HandleRButtonDown(Event* pEvent)
{

	MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);


	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
		stateComponent->SetRButtonDownState(true);

	}

}

void Quad::AnimationStateTransitionGraphBasePanel::HandleRButtonUp(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetHoverState() && stateComponent->GetRButtonDownState())
	{

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		float screenPosX =mouseEvent->GetScreenPositionX();
		float screenPosY =mouseEvent->GetScreenPositionY();


		PopupWindowController::ActivateWindow(50, screenPosX, screenPosY);

	//	auto popupWindowUiSystem = PopupWindowUiSystem::GetInstance();
	//	popupWindowUiSystem->ActivatePanel(100);
		//PopupWindow::SetVisibilityState(true);

		
	}
	//stateComponent->SetRButtonDownState(false);
//

}

void Quad::AnimationStateTransitionGraphBasePanel::InitGraphDefaultSetting(AnimationStateTransitionGraph* graph)
{
	graph->GetTransform().SetIndependentRotationFlag(true);
	graph->GetTransform().SetIndependentScaleFlag(true);

	graph->SetSelectAvailableFlag(false);
	graph->SetSize(500, 500);

	graph->SetTexture("Red.png");

	AddChildPanelUiEntity(graph);
	graph->SetPosition(0, 0, -1);
	graph->SetDrawFlag(GetDrawFlag());
}

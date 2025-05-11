#include "Map/SceneGraph.h"
#include"Object/Object.h"
#include"Object/Entity.h"
#include"Object/Camera/Camera.h"
//#include"BoundaryRect.h"
//#include"ResizeHandle.h"
//#include"Character.h"
//#include"BoundaryStick.h"
//#include"TextSelectBox.h"
//#include"TextPanelEntity.h"
//#include"TextBox.h"
//#include"TextCharacterPanelEntity.h"
#include"Application.h"


#include"ObjectFactory/ObjectFactory.h"
//#include"DockingSelectBox.h"
#include<stack>

#include"ObjectManager/ObjectManager.h"

#include"Component/ColliderComponent.h"
#include"Component/UiColliderComponent.h"
namespace Quad
{
	SceneGraph::SceneGraph() :mRoot(new Node)
		, mPauseState(false)
	{
		
	}

	SceneGraph::~SceneGraph()
	{
		delete mRoot;

	}



	void SceneGraph::Initialize(BaseObjectManager* objectManager, bool playMode)
	{
		mObjectManager = objectManager;
		mRoot->mObject = new Entity("Root");
		mRoot->mObject->GetTransform().Initialize();
		Transform& transform = mRoot->mObject->GetTransform();
		DirectX::XMFLOAT4X4 identityMatrix;
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		
		mPlayMode = playMode;
	}


	void SceneGraph::Update(float deltaTime)
	{

		//object들의 update호출


		//순회하면서  update를 호출한다.
		//std::vector<Node*> nodeStackcontainer;
		//nodeStackcontainer.reserve(mObjecVector.size());
		std::stack<Node*> nodeQueue;

		Node* childNode = nullptr;

		
		//전체오브젝트들에대해서 월드행렬을 계산한다.
		//change matrix를 초기화 
		childNode = mRoot->mFirstChildNode;

		while (childNode != nullptr)
		{
			nodeQueue.push(childNode);
			childNode = childNode->mNextSiblingNode;
		}

		while (!nodeQueue.empty())
		{

			Node* currNode = nodeQueue.top();
			nodeQueue.pop();




			if (currNode->mObject->GetActiveFlag())
			{
				Object* parentObject = currNode->mParentNode->mObject.GetPointer();
				Object* currObject = currNode->mObject.GetPointer();
				if (parentObject->GetTransform().GetDirtyFlag() ||
					currObject->GetTransform().GetDirtyFlag())
				{

					DirectX::XMFLOAT4X4 parentWorldMatrix = parentObject->GetTransform().GetWorldMatrix();
					currObject->GetTransform().UpdateWorldMatrix(parentWorldMatrix);
				}

				childNode = currNode->mFirstChildNode;
				while (childNode != nullptr)
				{
					nodeQueue.push(childNode);
					childNode = childNode->mNextSiblingNode;
				}

			}
		}

		


		//entity들에대해서 collider update
		childNode = mRoot->mFirstChildNode;

		while (childNode != nullptr)
		{
			nodeQueue.push(childNode);
			childNode = childNode->mNextSiblingNode;
		}


		while (!nodeQueue.empty())
		{

			Node* currNode = nodeQueue.top();
			nodeQueue.pop();


			if (currNode->mObject->GetActiveFlag())
			{
		
				Object* ob =  currNode->mObject.GetPointer();
					switch (ob->GetObjectType())
					{
					case EObjectType::eEntity:
					case EObjectType::eLine:
						//Entity* entity = (Entity*)ob;
					{
						ColliderComponent* colliderComponent = ob->GetModel()->GetColliderComponent();
						if (ob->GetName() == "OrthogoanlCamera2")
						{
							int a = 2;
						}
						if (colliderComponent && ob->GetTransform().GetDirtyFlag())
						{
							Collider* collider = colliderComponent->GetCollider();

							collider->Update(deltaTime);

						}
					}
						break;
					case EObjectType::eUiEntity:
					{
						UiColliderComponent* uiColliderComponent = ob->GetModel()->GetUiColliderComponent();
						if(uiColliderComponent && ob->GetTransform().GetDirtyFlag())
						{ 
							UiCollider* collider = uiColliderComponent->GetCollider();
							collider->Update(deltaTime);
						}
					}
					break;


					}



				childNode = currNode->mFirstChildNode;
				while (childNode != nullptr)
				{
					nodeQueue.push(childNode);
					childNode = childNode->mNextSiblingNode;
				}
			}
		}






		//1update시에 트랜스폼에 로컬변환행렬 생성하고
		//2여기서 부모의 월드변환행렬을가져와서 로컬변환행렬과 합쳐서 다시 트랜스폼에 월드변환행렬을 set
		//이것들 자식들로 내려가면서 반복

		//2그후에 공간분할구조 재구축

		//3절두체판정

		//4보여질 object들에대한 정보만 렌더링파이프라인에 입력으로 넣는다 (상수버퍼)
		//(지금은 다보여질것이니 그리고 부모 자식관계에 해당하는 object는없을것이니 (1, 4만하면됨)





	}

	

	void SceneGraph::ChangeParent(Object* newParent, Object* child)
	{

		//두개의 노드를 찾는게 필요 + child의 부모노드도 찾아야지
		//find 

		if (newParent == nullptr)
			newParent = mRoot->mObject.GetPointer();

		//순환구조를 방지해야한다.
		Node* newParentNode = findNode(newParent);


		Node* newChildNode = findNode(child);
		if (newChildNode == nullptr)
			return;


		Node* preParentNode = newChildNode->mParentNode;


		Node* preChildNode = preParentNode->mFirstChildNode;
		Node* currChildNode = preParentNode->mFirstChildNode;

		while (currChildNode != nullptr)
		{

			if (currChildNode == newChildNode)
			{
				if (currChildNode == preParentNode->mFirstChildNode)
				{
					preParentNode->mFirstChildNode = currChildNode->mNextSiblingNode;
				}
				else
				{
					preChildNode->mNextSiblingNode = currChildNode->mNextSiblingNode;
				}

				currChildNode->mNextSiblingNode = nullptr;
				currChildNode->mParentNode = nullptr;

				break;
			}

			preChildNode = currChildNode;
			currChildNode = currChildNode->mNextSiblingNode;
		}


		newChildNode->mNextSiblingNode = newParentNode->mFirstChildNode;
		newParentNode->mFirstChildNode = newChildNode;
		newChildNode->mParentNode = newParentNode;


		//부모와 자식간의월드정보를바탕으로 자식의 로컬transform 정보를 조정한다.
		Transform& parentTransform = newParentNode->mObject->GetTransform();
		Transform& childTransform = newChildNode->mObject->GetTransform();
		

		DirectX::XMFLOAT4X4 parentWorldMatrixF= parentTransform.GetWorldMatrix();
		DirectX::XMFLOAT4X4 childWorldMatrixF = childTransform.GetWorldMatrix();

		DirectX::XMMATRIX parentWorldMatrix = DirectX::XMLoadFloat4x4(&parentWorldMatrixF);
		DirectX::XMMATRIX childWorldMatrix = DirectX::XMLoadFloat4x4(&childWorldMatrixF);

		DirectX::XMMATRIX parentWorldInvMatrix =DirectX::XMMatrixIdentity();

		if (childTransform.GetIndependentScaleFlag() || childTransform.GetIndependentRotationFlag() || childTransform.GetIndependentTransitionFlag())
		{
			DirectX::XMVECTOR parentScaleWorld;
			DirectX::XMVECTOR parentQuaternionWorld;
			DirectX::XMVECTOR parentTranslationWorld;

			DirectX::XMMatrixDecompose(&parentScaleWorld, &parentQuaternionWorld, &parentTranslationWorld, parentWorldMatrix);

			if (!childTransform.GetIndependentScaleFlag())
			{
				parentWorldMatrix = DirectX::XMMatrixScalingFromVector(parentScaleWorld);
			}
			else
			{
				parentWorldMatrix = DirectX::XMMatrixIdentity();
			}

			if (!childTransform.GetIndependentRotationFlag())
			{
				parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixRotationQuaternion(parentQuaternionWorld));

			}

			if (!childTransform.GetIndependentTransitionFlag())
			{
				parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixTranslationFromVector(parentTranslationWorld));
			}

			DirectX::XMStoreFloat4x4(&parentWorldMatrixF, parentWorldMatrix);
			parentWorldInvMatrix = DirectX::XMMatrixInverse(nullptr, parentWorldMatrix);
		}


		DirectX::XMMATRIX childLocalMatrix = DirectX::XMMatrixMultiply(childWorldMatrix, parentWorldInvMatrix);
		childTransform.SetTransformLocal(childLocalMatrix);
		childTransform.UpdateWorldMatrix(parentWorldMatrixF);



		return;
	}

	bool SceneGraph::Add(Object* ob)
	{
		AddNewObjectNode(ob);
		return true;

	}

	void SceneGraph::RemoveObject(const std::wstring& name)
	{







	}

	void SceneGraph::RemoveObject(Object* ob)
	{
		
		//mObjectManager->KillObject(ob);
		//mObjectIDTable.erase(ob->GetUniqueID());


		Node* currNode = findNode(ob);
		if (currNode == nullptr)
			return;
		Node* parentNode = currNode->mParentNode;
		
		if (parentNode->mObject.GetPointer() != mRoot->mObject.GetPointer())
		{
			parentNode->mObject->RemoveChildObjectInVector(ob);
		}

		//제거된 노드의 자식노드들은 , 루트노드가 부모노드가된다.


		Node* childNode = currNode->mFirstChildNode;
		Node* lastChildNode = childNode;
		while (childNode != nullptr)
		{
			childNode->mParentNode = mRoot;
			lastChildNode = childNode;
			childNode = childNode->mNextSiblingNode;
		}
		if (lastChildNode != nullptr)
		{
			lastChildNode->mNextSiblingNode = mRoot->mFirstChildNode;
			mRoot->mFirstChildNode = currNode->mFirstChildNode;
		}



		//부모노드의 자식리스트에서 제거할려는 노드를 뺸다. 

		Node* preChildNode =nullptr;
		childNode = parentNode->mFirstChildNode;
		while (childNode != nullptr)
		{
			if (childNode == currNode)
			{
				if (currNode == parentNode->mFirstChildNode)
				{
					parentNode->mFirstChildNode = currNode->mNextSiblingNode;
				}
				else
				{
					preChildNode->mNextSiblingNode = currNode->mNextSiblingNode;
				}
				break;
			}
			preChildNode = childNode;
			childNode = childNode->mNextSiblingNode;
		}
	

		//노드제거
		delete currNode;




	}

	/*Object* SceneGraph::FindObject(unsigned long long id) const
	{
		std::unordered_map<unsigned long long, Object*>::const_iterator it = mObjectIDTable.find(id);
		return it != mObjectIDTable.end() ? it->second : nullptr;

	}

	Object* SceneGraph::FindObject(const std::string& name) const
	{
		
		unsigned long long id = mObjectManager->GetObjectFromName(name)->GetUniqueID();
		return FindObject(id);
	}*/

	bool SceneGraph::GetRootChildObjectVector(std::vector<Object*>& oRootChildObjectVector) const
	{
		return GetChildObjectVector(mRoot->mObject.GetPointer(), oRootChildObjectVector);
	}

	bool SceneGraph::GetChildObjectVector(Object* parent, std::vector<Object*>& oChildObjectVector) const
	{

		Node* parentNode = findNode(parent);

		if (parentNode != nullptr)
		{
			Node* childNode = parentNode->mFirstChildNode;

			while (childNode != nullptr)
			{
				Entity* test = (Entity*)childNode->mObject.GetPointer();
				oChildObjectVector.push_back(childNode->mObject.GetPointer());
				childNode = childNode->mNextSiblingNode;
			}
			return true;
		}

		return false;
	}

	size_t SceneGraph::GetChildObjectNum(Object* parent) const
	{
		Node* parentNode = findNode(parent);
		if (parentNode == nullptr)
		{
			//throw 
			return 0;
		}

		Node* childNode = parentNode->mFirstChildNode;
		size_t childNum = 0;
		while (childNode != nullptr)
		{
			childNum++;
			childNode = childNode->mNextSiblingNode;
		}

		return childNum;

	}

	const std::vector<Object*> SceneGraph::GetAllObjectVector() const
	{

		//return mObjecVector;
	
		std::vector<Object*> objectVector;

		std::queue<SceneGraph::Node*> nodeQueue;
		nodeQueue.push(mRoot);

		while (!nodeQueue.empty())
		{
			SceneGraph::Node* currNode = nodeQueue.front();
			nodeQueue.pop();


			SceneGraph::Node* childNode = currNode->mFirstChildNode;
			while (childNode != nullptr)
			{

				if (childNode->mObject->GetEntrieDrawFlag())
				{
					if (childNode->mObject->GetDrawFlag())
					{
						objectVector.push_back(childNode->mObject.GetPointer());
					}
					nodeQueue.push(childNode);
				}
				childNode = childNode->mNextSiblingNode;
			}
		}

		return objectVector;
		//return true;
	}
 

	/*void SceneGraph::Seralize()
	{




	}

	void SceneGraph::DeSeralize()
	{




	}*/



	void SceneGraph::Reset()
	{



		if (mRoot == nullptr)
			return;


		//mObjectIDTable.clear();

	

		std::stack<Node*> nodeStack;

		nodeStack.push(mRoot);
		Node* parentNode = nodeStack.top();
		nodeStack.pop();

		Node* childNode = parentNode->mFirstChildNode;
		while (childNode != nullptr)
		{
			nodeStack.push(childNode);
			childNode = childNode->mNextSiblingNode;
		}

		Node* currentNode = nullptr;
		while (!nodeStack.empty())
		{
			currentNode = nodeStack.top();
			nodeStack.pop();
			

			childNode = currentNode->mFirstChildNode;
			while (childNode != nullptr)
			{
				nodeStack.push(childNode);
				childNode = childNode->mNextSiblingNode;
			}

			//ObjectSmartPointer object = currentNode->mObject;
			//mObjectManager->KillObject(object.GetPointer());
			delete currentNode;
		}
		
		
		mRoot->mFirstChildNode = nullptr;

		//mObjectManager->RemoveDeadObject();


	}

	SceneGraph::Node* SceneGraph::findNode(Object* object) const
	{
		//순회를한다.


		//루트도 돌려줘야돼 
		std::queue<Node*> nodeQueue;
		nodeQueue.push(mRoot);

		Node* childNode = nullptr;//mRoot->mFirstChildNode;
		//while (childNode != nullptr)
		//{
		//	nodeQueue.push(childNode);
		//	childNode = childNode->mNextSiblingNode;
		//}


		while (!nodeQueue.empty())
		{

			Node* currNode = nodeQueue.front();
			nodeQueue.pop();


			if (currNode->mObject.GetPointer() == object)
			{
				return currNode;
			}

			childNode = currNode->mFirstChildNode;
			while (childNode != nullptr)
			{
				nodeQueue.push(childNode);
				childNode = childNode->mNextSiblingNode;
			}
		}

		//throw ex				(없을수가없다)
		return nullptr;

	}



	void SceneGraph::AddNewObjectNode(Object* ob)
	{
		//부모가 있는가
		//있으면 부모가이미 씬에 추가되었다고생각하고 그 부모의 자식노드가 생성되는것이다.
		Node* parentNode = mRoot;
		if (ob->GetParentObject() != nullptr)
		{
			//부모오브젝트는 씬에 안넣고 자식오브젝트만 씬에 넣는다 ? 그건없을거같은데, 
			parentNode = findNode(ob->GetParentObject());


		}


		//object의 자식들도 다 추가한다.
		//초기화
		//Node* parentNode = mRoot;

		//mRoot->mFirstChildNode = newNode;

		Node* newNode = new Node;
		newNode->mFirstChildNode = nullptr;
		newNode->mNextSiblingNode = parentNode->mFirstChildNode;
		newNode->mObject = ob;
		newNode->mParentNode = parentNode;
		parentNode->mFirstChildNode = newNode;
		

		
		ob->SetIsAddedToSceneFlag(true);
		//mObjectIDTable.try_emplace(ob->GetUniqueID(), ob);

		if (mPlayMode == false && ob->GetEnginObjectFlag())
		{
			//mEditUpdateObjectVector.push_back(ob);
		}




		//그오브젝트들의 자식들을 처리한다.
	//	std::queue<Object*> objectQueue;
		std::queue<Node*> nodeQueue;
		//	objectQueue.push(ob);
		nodeQueue.push(newNode);

		Object* currObject;
		while (!nodeQueue.empty())
		{
			//currObject = objectQueue.front();
			parentNode = nodeQueue.front();
			currObject = parentNode->mObject.GetPointer();

			//objectQueue.pop();
			nodeQueue.pop();

			//mObjectIDTable.try_emplace(currObject->GetUniqueID(), currObject);

			const std::vector<ObjectSmartPointer>& objectVector = currObject->GetChildObjectVector();

			for (int i = 0; i < objectVector.size(); ++i)

			{
				Object* childObject = objectVector[i].GetPointer();
				if (childObject == nullptr)
					continue;


				//중복될가능성을 구조적으로 막을것이니
				/*std::unordered_map<unsigned long long, Object*>::iterator ret = mObjectIDTable.find(childObject->GetUniqueID());
				if (ret != mObjectIDTable.end())
					continue;*/


				newNode = new Node;
				newNode->mFirstChildNode = nullptr;
				newNode->mNextSiblingNode = parentNode->mFirstChildNode;
				newNode->mObject = objectVector[i];
				newNode->mParentNode = parentNode;
				parentNode->mFirstChildNode = newNode;

				//mObjectIDTable[childObject->GetUniqueID()] = childObject;


				objectVector[i]->SetIsAddedToSceneFlag(true);

				if (mPlayMode == false && childObject->GetEnginObjectFlag())
				{
					//mEditUpdateObjectVector.push_back(childObject);
				}
				//objectQueue.push(objectVector[i].GetPointer());


				nodeQueue.push(newNode);
			}

		}




	}

	void SceneGraph::SetPauseState(bool state)
	{
		mPauseState = state;
	}

	bool SceneGraph::GetPauseState() const
	{
		return mPauseState;
	}


}
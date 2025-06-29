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

//#include"ObjectManager/ObjectManager.h"

#include"Component/ColliderBaseComponent.h"

#include"Component/UiColliderComponent.h"
namespace Quad
{
	namespace Scene
	{

		SceneGraph::SceneGraph() :mRoot(new Scene::Node)
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
			mRoot->mObject = new Object;
			//mRoot->mObject->GetTransform().Initialize();
			Transform& transform = mRoot->mObject->GetTransform();
			DirectX::XMFLOAT4X4 identityMatrix;
			DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());

			mPlayMode = playMode;
		}


		void SceneGraph::Update(float deltaTime)
		{

			//object들의 update호출
			//순회하면서  update를 호출한다.

			std::vector<Scene::Node*> nodeStack;
			nodeStack.reserve(64);

			Scene::Node* childNode = nullptr;


			//change matrix를 초기화 
			childNode = mRoot->mFirstChildNode;

			while (childNode != nullptr)
			{
				nodeStack.push_back(childNode);
				childNode = childNode->mNextSiblingNode;
			}

			while (!nodeStack.empty())
			{

				Scene::Node* currNode = nodeStack.back();
				nodeStack.pop_back();

				if (currNode->mObject->GetKilledState())
					continue;

				if (currNode->mObject->GetActiveFlag() == false)
					continue;


				currNode->mObject->Update(deltaTime);

				childNode = currNode->mFirstChildNode;
				while (childNode != nullptr)
				{
					nodeStack.push_back(childNode);
					childNode = childNode->mNextSiblingNode;
				}


			}


		}



		void SceneGraph::ChangeParent(Object* newParent, Object* child)
		{



			//두개의 노드를 찾는게 필요 + child의 부모노드도 찾아야지
			//find 

			if (newParent == nullptr)
				newParent = mRoot->mObject;

			Scene::Node* newChildNode = child->mSceneGraphNode;
			Scene::Node* newParentNode = newParent->mSceneGraphNode;
			//순환구조를 방지해야한다.

			if (IsAncestorNode(newParentNode, newChildNode))
			{
				//newParent가 child의 자식관계임으로
				//순환구조이다. 이 것은 실패한다.
				return;
			}


			/*	if (newChildNode == nullptr)
					return;*/


			Scene::Node* preParentNode = newChildNode->mParentNode;
			Scene::Node* preChildNode = preParentNode->mFirstChildNode;
			Scene::Node* currChildNode = preParentNode->mFirstChildNode;

			Object* preParentObject = preParentNode->mObject;


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


					break;
				}

				preChildNode = currChildNode;
				currChildNode = currChildNode->mNextSiblingNode;
			}


			newChildNode->mNextSiblingNode = newParentNode->mFirstChildNode;
			newParentNode->mFirstChildNode = newChildNode;
			newChildNode->mParentNode = newParentNode;



			//기존부모와의 관계 관련데이터 업데이트
			child->UpdateSceneComponentWorldTransformReculsivly();

			std::vector<Object*>& preParentChildObjectVector = preParentObject->mChildObjectVector;
			preParentChildObjectVector.erase(std::find(preParentChildObjectVector.begin(), preParentChildObjectVector.end(), child));


			//새로운 부모에 상대적인 로컬계산까지는 기존부모포인터는 유지해서 최종적으로 올바른 world값을 가져와서 계산하도록한다.

			//새로운 부모와의 관계구축을 위해 관련데이터 업데이트
			child->mParentObject = newParent;
			SceneComponent* newParentRootSceneComponent = newParent->GetRootSceneComponent();
			newParentRootSceneComponent->UpdateNewChildComponentLocal(child->GetRootSceneComponent());//자식의 루트컴포넌트의 로컬만 올바르게 조정하면된다. 루트컴포넌트의 자식들의 로컬은 루트컴포넌트에 상대적인거니깐
			child->UpdateSceneComponentWorldTransformReculsivly();
			;//처음에는 모든 자식컴포넌트들을 갱신하자.


			newParent->mChildObjectVector.push_back(child);


			return;
		}

		bool SceneGraph::Add(Object* ob)
		{


			AddNewObjectNode(ob);
			return true;

		}


		void SceneGraph::RemoveKilledObjectAll()
		{
			//kill상태인 최상위 object들의 부모노드는 rootNode
			//if(이미 이 프레임에 한번호출했다면 바로 빠져나오자 return )




			Scene::Node** ppNode = &mRoot->mFirstChildNode;

			while (*ppNode)
			{
				if ((*ppNode)->mObject->GetKilledState())
				{
					Scene::Node* tempNode = *ppNode;
					*ppNode = (*ppNode)->mNextSiblingNode;
					RemoveNodeAll(tempNode);
				}
				else
				{
					ppNode = &((*ppNode)->mNextSiblingNode);
				}
			}

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
			return GetChildObjectVector(mRoot->mObject, oRootChildObjectVector);
		}

		bool SceneGraph::GetChildObjectVector(Object* parent, std::vector<Object*>& oChildObjectVector) const
		{

			Scene::Node* parentNode = findNode(parent);

			if (parentNode != nullptr)
			{
				Scene::Node* childNode = parentNode->mFirstChildNode;

				while (childNode != nullptr)
				{
					Entity* test = (Entity*)childNode->mObject;
					oChildObjectVector.push_back(childNode->mObject);
					childNode = childNode->mNextSiblingNode;
				}
				return true;
			}

			return false;
		}

		size_t SceneGraph::GetChildObjectNum(Object* parent) const
		{
			Scene::Node* parentNode = findNode(parent);
			if (parentNode == nullptr)
			{
				//throw 
				return 0;
			}

			Scene::Node* childNode = parentNode->mFirstChildNode;
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

			std::queue<Scene::Node*> nodeQueue;
			nodeQueue.push(mRoot);

			while (!nodeQueue.empty())
			{
				Scene::Node* currNode = nodeQueue.front();
				nodeQueue.pop();


				Scene::Node* childNode = currNode->mFirstChildNode;
				while (childNode != nullptr)
				{

					if (childNode->mObject->GetEntrieDrawFlag())
					{
						if (childNode->mObject->GetDrawFlag())
						{
							objectVector.push_back(childNode->mObject);
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

			RemoveNodeAll(mRoot);


		}

		Scene::Node* SceneGraph::findNode(Object* object) const
		{
			//순회를한다.


			//루트도 돌려줘야돼 
			std::queue<Scene::Node*> nodeQueue;
			nodeQueue.push(mRoot);

			Scene::Node* childNode = nullptr;//mRoot->mFirstChildNode;
			//while (childNode != nullptr)
			//{
			//	nodeQueue.push(childNode);
			//	childNode = childNode->mNextSiblingNode;
			//}


			while (!nodeQueue.empty())
			{

				Scene::Node* currNode = nodeQueue.front();
				nodeQueue.pop();


				if (currNode->mObject == object)
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

		Quad::Scene::Node* SceneGraph::FindAncestorNode(Scene::Node* descendantNode, Object* ancestorObject)
		{

			Scene::Node* currNode = descendantNode;
			while (currNode != nullptr)
			{
				currNode = currNode->mParentNode;
				if (currNode->mObject == ancestorObject)
					break;
			}

			return currNode;
		}

		bool SceneGraph::IsAncestorNode(Scene::Node* descendantNode, Scene::Node* ancestorObject)
		{
			Scene::Node* currNode = descendantNode;
			while (currNode != nullptr)
			{
				currNode = currNode->mParentNode;
				if (currNode == ancestorObject)
					return true;
			}

			return false;
		}

		void SceneGraph::RemoveNodeAll(Scene::Node* node)
		{
			std::queue<Scene::Node*> nodeQueue;
			nodeQueue.push(node);
			while (!nodeQueue.empty())
			{
				Scene::Node* temp = nodeQueue.front();
				nodeQueue.pop();

				for (Scene::Node* childNode = temp->mFirstChildNode; childNode != nullptr; childNode = childNode->mNextSiblingNode)
				{
					nodeQueue.push(childNode);
				}
				delete temp;
			}
		}


		void SceneGraph::AddNewObjectNode(Object* ob)
		{
			//부모가 있는가
			//있으면 부모가이미 씬에 추가되었다고생각하고 그 부모의 자식노드가 생성되는것이다.
			Scene::Node* parentNode = mRoot;
			if (ob->GetParentObject() != nullptr)
			{
				//부모오브젝트는 씬에 안넣고 자식오브젝트만 씬에 넣는다 ? 그건없을거같은데, 
				parentNode = findNode(ob->GetParentObject());


			}


			//object의 자식들도 다 추가한다.
			//초기화
			//Node* parentNode = mRoot;

			//mRoot->mFirstChildNode = newNode;

			Scene::Node* newNode = new Scene::Node;
			newNode->mFirstChildNode = nullptr;
			newNode->mNextSiblingNode = parentNode->mFirstChildNode;
			newNode->mObject = ob;
			newNode->mParentNode = parentNode;
			parentNode->mFirstChildNode = newNode;



			//ob->SetIsAddedToSceneFlag(true);
			//mObjectIDTable.try_emplace(ob->GetUniqueID(), ob);

			//if (mPlayMode == false && ob->GetEnginObjectFlag())
			//{
			//	//mEditUpdateObjectVector.push_back(ob);
			//}

			ob->mSceneGraphNode = newNode;


			//	//그오브젝트들의 자식들을 처리한다.
			////	std::queue<Object*> objectQueue;
			//	std::queue<Node*> nodeQueue;
			//	//	objectQueue.push(ob);
			//	nodeQueue.push(newNode);

			//	Object* currObject;
			//	while (!nodeQueue.empty())
			//	{
			//		//currObject = objectQueue.front();
			//		parentNode = nodeQueue.front();
			//		currObject = parentNode->mObject.GetPointer();

			//		//objectQueue.pop();
			//		nodeQueue.pop();

			//		//mObjectIDTable.try_emplace(currObject->GetUniqueID(), currObject);

			//		const std::vector<ObjectSmartPointer>& objectVector = currObject->GetChildObjectVector();

			//		for (int i = 0; i < objectVector.size(); ++i)

			//		{
			//			Object* childObject = objectVector[i].GetPointer();
			//			if (childObject == nullptr)
			//				continue;


			//			//중복될가능성을 구조적으로 막을것이니
			//			/*std::unordered_map<unsigned long long, Object*>::iterator ret = mObjectIDTable.find(childObject->GetUniqueID());
			//			if (ret != mObjectIDTable.end())
			//				continue;*/


			//			newNode = new Node;
			//			newNode->mFirstChildNode = nullptr;
			//			newNode->mNextSiblingNode = parentNode->mFirstChildNode;
			//			newNode->mObject = objectVector[i];
			//			newNode->mParentNode = parentNode;
			//			parentNode->mFirstChildNode = newNode;

			//			//mObjectIDTable[childObject->GetUniqueID()] = childObject;


			//			//objectVector[i]->SetIsAddedToSceneFlag(true);

			//			//if (mPlayMode == false && childObject->GetEnginObjectFlag())
			//			//{
			//			//	//mEditUpdateObjectVector.push_back(childObject);
			//			//}
			//			//objectQueue.push(objectVector[i].GetPointer());


			//			nodeQueue.push(newNode);
			//		}

			//	}




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
}
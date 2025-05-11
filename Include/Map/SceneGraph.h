#pragma once

#include"Object/ObjectType.h"
#include<queue>
#include<unordered_map>
#include<string>
#include<vector>
#include"Predeclare.h"
//#include"SceneNode.h"
#include"Object/ObjectSmartPointer.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	
	class CORE_API_LIB SceneGraph
	{
		friend class Gizmo;
		friend class Map;


	private:

		struct Node
		{

			ObjectSmartPointer mObject;
			Node* mParentNode=nullptr;
			Node* mFirstChildNode=nullptr;
			Node* mNextSiblingNode=nullptr;


		};




	public:
		SceneGraph();
		~SceneGraph();


		void Initialize(BaseObjectManager *objectManage,bool playModer);

		void Update(float deltaTime);

		//object를 만들면 controller가 다른 모듈들과 상호작용해서 object에 필요한 속성들을 설정한다.
		//Object* CreateObject(EObjectType type, const std::wstring& name);
		//부모가 nullptr이면 root로 처리하자
		void ChangeParent(Object* newParent, Object* child);
		bool Add(Object* ob);
		void RemoveObject(const std::wstring& name);
		void RemoveObject(Object* ob);
	//	Object * FindObject(unsigned long long id) const;
		//Object * FindObject(const std::string & name) const;
		

		bool GetRootChildObjectVector(std::vector<Object*>& oRootChildObjectVector) const;
		bool GetChildObjectVector(Object* parent, std::vector<Object*>& oChildObjectVector) const;
		size_t GetChildObjectNum(Object* parent) const;

		const std::vector<Object*>  GetAllObjectVector() const;

		//씬그래프 컴포넌트가 들어나는게 정상이고
		//map에서만 접근해야하는 메서드는 map을 friend class로 만들어서 처리하고
		//씬그래프 단독으로 사용할수있는 메서드는 public 으로 제공하고
		//이런방식이 맞는거같은데
		//계속 map에서 인터페이스 한번 더 래핑하는게 무슨의미가있냐 

		//const std::unordered_map<std::string, Object*>& GetObjectUnMap() const;


		/*void Seralize();
		void DeSeralize();*/


	//	const std::unordered_map<unsigned long long, Object*>& GetObjectIDTable() const;
	//	std::unordered_map<unsigned long long, Object*>& GetObjectIDTable();

		void Reset();




		void AddNewObjectNode(Object* ob);



		void SetPauseState(bool state);
		bool GetPauseState()const;

	private:
	
		Node* findNode(Object* object) const;
		Node* findNode(unsigned long long id);

			
	
	private:
		Node* mRoot;
			
		BaseObjectManager* mObjectManager;


		//edit모드에서도 update되어야하는 엔진용 오브젝트들의 캐싱을 위한 벡터
	//	std::vector<Object*> mEditUpdateObjectVector;
		bool mPlayMode = false; //edit mode = false , play mode =true


		bool mPauseState;


	};

}
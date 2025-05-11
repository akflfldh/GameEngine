#pragma once

#include"PanelUiEntity.h"

#include"Singleton.h"
#include<unordered_map>
#include"EditorMacro.h"

namespace Quad
{

	class AnimationStateTransitionGraph;
	class AnimationStateNode;

	class AnimationStateTransitionGraphBasePanel:public PanelUiEntity,public Singleton<AnimationStateTransitionGraphBasePanel>
	{
	public:
		AnimationStateTransitionGraphBasePanel(const std::string& name = "");
		virtual ~AnimationStateTransitionGraphBasePanel();

		virtual void Initialize() override;

		static AnimationStateTransitionGraphBasePanel* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		virtual void SetDrawFlag(bool flag)override;
		void SetDestObject(Object* object);
		Object* GetDestObject() const;
		void InActive();//destObject->nullptr, setdrawflag 를 false처리한후 , 호출할것


		bool CreateStateNode(const std::string& stateName);
		bool RemoveStateNode(const std::string& stateName);
		AnimationStateNode* GetStateNode(const std::string& stateNode);

		AnimationStateTransitionGraph* GetCurrentAnimStateTransitionGraph();




		void Serialize();
		void DeSerialize();


		CLASSNAME(AnimationStateTransitionGraphBasePanel)
		CLASSSIZE(AnimationStateTransitionGraphBasePanel)
		
		
	
	protected:
		void InitCreating();
		virtual void HandleRButtonDown(Event* pEvent) override;
		virtual void HandleRButtonUp(Event* pEvent) override;

		void InitGraphDefaultSetting(AnimationStateTransitionGraph* graph);




		//<object id , object's animationStateTransitionGraph>
		std::unordered_map<unsigned long long, AnimationStateTransitionGraph*> mAnimationStateTransitionGraphTable;
		Object* mDestObject;
		AnimationStateTransitionGraph* mDestObjectGraph;

	};
	REGISTEREDITORCLASS(AnimationStateTransitionGraphBasePanel)
}


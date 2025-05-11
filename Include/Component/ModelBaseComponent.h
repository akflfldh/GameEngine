#pragma once

#include"Predeclare.h"
#include"Object/Macro.h"
namespace Quad
{
	class ModelBaseComponent
	{
	public:
		ModelBaseComponent(SceneElement *sceneElement);
		virtual ~ModelBaseComponent() = 0;

		SceneElement* GetDestSceneElement() const;

		CLASSSIZE(ModelBaseComponent)

			virtual void Update(float deltaTime) = 0;


		virtual void Serialize(const std::string & tag="") = 0;
		virtual void DeSerialize(const std::string & tag="") = 0;

	private:
		SceneElement* mDestSceneElement;	//컴포넌트를 소유하는 씬요소



	};


}

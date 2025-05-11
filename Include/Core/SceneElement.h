#pragma once


//interface class

#include"Predeclare.h"
#include"Component/Model.h"
#include"Core/Transform.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	enum class ESceneElementType
	{
		eObject= 0,
		eCollider

	};



	class CORE_API_LIB SceneElement
	{
	public:
		SceneElement(ESceneElementType sceneElementType);
		~SceneElement();

		virtual  Map* GetMap() const = 0;
		virtual Map* GetMap() = 0;


		virtual void SetSystem(System* system) =0;
		virtual System* GetSystem() const =0;


		const Model* GetModel() const;
		Model* GetModel();

		virtual Transform& GetTransform() = 0;
		virtual const Transform& GetTransform() const = 0;


		ESceneElementType GetSceneElementType() const;
		 
	protected:
		void Serialize(const std::string& tag="");
		void DeSerialize(const std::string& tag="");

	private:
		Model mModel;
		ESceneElementType mSceneElementType;

	};

}

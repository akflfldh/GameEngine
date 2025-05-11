#pragma once

#include<unordered_map>
#include<string>
#include"ComponentIDGenerator.h"
#include"BaseComponent.h"
namespace Quad
{

	enum class EComponentType
	{
		eMeshComponent = 0,
		eAnimationComponent,
		eColliderComponent,
		eUiColliderComponent,
		
	};



	template<typename ComponentType>
	class Component :public BaseComponent
	{
	public:
		Component()=default;
		virtual ~Component() = 0;

		virtual void Update(float delta) = 0;


		virtual int GetID() override final; 
	protected:


	private:




	};

	template<typename ComponentType>
	inline int Component<ComponentType>::GetID()
	{
		return ComponentIDGenerator::GetID<ComponentType>();
	}

	template<typename ComponentType>
	inline Component<ComponentType>::~Component()
	{

	}

}
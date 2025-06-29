#include "AnimationUpdateSystem.h"

#include"Object/Object.h"
#include"Component/AnimationComponent.h"

Quad::AnimationUpdateSystem::~AnimationUpdateSystem()
{
}

void Quad::AnimationUpdateSystem::Update(float deltaTime)
{
	std::vector<Object*> & objectVector = GetObjectVector();


	for (auto objectElement : objectVector)
	{
		if (objectElement->GetStartObjectFlag())
			continue;

		AnimationComponent* animationComponent = objectElement->GetComponent<AnimationComponent>("AnimationComponent");
		animationComponent->Update(deltaTime);
	}




}

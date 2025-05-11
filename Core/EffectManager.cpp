#include "ResourceManager/EffectManager/EffectManager.h"
#include"Effect/Effect.h"


Quad::EffectManager::EffectManager()
:mPoolAllocator(500)
{
	mPoolAllocator.Initialize();
}

Quad::EffectManager::~EffectManager()
{
}

Quad::Effect* Quad::EffectManager::CreateEffect(const std::string& name)
{
	EffectManager * instance = GetInstance();

	if (GetEffect(name) != nullptr)
		return nullptr;



	Effect* effect = instance->mPoolAllocator.GetInstance();
	effect->mName = name;

	instance->mIDTable.Register(effect);
	instance->mNameTable.Register(effect->GetName(),effect->GetUniqueID());

	return effect;


}

Quad::Effect* Quad::EffectManager::GetEffect(unsigned long long id)
{

	auto instance = GetInstance();
	if (id == 0)
		return nullptr;
	else
		return instance->mIDTable.GetElement(id);


}

Quad::Effect* Quad::EffectManager::GetEffect(const std::string& name)
{
	auto instance = GetInstance();
	unsigned long long id = instance->mNameTable.GetID(name);
	if (id == 0)
		return nullptr;
	else
		return instance->mIDTable.GetElement(id);

}

void Quad::EffectManager::RemoveEffect(unsigned long long id)
{
	if (id == 0)
		return;


	auto instance = GetInstance();
	Effect * effect =	instance->mIDTable.GetElement(id);
	if (effect == nullptr)
		return;

	instance->mIDTable.UnRegister(id);
	instance->mNameTable.UnRegister(effect->GetName());

	instance->mPoolAllocator.ReleaseInstance(effect);

}

void Quad::EffectManager::RemoveEffect(const std::string& name)
{

	auto instance = GetInstance();
	unsigned long long id =instance->mNameTable.GetID(name);

	RemoveEffect(id);

}

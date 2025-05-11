#include "RegisterAnimStateTransitionCallbackClassManager.h"
#include"Parser/JsonParser.h"

Quad::RegisterAnimStateTransitionCallbackClassManager::RegisterAnimStateTransitionCallbackClassManager(Map* destMap)
	:mRegisterAnimStateTransitionCallbackClassAllocator(10000),mDestMap(destMap)
{
	mRegisterAnimStateTransitionCallbackClassAllocator.Initialize();

}

Quad::RegisterAnimStateTransitionCallbackClassManager::~RegisterAnimStateTransitionCallbackClassManager()
{


}

void Quad::RegisterAnimStateTransitionCallbackClassManager::Start()
{
	//auto instance = GetInstance();
	for (auto& element : mRegisterAnimStateTransitionCallbackClassVector)
	{
		element->Start();
	}
}

void Quad::RegisterAnimStateTransitionCallbackClassManager::Clear()
{
	for (auto& callbackClass : mRegisterAnimStateTransitionCallbackClassVector)
	{
		Release(callbackClass);
	}

	mRegisterAnimStateTransitionCallbackClassVector.clear();



}

Quad::RegisterAnimStateTransitionCallbackClass* Quad::RegisterAnimStateTransitionCallbackClassManager::CreateRegisterAnimStateTransitionCallbackClass(AnimationComponent* animComponen)
{
	RegisterAnimStateTransitionCallbackClass * ob= CreateRegisterAnimStateTransitionCallbackClass();
	if (ob)
	{
		ob->SetAnimaitonComponent(animComponen);
		ob->SetDestRegisterAnimStateTransitionCallbackClassMangaer(this);
	}

	return ob;
}

void Quad::RegisterAnimStateTransitionCallbackClassManager::Release(RegisterAnimStateTransitionCallbackClass* ob)
{
	if (ob == nullptr)
		return;
	//auto instance = GetInstance();

	


	std::vector< RegisterAnimStateTransitionCallbackClass*>::iterator callbackClassIt= 	std::find(mRegisterAnimStateTransitionCallbackClassVector.begin(), mRegisterAnimStateTransitionCallbackClassVector.end(), ob);

	*callbackClassIt = mRegisterAnimStateTransitionCallbackClassVector.back();
	mRegisterAnimStateTransitionCallbackClassVector.pop_back();


	mRegisterAnimStateTransitionCallbackClassAllocator.ReleaseInstance(ob);

}

void Quad::RegisterAnimStateTransitionCallbackClassManager::Release(AnimationComponent* animComponent)
{


	RegisterAnimStateTransitionCallbackClass* ob =	GetRegisterAnimStateTransitionCallbackClass(animComponent);

	Release(ob);

}

Quad::RegisterAnimStateTransitionCallbackClass* Quad::RegisterAnimStateTransitionCallbackClassManager::GetRegisterAnimStateTransitionCallbackClass(const AnimationComponent* animComponent)
{
//	auto instance = GetInstance();

	std::vector< RegisterAnimStateTransitionCallbackClass*> & classVector = mRegisterAnimStateTransitionCallbackClassVector;

	auto  callbackClassIt =	std::find_if(classVector.begin(), classVector.end(), [animComponent](RegisterAnimStateTransitionCallbackClass  * callbackClass) {
		if (callbackClass->GetAnimationComponent() == animComponent)
		{
		return true;
		}
		return false;
		
	});


	if (callbackClassIt != classVector.end())
		return *callbackClassIt;

	return nullptr;

}

Quad::Map* Quad::RegisterAnimStateTransitionCallbackClassManager::GetDestMap() const
{
	return mDestMap;
}

void Quad::RegisterAnimStateTransitionCallbackClassManager::Serialize()
{

	//auto instance = GetInstance();

	JsonParser::StartWriteObject();

	JsonParser::Write("RegisterAnimStateTransitionCallbackClassManager_CallbackClassNum", mRegisterAnimStateTransitionCallbackClassVector.size());


	for (auto& element :mRegisterAnimStateTransitionCallbackClassVector)
	{
		JsonParser::StartWriteObject();
		element->Serialize();
	}

}

void Quad::RegisterAnimStateTransitionCallbackClassManager::DeSerialize()
{


	
	JsonParser::IncrementCurrentIndex();
	//auto instance = GetInstance();
	size_t callbackClassNum = 0;
	JsonParser::Read("RegisterAnimStateTransitionCallbackClassManager_CallbackClassNum", callbackClassNum);

	for(size_t i =0 ; i< callbackClassNum; ++i)
	{
		JsonParser::IncrementCurrentIndex();
		RegisterAnimStateTransitionCallbackClass * callbackClassOb =  CreateRegisterAnimStateTransitionCallbackClass();
		callbackClassOb->DeSerialize();

	}





}

Quad::RegisterAnimStateTransitionCallbackClass* Quad::RegisterAnimStateTransitionCallbackClassManager::CreateRegisterAnimStateTransitionCallbackClass()
{
	//auto instance = GetInstance();
	RegisterAnimStateTransitionCallbackClass* ob = mRegisterAnimStateTransitionCallbackClassAllocator.GetInstance();
	ob->SetDestRegisterAnimStateTransitionCallbackClassMangaer(this);

	mRegisterAnimStateTransitionCallbackClassVector.push_back(ob);
	return ob;

}

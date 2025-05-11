#pragma once

#include"Utility/Singleton.h"
#include"Allocator/DestructingPoolAllocator.h"

#include"RegisterAnimStateTransitionCallbackClass.h"
#include<vector>
#include"CorE/CoreDllExport.h"


namespace Quad
{
	class CORE_API_LIB RegisterAnimStateTransitionCallbackClassManager/*:public Singleton<RegisterAnimStateTransitionCallbackClassManager>*/
	{
	public:
		RegisterAnimStateTransitionCallbackClassManager(Map * destMap);
		~RegisterAnimStateTransitionCallbackClassManager();


		void Start();
		

		void Clear();

		RegisterAnimStateTransitionCallbackClass* CreateRegisterAnimStateTransitionCallbackClass(AnimationComponent * animComponent);
		void Release(RegisterAnimStateTransitionCallbackClass* ob);
		void Release(AnimationComponent* animComponent);
		


		RegisterAnimStateTransitionCallbackClass* GetRegisterAnimStateTransitionCallbackClass(const AnimationComponent* animComponent);

		Map* GetDestMap() const;

		 void Serialize();
		 void DeSerialize();
	private:

		 RegisterAnimStateTransitionCallbackClass* CreateRegisterAnimStateTransitionCallbackClass();

	private:


		DestructingPoolAllocator< RegisterAnimStateTransitionCallbackClass> mRegisterAnimStateTransitionCallbackClassAllocator;

		std::vector< RegisterAnimStateTransitionCallbackClass*> mRegisterAnimStateTransitionCallbackClassVector;

		Map* mDestMap;
	};

}

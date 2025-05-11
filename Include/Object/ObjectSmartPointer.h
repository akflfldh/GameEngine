#pragma once



#include"Core/CoreDllExport.h"

namespace Quad
{
	class Object;

	class CORE_API_LIB ObjectSmartPointer
	{
	public:
		ObjectSmartPointer();
		~ObjectSmartPointer();

		ObjectSmartPointer(const ObjectSmartPointer& pointer);
		ObjectSmartPointer(ObjectSmartPointer&&) = default;
		ObjectSmartPointer& operator=(ObjectSmartPointer&&) = default;


		ObjectSmartPointer(Object* object);


		ObjectSmartPointer(unsigned long long objectID);

		ObjectSmartPointer& operator=(const ObjectSmartPointer& pointer);
		ObjectSmartPointer& operator=(Object* object);
		ObjectSmartPointer& operator=(unsigned long long id);

		Object* operator->() const;

		//매개변수가 true라면 Table을 검사해서 그 id를 가지는 엔티티가 여전히 유효한지검사한다.
		Object* GetPointer(bool checkTableFlag=false);
		Object* GetPointer() const;
		unsigned long long GetObjectID() const;

		//table을 검사하여 이 id를 가지는 엔티티가 여전히 존재하는지 검사한다, 없으면 주소는nullptr과 id는 0으로 설정된다.
		void CheckAddress();

	private:

		unsigned long long mObjectID;
		mutable Object* mObject;

	};

}

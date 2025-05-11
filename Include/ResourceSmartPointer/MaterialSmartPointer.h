#pragma once


#include"Core/CoreDllExport.h"

#include"ResourceSmartPointer/IAssetSmartPointer.h"
#include"Asset/Material/Material.h"


namespace Quad
{
	class Material;


	class CORE_API_LIB	MaterialSmartPointer : public IAssetSmartPointer
	{
	public:
		MaterialSmartPointer();
		~MaterialSmartPointer();

		MaterialSmartPointer(const MaterialSmartPointer& pointer)=default;
		MaterialSmartPointer(MaterialSmartPointer&&) = default;
		MaterialSmartPointer& operator=(MaterialSmartPointer&&) = default;


		MaterialSmartPointer(Material* mat);


		MaterialSmartPointer(unsigned long long objectID);

		MaterialSmartPointer& operator=(const MaterialSmartPointer& pointer);
		MaterialSmartPointer& operator=(Material* mat);
		MaterialSmartPointer& operator=(unsigned long long id);

		Material* operator->() const;
		Material& operator*() const;
	
		//매개변수가 true라면 Table을 검사해서 그 id를 가지는 엔티티가 여전히 유효한지검사한다.
		virtual Material* GetPointer(bool checkTableFlag = false) const override;

		unsigned long long GetID() const;

		//table을 검사하여 이 id를 가지는 엔티티가 여전히 존재하는지 검사한다, 없으면 주소는nullptr과 id는 0으로 설정된다.
		void CheckAddress() const;


	private:

		unsigned long long mID;
		mutable Material* mMaterial;



	};

}
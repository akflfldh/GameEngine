#pragma once




#include"Core/CoreDllExport.h"
#include"Asset/Mesh/Mesh.h"

#include"ResourceSmartPointer/IAssetSmartPointer.h"

namespace Quad
{
	class Mesh;

	class CORE_API_LIB MeshSmartPointer :public IAssetSmartPointer
	{
	public:
		MeshSmartPointer();
		~MeshSmartPointer();

		MeshSmartPointer(const MeshSmartPointer& pointer)=default;
		MeshSmartPointer(MeshSmartPointer&&) = default;
		MeshSmartPointer& operator=(MeshSmartPointer&&) = default;

		bool operator == (Mesh* mesh) const ;
		bool operator!=(Mesh* mesh) const;


		MeshSmartPointer(Mesh* object);


		MeshSmartPointer(unsigned long long iD);

		MeshSmartPointer& operator=(const MeshSmartPointer& pointer);
		MeshSmartPointer& operator=(Mesh* mesh);
		MeshSmartPointer& operator=(unsigned long long id);

		Mesh* operator->() const;

		//매개변수가 true라면 Table을 검사해서 그 id를 가지는 엔티티가 여전히 유효한지검사한다.
		virtual Mesh* GetPointer(bool checkTableFlag = false) const override;

		unsigned long long GetID() const;

		//table을 검사하여 이 id를 가지는 엔티티가 여전히 존재하는지 검사한다, 없으면 주소는nullptr과 id는 0으로 설정된다.
		void CheckAddress()const;

	private:

		unsigned long long mID;
		mutable Mesh* mMesh;

	};

}

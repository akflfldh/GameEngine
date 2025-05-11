#pragma once



#include"Predeclare.h"
#include<vector>
#include<string>
#include<DirectXMath.h>
#include"Asset/Asset.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB Skeleton:public Asset
	{
		friend class SkeletonManager;
		friend class ResourceLoader;
	public:
		Skeleton(const std::string& name = "");
		~Skeleton();

		void Copy(Skeleton&& skeleton);
		

		void AddBone(Bone* bone, int parentBoneIndex);
		//Bone* GetBone(const std::wstring & boneName);

		const std::vector<Bone*>& GetBoneVector()const;
	//	std::vector<Bone*>& GetBoneVector();
		const std::vector<int> &GetBoneHiererachyVector()const;
		
		int GetBoneID(const std::string& name)const;
		Bone* GetBone(const std::string& name)const;
		Bone* GetBone(int boneID) const ;
		

		void SetOffsetMatrix(int boneID, const DirectX::XMFLOAT4X4& matrix);
		
		const std::vector<DirectX::XMFLOAT4X4>& GetOffsetMatrixVector()const;




	private:
	


		std::vector<Bone*> mBoneVector;
		std::vector<int> mBoneHiererachyVector;
		std::vector<DirectX::XMFLOAT4X4> mOffsetMatrixVector;


	};

}
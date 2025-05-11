#pragma once



#include"RawResourceType.h"
#include<fstream>
#include"Core/CoreDllExport.h"
#include"Utility/Singleton.h"
namespace Quad
{

	
	class CORE_API_LIB ResourceStorer:public Singleton<ResourceStorer>
	{
	public:

		ResourceStorer() = default;
		~ResourceStorer() = default;

		void Initialize();

		void SaveMesh(const std::string & folderPath ,Mesh * mesh);
		
		void SaveTexture();

		void SaveMaterial(const std::string& materialFilePath, Material* material);

		void SaveEffect();
		
		void SaveSkeleton(const std::string& skeletonFolderPath, Skeleton* skeleton);

		void SaveAnimationClip(const std::string& animationClipFolderPath, AnimationClip* animClip);

	private:

		inline void Write(std::ofstream& fout, const DirectX::XMFLOAT2& float2);
		inline void Write(std::ofstream& fout, const DirectX::XMFLOAT3& float3);
		inline void Write(std::ofstream& fout, const DirectX::XMFLOAT4& float4);
		inline void Write(std::ofstream& fout, const DirectX::XMINT4& int4);
		
		inline void Write(std::ofstream& fout, const DirectX::XMFLOAT4X4& float4x4);
		//inline void Write(std::ofstream& fout, const size_t st);
		inline void Write(std::ofstream& fout, const int i);
		inline void Write(std::ofstream& fout, const float f);
		inline void Write(std::ofstream& fout, const unsigned long long ull);


		void SaveBone(std::ofstream& fout, const std::vector<Bone*>& boneVector);






	};
}

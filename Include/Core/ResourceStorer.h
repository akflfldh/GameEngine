#pragma once



#include"RawResourceType.h"
#include<fstream>
#include"Core/CoreDllExport.h"
#include"Utility/Singleton.h"

#include<vector>
#include"ResourceFileType.h"
#include"DirectTexX/DirectXTex.h"

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



		void SaveTexturePackage(const std::string& filePath, const std::vector<ResourcePackageEntryMeta >& texturePackageEntryMetaVector, const std::vector<DirectX::Blob>& textureBlobVector);
		
		void SaveMaterialPackage(const std::string& filePath, const std::vector<Material*>& materialVector);

		void SaveMeshPackage(const std::string& filePath, const std::vector<Mesh*>& meshVector);

		void SaveSkeletonPackage(const std::string& filePath, const std::vector<Skeleton*> & skeletonVector);

		void SaveAnimationClipPackage(const std::string& filePath, const std::vector<AnimationClip*> & animClipVector);



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
		inline void Write(std::ofstream& fout, const std::string & str);
		inline void Write(std::ofstream& fout, const std::uint8_t* ptr, size_t size);

		inline void Write(std::ofstream& fout,const Material* material);
		inline void Write(std::ofstream& fout, const Mesh* mesh);
		
		inline void WriteMeshCore(std::ofstream& fout, const Mesh* mesh);


		inline void WriteHeader(std::ofstream & fout ,unsigned long long entryNum);

		inline void Write(std::ofstream& fout, const ResourcePackageEntryMeta& resourcePackageEntryMeta);
		inline void Write(std::ofstream& fout, const ResourceMeshPackageEntryMeta& resourcePackageMeshEntryMeta);



		inline void WriteSkeletonCore(std::ofstream& fout, Skeleton* skeleton);
		inline void WriteAnimationClipCore(std::ofstream& fout, AnimationClip* animClip);


		void SaveBone(std::ofstream& fout, const std::vector<Bone*>& boneVector);




		unsigned long long GetMeshDataSize(Mesh* mesh);
		unsigned long long GetSkeletonDataSize(Skeleton* skeleton);
		unsigned long long GetAnimationDataSize(AnimationClip* animClip);




	};
}

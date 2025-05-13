#pragma once



#include<string>
#include<fstream>
#include<vector>

#include"Asset/Mesh/MeshType.h"
#include"TextureLoader.h"

#include"RawResourceType.h"
#include"SchemaLoader.h"
#include"Core/EffectLoader.h"
//#include"FbxParser.h"
//using MeshIndexType = uint32_t;

#include"ResourceImportCommonHeader.h"
#include"Core/CoreDllExport.h"
#include"Utility/Singleton.h"

#include"ResourceFileType.h"
namespace Quad
{


	

	class CORE_API_LIB ResourceLoader:public Singleton<ResourceLoader>
	{
	public:

		ResourceLoader() = default;
		~ResourceLoader() = default;


		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			GraphicCommand * graphicsCommandObject,DescriptorHeapManagerMaster * descriptorHeapManagerMaster);
		//void LoadResource(const std::string& filePath);


		Mesh * LoadMeshFile(const std::string& meshMetaFilePath);
		//bool LoadMaterialFile(const std::string& filePath, RawMaterialData& oRawMaterialData);

		Texture * LoadTextureFile(const std::string& filePath);
		//bool LoadTexture(const std::wstring& filePath, Texture& oTextureData);
		//이미존재하는 metaData가 존재하는 텍스처를 로드하는메서드
		Texture* LoadTextureFromFile(const std::string& filePath, const std::string& unqiueName, unsigned long long uniqueID);
			


	/*	bool LoadFbxFile(const std::string& filePath, std::vector<RawMeshData*>& oRawMeshDataVector,
			std::vector<RawMaterialData>& oRawMaterialDataVector, std::vector<RawTextureData>& oRawTextureDataVector,
			Skeleton*& oSkeleton,
			AnimationClip*& oAnimationClip, LoadResultCommonData & loadResultCommonData);*/

		bool LoadSchema(const std::wstring& filePath, RawSchemaData & oSchemaData);
		bool LoadEffect(const std::string& filePath);



		void LoadProjectData();



		Material * LoadMaterial(const std::string& filePath);
		Skeleton* LoadSkeleton(const std::string& filePath);
		AnimationClip* LoadAnimationClip(const std::string& filePath);



		void LoadTexturePackage(const std::string& filePath);
		
		void LoadMaterialPackage(const std::string& filePath);


		void LoadMeshPackage(const std::string& filePath);


		void LoadSkeletonPackage(const std::string& filePath);


		void LoadAnimClipPackage(const std::string& filePath);

		
	private:
		inline void Read(std::ifstream& fin, DirectX::XMFLOAT2& float2);
		inline void Read(std::ifstream& fin, DirectX::XMFLOAT3& float3);
		inline void Read(std::ifstream& fin, DirectX::XMFLOAT4& float4);
		inline void Read(std::ifstream& fin, DirectX::XMINT4& int4);
		inline void Read(std::ifstream& fin, DirectX::XMFLOAT4X4& float4x4);
		inline void Read(std::ifstream& fin, float& f);
		inline void Read(std::ifstream& fin, int& i);
		inline void Read(std::ifstream& fin, unsigned long long& ull);
		inline void Read(std::ifstream& fin, unsigned int& ui);

		inline void Read(std::ifstream& fin, char* cp, size_t size);
		//inline void Read(std::ifstream& fin, size_t & st);
		
		inline void Read(std::ifstream& fin, std::vector< ResourcePackageEntryMeta>& oResourcePackageEntryMetaVector);
		inline void Read(std::ifstream& fin, std::vector< ResourceMeshPackageEntryMeta>& oResourceMeshPackageEntryMetaVector);

		inline void Read(std::ifstream& fin, Material * mat);




		void LoadProjectEffect();
		void LoadProjectAsset();
		void LoadProjectTexture();
		void LoadProjectMaterial();
		void LoadProjectMesh();

		void LoadProjectObject();

		void LoadBone(std::ifstream &fin ,std::vector<Bone*>& oBoneVector);

		void ReadMeshCore(std::ifstream& fin, Mesh* mesh);
		void ReadSkeletonCore(std::ifstream& fin, Skeleton* skeleton);
		void ReadAnimationClipCore(std::ifstream& fin, AnimationClip* animClip);

		//void CreateSrvTex2D(Texture * texure);
		//D3D12_SRV_DIMENSION ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension);
		//void FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		//ETextureFormat ConvertTextureFormatEnum(DXGI_FORMAT format);

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

	
		GraphicCommand* mGraphicsCommandObject;
		TextureLoader mTextureLoader;
		//SchemaLoader mSchemaLoader;
		EffectLoader mEffectLoader;

		//FbxParser mFbxParser;

		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;

	};



}
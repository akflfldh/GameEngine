#pragma once
#include"Predeclare.h"
#include"Utility/Singleton.h"
#include<vector>
#include<string>
#include"FbxParser.h"
namespace Quad
{
	
	class ImportModule:public Singleton<ImportModule>
	{
	public:
		ImportModule() = default;
		~ImportModule() = default;

		//copyFolderPath는 texture의경우에만 유효하다.
		//logicalFolderPath에 상위디렉토리가있는경우에는 Dir에 '/'(슬래시)까지 헝성 붙여준다. 
		std::vector<Asset*> ImportFile(const std::string& filePath,const std::string & logicalFolderPath,const std::string & copyFolderPath);




	private:
		void ImportFbxFile(const std::string& filePath, const std::string& logicalFolderPath,std::vector<Asset*>& oAssetVector,
			const std::string& copyFolderPath);
		Texture* ImportTextureFile(const std::string& filePath, const std::string& logicalFolderPath,
			const std::string& copyFolderPath);
		Material* CreateMaterial(Quad::RawMaterialData& rawMaterialData, const std::string& logicalFolderPath);
		Quad::Mesh* CreateMesh(Quad::RawMeshData* rawMeshData, const std::string& logicalFolderPath, const std::vector<RawMaterialData>& rawMaterialDataVector);

		FbxParser mFbxParser;



	};

}

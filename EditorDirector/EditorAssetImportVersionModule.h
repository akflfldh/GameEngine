#pragma once



#include"Predeclare.h"
#include<string>

namespace Quad
{

	//에디터에서 사용할 에셋들은 임포트하고 가공해서 저장한다
	//eidtor를 위한것이고 이렇게 가공된 asset들이 정상적인 eidtor실행에 사용된다.
	class EditorAssetImportVersionModule
	{
	public:

		EditorAssetImportVersionModule() = delete;
		~EditorAssetImportVersionModule() = default;


		static void ImportAsset(const std::string& assetFolderPath);

		static void SaveAsset(const std::string& folderPath);

		static void ImportMesh(const std::string& meshFolderPath);
		static void ImportTexture(const std::string& textureFolderPath);


	private:
	
		


		static void SaveMesh(const std::string& assetFolderPath);
		static void SaveMaterial(const std::string& assetFolderPath);
		static void SaveTexture(const std::string& assetFolderPath);



	};

}

#pragma once

namespace QuadPF {

	class PhysicalFileSystem;
}


namespace QuadLF {
	class LogicalFileSystem;
}

namespace CoreAsset
{
	class AssetLoader;
}

namespace Quad
{

	//프로젝트에 관한 부분에대해서 초기셋팅을 수행한다.

	class SerializedAssetTContainer;

	class EditorProjectInitializer
	{
	public:
		static EditorProjectInitializer* GetInstance();

		EditorProjectInitializer(QuadLF::LogicalFileSystem * logicalFileSystem, QuadPF::PhysicalFileSystem * physicalFileSystem,
			CoreAsset::AssetLoader * assetLoader);
		~EditorProjectInitializer();


		bool Initialize();


	private:
		//논리적파일시스템을 프로젝트의 디렉터리구조로 초기화한다.
		void InitLogicalDirectoryStructure();


		//프로젝트의 asset들을 로드한다.
		//디렉터리 계층구조내에 존재하는 에셋들을 모두 로드한다.
		void LoadProjectAsset();
		
		//로드한 asset들의 인스턴스와 해당 logicalFile들을 생성한다
		void CreateAssetAndLogicalFile();
		void CreateTextureAndLogicalFile(SerializedAssetTContainer* serializedAssetContainer);

	private:
		static EditorProjectInitializer* mInstance;


		QuadLF::LogicalFileSystem* mLogicalFileSystem;
		QuadPF::PhysicalFileSystem* mPhysicalFileSystem;
		CoreAsset::AssetLoader* mAssetLoader;

	};

}

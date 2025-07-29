#include "LogicalFileSystem.h"

#include"LogicalFolder.h"
#include"LogicalFile.h"
#include<Logger.h>


#include<PhysicalFileSystem.h>
#include<AssetCommon.h>

#include<Utility.h>
#include<stack>
#include<queue>
#include<assert.h>

#include<BinaryWriter.h>
#include<BinaryReader.h>






namespace QuadLF
{	
	
	//논리적 디렉터리 구조 파일의헤더필드
	//signature
	//version

	static std::string LDSHeaderFileSignature = "Quad_LDS";
	static std::string LDSHeaderFileVersion = "1.0.0";
}





QuadLF::LogicalFileSystem* QuadLF::LogicalFileSystem::mInstance = nullptr;

QuadLF::LogicalFileSystem* QuadLF::LogicalFileSystem::GetInstance()
{
	if (mInstance == nullptr)
	{
		//log
		assert(0);
	}

	return mInstance;




}

QuadLF::LogicalFileSystem::LogicalFileSystem(QuadPF::PhysicalFileSystem* physicalFileSystem)
{
	if (mInstance != nullptr || physicalFileSystem ==nullptr)
	{
		//log
		assert(0);
	}


	mInstance = this;
	mRootFolder = new LogicalFolder(0);
	mNextNodeID = 1;
	mPhysicalFileSystem = physicalFileSystem;
	mRootFolder->SetName("Asset");
	mRootFolder->SetParent(nullptr);



	LOG_MESSAGE_INFO("LogicalFileSystem", "논리적파일시스템 인스턴스 생성");

}

QuadLF::LogicalFile* QuadLF::LogicalFileSystem::MakeFile(const LogicalFileAssetInfo & logicalAssetFileInfo, const std::string& fileName, const std::string& path, bool physicalFileFlag)
{

	//path를 통해서 parentLogicalFolder를 찾는다

	LogicalFolder* parentFolder = GetFolder(path);

	return MakeFile(logicalAssetFileInfo, fileName, parentFolder, physicalFileFlag);

}

QuadLF::LogicalFile* QuadLF::LogicalFileSystem::MakeFile(const LogicalFileAssetInfo & logicalAssetFileInfo, const std::string& fileName, LogicalFolder* parentFolder, bool physicalFileFlag)
{
	//매개변수 검사	
	if (fileName=="" || parentFolder == nullptr )
		return nullptr;


	//파일이름 중복확인
	if (parentFolder->HasChildNamed(fileName))
		return nullptr;

	

	bool IsPhysicalBinding = false;
	if (physicalFileFlag)
	{
		//물리적파일도 생성
		const std::string physicalParentPath = GetPhysicalFullPath(parentFolder);
		if (!mPhysicalFileSystem->CreatePhysicalFile(fileName +"."+CoreAsset::GetAssetFileExtension(), physicalParentPath))
		{
			//log

			LOG_MESSAGE_ERROR("LogicalFileSystem", (fileName + "." + CoreAsset::GetAssetFileExtension() + "물리적 파일 생성실패").c_str());
			return nullptr;
		}
		IsPhysicalBinding = true;
	}


	LogicalFile * file = CreateFileInstance(GetNextNodeID());
	InitSetting(file, parentFolder, fileName);
	file->SetAssetInfo(logicalAssetFileInfo);
	file->SetPhysicalBindingFlag(IsPhysicalBinding);



	return file;
}

QuadLF::LogicalFolder * QuadLF::LogicalFileSystem::CreateFolder(const std::string& folderName, LogicalFolder* parentFolder
, bool physicalFolderFlag)
{

	if (parentFolder == nullptr)
		return nullptr;
	

	//folderName이름 중복확인
	//parentFolder의 자식폴더들중에서 이름이겹치는지를확인한다.

	if (parentFolder->HasChildNamed(folderName))
	{
		//log
		LOG_MESSAGE_WARNING("LogicalFileSystem", (folderName + " 폴더가 이미 존재합니다.").c_str());

		return nullptr;
	}
	
	bool isPhysicalBinding = false;
	if (physicalFolderFlag)
	{
		//실제 물리적 파일생성
		if (CreatePhysicalFolder(parentFolder, folderName) == false)
		{

			LOG_MESSAGE_WARNING("LogicalFileSystem", (folderName + "  물리적폴더 생성실패.").c_str());
			return nullptr;
		}
		isPhysicalBinding = true;
	}


	LogicalFolder * logicalFolder =	CreateLogicalFolder(parentFolder,folderName,isPhysicalBinding);


	return logicalFolder;
}

bool QuadLF::LogicalFileSystem::RemoveFolder(LogicalFolder* folder)
{
	if (folder == nullptr || folder == mRootFolder)
		return false;
	
	//폴더 제거시

	//자식 ,자손들의 폴더 , 파일들을 모두제거한다.


	//하기전에 물어야한다. 근데 묻는것은 여기서하는게아니라 외부에서 알아서할것

	//이모듈에서는 순수하게 자신의 동작만수행한다.

	

	//물리적폴더과 연결된 폴더이라면
	//물리적 폴더도 제거한다.
	if (folder->GetPhyscicalBindingFlag())
	{
		mPhysicalFileSystem->DeletePhysicalFolder(GetPhysicalFullPath(folder));
	}



	LogicalFolder * parentFolder= static_cast<LogicalFolder*>(folder->GetParent());
	parentFolder->removeChildNode(folder);


	std::stack<LogicalNode*> nodeStack;
	nodeStack.push(folder);

	while (!nodeStack.empty())
	{

		LogicalNode * node = nodeStack.top();

		switch (node->GetNodeType())
		{
			case ELogicalNodeType::eFolder:
			{
				LogicalFolder* folder = static_cast<LogicalFolder*>(node);
				const std::vector<LogicalNode*> & childNodeVector = folder->GetChildNodeVector();

				for (auto node : childNodeVector)
				{
					nodeStack.push(node);
				}
				ReleaseFolderInstance(folder);
			}
			break;

			case ELogicalNodeType::eFile:
			{
				LogicalFile* file = static_cast<LogicalFile*>(node);

				//RemoveFile보다 더 가볍게수행된다(부모파일의 자식리스트에서 제거될필요가없기에)
				CleanFileDataUp(file);
				ReleaseFileInstance(file);
			
			}
			break;
		}


	}



	return true;
}

bool QuadLF::LogicalFileSystem::RemoveFile(LogicalFile* file)
{

	LogicalFolder * folder =static_cast<LogicalFolder*>(file->GetParent());

	folder->removeChildNode(file);
	//추가적인작업이필요하다면 수행


		//물리적 파일과 연결된 파일이면
	//물리적 파일도 제거한다.
	if (folder->GetPhyscicalBindingFlag())
	{
		mPhysicalFileSystem->DeletePhysicalFile(GetPhysicalFullPath(file));
	}







	CleanFileDataUp(file);
	ReleaseFileInstance(file);








	return true;
}

QuadLF::LogicalFolder* QuadLF::LogicalFileSystem::GetFolder(const std::string& path) const
{

	//   slash단위로 토큰을 분할해야할것

	//path는 Root/Folder1/Folder2/Folder3 처럼 되어있다라고 가정한다.
	std::vector<std::string> folderListVector= CoreUtility::Utility::Split(path, '/');

	if (folderListVector.size() == 0)
		return nullptr;


	if(folderListVector[0] !=mRootFolder->GetName())
		return nullptr;



	if (folderListVector.size() == 1)
		return mRootFolder;



	LogicalFolder* parentFolder = mRootFolder;
	


	for (int i = 1; i < folderListVector.size(); ++i)
	{
		LogicalNode * childNode =  parentFolder->GetChild(folderListVector[i]);
	
		if (childNode == nullptr || childNode->GetNodeType() == ELogicalNodeType::eFile)
		{
			return nullptr;
		}

		parentFolder = static_cast<LogicalFolder*>(childNode);
	}
	
	return parentFolder;
}


QuadLF::LogicalFolder* QuadLF::LogicalFileSystem::GetRootFolder() const
{
	return mRootFolder;
}

std::vector<QuadLF::LogicalFile*> QuadLF::LogicalFileSystem::GetFile(const std::string& fileName,
	uint32_t assetTypeFilter,LogicalFolder* parentFolder)
{

	if (parentFolder == nullptr)
		parentFolder = mRootFolder;



	std::vector<LogicalFile*> fileVector;

	std::stack<LogicalFolder*> folderStack;
	folderStack.push(parentFolder);

	while (!folderStack.empty())
	{
		LogicalFolder* folder = folderStack.top();
		folderStack.pop();
		
		for (auto childNode : folder->GetChildNodeVector())
		{
			switch (childNode->GetNodeType())
			{
			case ELogicalNodeType::eFolder:
				folderStack.push(static_cast<LogicalFolder*>(childNode));

				break;

			case ELogicalNodeType::eFile:
			{

				LogicalFile* file = static_cast<LogicalFile*>(childNode);

				if ((uint32_t)assetTypeFilter && (uint32_t)file->GetAssetInfo().mAssetType)
				{
					if (childNode->GetName() == fileName)
						fileVector.push_back(static_cast<LogicalFile*>(childNode));
				}
			}

				break;
			}
		}
	}



	return fileVector;
}


QuadLF::LogicalFileSystem::~LogicalFileSystem()
{
	delete mRootFolder;
}

void QuadLF::LogicalFileSystem::Initialize(const std::string& physicalRootPath)
{
	mPhysicalRootPath = physicalRootPath;

}



QuadLF::LogicalNodeID QuadLF::LogicalFileSystem::GetNextNodeID()
{
	
	if (!mFreeNodeIDVector.empty())
	{
		LogicalNodeID id = mFreeNodeIDVector.back();
		mFreeNodeIDVector.pop_back();
		
		return id;
	}

	return mNextNodeID++;
}

void QuadLF::LogicalFileSystem::ReleaseNodeID(QuadLF::LogicalNodeID id)
{
	mFreeNodeIDVector.push_back(id);





}

bool QuadLF::LogicalFileSystem::CreatePhysicalFolder(LogicalFolder* parentFolde, const std::string& folderName)
{

	const std::string physicalParentPath = GetPhysicalFullPath(parentFolde);
	if (!mPhysicalFileSystem->CreatePhysicalFolder(folderName, physicalParentPath))
	{
		//log
		LOG_MESSAGE("Warning", "LogicalFileSystem", (folderName + " 물리적폴더생성실패").c_str());
		return false;
	}

	return true;
}

QuadLF::LogicalFolder* QuadLF::LogicalFileSystem::CreateLogicalFolder(LogicalFolder* parentFolder, const std::string& folderName, bool physicalFolderBindingFlag)
{
	LogicalFolder* newFolder = CreateFolderInstance(GetNextNodeID());

	if (newFolder)
	{
		InitSetting(newFolder, parentFolder, folderName);
		newFolder->SetPhysicalBindingFlag(physicalFolderBindingFlag);
	}

	return newFolder;
}

QuadLF::LogicalFolder* QuadLF::LogicalFileSystem::CreateFolderInstance(LogicalNodeID id)
{
	//간단히 현재는 new로 생성한다.

	return new LogicalFolder(id);
}

QuadLF::LogicalFile* QuadLF::LogicalFileSystem::CreateFileInstance(LogicalNodeID id)
{


	return new LogicalFile(id);
}

void QuadLF::LogicalFileSystem::CleanFileDataUp(LogicalFile* file)
{


	//아직 하는것은없다.

}

bool QuadLF::LogicalFileSystem::ReleaseFolderInstance(LogicalFolder* folder)
{

	if (folder == nullptr)
		return false;

	ReleaseNodeID(folder->GetNodeID());
	
	delete folder;

	return true;
}

bool QuadLF::LogicalFileSystem::ReleaseFileInstance(LogicalFile* file)
{
	if (file == nullptr)
		return false;

	ReleaseNodeID(file->GetNodeID());


	delete file;

	return true;

}

void QuadLF::LogicalFileSystem::InitSetting(LogicalNode* node, LogicalFolder* parentFolder, const std::string& name)
{
	node->SetName(name);
	node->SetParent(parentFolder);
	parentFolder->AddChildNode(node);


}

std::vector<QuadLF::LDSFolder> QuadLF::LogicalFileSystem::GetLDSFolderAll() const
{
	std::vector<LDSFolder> ldsVector;


	LogicalFolder* folder = mRootFolder;
	std::queue<LogicalFolder*> folderQueue;
	folderQueue.push(folder);
	while (!folderQueue.empty())
	{
		folder = folderQueue.front();
		folderQueue.pop();


		for (auto childNode : folder->GetChildNodeVector())
		{
			if (childNode->GetNodeType() == ELogicalNodeType::eFolder)
			{
				folderQueue.push(static_cast<LogicalFolder*>(childNode));
				ldsVector.push_back({ childNode->GetFullPath(),childNode->GetPhyscicalBindingFlag() });
			}
		}
	}


	return ldsVector;
}

std::string QuadLF::LogicalFileSystem::GetPhysicalFullPath(LogicalNode* node) const
{

	if (node == nullptr)
		return "";


	std::string path = mPhysicalRootPath + '/' + node->GetFullPath();
	if (node->GetNodeType() == ELogicalNodeType::eFile)
	{
		path += ".";
		path+=CoreAsset::GetAssetFileExtension();
	}

	return path;
	
}

bool QuadLF::LogicalFileSystem::SaveLogicalDirectoryStructureAsBinaryWriter(QuadRW::BinaryWriter& binaryWriter,
	const std::string& path, const std::string& fileName)
{
	binaryWriter.StartWrite();
	binaryWriter.Write(LDSHeaderFileSignature);
	binaryWriter.Write(LDSHeaderFileVersion);

	//write LDS Header AsBinaryWriter()


	std::vector<LDSFolder> ldsFolderVector = GetLDSFolderAll();
	

	binaryWriter.Write(ldsFolderVector.size());

	for (auto& ldsFolderElement : ldsFolderVector)
	{
		binaryWriter.Write(ldsFolderElement.path);
		binaryWriter.Write(ldsFolderElement.pyhsicalBindingFlag);
	}

	const std::string filePath = path + "/"+fileName + ".lds";
	return binaryWriter.Close(filePath);

}

bool QuadLF::LogicalFileSystem::LoadLogicalDirectoryStructureAsBinaryReader(QuadRW::BinaryReader& binaryReader,
	const std::string& filePath)
{





	bool ret =binaryReader.StartRead(filePath);
	if (ret == false)
		return false;


	std::string ldsHeaderFileSignature;
	std::string ldsHeaderFileVersion;

	ret =binaryReader.Read(ldsHeaderFileSignature);
	if (ret == false || ldsHeaderFileSignature != LDSHeaderFileSignature)
	{
		//log
		return false;
	}

	ret = binaryReader.Read(ldsHeaderFileVersion);
	if (ret == false || ldsHeaderFileVersion != LDSHeaderFileVersion)
	{
		//log
		return false;
	}
	
	size_t ldsFolderNum;
	ret = binaryReader.Read(ldsFolderNum);

	if (ret == false)
	{
		//log
		return false;
	}

	std::vector<LDSFolder> ldsFolderVector(ldsFolderNum);

	for (size_t i = 0; i < ldsFolderNum; ++i)
	{


		if (binaryReader.Read(ldsFolderVector[i].path) == false)
		{
			//log
			return false;
		}
		
		if (binaryReader.Read(ldsFolderVector[i].pyhsicalBindingFlag) == false)
		{
			//log
			return false;
		}
	}

	//이 ret을 계속확인해야하나? 아니면 좀더 편리하게 자동화할수없나?
	


	for (auto& ldsFolderElement : ldsFolderVector)
	{


		LogicalFolder * parentFolder =	GetFolder(CoreUtility::Utility::GetParentFolderPathFromPath(ldsFolderElement.path));
		

		if (parentFolder == nullptr)
		{
			//log
			continue;
		}

		LogicalFolder * childFolder = CreateLogicalFolder(parentFolder, CoreUtility::Utility::GetFileNameFromPath(ldsFolderElement.path), true);
		if (childFolder == nullptr)
		{
			//log
		}
	
	}

	return true;


}

void QuadLF::LogicalFileSystem::SetPhysicalBindingFlag(LogicalNode* node, bool flag)
{
	if (node == nullptr)
		return;

	node->SetPhysicalBindingFlag(flag);

}
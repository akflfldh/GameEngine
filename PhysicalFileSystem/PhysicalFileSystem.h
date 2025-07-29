#pragma once

#include"PhysicalFileSystemDLLMacro.h"
#include<string>
#include<vector>


namespace QuadPF
{

	class PHYSICAL_FILESYSTEM_API PhysicalFileSystem
	{
	public:
		static PhysicalFileSystem* GetInstance();
		 virtual ~PhysicalFileSystem() = 0;

		 //향후에 더많은 매개변수를 통해 다양한 제어를 제공할수있다.
		 virtual bool CreatePhysicalFile(const std::string& fileName, const std::string & path)= 0;
		 virtual bool CreatePhysicalFolder(const std::string& folderName, const std::string& path)= 0;

		 //이 두 메서드는 하나로 합칠수도있을것이다.
		 virtual bool DeletePhysicalFile(const std::string& path) = 0;

		 //하위 folder,file들을 모두제거
		 virtual bool DeletePhysicalFolder(const std::string& path) = 0;


		 virtual bool ReadFileToBuffer(const std::string& path, std::vector<uint8_t>& oBuffer) = 0;
		 //기존에있는 존재하는 파일에 쓰기
		 virtual bool WriteBufferToValidFile(const std::string& path, void* data, size_t size) = 0;
		 //파일이 없다면 생성하여 쓰기수행.
		 virtual bool WriteBufferToFile(const std::string& path, void* data, size_t size) = 0;


		

		 virtual bool IsValidFolderPath(const std::string& path) const = 0;
		 virtual bool IsValidFilePath(const std::string& path) const = 0;
		 
		 //유효한 파일,폴더 인지 확인할수있다.
		 virtual bool IsValidPath(const std::string& path, bool& isFolder) const = 0;

		// static void SetPhysicalFileSystemImpl(PhysicalFileSystem* pImpl);

		//특정폴더내에 특정확장자를 가지는 파일들의 이름을 담은 벡터를 리턴한다
		 //oFileList에 찾은 파일이름들이 들어간다 ex) aaa.asset
		 virtual bool  GetFileListByExtension(const std::string& folderPath, const std::string& extension , std::vector<std::string> & oFileList) =0;


	protected:
		PhysicalFileSystem();


	private:
		void LogError() const;
		



	private:
	

		static PhysicalFileSystem* mImplSystem;


	};

}

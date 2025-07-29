#pragma once

#include<string>
namespace QuadPSA
{

	class ProjectConfig
	{
	public:
		ProjectConfig();
		~ProjectConfig();


		void Initialize(const std::string& path, const std::string& version, const std::string& name);



		void SetPath(const std::string& path);
		const std::string &GetPath() const;

		void SetVersion(const std::string& version);
		const std::string& GetVersion() const;


		void SetName(const std::string& name);
		const std::string& GetName() const;


	private:
		std::string mPath;
		std::string mVersion;
		std::string mName;


	};

}

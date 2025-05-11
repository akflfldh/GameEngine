#include "UserBuildFileGenerator.h"

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<Windows.h>
#include<objbase.h>
#include<iomanip>


#include"rapidjson/document.h"
#include"rapidjson/filereadstream.h"



void replaceAll(std::string& str, const std::string& from, const std::string& to)
{

	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos)
	{
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

std::string loadTemplate(const std::string& path)
{
	std::ifstream fin(path);
	if (!fin.is_open())
	{
		throw std::runtime_error("템플릿 파일을 열 수 없습니다:" + path);
	}


	std::stringstream buffer;
	buffer << fin.rdbuf();
	return buffer.str();
}


std::string generateSources(const std::vector<std::string >& sources)
{
	std::string result;

	for (const auto& src : sources)
	{
		result += "    <ClCompile Include=\"" + src + "\"/>\n";
	}
	return result;
}


std::string joinWithSemicolon(const std::vector<std::string>& items)
{
	std::string result;
	for (size_t i = 0; i < items.size(); ++i)
	{
		result += items[i];
		if (i != items.size() - 1) result += ";";
	}

	return result;
}



std::string generateGUID()
{


	GUID guid;
	CoCreateGuid(&guid);

	std::stringstream ss;
	ss << std::uppercase << std::hex << std::setfill('0')
		<< '{'
		<< std::setw(8) << guid.Data1 << '-'
		<< std::setw(4) << guid.Data2 << '-'
		<< std::setw(4) << guid.Data3 << '-'
		<< std::setw(2) << static_cast<int>(guid.Data4[0])
		<< std::setw(2) << static_cast<int>(guid.Data4[1]) << '-'
		<< std::setw(2) << static_cast<int>(guid.Data4[2])
		<< std::setw(2) << static_cast<int>(guid.Data4[3])
		<< std::setw(2) << static_cast<int>(guid.Data4[4])
		<< std::setw(2) << static_cast<int>(guid.Data4[5])
		<< std::setw(2) << static_cast<int>(guid.Data4[6])
		<< std::setw(2) << static_cast<int>(guid.Data4[7])
		<< '}';

	return ss.str();
}

std::string addQuotes(const std::string& str)
{

	return "\"" + str + "\"";



}

std::string GetProjectConfigType(const std::string& type)
{
	if (type == "dll")
	{
		return "DynamicLibrary";
	}
	else if (type == "exe")
	{
		return "Application";
	}
	else if (type == "lib")
	{
		return "StaticLibrary";
	}
	else
	{
		throw std::runtime_error("잘못된 project type");
	}
	

}

std::string GenerateFilterDefinitionXmi(const std::vector<std::string>& filters)
{
	std::string ret;



	for (auto filterName : filters)
	{
		ret +="    <Filter Include=\"" + filterName + "\">\n";
		ret +="      <UniqueIdentifier>" + generateGUID() + "</UniqueIdentifier>\n";
		ret +="    </Filter>\n";
	}

	return ret;

}


bool EndsWithSuffix(const std::string& str, const std::string suffix)
{

	return ((str.size() >= suffix.size()) && 
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);

}


void SplitSourceFiles(const std::vector<std::string>& sourceFiles, std::vector <std::string>& cppFiles, std::vector<std::string>& headerFiles)
{

	for (auto& file : sourceFiles)
	{

		if (EndsWithSuffix(file, ".cpp") || EndsWithSuffix(file, ".cxx") || EndsWithSuffix(file, ".cc"))
		{
			cppFiles.push_back(file);
		}
		else if (EndsWithSuffix(file, ".h") || EndsWithSuffix(file, ".hpp") || EndsWithSuffix(file, ".inl"))
		{
			headerFiles.push_back(file);
		}

	}

}

std::string GenerateFilterFileEntries(const std::vector<std::string>& cppFiles, const std::vector<std::string>& headerFiles)
{

	std::stringstream ss;
	for (auto& cpp : cppFiles)
	{
		ss << "    <ClCompile Include=\"" << cpp << "\">\n";
		ss << "      <Filter>소스 파일</Filter>\n";
		ss << "    </ClCompile>\n";
	}

	for (auto& header : headerFiles)
	{
		ss << "    <ClInclude Include=\"" << header << "\">\n";
		ss << "      <Filter>헤더 파일<<\Filter>\n";
		ss << "    </CIInclude>\n";
	}
	
	return ss.str();

}


bool Quad::UserBuildFileGenerator::CreateBuildFile(const std::string& templateDirectoryPath, const std::string userProjectDirectory)
{


	const std::string configFile = templateDirectoryPath + "\\project_config.json";


	FILE* fp = nullptr;

	fopen_s(&fp, configFile.c_str(), "r");
	if (!fp)
	{
		std::cerr << "config 파일을 열 수 없습니다.\n";

		return 1;
	}


	char buffer[65536];
	rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
	rapidjson::Document config;
	config.ParseStream(is);
	fclose(fp);
	if (!config.IsObject())
	{
		std::cerr << "config 파일이 올바른 json 형식이 아닙니다.\n";
		return 1;
	}

	std::string name = config["name"].GetString();

	std::vector<std::string> sources;
	for (auto& s : config["sources"].GetArray())
		sources.push_back(s.GetString());

	std::vector<std::string>cppFiles;
	std::vector<std::string>headerFiles;
	SplitSourceFiles(sources, cppFiles, headerFiles);


	std::vector<std::string> includeDirs;
	for (auto& i : config["includeDirs"].GetArray())
		includeDirs.push_back(i.GetString());

	std::vector<std::string> libs;
	for (auto& l : config["libs"].GetArray())
		libs.push_back(l.GetString());

	std::string projectType = config["type"].GetString();
	
	std::vector<std::string> filters;
	for (auto& i : config["filters"].GetArray())
		filters.push_back(i.GetString());

	std::vector<std::string> libDirs;
	for (auto& i : config["libDirs"].GetArray())
		libDirs.push_back(i.GetString());

	std::string cppStandard = config["cpp_standard"].GetString();



	//프로젝트파일생성
	const std::string projectTemplateFile = templateDirectoryPath + "\\ProjectTemplate.vcxproj";

	std::string vcxproj = loadTemplate(projectTemplateFile);

	//값 치환
	replaceAll(vcxproj, "{{SOURCES}}", generateSources(sources));
	replaceAll(vcxproj, "{{INCLUDE_DIRS}}", joinWithSemicolon(includeDirs));
	replaceAll(vcxproj, "{{LIBRARY_DIRS}}", joinWithSemicolon(libDirs));
	replaceAll(vcxproj, "{{LIBRARIES}}", joinWithSemicolon(libs));
	std::string projectGuid = generateGUID();
	replaceAll(vcxproj, "{{PROJECT_GUID}}", projectGuid);
	replaceAll(vcxproj, "{{PROJECT_TYPE}}", GetProjectConfigType(projectType));
	replaceAll(vcxproj, "{{CPP_STANDARD}}", cppStandard);
	


	bool ret = true;
	if (!CreateProjectFile(userProjectDirectory, name, vcxproj))
	{
		return false;
	}
	
	const std::string filterFilePath = templateDirectoryPath + "\\FilterTemplate.filters";
	std::string filter = loadTemplate(filterFilePath);
	//filter파일 생성
	replaceAll(filter, "{{FILTER_DEFINITIONS}}", GenerateFilterDefinitionXmi(filters));
	replaceAll(filter, "{{FILTERED_FILE_ENTRIES}}", GenerateFilterFileEntries(cppFiles, headerFiles));

	CreateFilterFile(userProjectDirectory, name,filter);







	//솔루션 파일 생성
	const std::string slnTemplateFile = templateDirectoryPath + "\\SolutionTemplate.sln";
	std::string slnTemplate = loadTemplate(slnTemplateFile);
	replaceAll(slnTemplate, "{{PROJECT_NAME}}", addQuotes(name));
	replaceAll(slnTemplate, "{{PROJECT_PATH}}", addQuotes(name + "\\" + name + ".vcxproj"));
	replaceAll(slnTemplate, "{{PROJECT_GUID}}", addQuotes(projectGuid));


	if (!CreateSolutionFile(userProjectDirectory, name,slnTemplate))
	{
		return false;
	}



    return true;
}

bool Quad::UserBuildFileGenerator::CreateClassFile(const std::string& className, const std::string& templateDirectoryPath, const std::string userProjectDirectory)
{
	const std::string cppFilePath = templateDirectoryPath + "\\ClassTemplate.cpp";
	std::string file = loadTemplate(cppFilePath);

	replaceAll(file, "{{CLASSNAME}}", className);


	const std::string newCppFilePath = userProjectDirectory + "\\UserCode\\" + className + ".cpp";
	std::ofstream fout(newCppFilePath);

	if (!fout.is_open())
	{
		//클래스 파일생성실패
		std::cerr << "클래스 파일 생성실패" << std::endl;
		return false;
	}

	fout << file;
	fout.close();



	
	const std::string headerFilePath = templateDirectoryPath + "\\ClassTemplate.h";
	file = loadTemplate(headerFilePath);

	replaceAll(file, "{{CLASSNAME}}", className);

	const std::string newHeaderFilePath = userProjectDirectory + "\\UserCode\\" + className + ".h";

	fout.open(newHeaderFilePath);
	if (!fout.is_open())
	{
		std::cerr << "클래스 헤더파일 생성실패" << std::endl;
		return false;
	}

	fout << file;
	fout.close();


	//cpp, h 가 userCode dll 프로젝트 에 속해야한다.

	const std::string vcxprojFilePath = userProjectDirectory + "\\UserCode\\UserCode.vcxproj";
	const std::string filtersFilePath = userProjectDirectory + "\\UserCode\\UserCode.vcxproj.filters";
	//템플릿은 아니지만 그냥 로드
	std::string vcxproj = loadTemplate(vcxprojFilePath);
	std::string filters = loadTemplate(filtersFilePath);
	
	replaceAll(vcxproj, "<!--{{SOURCE}}-->", "<ClCompile Include=\"" + className + ".cpp\" />\n    <!--{{SOURCE}}-->");
	replaceAll(vcxproj, "<!--{{HEADER}}-->", "<ClInclude Include=\"" + className + ".h\" />\n    <!--{{HEADER}}-->");

	replaceAll(filters, "<!--{{SOURCE}}-->", "<ClCompile Include=\"" + className + ".cpp\" />\n    <!--{{SOURCE}}-->");
	replaceAll(filters, "<!--{{HEADER}}-->", "<ClInclude Include=\"" + className + ".h\" />\n    <!--{{HEADER}}-->");

	fout.open(vcxprojFilePath);
	if (!fout.is_open())
	{
		std::cerr << "클래스파일생성실패\n";
		return false;
	}

	fout << vcxproj;
	fout.close();

	fout.open(filtersFilePath);

	if (!fout.is_open())
	{
		std::cerr << "클래스파일생성실패\n";
		return false;
	}

	fout << filters;
	fout.close();



	return true;
}

bool Quad::UserBuildFileGenerator::CreateProjectFile(const std::string& UserProjectDirectory, const std::string & name , const std::string  & projectData)
{



	//파일저장

//프로젝트파일을 저장할 하위폴더 생성
	const std::string projectDirectory = UserProjectDirectory + "\\" + name;
	CreateDirectoryA(projectDirectory.c_str(), nullptr);



	std::string outPath = projectDirectory + "\\" + name + ".vcxproj";
	std::ofstream fout(outPath);
	if (!fout.is_open())
	{
		std::cerr << "vcxproj저장 실패 \n";
		return false;
	}
	fout << projectData;
	fout.close();

	std::cout << "[완료] " << outPath << " 파일이 생성되었습니다/\n";

	return true;
}

bool Quad::UserBuildFileGenerator::CreateSolutionFile(const std::string& UserProjectDirectory, const std::string& name ,const std::string& solutionData)
{



	const std::string slnPath = UserProjectDirectory + "\\" + name + ".sln";
	
	std::ofstream fout(slnPath);
	if (!fout.is_open())
	{
		std::cerr << "솔루션 파일저장실패\n";
		return false;
	}

	fout << solutionData;
	fout.close();

	return true;
}

bool Quad::UserBuildFileGenerator::CreateFilterFile(const std::string& UserProjectDirectory, const std::string & name ,const std::string& filterData)
{
	const std::string filterPath = UserProjectDirectory + "\\" + name + "\\" + name + ".vcxproj.filters";
	std::ofstream fout(filterPath);
	if (!fout.is_open())
	{
		std::cerr << "filter 파일 저장실패\n";
		return false;
	}
	fout << filterData;

	return true;
}

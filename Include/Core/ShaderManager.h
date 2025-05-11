#pragma once




#include"Predeclare.h"
#include"header.h"
#include"Utility/Singleton.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB ShaderManager:public Singleton<ShaderManager>
	{
	public:
		ShaderManager() = default;
		~ShaderManager() = default;
		
		static Shader* GetShader(const std::string& file, const std::string& entryPoint, const std::string& version,
			const std::string& shaderMeshType);
		static bool AddShader(Shader* shader);

	private:
		std::unordered_map<std::string, Shader*> mShaderTableUnMap;



	};
}


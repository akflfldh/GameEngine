#include "ShaderManager.h"
#include"Shader.h"

Quad::Shader* Quad::ShaderManager::GetShader(const std::string& file, const std::string& entryPoint, const std::string& version,const std::string & shaderMeshType) 
{
	ShaderManager* managerInstance = GetInstance();
	
	const std::string key = file + entryPoint + version + shaderMeshType;
	std::unordered_map<std::string, Shader*>::const_iterator it = managerInstance->mShaderTableUnMap.find(key);
	return it != managerInstance->mShaderTableUnMap.cend() ? it->second : nullptr;
}

bool Quad::ShaderManager::AddShader(Shader* shader)
{
	ShaderManager* managerInstance = GetInstance();


	const std::string  key = shader->GetFileName() + shader->GetEntryPoint() + shader->GetVersion();
	std::pair<std::unordered_map<std::string, Shader*>::iterator,bool>	ret= managerInstance->mShaderTableUnMap.try_emplace(key, shader);
	return ret.second;

}

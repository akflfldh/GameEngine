#pragma once

#include"Predeclare.h"
#include"Texture.h"


namespace Quad
{
	class EnginTexture:public Texture
	{
	public:
		EnginTexture(const std::string& name = "");
		~EnginTexture() = default;

		void SetFilePath(const std::string& filePath);
		const std::string& GetFilePath()const;


		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;
	private:

		std::string mFilePath;


	};

}

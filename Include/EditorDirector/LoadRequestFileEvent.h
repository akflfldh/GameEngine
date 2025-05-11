#pragma once


#include"Predeclare.h"
#include"Core/Event.h"
#include<vector>
#include<string>

namespace Quad
{
	class LoadRequestFileEvent:public Event
	{
	public:
		LoadRequestFileEvent();
		virtual ~LoadRequestFileEvent();


		void SetFileVector(const std::vector<std::string>& fileVector);
		void SetFileVector(std::vector<std::string>&& fileVector);

		const std::vector<std::string>& GetFileVector()const;

	private:
		std::vector<std::string> mFileVector;

	};
}


#pragma once

#include"header.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB State
	{
	public:
		State(const std::string& name);
		virtual ~State() = 0;


		virtual void EnterState() = 0;
		virtual void RunState() = 0;

		//ExitState는 현재상태에서 빠져나가는고 그말은 상태가 바뀐다는거니깐
		//이메서드에서 새로운상태 인스턴스를 리턴하는거지
		virtual void ExitState() = 0;

		const std::string& GetName() const;

	private:
		std::string mName;

	};

}
#pragma once

#include"Predeclare.h"


#include<stack>


#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif

namespace Quad
{

	class UTILITY_LIB_API IDGenerator
	{
	public:

		IDGenerator();
		~IDGenerator();

		unsigned long long GetNewID();

		//void ReturnID(unsigned long long  id);
		unsigned long long PeekNewID() const;


		void SetNextAvailableID(unsigned long long id);

	private:
		unsigned long long mNextID;
		//std::stack<unsigned long long> mReturnedIDStack;




	};

}
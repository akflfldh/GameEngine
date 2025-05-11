#include "Utility/IDGenerator.h"


namespace Quad
{
	IDGenerator::IDGenerator()
		:mNextID(1)
	{
	}

	IDGenerator::~IDGenerator()
	{
	}

	unsigned long long IDGenerator::GetNewID()
	{
		/*if (!mReturnedIDStack.empty())
		{

			unsigned long long ret = mReturnedIDStack.top();
			mReturnedIDStack.pop();
			return ret;
		}*/

		return mNextID++;


	}

	unsigned long long IDGenerator::PeekNewID() const
	{
		return mNextID;
	}

	void IDGenerator::SetNextAvailableID(unsigned long long id)
	{
		mNextID = id;
	}

	//void IDGenerator::ReturnID(unsigned long long id)
	//{
	//	if (id >= mNextID || id == 0)	//잘못된인수
	//		return;

	//	mReturnedIDStack.push(id);
	//}

}
#include "State.h"
namespace Quad
{
	State::~State()
	{

	}

	State::State(const std::string& name) :mName(name)
	{
	}

	const std::string& State::GetName() const
	{

		return mName;
		// TODO: ���⿡ return ���� �����մϴ�.
	}
}
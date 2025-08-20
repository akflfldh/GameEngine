#include "PhysicalFileSystem/PhysicalFileSystem.h"

#include <assert.h>

#ifdef _WINDOWS
#include "PhysicalFileSystem/WindowPhysicalFileSystem.h"

#endif

QuadPF::PhysicalFileSystem *QuadPF::PhysicalFileSystem::mImplSystem = nullptr;

QuadPF::PhysicalFileSystem *QuadPF::PhysicalFileSystem::GetInstance()
{

#ifdef _WINDOWS

    static WindowsPhysicalFileSystem instance;

#endif

    return &instance;

    // if (mImplSystem == nullptr)
    //{
    //	//log
    //	assert(0);
    // }

    // return mImplSystem;
}

QuadPF::PhysicalFileSystem::PhysicalFileSystem() {}

QuadPF::PhysicalFileSystem::~PhysicalFileSystem() {}

// void QuadPF::PhysicalFileSystem::SetPhysicalFileSystemImpl(PhysicalFileSystem* pImpl)
//{
//	if (mImplSystem != nullptr)
//	{
//		//log
//		assert(0);
//	}
//
//	mImplSystem = pImpl;
//
// }
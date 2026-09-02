#include "PhysicalFileSystem/PhysicalFileSystem.h"

#include <assert.h>

#include "PhysicalFileSystem/StandardPhysicalFileSystem.h"

QuadPF::PhysicalFileSystem *QuadPF::PhysicalFileSystem::mImplSystem = nullptr;

QuadPF::PhysicalFileSystem *QuadPF::PhysicalFileSystem::GetInstance()
{

    static StandardPhysicalFileSystem instance;

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
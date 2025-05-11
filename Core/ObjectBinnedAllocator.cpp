#include "Core/ObjectBinnedAllocator.h"
#include<vector>

Quad::BinnedAllocator* Quad::GetObjectBinnedAllocator(unsigned int size)
{

    static BinnedAllocator allocator16(16);
    static BinnedAllocator allocator32(32);
    static BinnedAllocator allocator48(48);
    static BinnedAllocator allocator64(64);
    static BinnedAllocator allocator80(80);
    static BinnedAllocator allocator96(96);
    static BinnedAllocator allocator128(128);
    static BinnedAllocator allocator256(256);
    static BinnedAllocator allocator512(512);
    static BinnedAllocator allocator1024(1024);
    static BinnedAllocator allocator2048(2048);

    
    static std::vector<int> sizeVector = { 16,32,48,64,80,96,128,256,512,1024,2048 };
    static std::vector<BinnedAllocator*> allocatorVector = { &allocator16,&allocator32,&allocator48,&allocator64,&allocator80,&allocator96,
    &allocator128,&allocator256,&allocator512,&allocator1024,&allocator2048 };


    for (int i = 0; i < sizeVector.size(); ++i)
    {

        int allocSize = sizeVector[i];
        if (size <= allocSize)
        {
            return allocatorVector[i];
        }
    }
    throw std::bad_alloc();

}

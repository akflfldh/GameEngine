#include "DefaultAllocator.h"
#include <stdint.h>
#include <stdlib.h>
CoreAlloc::DefaultAllocator::DefaultAllocator() {}

CoreAlloc::DefaultAllocator::~DefaultAllocator() {}

void *CoreAlloc::DefaultAllocator::Allocate(size_t size, size_t alignment)
{

    // 정렬을 고려한다면
    // alignment만큼 더한 크기를 할당해서 할당받은 시작주소를 확인후 시작주소가 정렬의 배수에위치하도록 시작주소를
    // 계산후 그 시작주소를 넘긴다 그럼 할당해제할떄 이게 크기가얼만큼 할당되었는지 어떻게알음? 그리고 시작주소가 원래
    // 어디였는지?
    // 계산된 시작주소 바로앞 1바이트공간에 실제 이동한 거리값을 넣는거임 ,그럼 애초에 시작주소가 정렬의 배수였다면?
    // 그럴경우에는 무조건 정렬크기만큼 이동해버리는거임 그래서 할당하는크기는 요구하는사이즈 + 정렬크기

    // size_t bufferSize = size + alignment;
    /// uint8_t *buffer = new uint8_t[alignment];

    // ( buffer 시작주소 + alignment ) & (~alignment) 라고 계산하면 시작주소에서 가장가까운 alignment의 배수 주소가 나옴
    // 만약  계산된 시작주소가 기존 buffer의 시작주소와 동일하다면 시작주소에 alignment만큼더함

    // 그리고 계산된 시작주소에서 - 기존 시작주소를 뺴서 이동값(offset)을 게산하고 그것을 계산된 시작주소 바로 앞
    // 1바이트에 기록함
    //
    // 표준 c++11 제공
    return _aligned_malloc(size, alignment);

    //// 근데 c++ 17 부터는 정렬 기능을 제공함
    // uint8_t *buffer = new (std::align_val_t(alignment)) uint8_t[size];
    // return buffer;
}

void CoreAlloc::DefaultAllocator::Release(void *ptr)
{
    /*   delete[] static_cast<uint8_t *>(ptr);*/

    _aligned_free(ptr);
}

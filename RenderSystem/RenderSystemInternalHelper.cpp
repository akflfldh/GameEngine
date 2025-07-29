#include "RenderSystemInternalHelper.h"



uint32_t GetScissorRecttHash(const Render::ScissorRect& rect)
{

    //fnv-1a hash 
    uint32_t  hash = 2166136261u;

    hash = (hash ^ rect.Left) * 16777619u;
    hash = (hash ^ rect.Right) * 16777619u;
    hash = (hash ^ rect.Top) * 16777619u;
    hash = (hash ^ rect.Bottom) * 16777619u;

    return hash;

    //충돌이발생해도, 어차피 정렬을위한것이기 크게 상관없다.
}

uint64_t GetMeshItemHash(const Render::MeshItem& meshItem)
{

    uint64_t hash = 14695981039346656037u;

    hash = (hash ^ (uintptr_t)meshItem.mVertexBuffer) * 1099511628211u;
    hash = (hash ^ (uintptr_t)meshItem.mIndexBuffer) * 1099511628211u;
    hash = (hash ^ meshItem.mVertexNum) * 1099511628211u;
    hash = (hash ^ meshItem.mVertexOffset) * 1099511628211u;
    hash = (hash ^ meshItem.mIndexNum) * 1099511628211u;
    hash = (hash ^ meshItem.mIndexOffset) * 1099511628211u;

    return hash;

}


uint64_t Render::GetRenderItemFirstSortKey(ScissorRect scissorRect, MaterialID matID)
{
    

 
    uint64_t scissorRectKey = (uint64_t)(GetScissorRecttHash(scissorRect));             //상위 32비트

    uint64_t matSortKey = (uint64_t)matID;  //matID는 32비트라 가정하자 (하위  32비트)

    uint64_t sortKey = (scissorRectKey << 32) | (matSortKey);

    return sortKey;

}

uint64_t Render::GetRenderItemSecondSortKey(const MeshItem& meshItem, const InstanceItem& instanceItem)
{


    uint64_t meshItemKey = GetMeshItemHash(meshItem);

    //인스턴스의 셰이더리소스들에대해서는너무종류가많고 어차피 draw마다 비교판정은 수행되니 정렬에서 일단뺴고
    //후에 개선할필요가있다면 그때가서 추가하자.

    return meshItemKey;
}

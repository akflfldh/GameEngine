#include "MapMetaData.h"
#include"Parser/JsonParser.h"
//void Quad::MapMetaData::SetMapPath(const std::string& mapPath)
//{
//
//    //mMapPath = mapPath;
//}

void Quad::MapMetaData::SetMapName(const std::string& mapName)
{
    mMapName = mapName;

}

//const std::string Quad::MapMetaData::GetMapPath() const
//{
//    return mMapPath;
//}

const std::string Quad::MapMetaData::GetMapName() const
{
    return mMapName;
}

void Quad::MapMetaData::Serialize()
{
    JsonParser::Write("MapMetaData_MapName", mMapName);
}

void Quad::MapMetaData::DeSerialize()
{
    JsonParser::Read("MapMetaData_MapName", mMapName);

}

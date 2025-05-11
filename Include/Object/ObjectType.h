#pragma once



enum class ECameraType
{
	ePerspectiveCamera = 0,
	eOrthogonalCamera

};

enum class ELightType
{
	eDirect = 0,
	ePoint,
	eSpot
};


enum class EObjectType
{
	eEntity=0,
	eUiEntity,
	eLight,
	eCamera,
	eLine,
	eSpline,
	eCollider

};

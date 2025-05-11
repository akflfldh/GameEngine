#pragma once



#ifdef MESH_EXPORT_LIB 
#define MESH_API_LIB __declspec(dllexport)
#else  
#define MESH_API_LIB __declspec(dllimport)
#endif
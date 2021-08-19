#include <Windows.h>
#include <d3d9.h>

#include <thread>

#include "D3D9Hook.hpp"

extern D3D9Hook* pD3D9Hook;

#pragma comment(lib, "PolyHook_2.lib")
#pragma comment(lib, "capstone.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
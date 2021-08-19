#include "D3D9Hook.hpp"
#include "dllmain.hpp"

void D3D9Hook::postReset(IDirect3DDevice9* pDevice)
{
	//render.Initialize(pDevice);
	for (size_t i = 0; i < pD3D9Hook->event_PostReset.size(); i++)
		pD3D9Hook->event_PostReset.at(i)(pDevice);
}

void D3D9Hook::lostReset()
{
	//render.Invalidate();
	for (size_t i = 0; i < pD3D9Hook->event_LostReset.size(); i++)
		pD3D9Hook->event_LostReset.at(i)();
}

NOINLINE HRESULT __stdcall D3D9Hook::HookedPresent(IDirect3DDevice9* pDevice, const RECT* pSrcRect, const RECT* pDestRect, HWND hDestWindow, const RGNDATA* pDirtyRegion)
{
	if (pDevice) {

		if (!pD3D9Hook->init) {
			pD3D9Hook->postReset(pDevice);
			pD3D9Hook->init = true;
		}

		for (size_t i = 0; i < pD3D9Hook->event_PresentEvent.size(); i++)
			pD3D9Hook->event_PresentEvent.at(i)(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
		

	}
	return  ((HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*))pD3D9Hook->hookPressentTramp)
		(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

NOINLINE HRESULT __stdcall D3D9Hook::HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams)
{
	if (!pDevice)
		return ((HRESULT(__stdcall*)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS * pPresentParams))pD3D9Hook->hookResetTramp) (pDevice, pPresentParams);

	if (pD3D9Hook->init)
	{
		// Lost Reset
		pD3D9Hook->lostReset();

		pD3D9Hook->init = false;
	}

	HRESULT ret = 
		((HRESULT(__stdcall*)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS * pPresentParams))pD3D9Hook->hookResetTramp) (pDevice, pPresentParams);

	if (ret == D3D_OK)
	{
		// Post Reset
		pD3D9Hook->postReset(pDevice);

		pD3D9Hook->init = true;
	}

	return ret;
}

D3D9Hook::D3D9Hook()
{
	PVOID hookPrs = reinterpret_cast<void*>(GetDeviceAddress(17));
	PVOID hookRes = reinterpret_cast<void*>(GetDeviceAddress(16));

	detourPresent	= new PLH::x86Detour((char*)hookPrs, (char*)&HookedPresent, &hookPressentTramp, dis);
	detourReset		= new PLH::x86Detour((char*)hookRes, (char*)&HookedReset, &hookResetTramp, dis);

	detourPresent->hook();
	detourReset->hook();
}

D3D9Hook::~D3D9Hook()
{
	detourPresent->unHook();
	detourReset->unHook();

	delete detourPresent, detourReset;
}

DWORD D3D9Hook::FindDevice(DWORD Len)
{
	DWORD dwObjBase = NULL;
	char infoBuf[MAX_PATH]{};
	GetSystemDirectoryA(infoBuf, MAX_PATH);
	strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");
	dwObjBase = reinterpret_cast<DWORD>(LoadLibraryA(infoBuf));
	while (dwObjBase++ < dwObjBase + Len)
	{
		if ((*reinterpret_cast<WORD*>(dwObjBase + 0x00)) == 0x06C7 &&
			(*reinterpret_cast<WORD*>(dwObjBase + 0x06)) == 0x8689 &&
			(*reinterpret_cast<WORD*>(dwObjBase + 0x0C)) == 0x8689)
		{
			dwObjBase += 2;
			break;
		}
	}
	return dwObjBase;
};
DWORD D3D9Hook::GetDeviceAddress(int VTableIndex)
{
	PDWORD VTable;
	*reinterpret_cast<DWORD*>(&VTable) = *reinterpret_cast<DWORD*>(FindDevice(0x128000));
	return VTable[VTableIndex];
};
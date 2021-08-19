#ifndef _D3D9HOOK_HPP_
#define _D3D9HOOK_HPP_

#include <d3d9.h>
#include <d3dx9.h>

#include <polyhook2/Detour/x86Detour.hpp>
#include <polyhook2/CapstoneDisassembler.hpp>

#include <vector>

#include "d3drender.h"


class D3D9Hook {
public:

	uint64_t hookPressentTramp = NULL;
	uint64_t hookResetTramp = NULL;

	PLH::CapstoneDisassembler dis = { PLH::Mode::x86 };

	static NOINLINE HRESULT __stdcall
		HookedPresent(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
	static NOINLINE HRESULT __stdcall
		HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams);

	PLH::x86Detour *detourPresent;
	PLH::x86Detour *detourReset;

	/////////////

	template<typename T>
	struct on_hookevent : public std::vector<std::function<T>> {
		on_hookevent& operator+=(std::function<T> func) {
			this->push_back(func);
			return *this;
		}
	};

	on_hookevent<void(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)> event_PresentEvent;

	on_hookevent<void(LPDIRECT3DDEVICE9 pDevice)> event_PostReset;
	on_hookevent<void()> event_LostReset;

	/////////////

	bool init = false;

	D3D9Hook();
	~D3D9Hook();

	void lostReset();
	void postReset(IDirect3DDevice9* pDevice);

private:

	DWORD FindDevice(DWORD Len);
	DWORD GetDeviceAddress(int VTableIndex);

};

#endif
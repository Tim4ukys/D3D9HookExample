# D3D9HookExample
Пример хука Present и Reset в DirectX 9

d3d9hook.cpp
```cpp
...
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
...
```
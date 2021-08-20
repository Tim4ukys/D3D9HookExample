[![GitHub license](https://img.shields.io/github/license/Tim4ukys/D3D9HookExample)](https://github.com/Tim4ukys/D3D9HookExample/blob/main/LICENSE)

# D3D9HookExample
Пример хука Present и Reset в DirectX 9 через DLL.
____
dllmain.cpp
```cpp
...
class myClass {
public:

    CD3DRender render{ 128 };
    CD3DFont font{ "Segoe UI", 16, FCR_BORDER };

    void initRender()
    {
        pD3D9Hook->event_PresentEvent += [](IDirect3DDevice9* pDevice, const RECT* pSrcRect, const RECT* pDestRect, HWND hDestWindow, const RGNDATA* pDirtyRegion)
        {
            if (SUCCEEDED(CD3DBaseRender::BeginRender()))
            {
                //my_class.font.Print(350, 350, D3DCOLOR_XRGB(29, 181, 173), "Дима лох", true);

                CD3DBaseRender::EndRender();
            }
        };

        pD3D9Hook->event_PostReset += [](IDirect3DDevice9* pDevice)
        {
            my_class.render.Initialize(pDevice);
            my_class.font.Initialize(pDevice);
        };

        pD3D9Hook->event_LostReset += []()
        {
            my_class.render.Invalidate();
            my_class.font.Invalidate();
        };
    }

    myClass()
    {
        std::thread([&]{ 
            while (!pD3D9Hook) Sleep(1000); 
            initRender();
            }).detach();
    }

} my_class;
...
```
____
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

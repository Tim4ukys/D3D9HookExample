#include "dllmain.hpp"

D3D9Hook* pD3D9Hook = nullptr;

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        pD3D9Hook = new D3D9Hook();
        break;
    case DLL_PROCESS_DETACH:

        delete pD3D9Hook;
        break;
    }
    return TRUE;
}


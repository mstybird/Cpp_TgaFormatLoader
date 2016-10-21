#pragma once

#define STRICT					// 型チェックを厳密に行なう
#define WIN32_LEAN_AND_MEAN		// ヘッダーからあまり使われない関数を省く
#define WINVER        0x0600	// Windows Vista以降対応アプリを指定(なくてもよい)
#define _WIN32_WINNT  0x0600	// 同上

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// 解放マクロ


#include <windows.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include"NcgTGALoader\TgaLoader.h"
//#include <dxerr.h>
// 必要なライブラリをリンクする
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment(lib,"dxgi.lib")

/*-------------------------------------------
関数定義
--------------------------------------------*/

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
HRESULT InitBackBuffer(void);
HRESULT InitApp(HINSTANCE hInst);
HRESULT InitDirect3D(void);
void InitResource();
HRESULT InitBackBuffer(void);
HRESULT Render(void);
bool CleanupDirect3D(void);
bool CleanupApp(void);
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
HRESULT IsDeviceRemoved(void);
bool AppIdle(void);
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int);
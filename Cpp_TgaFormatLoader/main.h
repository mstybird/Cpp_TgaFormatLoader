#pragma once

#define STRICT					// �^�`�F�b�N�������ɍs�Ȃ�
#define WIN32_LEAN_AND_MEAN		// �w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
#define WINVER        0x0600	// Windows Vista�ȍ~�Ή��A�v�����w��(�Ȃ��Ă��悢)
#define _WIN32_WINNT  0x0600	// ����

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }	// ����}�N��


#include <windows.h>
#include <crtdbg.h>
#include <d3dx11.h>
#include"NcgTGALoader\TgaLoader.h"
//#include <dxerr.h>
// �K�v�ȃ��C�u�����������N����
#pragma comment( lib, "d3d11.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )
#else
#pragma comment( lib, "d3dx11.lib" )
#endif
#pragma comment(lib,"dxgi.lib")

/*-------------------------------------------
�֐���`
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
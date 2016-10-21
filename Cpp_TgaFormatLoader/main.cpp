/*----------------------------------------------------------
Direct3D 11�T���v��
�EMicrosoft DirectX SDK (February 2010)
�EVisual Studio 2010 Express
�EWindows 7/Vista
�E�V�F�[�_���f��4.0
�Ή�

D3D11Sample01.cpp
�u��{�I�ȃA�v���P�[�V�����v
--------------------------------------------------------------*/


#include"main.h"




/*-------------------------------------------
�O���[�o���ϐ�(�A�v���P�[�V�����֘A)
--------------------------------------------*/
HINSTANCE	g_hInstance = NULL;	// �C���X�^���X �n���h��
HWND		g_hWindow = NULL;	// �E�C���h�E �n���h��

WCHAR		g_szAppTitle[] = L"Direct3D 11 Sample01";
WCHAR		g_szWndClass[] = L"D3D11S01";

// �N�����̕`��̈�T�C�Y
SIZE		g_sizeWindow = { 640, 480 };		// �E�C���h�E�̃N���C�A���g�̈�

												/*-------------------------------------------
												�O���[�o���ϐ�(Direct3D�֘A)
												--------------------------------------------*/

												//�@�\���x���̔z��
D3D_FEATURE_LEVEL g_pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
UINT              g_FeatureLevels = 3;   // �z��̗v�f��
D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // �f�o�C�X�쐬���ɕԂ����@�\���x��

											// �C���^�[�t�F�C�X
IDXGIFactory*			g_pFactory = NULL;			//DXGI
ID3D11Device*           g_pD3DDevice = NULL;        // �f�o�C�X
ID3D11DeviceContext*    g_pImmediateContext = NULL; // �f�o�C�X�E�R���e�L�X�g
IDXGISwapChain*         g_pSwapChain = NULL;        // �X���b�v�E�`�F�C��

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // �`��^�[�Q�b�g�E�r���[
D3D11_VIEWPORT          g_ViewPort[1];				// �r���[�|�[�g

ID3D11Texture2D*         g_pDepthStencil;           // �[�x/�X�e���V���E�e�N�X�`��
ID3D11DepthStencilView*  g_pDepthStencilView;       // �[�x/�X�e���V���E�r���[

bool g_bStandbyMode = false;
float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // �N���A����l(RGBA)



/*-------------------------------------------
�A�v���P�[�V����������
--------------------------------------------*/
HRESULT InitApp(HINSTANCE hInst)
{
	// �A�v���P�[�V�����̃C���X�^���X �n���h����ۑ�
	g_hInstance = hInst;

	// �E�C���h�E �N���X�̓o�^
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_ASTERISK);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szWndClass;

	if (!RegisterClass(&wc))
		return E_FAIL;

	// ���C�� �E�C���h�E�쐬
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = g_sizeWindow.cx;
	rect.bottom = g_sizeWindow.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	g_hWindow = CreateWindow(g_szWndClass, g_szAppTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInst, NULL);
	if (g_hWindow == NULL)
		return E_FAIL;

	// �E�C���h�E�\��
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/*-------------------------------------------
Direct3D������
--------------------------------------------*/
HRESULT InitDirect3D(void)
{
	// �E�C���h�E�̃N���C�A���g �G���A
	RECT rc;
	GetClientRect(g_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// �f�o�C�X�ƃX���b�v�E�`�F�C���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));    // �\���̂̒l��������(�K�v�ȏꍇ)
	sd.BufferCount = 1;       // �o�b�N�E�o�b�t�@��
	sd.BufferDesc.Width = 640;     // �o�b�N�E�o�b�t�@�̕�
	sd.BufferDesc.Height = 480;     // �o�b�N�E�o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �t�H�[�}�b�g
	sd.BufferDesc.RefreshRate.Numerator = 60;  // ���t���b�V���E���[�g(���q)
	sd.BufferDesc.RefreshRate.Denominator = 1; // ���t���b�V���E���[�g(����)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N�E�o�b�t�@�̎g�p�@
	sd.OutputWindow = g_hWindow;    // �֘A�t����E�C���h�E
	sd.SampleDesc.Count = 1;        // �}���`�E�T���v���̐�
	sd.SampleDesc.Quality = 0;      // �}���`�E�T���v���̃N�I���e�B
	sd.Windowed = TRUE;             // �E�C���h�E�E���[�h
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�����؂�ւ�

#if defined(DEBUG) || defined(_DEBUG)
													   //UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	UINT createDeviceFlags = 0;

#else
	UINT createDeviceFlags = 0;
#endif

	// �n�[�h�E�F�A�E�f�o�C�X���쐬
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
		&g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
		&g_pImmediateContext);
	if (FAILED(hr)) {
		// WARP�f�o�C�X���쐬
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
			g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
			&g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
			&g_pImmediateContext);
		if (FAILED(hr)) {
			// ���t�@�����X�E�f�o�C�X���쐬
			hr = D3D11CreateDeviceAndSwapChain(
				NULL, D3D_DRIVER_TYPE_REFERENCE, NULL, createDeviceFlags,
				g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
				&g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
				&g_pImmediateContext);
			if (FAILED(hr)) {
				return E_FAIL;
			}
		}
	}

	// �o�b�N �o�b�t�@�̏�����
	hr = InitBackBuffer();
	if (FAILED(hr))
		return E_FAIL;


	//IDXGIFactory�C���^�[�t�F�[�X�̎擾
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pFactory));
	if (FAILED(hr)) {
		return E_FAIL;
	}

	//Alt+Enter�ɂ���ʐ؂�ւ���L��������
	hr = g_pFactory->MakeWindowAssociation(g_hWindow, 0);

	if (FAILED(hr)) {
		return E_FAIL;
	}


	return S_OK;
}

void InitResource()
{
	Comfort::TGAImage tga;
	tga.ReadTGA("Resource/Building_Texture.tga");
}

/*-------------------------------------------
�o�b�N �o�b�t�@�̏�����(�o�b�N �o�b�t�@��`��^�[�Q�b�g�ɐݒ�)
--------------------------------------------*/
HRESULT InitBackBuffer(void)
{
	HRESULT hr;

	// �X���b�v�E�`�F�C������ŏ��̃o�b�N�E�o�b�t�@���擾����
	ID3D11Texture2D *pBackBuffer;  // �o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�C�X
	hr = g_pSwapChain->GetBuffer(
		0,                         // �o�b�N�E�o�b�t�@�̔ԍ�
		__uuidof(ID3D11Texture2D), // �o�b�t�@�ɃA�N�Z�X����C���^�[�t�F�C�X
		(LPVOID*)&pBackBuffer);    // �o�b�t�@���󂯎��ϐ�
	if (FAILED(hr))
		return E_FAIL;

	// �o�b�N�E�o�b�t�@�̏��
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// �o�b�N�E�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
	hr = g_pD3DDevice->CreateRenderTargetView(
		pBackBuffer,           // �r���[�ŃA�N�Z�X���郊�\�[�X
		NULL,                  // �`��^�[�Q�b�g�E�r���[�̒�`
		&g_pRenderTargetView); // �`��^�[�Q�b�g�E�r���[���󂯎��ϐ�
	SAFE_RELEASE(pBackBuffer);  // �ȍ~�A�o�b�N�E�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
	if (FAILED(hr))
		return E_FAIL;

	// �[�x/�X�e���V���E�e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
	//	descDepth.Width     = descBackBuffer.Width;   // ��
	//	descDepth.Height    = descBackBuffer.Height;  // ����
	descDepth.MipLevels = 1;       // �~�b�v�}�b�v�E���x����
	descDepth.ArraySize = 1;       // �z��T�C�Y
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;  // �t�H�[�}�b�g(�[�x�̂�)
											   //	descDepth.SampleDesc.Count   = 1;  // �}���`�T���v�����O�̐ݒ�
											   //	descDepth.SampleDesc.Quality = 0;  // �}���`�T���v�����O�̕i��
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // �f�t�H���g�g�p�@
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // �[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0;   // CPU����̓A�N�Z�X���Ȃ�
	descDepth.MiscFlags = 0;   // ���̑��̐ݒ�Ȃ�
	hr = g_pD3DDevice->CreateTexture2D(
		&descDepth,         // �쐬����2D�e�N�X�`���̐ݒ�
		NULL,               // 
		&g_pDepthStencil);  // �쐬�����e�N�X�`�����󂯎��ϐ�
	if (FAILED(hr))
		return E_FAIL;

	// �[�x/�X�e���V���E�r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;            // �r���[�̃t�H�[�}�b�g
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
		g_pDepthStencil,       // �[�x/�X�e���V���E�r���[�����e�N�X�`��
		&descDSV,              // �[�x/�X�e���V���E�r���[�̐ݒ�
		&g_pDepthStencilView); // �쐬�����r���[���󂯎��ϐ�
	if (FAILED(hr))
		return E_FAIL;

	// �r���[�|�[�g�̐ݒ�
	g_ViewPort[0].TopLeftX = 0.0f;    // �r���[�|�[�g�̈�̍���X���W�B
	g_ViewPort[0].TopLeftY = 0.0f;    // �r���[�|�[�g�̈�̍���Y���W�B
	g_ViewPort[0].Width = 640.0f;  // �r���[�|�[�g�̈�̕�
	g_ViewPort[0].Height = 480.0f;  // �r���[�|�[�g�̈�̍���
	g_ViewPort[0].MinDepth = 0.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
	g_ViewPort[0].MaxDepth = 1.0f; // �r���[�|�[�g�̈�̐[�x�l�̍ő�l

	return S_OK;
}

/*--------------------------------------------
��ʂ̕`�揈��
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

	// �`��^�[�Q�b�g�̃N���A
	g_pImmediateContext->ClearRenderTargetView(
		g_pRenderTargetView, // �N���A����`��^�[�Q�b�g
		ClearColor);         // �N���A����l

							 // �[�x/�X�e���V���l�̃N���A
	g_pImmediateContext->ClearDepthStencilView(
		g_pDepthStencilView, // �N���A����[�x/�X�e���V���E�r���[
		D3D11_CLEAR_DEPTH,   // �[�x�l�������N���A����
		1.0f,         // �[�x�o�b�t�@���N���A����l
		0);           // �X�e���V���E�o�b�t�@���N���A����l(���̏ꍇ�A���֌W)

					  // ���X�^���C�U�Ƀr���[�|�[�g��ݒ�
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);

	// �`��^�[�Q�b�g�E�r���[���o�̓}�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
	g_pImmediateContext->OMSetRenderTargets(
		1,                    // �`��^�[�Q�b�g�̐�
		&g_pRenderTargetView, // �`��^�[�Q�b�g�E�r���[�̔z��
		g_pDepthStencilView); // �ݒ肷��[�x/�X�e���V���E�r���[

							  // �`��(�ȗ�)

							  // �o�b�N�E�o�b�t�@�̕\��
	hr = g_pSwapChain->Present(0,   // ��ʂ𒼂��ɍX�V����
		0);  // ��ʂ����ۂɍX�V����

	return hr;
}

/*-------------------------------------------
Direct3D�̏I������
--------------------------------------------*/
bool CleanupDirect3D(void)
{
	// �f�o�C�X�E�X�e�[�g�̃N���A
	if (g_pImmediateContext)
		g_pImmediateContext->ClearState();

	//�t���X�N���[���̏ꍇ�A��x�E�B���h�E���[�h�ɂ���
	if (g_pSwapChain) {
		g_pSwapChain->SetFullscreenState(FALSE, NULL);
	}

	// �擾�����C���^�[�t�F�C�X�̊J��
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencil);

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);

	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);

	return true;
}

/*-------------------------------------------
�A�v���P�[�V�����̏I������
--------------------------------------------*/
bool CleanupApp(void)
{
	// �E�C���h�E �N���X�̓o�^����
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/*-------------------------------------------
�E�B���h�E����
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch (msg)
	{
	case WM_DESTROY:
		// Direct3D�̏I������
		CleanupDirect3D();
		// �E�C���h�E�����
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;
		break;
	case WM_KEYDOWN:
		// �L�[���͂̏���
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]�L�[�ŃE�C���h�E�����
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case VK_F5:
			if (g_pSwapChain != NULL) {
				BOOL fullscreen;
				g_pSwapChain->GetFullscreenState(&fullscreen, 0);
				g_pSwapChain->SetFullscreenState(!fullscreen, 0);
			}
			break;
		case VK_F6:
			//�f�B�X�v���C���[�h�̕ύX
			DXGI_MODE_DESC desc;
			desc.Width = 800;
			desc.Height = 600;
			desc.RefreshRate.Numerator = 60;
			desc.RefreshRate.Denominator = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			hr = g_pSwapChain->ResizeTarget(&desc);
			if (FAILED(hr)) {
				return E_FAIL;
			}
			break;
		}
		break;
	}

	// �f�t�H���g����
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*--------------------------------------------
�f�o�C�X�̏�������
--------------------------------------------*/
HRESULT IsDeviceRemoved(void)
{
	HRESULT hr;

	// �f�o�C�X�̏����m�F
	hr = g_pD3DDevice->GetDeviceRemovedReason();
	switch (hr) {
	case S_OK:
		break;         // ����

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		CleanupDirect3D();   // Direct3D�̉��(�A�v���P�[�V������`)
		hr = InitDirect3D();  // Direct3D�̏�����(�A�v���P�[�V������`)
		if (FAILED(hr))
			return E_FAIL;
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
	case DXGI_ERROR_INVALID_CALL:
	default:
		return E_FAIL;
	};

	return S_OK;         // ����
}

/*--------------------------------------------
�A�C�h�����̏���
--------------------------------------------*/
bool AppIdle(void)
{
	if (!g_pD3DDevice)
		return false;

	HRESULT hr;
	// �f�o�C�X�̏�������
	hr = IsDeviceRemoved();
	if (FAILED(hr))
		return false;

	//�X�^���o�C���[�h
	if (g_bStandbyMode) {
		hr = g_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		if (hr == DXGI_STATUS_OCCLUDED) {
			//�`��s�v���
			return true;
		}
		else {
			//�X�^���o�C���畜�A�����ꍇ
			g_bStandbyMode = false;
		}
	}

	hr = Render();

	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;
		//�`��^�[�Q�b�g�J���[�ύX
		float a = ClearColor[0];
		int i;
		for (i = 0; i < 3; i++) {
			ClearColor[i] = ClearColor[i + 1];
		}
		ClearColor[i] = a;
	}


	return true;
}

/*--------------------------------------------
���C��
---------------------------------------------*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// �f�o�b�O �q�[�v �}�l�[�W���ɂ�郁�������蓖�Ă̒ǐՕ��@��ݒ�
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �A�v���P�[�V�����Ɋւ��鏉����
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		return -1;
	}

	// Direct3D�̏�����
	hr = InitDirect3D();
	if (FAILED(hr)) {
		CleanupDirect3D();
		CleanupApp();
		return -2;
	}
	InitResource();
	// ���b�Z�[�W ���[�v
	MSG msg;
	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �A�C�h������
			if (!AppIdle())
				// �G���[������ꍇ�C�A�v���P�[�V�������I������
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	// �A�v���P�[�V�����̏I������
	CleanupApp();

	return (int)msg.wParam;
}

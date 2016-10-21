/*----------------------------------------------------------
Direct3D 11サンプル
・Microsoft DirectX SDK (February 2010)
・Visual Studio 2010 Express
・Windows 7/Vista
・シェーダモデル4.0
対応

D3D11Sample01.cpp
「基本的なアプリケーション」
--------------------------------------------------------------*/


#include"main.h"




/*-------------------------------------------
グローバル変数(アプリケーション関連)
--------------------------------------------*/
HINSTANCE	g_hInstance = NULL;	// インスタンス ハンドル
HWND		g_hWindow = NULL;	// ウインドウ ハンドル

WCHAR		g_szAppTitle[] = L"Direct3D 11 Sample01";
WCHAR		g_szWndClass[] = L"D3D11S01";

// 起動時の描画領域サイズ
SIZE		g_sizeWindow = { 640, 480 };		// ウインドウのクライアント領域

												/*-------------------------------------------
												グローバル変数(Direct3D関連)
												--------------------------------------------*/

												//機能レベルの配列
D3D_FEATURE_LEVEL g_pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
UINT              g_FeatureLevels = 3;   // 配列の要素数
D3D_FEATURE_LEVEL g_FeatureLevelsSupported; // デバイス作成時に返される機能レベル

											// インターフェイス
IDXGIFactory*			g_pFactory = NULL;			//DXGI
ID3D11Device*           g_pD3DDevice = NULL;        // デバイス
ID3D11DeviceContext*    g_pImmediateContext = NULL; // デバイス・コンテキスト
IDXGISwapChain*         g_pSwapChain = NULL;        // スワップ・チェイン

ID3D11RenderTargetView* g_pRenderTargetView = NULL; // 描画ターゲット・ビュー
D3D11_VIEWPORT          g_ViewPort[1];				// ビューポート

ID3D11Texture2D*         g_pDepthStencil;           // 深度/ステンシル・テクスチャ
ID3D11DepthStencilView*  g_pDepthStencilView;       // 深度/ステンシル・ビュー

bool g_bStandbyMode = false;
float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // クリアする値(RGBA)



/*-------------------------------------------
アプリケーション初期化
--------------------------------------------*/
HRESULT InitApp(HINSTANCE hInst)
{
	// アプリケーションのインスタンス ハンドルを保存
	g_hInstance = hInst;

	// ウインドウ クラスの登録
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

	// メイン ウインドウ作成
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

	// ウインドウ表示
	ShowWindow(g_hWindow, SW_SHOWNORMAL);
	UpdateWindow(g_hWindow);

	return S_OK;
}

/*-------------------------------------------
Direct3D初期化
--------------------------------------------*/
HRESULT InitDirect3D(void)
{
	// ウインドウのクライアント エリア
	RECT rc;
	GetClientRect(g_hWindow, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// デバイスとスワップ・チェインの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));    // 構造体の値を初期化(必要な場合)
	sd.BufferCount = 1;       // バック・バッファ数
	sd.BufferDesc.Width = 640;     // バック・バッファの幅
	sd.BufferDesc.Height = 480;     // バック・バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // フォーマット
	sd.BufferDesc.RefreshRate.Numerator = 60;  // リフレッシュ・レート(分子)
	sd.BufferDesc.RefreshRate.Denominator = 1; // リフレッシュ・レート(分母)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バック・バッファの使用法
	sd.OutputWindow = g_hWindow;    // 関連付けるウインドウ
	sd.SampleDesc.Count = 1;        // マルチ・サンプルの数
	sd.SampleDesc.Quality = 0;      // マルチ・サンプルのクオリティ
	sd.Windowed = TRUE;             // ウインドウ・モード
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モード自動切り替え

#if defined(DEBUG) || defined(_DEBUG)
													   //UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	UINT createDeviceFlags = 0;

#else
	UINT createDeviceFlags = 0;
#endif

	// ハードウェア・デバイスを作成
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
		&g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
		&g_pImmediateContext);
	if (FAILED(hr)) {
		// WARPデバイスを作成
		hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags,
			g_pFeatureLevels, g_FeatureLevels, D3D11_SDK_VERSION, &sd,
			&g_pSwapChain, &g_pD3DDevice, &g_FeatureLevelsSupported,
			&g_pImmediateContext);
		if (FAILED(hr)) {
			// リファレンス・デバイスを作成
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

	// バック バッファの初期化
	hr = InitBackBuffer();
	if (FAILED(hr))
		return E_FAIL;


	//IDXGIFactoryインターフェースの取得
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&g_pFactory));
	if (FAILED(hr)) {
		return E_FAIL;
	}

	//Alt+Enterによる画面切り替えを有効化する
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
バック バッファの初期化(バック バッファを描画ターゲットに設定)
--------------------------------------------*/
HRESULT InitBackBuffer(void)
{
	HRESULT hr;

	// スワップ・チェインから最初のバック・バッファを取得する
	ID3D11Texture2D *pBackBuffer;  // バッファのアクセスに使うインターフェイス
	hr = g_pSwapChain->GetBuffer(
		0,                         // バック・バッファの番号
		__uuidof(ID3D11Texture2D), // バッファにアクセスするインターフェイス
		(LPVOID*)&pBackBuffer);    // バッファを受け取る変数
	if (FAILED(hr))
		return E_FAIL;

	// バック・バッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// バック・バッファの描画ターゲット・ビューを作る
	hr = g_pD3DDevice->CreateRenderTargetView(
		pBackBuffer,           // ビューでアクセスするリソース
		NULL,                  // 描画ターゲット・ビューの定義
		&g_pRenderTargetView); // 描画ターゲット・ビューを受け取る変数
	SAFE_RELEASE(pBackBuffer);  // 以降、バック・バッファは直接使わないので解放
	if (FAILED(hr))
		return E_FAIL;

	// 深度/ステンシル・テクスチャの作成
	D3D11_TEXTURE2D_DESC descDepth = descBackBuffer;
	//	descDepth.Width     = descBackBuffer.Width;   // 幅
	//	descDepth.Height    = descBackBuffer.Height;  // 高さ
	descDepth.MipLevels = 1;       // ミップマップ・レベル数
	descDepth.ArraySize = 1;       // 配列サイズ
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;  // フォーマット(深度のみ)
											   //	descDepth.SampleDesc.Count   = 1;  // マルチサンプリングの設定
											   //	descDepth.SampleDesc.Quality = 0;  // マルチサンプリングの品質
	descDepth.Usage = D3D11_USAGE_DEFAULT;      // デフォルト使用法
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 深度/ステンシルとして使用
	descDepth.CPUAccessFlags = 0;   // CPUからはアクセスしない
	descDepth.MiscFlags = 0;   // その他の設定なし
	hr = g_pD3DDevice->CreateTexture2D(
		&descDepth,         // 作成する2Dテクスチャの設定
		NULL,               // 
		&g_pDepthStencil);  // 作成したテクスチャを受け取る変数
	if (FAILED(hr))
		return E_FAIL;

	// 深度/ステンシル・ビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;            // ビューのフォーマット
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pD3DDevice->CreateDepthStencilView(
		g_pDepthStencil,       // 深度/ステンシル・ビューを作るテクスチャ
		&descDSV,              // 深度/ステンシル・ビューの設定
		&g_pDepthStencilView); // 作成したビューを受け取る変数
	if (FAILED(hr))
		return E_FAIL;

	// ビューポートの設定
	g_ViewPort[0].TopLeftX = 0.0f;    // ビューポート領域の左上X座標。
	g_ViewPort[0].TopLeftY = 0.0f;    // ビューポート領域の左上Y座標。
	g_ViewPort[0].Width = 640.0f;  // ビューポート領域の幅
	g_ViewPort[0].Height = 480.0f;  // ビューポート領域の高さ
	g_ViewPort[0].MinDepth = 0.0f; // ビューポート領域の深度値の最小値
	g_ViewPort[0].MaxDepth = 1.0f; // ビューポート領域の深度値の最大値

	return S_OK;
}

/*--------------------------------------------
画面の描画処理
--------------------------------------------*/
HRESULT Render(void)
{
	HRESULT hr;

	// 描画ターゲットのクリア
	g_pImmediateContext->ClearRenderTargetView(
		g_pRenderTargetView, // クリアする描画ターゲット
		ClearColor);         // クリアする値

							 // 深度/ステンシル値のクリア
	g_pImmediateContext->ClearDepthStencilView(
		g_pDepthStencilView, // クリアする深度/ステンシル・ビュー
		D3D11_CLEAR_DEPTH,   // 深度値だけをクリアする
		1.0f,         // 深度バッファをクリアする値
		0);           // ステンシル・バッファをクリアする値(この場合、無関係)

					  // ラスタライザにビューポートを設定
	g_pImmediateContext->RSSetViewports(1, g_ViewPort);

	// 描画ターゲット・ビューを出力マージャーの描画ターゲットとして設定
	g_pImmediateContext->OMSetRenderTargets(
		1,                    // 描画ターゲットの数
		&g_pRenderTargetView, // 描画ターゲット・ビューの配列
		g_pDepthStencilView); // 設定する深度/ステンシル・ビュー

							  // 描画(省略)

							  // バック・バッファの表示
	hr = g_pSwapChain->Present(0,   // 画面を直ぐに更新する
		0);  // 画面を実際に更新する

	return hr;
}

/*-------------------------------------------
Direct3Dの終了処理
--------------------------------------------*/
bool CleanupDirect3D(void)
{
	// デバイス・ステートのクリア
	if (g_pImmediateContext)
		g_pImmediateContext->ClearState();

	//フルスクリーンの場合、一度ウィンドウモードにする
	if (g_pSwapChain) {
		g_pSwapChain->SetFullscreenState(FALSE, NULL);
	}

	// 取得したインターフェイスの開放
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencil);

	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);

	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pD3DDevice);

	return true;
}

/*-------------------------------------------
アプリケーションの終了処理
--------------------------------------------*/
bool CleanupApp(void)
{
	// ウインドウ クラスの登録解除
	UnregisterClass(g_szWndClass, g_hInstance);
	return true;
}

/*-------------------------------------------
ウィンドウ処理
--------------------------------------------*/
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	HRESULT hr = S_OK;

	switch (msg)
	{
	case WM_DESTROY:
		// Direct3Dの終了処理
		CleanupDirect3D();
		// ウインドウを閉じる
		PostQuitMessage(0);
		g_hWindow = NULL;
		return 0;
		break;
	case WM_KEYDOWN:
		// キー入力の処理
		switch (wParam)
		{
		case VK_ESCAPE:	// [ESC]キーでウインドウを閉じる
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
			//ディスプレイモードの変更
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

	// デフォルト処理
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*--------------------------------------------
デバイスの消失処理
--------------------------------------------*/
HRESULT IsDeviceRemoved(void)
{
	HRESULT hr;

	// デバイスの消失確認
	hr = g_pD3DDevice->GetDeviceRemovedReason();
	switch (hr) {
	case S_OK:
		break;         // 正常

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		CleanupDirect3D();   // Direct3Dの解放(アプリケーション定義)
		hr = InitDirect3D();  // Direct3Dの初期化(アプリケーション定義)
		if (FAILED(hr))
			return E_FAIL;
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
	case DXGI_ERROR_INVALID_CALL:
	default:
		return E_FAIL;
	};

	return S_OK;         // 正常
}

/*--------------------------------------------
アイドル時の処理
--------------------------------------------*/
bool AppIdle(void)
{
	if (!g_pD3DDevice)
		return false;

	HRESULT hr;
	// デバイスの消失処理
	hr = IsDeviceRemoved();
	if (FAILED(hr))
		return false;

	//スタンバイモード
	if (g_bStandbyMode) {
		hr = g_pSwapChain->Present(0, DXGI_PRESENT_TEST);
		if (hr == DXGI_STATUS_OCCLUDED) {
			//描画不要状態
			return true;
		}
		else {
			//スタンバイから復帰した場合
			g_bStandbyMode = false;
		}
	}

	hr = Render();

	if (hr == DXGI_STATUS_OCCLUDED) {
		g_bStandbyMode = true;
		//描画ターゲットカラー変更
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
メイン
---------------------------------------------*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	// デバッグ ヒープ マネージャによるメモリ割り当ての追跡方法を設定
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// アプリケーションに関する初期化
	HRESULT hr = InitApp(hInst);
	if (FAILED(hr))
	{
		return -1;
	}

	// Direct3Dの初期化
	hr = InitDirect3D();
	if (FAILED(hr)) {
		CleanupDirect3D();
		CleanupApp();
		return -2;
	}
	InitResource();
	// メッセージ ループ
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
			// アイドル処理
			if (!AppIdle())
				// エラーがある場合，アプリケーションを終了する
				DestroyWindow(g_hWindow);
		}
	} while (msg.message != WM_QUIT);

	// アプリケーションの終了処理
	CleanupApp();

	return (int)msg.wParam;
}

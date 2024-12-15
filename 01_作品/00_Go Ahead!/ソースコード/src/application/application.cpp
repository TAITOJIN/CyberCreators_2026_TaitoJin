//--------------------------------------------------------------------------------
// 
// アプリケーション [application.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
#include "application.h"
#include "glyph_ranges.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const std::string FILENAME_INI = "res\\imgui.ini";						// 初期設定ファイルの名前
	const std::string FILENAME_FONT = "res\\FONT\\ZenMaruGothic-Bold.ttf";	// フォント名
	constexpr float FONT_SIZE = 18.0f;										// 日本語フォントサイズ
	constexpr float NOTIFY_FONT_SIZE = 17.0f;								// 通知フォントサイズ
	constexpr float INIT_NOTIFY_FONT_SIZE = 16.0f;							// 通知の初期化フォントサイズ

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
App::App()
	: m_windowHandle(nullptr)
	, m_instanceHandle(nullptr)
{
	// DO_NOTHING
}

//---------------------------------------------------
// アプリケーションの実行
//---------------------------------------------------
void App::run(const HINSTANCE& inInstancehandle, const int& inCmdShow)
{
	if (initApp(inInstancehandle, inCmdShow))
	{ // 初期設定成功
		// メインループ
		mainLoop();
	}

	// 終了処理
	uninitApp();
}

//---------------------------------------------------
// アプリケーションの初期設定
//---------------------------------------------------
bool App::initApp(const HINSTANCE& inInstancehandle, const int& inCmdShow)
{
	// インスタンスハンドルの設定
	m_instanceHandle = inInstancehandle;

	// ウィンドウの初期設定
	HRESULT hr = initWindow(inCmdShow);
	if (FAILED(hr))
	{
		return false;
	}

	// ゲームマネージャーの初期化
	hr = GM.init(m_instanceHandle, m_windowHandle, TRUE);
	if (FAILED(hr))
	{
		return false;
	}

	// ImGui の初期設定
	hr = initImGui();
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

//---------------------------------------------------
// アプリケーションの終了処理
//---------------------------------------------------
void App::uninitApp()
{
	// ImGui の終了処理
	uninitImGui();

	// ゲームマネージャーの終了処理
	GM.uninit();

	// ウィンドウの終了処理
	uninitWindow();
}

//---------------------------------------------------
// ウィンドウの初期設定
//---------------------------------------------------
HRESULT App::initWindow(const int& inCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),					// WNDCLASSEX のメモリサイズ
		CS_CLASSDC,							// ウィンドウのスタイル
		windowProc,							// ウィンドウプロシージャ
		0,									// 0 にする (通常は使用しない)
		0,									// 0 にする (通常は使用しない)
		m_instanceHandle,					// インスタンスハンドル
		LoadIcon(NULL, IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(NULL, IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),			// クライアント領域の背景色
		nullptr,							// メニューバー
		CLASS_NAME.c_str(),					// ウィンドウクラスの名前
		LoadIcon(nullptr, IDI_APPLICATION)	// ファイルのアイコン
	};

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };	// ウィンドウのサイズ

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウを生成
	m_windowHandle = CreateWindowExA(
		0,							// 拡張ウィンドウスタイル
		CLASS_NAME.c_str(),			// ウィンドウクラスの名前
		WINDOW_NAME.c_str(),		// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,		// ウィンドウスタイル
		CW_USEDEFAULT,				// ウィンドウの左上 X 座標
		CW_USEDEFAULT,				// ウィンドウの左上 Y 座標
		(rect.right - rect.left),	// ウィンドウの幅
		(rect.bottom - rect.top),	// ウィンドウの高さ
		nullptr,					// 親ウィンドウのハンドル
		nullptr,					// メニューハンドルまたは子ウィンドウ
		m_instanceHandle,			// インスタンスハンドル
		nullptr);					// ウィンドウ作成データ

	// ウィンドウの表示
	ShowWindow(m_windowHandle, inCmdShow);	// ウィンドウの表示状態を設定
	UpdateWindow(m_windowHandle);			// クライアント領域を更新

	return S_OK;
}

//---------------------------------------------------
// ウィンドウの終了処理
//---------------------------------------------------
void App::uninitWindow()
{
	// ウィンドウクラスの登録を解除
	UnregisterClassA(CLASS_NAME.c_str(), m_instanceHandle);
}

//---------------------------------------------------
// ImGui の初期設定
//---------------------------------------------------
HRESULT App::initImGui()
{
	// ImGui の設定
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = FILENAME_INI.c_str();					// ファイル名の変更
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// ドッキングを有効にする
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// キーボード操作を有効にする
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// コントローラー操作を有効にする

	// スタイルを設定
	ImGui::StyleColorsLight();

	// platform の設定
	ImGui_ImplWin32_Init(m_windowHandle);
	ImGui_ImplDX9_Init(GM.getDevice());

	// 日本語フォントの追加
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		FILENAME_FONT.c_str(),
		FONT_SIZE,
		nullptr,
		GlyphRanges::Japanese);

	IM_ASSERT(font != nullptr);

	// MEMO: 2 個目に読み込むこと (scene_debug の notify で Fonts[1] を指定している)
	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(static_cast<const void*>(tahoma)), sizeof(tahoma), NOTIFY_FONT_SIZE, &font_cfg);

	// notify の初期設定
	ImGui::MergeIconsWithLatestFont(INIT_NOTIFY_FONT_SIZE, false);

	return S_OK;
}

//---------------------------------------------------
// ImGui の終了処理
//---------------------------------------------------
void App::uninitImGui()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void App::update()
{
	// フレーム開始
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ゲームマネージャーの更新
	GM.update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void App::draw() const
{
	// ImGui のフレーム終了
	ImGui::EndFrame();

	// ゲームマネージャーの描画
	GM.draw();
}

//---------------------------------------------------
// メインループ
//---------------------------------------------------
void App::mainLoop()
{
	MSG msg = {};	// メッセージを格納する変数

	m_tick.Reset();

	// メインループ
	bool done = false;
	while (!done)
	{
		// Windows の処理
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			// メッセージの設定
			TranslateMessage(&msg);	// 仮想キーメッセージを文字メッセージへ変換
			DispatchMessage(&msg);	// ウィンドウプロシージャへメッセージを送出
		}

		if (done)
		{ // ループを抜ける
			break;
		}

		// DirectX の処理
		m_tick.Update();
		if (m_tick.GetIsElapsed())
		{
			// 更新処理
			update();

			// 描画処理
			draw();
		}
	}
}

//---------------------------------------------------
// ImGui のウィンドウプロシージャ
//---------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------
// ウィンドウプロシージャ
//---------------------------------------------------
LRESULT CALLBACK App::windowProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wP, lP))
	{
		return true;
	}

	auto message = [=]() -> int { return MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO | MB_ICONQUESTION); };

	switch (msg)
	{
	case WM_CREATE:
	{
		// ドラッグアンドドロップを有効にする
		DragAcceptFiles(hWnd, TRUE);

	} break;
	case WM_DESTROY:
	{
		// メッセージを送る
		PostQuitMessage(0);

	} break;
	case WM_KEYDOWN:
	{
		switch (wP)
		{
		case VK_ESCAPE:
		{
			if (message() == IDYES)
			{
				// ウィンドウを破棄する
				DestroyWindow(hWnd);
			}
		} break;
		case VK_F11:
		{
			// ウィンドウ最大化
			ToggleFullscreen(hWnd);
		} break;
		}
	} break;
	case WM_CLOSE:
	{
		if (message() == IDYES)
		{
			// ウィンドウを破棄する
			DestroyWindow(hWnd);
		}
		else
		{
			// 0 を返さないと終了してしまう
			return 0;
		}	
	} break;
	case WM_SYSCOMMAND:
	{
		if ((wP & 0xfff0) == SC_KEYMENU)
		{
			return 0;
		}

	} break;
	case WM_DROPFILES:
	{
		char filepath[MAX_PATH] = {};
		DragQueryFile(reinterpret_cast<HDROP>(wP), 0U, &filepath[0], MAX_PATH);
		DragFinish(reinterpret_cast<HDROP>(wP));

	} break;
	case WM_LBUTTONDOWN:
	{
		// フォーカスを合わせてアクティブにする
		SetFocus(hWnd);
	} break;
	}

	return DefWindowProc(hWnd, msg, wP, lP);
}

//---------------------------------------------------
// フルスクリーン
//---------------------------------------------------
void App::ToggleFullscreen(HWND hWnd)
{
	static bool isFullScreen = false;	// フルスクリーンかどうか

	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	int nCmdShow = SW_NORMAL;

	if (isFullScreen)
	{ // 現在フルスク
		// ウィンドウモードに切り替え
		dwStyle |= WS_OVERLAPPEDWINDOW;
	}
	else
	{ // 現在フルスクでない
		// フルスクリーンモードに切り替え
		dwStyle &= ~WS_OVERLAPPEDWINDOW;
		rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
		nCmdShow = SW_MAXIMIZE;
	}

	SetWindowLong(
		hWnd,
		GWL_STYLE,
		dwStyle);
	ShowWindow(hWnd, nCmdShow);
	SetWindowPos(
		hWnd,
		HWND_TOP,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);
	ShowWindow(hWnd, nCmdShow);

	// フルスクフラグを切り替える
	isFullScreen = !isFullScreen;
}

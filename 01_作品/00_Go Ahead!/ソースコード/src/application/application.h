//--------------------------------------------------------------------------------
// 
// アプリケーション [application.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_manager
#include "game_manager/game_manager.h"
// system
#include "system/tick.h"

//===================================================
// アプリケーションのクラス
//===================================================
class App
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	App();
	~App() = default;

	void run(const HINSTANCE& inInstancehandle, const int& inCmdShow);	// 実行

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	App(const App&) = delete;
	void operator=(const App&) = delete;

	bool initApp(const HINSTANCE& inInstancehandle, const int& inCmdShow);	// アプリケーションの初期設定
	void uninitApp();														// アプリケーションの終了処理
	HRESULT initWindow(const int& inCmdShow);								// ウィンドウの初期設定
	void uninitWindow();													// ウィンドウの終了処理
	HRESULT initImGui();													// ImGui の初期設定
	void uninitImGui();														// ImGui の終了処理
	void update();															// 更新処理
	void draw() const;														// 描画処理
	void mainLoop();														// メインループ

	static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP);	// ウィンドウプロシージャ
	static void ToggleFullscreen(HWND hWnd);										// フルスクリーン

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	HWND m_windowHandle;		// ウィンドウハンドル
	HINSTANCE m_instanceHandle;	// インスタンスハンドル
	Tick m_tick;				// 時間
};

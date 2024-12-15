//--------------------------------------------------------------------------------
// 
// メイン [main.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// application
#include "application/application.h"

//---------------------------------------------------
// Windows アプリケーション
//---------------------------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR /*pCmdLine*/,
	_In_ int nCmdShow)
{
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// アプリケーションの実行
	App app;
	app.run(hInstance, nCmdShow);

	return 0;
}

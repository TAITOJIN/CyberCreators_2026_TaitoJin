//--------------------------------------------------------------------------------
// 
// 入力処理のマネージャー (管理者) [input_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_keyboard.h"
#include "input/input_mouse.h"
#include "input/input_gamepad.h"

//===================================================
// 入力処理のマネージャーのクラス
//===================================================
class InputManager
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	InputManager();
	~InputManager() = default;

	HRESULT init(const HINSTANCE& inInstanceHandle, const HWND& inWindowHandle);	// 初期設定
	void uninit();	// 終了処理
	void update();	// 更新処理

	const InputKeyboard& getKeyboard() const { return *m_keyboard; };	// キーボードの取得
	const InputMouse& getMouse() const { return *m_mouse; }				// マウスの取得
	InputGamepad& getGamepad() { return *m_gamepad; }					// ゲームパッドの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	InputManager(const InputManager&) = delete;
	void operator=(const InputManager&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	unique_ptr<InputKeyboard> m_keyboard;	// キーボード
	unique_ptr<InputMouse> m_mouse;			// マウス
	unique_ptr<InputGamepad> m_gamepad;		// ゲームパッド
};

//--------------------------------------------------------------------------------
// 
// キーボードの入力処理 [input_keyboard.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_base.h"

//===================================================
// キーボードの入力処理のクラスを定義
//===================================================
class InputKeyboard : public InputBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	InputKeyboard();
	~InputKeyboard() override = default;

	HRESULT init(const HINSTANCE& hInstance, const HWND& hWnd) override;	// 初期設定
	void uninit() override;	// 終了処理
	void update() override;	// 更新処理

	bool getPress(const int& inKey) const { return m_keyState[inKey]; }				// プレス状態の取得
	bool getTrigger(const int& inKey) const { return m_keyStateTrigger[inKey]; }	// トリガー状態の取得
	bool getRelease(const int& inKey) const { return m_keyStateRelease[inKey]; }	// リリース状態の取得
	bool getRepeat(const int& inKey) const { return m_keyStateRepeat[inKey]; }		// リピート状態の取得

private:
	//---------------------------------------------------
	// private メンバ定数宣言
	//---------------------------------------------------
	static constexpr uint32_t MAX_KEY = 256;	// キーの数

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	BYTE m_keyState[MAX_KEY];			// プレス状態取得
	BYTE m_keyStateTrigger[MAX_KEY];	// トリガー状態取得
	BYTE m_keyStateRelease[MAX_KEY];	// リリース状態取得
	BYTE m_keyStateRepeat[MAX_KEY];		// リピート状態取得
	DWORD m_keyCurtTime[MAX_KEY];		// キーボードの現在の時間
	DWORD m_keyExecLastTime[MAX_KEY];	// キーボードの最後に真を返した時間
};

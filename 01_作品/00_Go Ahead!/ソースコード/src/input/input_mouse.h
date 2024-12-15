//--------------------------------------------------------------------------------
// 
// マウスの入力処理 [input_mouse.h]
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
// キーボードの入力処理のクラス
//===================================================
class InputMouse : public InputBase
{
public:
	//---------------------------------------------------
	// public 列挙型
	//---------------------------------------------------
	// マウスのボタンの種類
	enum class BUTTON : uint32_t
	{
		LEFT = 0U,	// 左クリック
		RIGHT,		// 右クリック
		MIDDLE,		// 中央 (ホイール)
		MAX
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	InputMouse();
	~InputMouse() override = default;

	HRESULT init(const HINSTANCE& hInstance, const HWND& hWnd) override;	// 初期設定
	void uninit() override;	// 終了処理
	void update() override;	// 更新処理

	const Vec2& getPos() const { return m_pos; }	// カーソルの位置の取得
	const Vec2& getMove() const { return m_move; }	// 移動量の取得
	float getWheelRotVolume() const { return static_cast<float>(m_curtState.lZ) * REVISION_WHEEL_ROT_VOL; }	// ホイールの回転量を取得

	//---------------------------------------------------
	// プレス状態の取得
	//---------------------------------------------------
	bool getPress(const BUTTON& inButton) const
	{
		uint32_t button = static_cast<uint32_t>(inButton);
		return (m_prevState.rgbButtons[button] & BUTTON_PRESSED_MASK) && (m_curtState.rgbButtons[button] & BUTTON_PRESSED_MASK);
	}

	//---------------------------------------------------
	// トリガー状態の取得
	//---------------------------------------------------
	bool getTrigger(const BUTTON& inButton) const
	{
		uint32_t button = static_cast<uint32_t>(inButton);
		return !(m_prevState.rgbButtons[button] & BUTTON_PRESSED_MASK) && (m_curtState.rgbButtons[button] & BUTTON_PRESSED_MASK);
	}

	//---------------------------------------------------
	// リリース状態の取得
	//---------------------------------------------------
	bool getRelease(const BUTTON& inButton) const
	{
		uint32_t button = static_cast<uint32_t>(inButton);
		return (m_prevState.rgbButtons[button] & BUTTON_PRESSED_MASK) && !(m_curtState.rgbButtons[button] & BUTTON_PRESSED_MASK);
	}

private:
	//---------------------------------------------------
	// private メンバ定数宣言
	//---------------------------------------------------
	static constexpr float REVISION_WHEEL_ROT_VOL = 1.0f;	// ホイールの回転補正量

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	DIMOUSESTATE m_prevState;	// 過去の入力情報
	DIMOUSESTATE m_curtState;	// 現在の入力情報
	Vec2 m_pos;					// カーソルの位置
	Vec2 m_move;				// 移動量
};

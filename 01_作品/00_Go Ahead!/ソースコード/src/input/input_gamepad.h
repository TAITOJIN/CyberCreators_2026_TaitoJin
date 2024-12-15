//--------------------------------------------------------------------------------
// 
// ゲームパッドの入力処理 [input_gamepad.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_base.h"
// utility
#include "utility/enum.h"

//===================================================
// ゲームパッドの入力処理のクラスを定義
//===================================================
class InputGamepad : public InputBase
{
public:
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	static constexpr float IS_DIR = 0.1f;	// 傾けている状態

	//---------------------------------------------------
	// ボタンの種類
	//---------------------------------------------------
	enum class BUTTON_TYPE : WORD
	{
		UP = 0u,		// 十字 (上)
		DOWN,			// 十字 (下)
		LEFT,			// 十字 (左)
		RIGHT,			// 十字 (右)
		START,			// スタート
		BACK,			// バック
		LEFT_THUMB,		// 左スティック押込み
		RIGHT_THUMB,	// 右スティック押込み
		LEFT_SHOULDER,	// LB
		RIGHT_SHOULDER,	// RB
		LEFT_TRIGGER,	// LT トリガー
		RIGHT_TRIGGER,	// RT トリガー
		A,				// A
		B,				// B
		X,				// X
		Y,				// Y
		MAX
	};

	//---------------------------------------------------
	// トリガーボタンの種類
	//---------------------------------------------------
	enum class TRIGGER_TYPE : BYTE
	{
		LEFT = 0u,	// 左トリガー
		RIGHT,		// 右トリガー
		MAX
	};

	//---------------------------------------------------
	// スティックの種類
	//---------------------------------------------------
	enum class STICK_TYPE : BYTE
	{
		LEFT = 0u,	// 左スティック
		RIGHT,		// 右スティック
		MAX
	};

	//---------------------------------------------------
	// スティックの方向
	//---------------------------------------------------
	enum class STICK_ANGLE : BYTE
	{
		UP = 0u,	// 上
		DOWN,		// 下
		LEFT,		// 左
		RIGHT,		// 右
		MAX,
	};

	//---------------------------------------------------
	// スティックの各方向の情報
	//---------------------------------------------------
	struct StickState
	{
		bool isPress;	// プレス情報
		bool isTrigger;	// トリガー情報
		bool isRepeat;	// リピート情報
		bool isRelease;	// リリース情報
		std::chrono::steady_clock::time_point execLastTime;	// 最後に押した時間
	};

	//---------------------------------------------------
	// スティックの入力情報
	//---------------------------------------------------
	struct Stick
	{
		float dir;												// スティックの倒し具合
		float angle;											// スティックの角度
		StickState stickState[Enum::from(STICK_ANGLE::MAX)];	// スティックの方向の情報
	};

	//---------------------------------------------------
	// トリガーボタンの入力情報
	//---------------------------------------------------
	struct TriggerButton
	{
		BYTE pressure;	// 押し具合 (0 ~ 255)
		bool isPress;	// プレス情報
		bool isTrigger;	// トリガー情報
		bool isRepeat;	// リピート情報
		bool isRelease;	// リリース情報
		std::chrono::steady_clock::time_point execLastTime;	// 最後に押した時間
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	InputGamepad();
	~InputGamepad() override = default;

	HRESULT init(const HINSTANCE& hInstance, const HWND& hWnd) override;	// 初期設定
	void uninit() override;	// 終了処理
	void update() override;	// 更新処理

	void setVibration(const int& inTime, const WORD& inStrength);	// 振動の設定処理

	// ボタンの入力情報の取得
	bool getButtonPress(const BUTTON_TYPE& inType) { return (m_buttonState & (0x01 << Enum::from(inType))) != 0; }			// プレス
	bool getButtonTrigger(const BUTTON_TYPE& inType) { return (m_buttonStateTrigger & (0x01 << Enum::from(inType))) != 0; }	// トリガー
	bool getButtonRepeat(const BUTTON_TYPE& inType) { return (m_buttonStateRepeat & (0x01 << Enum::from(inType))) != 0; }	// リピート
	bool getButtonRelease(const BUTTON_TYPE& inType) { return (m_buttonStateRelease & (0x01 << Enum::from(inType))) != 0; }	// リリース

	// スティックの入力情報の取得
	const Stick& getStick(const STICK_TYPE& inType) { return m_sticks[Enum::from(inType)]; }
	const float& getStickDir(const STICK_TYPE& inType) { return m_sticks[Enum::from(inType)].dir; }
	const float& getStickAngle(const STICK_TYPE& inType) { return m_sticks[Enum::from(inType)].angle; }
	bool getStickPress(const STICK_TYPE& inType, const STICK_ANGLE& inAngle) { return m_sticks[Enum::from(inType)].stickState[Enum::from(inAngle)].isPress; }	// プレス
	bool getStickTrigger(const STICK_TYPE& inType, const STICK_ANGLE& inAngle) { return m_sticks[Enum::from(inType)].stickState[Enum::from(inAngle)].isTrigger; }	// トリガー
	bool getStickRelease(const STICK_TYPE& inType, const STICK_ANGLE& inAngle) { return m_sticks[Enum::from(inType)].stickState[Enum::from(inAngle)].isRelease; }	//　リリース
	bool getStickRepeat(const STICK_TYPE& inType, const STICK_ANGLE& inAngle) { return m_sticks[Enum::from(inType)].stickState[Enum::from(inAngle)].isRepeat; }	// リピート

	// トリガーボタンの入力情報の取得
	const TriggerButton& getTrigger(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)]; }
	const float& getTriggerPressure(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)].pressure; }	// 押し具合
	bool getStickPress(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)].isPress; }		// プレス
	bool getStickTrigger(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)].isTrigger; }	// トリガー
	bool getStickRelease(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)].isRelease; }	// リリース
	bool getStickRepeat(const TRIGGER_TYPE& inType) { return m_triggerButton[Enum::from(inType)].isRepeat; }	// リピート

	// スティックを傾けているか否かの取得
	bool isDir(const STICK_TYPE& inType) const { return m_sticks[Enum::from(inType)].dir > IS_DIR; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	void updateKeys(const XINPUT_STATE& inXInput);					// キーの更新処理
	void updateVibration();											// 振動の更新処理
	void updateStick(const XINPUT_STATE& inXInput);					// スティックの更新処理
	void updateTriggerPedal(const XINPUT_STATE& inXInput);			// トリガーボタンの更新処理
	float findAngle(const Vec3& inPos, const Vec3& inTargetPos);	// 2 点の角度を求める関数

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	// ボタン
	WORD m_buttonState;											// ボタンの入力情報
	WORD m_buttonStateTrigger;									// ボタンのトリガー情報
	WORD m_buttonStateRepeat;									// ボタンのリピート情報
	WORD m_buttonStateRelease;									// ボタンのリリース情報
	std::chrono::steady_clock::time_point m_buttonExecLastTime;	// ボタンの最後に真を返した時間
	// モーター
	XINPUT_VIBRATION m_moter;	// ゲームパッドのモーター
	int m_vibrationTime;		// 振動持続時間
	WORD m_vibrationStrength;	// 振動の強さ (0 ~ 65535)
	// スティック
	Stick m_sticks[Enum::from(STICK_TYPE::MAX)];	// スティックの入力情報
	// トリガーボタン
	TriggerButton m_triggerButton[Enum::from(TRIGGER_TYPE::MAX)];	// トリガーボタンの入力情報
};

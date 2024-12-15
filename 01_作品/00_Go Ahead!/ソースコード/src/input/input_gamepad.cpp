//--------------------------------------------------------------------------------
// 
// ゲームパッドの入力処理 [input_gamepad.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_gamepad.h"

//---------------------------------------------------
// 警告オフ
//---------------------------------------------------
#pragma warning(disable : 4995)

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr WORD MAX_VIBRATION = 65535;		// 最大振動値
	constexpr float INITVALUE_DIR = 32768.0f;	// 傾けている状態の初期値
	constexpr long long REPEAT_DURATION = 120;	// リピートの間隔 (ミリ秒)

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
InputGamepad::InputGamepad()
	: m_buttonState(0)
	, m_buttonStateTrigger(0)
	, m_buttonStateRepeat(0)
	, m_buttonStateRelease(0)
	, m_triggerButton()
	, m_buttonExecLastTime()
	, m_moter()
	, m_vibrationTime(0)
	, m_vibrationStrength(0)
	, m_sticks()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT InputGamepad::init(const HINSTANCE& /*hInstance*/, const HWND& /*hWnd*/)
{
	// XInput のステートを設定 (有効にする)
	XInputEnable(true);

	// ゲームパッドの振動制御の 0 クリア
	ZeroMemory(&m_moter, sizeof(XINPUT_VIBRATION));

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void InputGamepad::uninit()
{
	// XInoutのステートを設定 (無効にする)
	XInputEnable(false);

	// 親クラスの処理
	InputBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void InputGamepad::update()
{
	XINPUT_STATE gamepadState;	// ゲームパッドの入力情報

	// ジョイパッドの状態を取得
	DWORD result = XInputGetState(0, &gamepadState);
	if (result != ERROR_SUCCESS)
	{
		return;
	}

	// ボタンの更新処理
	updateKeys(gamepadState);

	// 振動の更新処理
	updateVibration();

	// スティックの更新処理
	updateStick(gamepadState);

	// トリガーボタンの更新処理
	updateTriggerPedal(gamepadState);
}

//---------------------------------------------------
// ボタンの更新処理
//---------------------------------------------------
void InputGamepad::updateKeys(const XINPUT_STATE& inXInput)
{
	WORD curtButton = inXInput.Gamepad.wButtons;		// 現在のボタン状態
	WORD prevButton = m_buttonState;					// 過去のボタン状態
	m_buttonStateTrigger = curtButton & ~prevButton;	// トリガー処理
	m_buttonStateRelease = ~curtButton & prevButton;	// リリース処理

	// 現在の時刻を取得
	auto now = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_buttonExecLastTime).count();

	if (inXInput.Gamepad.wButtons && (duration > REPEAT_DURATION))
	{ // ボタンが入力されていて、かつ現在の時間と最後に真を返した時間の差がリピートの間隔を越えていた

		// 最後に真を返した時間を保存
		m_buttonExecLastTime = now;

		// ボタンボードのリピート情報を保存
		m_buttonStateRepeat = curtButton;
	}
	else
	{
		// ボタンボードのリピート情報を保存
		m_buttonStateRepeat = 0;
	}

	m_buttonState = curtButton;	// プレス情報
}

//---------------------------------------------------
// 振動の更新処理
//---------------------------------------------------
void InputGamepad::updateVibration()
{
	m_moter.wLeftMotorSpeed = m_vibrationStrength;	// 左のモーター
	m_moter.wRightMotorSpeed = m_vibrationStrength;	// 右のモーター
	XInputSetState(0, &m_moter);					// 振動情報を送る

	// 振動時間の減算
	if (m_vibrationTime > 0)
	{
		m_vibrationTime--;
	}
	else
	{
		m_vibrationTime = 0;
		m_vibrationStrength = 0;
	}
}

//==========================================================================================
// スティックの更新処理
//==========================================================================================
void InputGamepad::updateStick(const XINPUT_STATE& inXInput)
{
	// 現在の時刻を取得
	auto now = std::chrono::steady_clock::now();

	for (uint32_t i = 0; i < Enum::from(STICK_TYPE::MAX); i++)
	{
		float x, y;	// スティックの x, y 軸
		x = y = 0.0f;

		switch (i)
		{
		case Enum::from(STICK_TYPE::LEFT):
			x = inXInput.Gamepad.sThumbLX;
			y = inXInput.Gamepad.sThumbLY;
			break;
		case Enum::from(STICK_TYPE::RIGHT):
			x = inXInput.Gamepad.sThumbRX;
			y = inXInput.Gamepad.sThumbRY;
			break;
		default:
			assert(false);
			break;
		}

		bool isLeaning[Enum::from(STICK_ANGLE::MAX)] = {};	// 各方向の傾き状態

		//　角度を計算
		m_sticks[i].angle = findAngle(Vec3(x, y, 0.0f), Vec3(0.0f, 0.0f, 0.0f));

		// スティックの倒し具合を取得
		m_sticks[i].dir = fabsf(x);
		if (m_sticks[i].dir < fabsf(y))
		{
			m_sticks[i].dir = fabsf(y);
		}

		m_sticks[i].dir /= INITVALUE_DIR;	// <= 初期値 倒している状態

		if (m_sticks[i].dir > IS_DIR)
		{ // 傾いている
			// 角度を 4 分割で見る
			if ((m_sticks[i].angle < D3DX_PI * -0.75f) ||
				(m_sticks[i].angle > D3DX_PI * 0.75f))
			{ // 上
				isLeaning[Enum::from(STICK_ANGLE::UP)] = true;
			}
			else if ((m_sticks[i].angle > D3DX_PI * -0.25f) &&
				(m_sticks[i].angle < D3DX_PI * 0.25f))
			{ // 下
				isLeaning[Enum::from(STICK_ANGLE::DOWN)] = true;
			}
			else if ((m_sticks[i].angle > D3DX_PI * -0.75f) &&
				(m_sticks[i].angle < D3DX_PI * -0.25f))
			{ // 左
				isLeaning[Enum::from(STICK_ANGLE::LEFT)] = true;
			}
			else if ((m_sticks[i].angle > D3DX_PI * 0.25f) &&
				(m_sticks[i].angle < D3DX_PI * 0.75f))
			{ // 右
				isLeaning[Enum::from(STICK_ANGLE::RIGHT)] = true;
			}
		}

		// 角度に応じた入力処理
		for (uint32_t j = 0; j < Enum::from(STICK_ANGLE::MAX); j++)
		{
			// スティックのトリガー情報を保存
			m_sticks[i].stickState[j].isTrigger = !m_sticks[i].stickState[j].isPress && isLeaning[j];

			// スティックのリリース情報を保存
			m_sticks[i].stickState[j].isRelease = m_sticks[i].stickState[j].isPress && !isLeaning[j];

			// 間隔
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_sticks[i].stickState[j].execLastTime).count();

			// スティックのリピート状態の更新
			m_sticks[i].stickState[j].isRepeat = (isLeaning[j] && (duration > REPEAT_DURATION));

			// 最後に押した時間を更新
			if (m_sticks[i].stickState[j].isTrigger || m_sticks[i].stickState[j].isRepeat)
			{ // トリガーもしくはリピートが真
				m_sticks[i].stickState[j].execLastTime = now;
			}

			// スティックのプレス情報を保存
			m_sticks[i].stickState[j].isPress = isLeaning[j];
		}
	}
}

//---------------------------------------------------
// トリガーボタンの更新処理
//---------------------------------------------------
void InputGamepad::updateTriggerPedal(const XINPUT_STATE& inXInput)
{
	// 現在の時刻を取得
	auto now = std::chrono::steady_clock::now();

	for (BYTE i = 0; i < Enum::from(TRIGGER_TYPE::MAX); i++)
	{
		// 前回のトリガーの状態
		bool prevTrigger = m_triggerButton[i].isPress;

		// 現在のトリガーボタンの状態
		BYTE currTrigger = 0;
		switch (i)
		{
		case Enum::from(TRIGGER_TYPE::LEFT):
			currTrigger = inXInput.Gamepad.bLeftTrigger;
			break;
		case Enum::from(TRIGGER_TYPE::RIGHT):
			currTrigger = inXInput.Gamepad.bRightTrigger;
			break;
		default:
			assert(false);
			break;
		}

		// 押し具合の更新
		m_triggerButton[i].pressure = currTrigger;

		// プレス状態の更新
		bool isPress = (currTrigger > 0);
		m_triggerButton[i].isPress = isPress;

		// トリガー状態の更新
		m_triggerButton[i].isTrigger = (!prevTrigger && isPress);

		// リリース状態の更新
		m_triggerButton[i].isRelease = (prevTrigger && !isPress);

		// 間隔
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_triggerButton[i].execLastTime).count();

		// リピート状態の更新
		m_triggerButton[i].isRepeat = (isPress && (duration > REPEAT_DURATION));

		// 最後に押した時間を更新
		if (m_triggerButton[i].isTrigger || m_triggerButton[i].isRepeat)
		{
			m_triggerButton[i].execLastTime = now;
		}
	}
}

//---------------------------------------------------
// 振動の設定処理
//---------------------------------------------------
void InputGamepad::setVibration(const int& inTime, const WORD& inStrength)
{
	m_vibrationTime = inTime;
	m_vibrationStrength = inStrength;
}

//==========================================================================================
// 2点の角度を求める
//==========================================================================================
float InputGamepad::findAngle(const Vec3& inPos, const Vec3& inTargetPos)
{
	float angle = atan2f(inTargetPos.y - inPos.y, inTargetPos.x - inPos.x);

	angle -= (D3DX_PI * HALF);
	angle *= -1.0f;

	return angle;
}

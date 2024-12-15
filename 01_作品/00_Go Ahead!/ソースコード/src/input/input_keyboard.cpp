//--------------------------------------------------------------------------------
// 
// キーボードの入力処理 [input_keyboard.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_keyboard.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr int INTERVAL_REPEAT = 120;	// リピートの間隔

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
InputKeyboard::InputKeyboard()
	: m_keyState()
	, m_keyStateTrigger()
	, m_keyStateRelease()
	, m_keyStateRepeat()
	, m_keyCurtTime()
	, m_keyExecLastTime()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT InputKeyboard::init(const HINSTANCE& hInstance, const HWND& hWnd)
{
	// 親クラスの処理
	HRESULT hr = InputBase::init(hInstance, hWnd);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 入力デバイスの生成
	hr = m_input->CreateDevice(GUID_SysKeyboard, m_device.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	hr = m_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	hr = m_device->SetCooperativeLevel(
		hWnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得
	m_device->Acquire();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void InputKeyboard::uninit()
{
	// 親クラスの処理
	InputBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void InputKeyboard::update()
{
	BYTE keyState[MAX_KEY] = {};	// キーボードの入力情報

	// 入力デバイスからデータを取得
	HRESULT hr = m_device->GetDeviceState(sizeof(keyState), &keyState[0]);
	if (FAILED(hr))
	{
		// アクセス権を獲得
		m_device->Acquire();
		return;
	}

	for (int i = 0; i < MAX_KEY; i++)
	{
		// トリガー情報を保存
		{
			BYTE trigger = (m_keyState[i] ^ keyState[i]) & keyState[i];
			m_keyStateTrigger[i] = trigger;
		}

		// リリース情報を保存
		{
			BYTE release = (m_keyState[i] ^ keyState[i]) & ~keyState[i];
			m_keyStateRelease[i] = release;
		}

		// 現在の時間を取得する
		m_keyCurtTime[i] = timeGetTime();

		if (keyState[i] && ((m_keyCurtTime[i] - m_keyExecLastTime[i]) > INTERVAL_REPEAT))
		{
			// 最後に真を返した時間を保存
			m_keyExecLastTime[i] = m_keyCurtTime[i];

			// リピート情報を保存
			m_keyStateRepeat[i] = keyState[i];
		}
		else
		{
			// リピート情報を保存
			m_keyStateRepeat[i] = 0;
		}

		// プレス情報を保存
		m_keyState[i] = keyState[i];
	}
}

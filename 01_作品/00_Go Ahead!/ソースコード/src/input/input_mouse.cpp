//-------------------------------------------------------------------
//
// マウスの入力処理 [input_mouse.cpp]
// Author: Taito Jin
//
//-------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_mouse.h"
// game_manager
#include "game_manager/game_manager.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
InputMouse::InputMouse()
	: m_prevState()
	, m_curtState()
	, m_pos(DEF_VEC2)
	, m_move(DEF_VEC2)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT InputMouse::init(const HINSTANCE& hInstance, const HWND& hWnd)
{
	// 親クラスの処理
	HRESULT hr = InputBase::init(hInstance, hWnd);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 入力デバイスの生成
	hr = m_input->CreateDevice(GUID_SysMouse, m_device.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	hr = m_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// モードを設定 (フォアグラウンド && 非排他モード)
	hr = m_device->SetCooperativeLevel(
		hWnd,
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
#if 1
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モード
#else
	diprop.dwData = DIPROPAXISMODE_ABS;	// 絶対値モード
#endif

	hr = m_device->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// アクセス権を獲得していざ入力制御開始！
	m_device->Acquire();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void InputMouse::uninit()
{
	// 親クラスの処理
	InputBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void InputMouse::update()
{
	// 過去の状態を更新
	m_prevState = m_curtState;

	// マウスの状態の取得と更新
	HRESULT hr = m_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_curtState);
	if (FAILED(hr))
	{
		// アクセス権を獲得
		m_device->Acquire();
		return;
	}

	// ウィンドウハンドルの取得
	const HWND& hWnd = GM.getWindowHandle();

	// 過去の位置を保持
	Vec2 prevPos = m_pos;

	// カーソルの位置を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	// 位置の更新
	m_pos.x = static_cast<float>(cursorPos.x);
	m_pos.y = static_cast<float>(cursorPos.y);

	// 移動量の更新
	m_move = m_pos - prevPos;
}

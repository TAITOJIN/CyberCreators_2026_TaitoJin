//--------------------------------------------------------------------------------
// 
// 入力処理のマネージャー (管理者) [input_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_manager.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
InputManager::InputManager()
	: m_keyboard(nullptr)
	, m_mouse(nullptr)
	, m_gamepad(nullptr)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT InputManager::init(const HINSTANCE& inInstanceHandle, const HWND& inWindowHandle)
{
	// キーボードの生成
	m_keyboard = make_unique<InputKeyboard>();
	if (!m_keyboard)
	{
		return E_FAIL;
	}

	// キーボードの初期設定
	HRESULT hr = m_keyboard->init(inInstanceHandle, inWindowHandle);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// マウスの生成
	m_mouse = make_unique<InputMouse>();
	if (!m_mouse)
	{
		return E_FAIL;
	}

	// マウスの初期設定
	hr = m_mouse->init(inInstanceHandle, inWindowHandle);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// ゲームパッドの生成
	m_gamepad = make_unique<InputGamepad>();
	if (!m_gamepad)
	{
		return E_FAIL;
	}

	// ゲームパッドの初期設定
	hr = m_gamepad->init(inInstanceHandle, inWindowHandle);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void InputManager::uninit()
{
	// ゲームパッドの終了処理
	if (m_gamepad)
	{
		m_gamepad->uninit();
	}

	// マウスの終了処理
	if (m_mouse)
	{
		m_mouse->uninit();
	}

	// キーボードの終了処理
	if (m_keyboard)
	{
		m_keyboard->uninit();
	}
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void InputManager::update()
{
	// キーボードの更新
	m_keyboard->update();

	// マウスの更新
	m_mouse->update();

	// ゲームパッドのの更新
	m_gamepad->update();
}

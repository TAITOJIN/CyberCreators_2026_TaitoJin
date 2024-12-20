﻿//--------------------------------------------------------------------------------
// 
// 入力処理の基底クラス [input_base.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// input
#include "input/input_base.h"

//---------------------------------------------------
// 静的メンバの初期化
//---------------------------------------------------
ComPtr<IDirectInput8> InputBase::m_input;	// DirectInput オブジェクトへのポインタ

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
InputBase::InputBase()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT InputBase::init(const HINSTANCE& hInstance, const HWND& /*hWnd*/)
{
	if (!m_input)
	{
		// DirectInput オブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
			IID_IDirectInput8, reinterpret_cast<void**>(m_input.GetAddressOf()), nullptr)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void InputBase::uninit()
{
	// 入力デバイス
	if (m_device)
	{
		m_device->Unacquire();
	}
}

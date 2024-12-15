//--------------------------------------------------------------------------------
// 
// 入力処理の基底クラス [input_base.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 入力処理の基底クラスを定義
//===================================================
class InputBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	InputBase();
	virtual ~InputBase() = default;

	virtual HRESULT init(const HINSTANCE& hInstance, const HWND& hWnd);	// 初期設定
	virtual void uninit();		// 終了処理
	virtual void update() = 0;	// 更新処理

protected:
	
	//---------------------------------------------------
	//  protected メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirectInputDevice8> m_device;	// 入力デバイスへのポインタ

	static ComPtr<IDirectInput8> m_input;	// DirectInput オブジェクトへのポインタ

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	InputBase(const InputBase&) = delete;
	void operator=(const InputBase&) = delete;
};

//--------------------------------------------------------------------------------
// 
// レンダーラー [renderer.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// レンダーラーのクラス
//===================================================
class Renderer
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Renderer();
	~Renderer() = default;
	HRESULT init(const HWND& inWndHandle, const BOOL& inIsWindowed);	// 初期設定
	void uninit();														// 終了処理
	void update();														// 更新処理
	void draw(const std::function<void()>& inDrawFunc, const std::function<void()>& inDrawWithShadowFunc);	// 描画処理

	Device getDevice() const { return m_d3dDevice.Get(); };	// デバイスの取得

	static void setAlphaBlendAdd();	// α ブレンドを加算合成にする
	static void resetAlphaBlend();	// α ブレンドを元に戻す
	static void onWireFrame();		// ワイヤーフレームの表示
	static void offWireFrame();		// ワイヤーフレームの非表示
	static void onLight();			// ライトを有効にする
	static void offLight();			// ライトを無効にする

	void setDestBgColor(const Color& inCol) { m_destBgColor = inCol; }
	void setBgColor(const Color& inCol) { m_bgColor = inCol; }

	IDirect3DSurface9* getBackBuf() { return m_backBuf.Get(); }				// バックバッファの取得
	IDirect3DSurface9* getZBuf() { return m_zBuf.Get(); }					// Z バッファの取得
	IDirect3DTexture9* getDepthTexture() { return m_depthTexture.Get(); }	// 深度値格納用の取得
	IDirect3DSurface9* getTexZBuf() { return m_texZBuf.Get(); }				// テクスチャバッファの取得
	ID3DXEffect* getEffect() { return m_effect.Get(); }						// エフェクトの取得

	void setCameraAndLight(std::function<void()> inFunc) { m_setCameraAndLight = inFunc; }	// カメラとライトの設定処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Renderer(const Renderer&) = delete;
	void operator=(const Renderer&) = delete;

	HRESULT initShadowMap();	// シャドウマップの初期設定

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirect3D9> m_d3d;				// Direct3D オブジェクト
	ComPtr<IDirect3DDevice9> m_d3dDevice;	// Direct3D デバイス

	// シャドウマップ用
	ComPtr<IDirect3DSurface9> m_backBuf;		// バックバッファ
	ComPtr<IDirect3DSurface9> m_zBuf;			// Z バッファ
	ComPtr<IDirect3DTexture9> m_depthTexture;	// 深度値格納用
	ComPtr<IDirect3DSurface9> m_texZBuf;		// テクスチャバッファ
	ComPtr<ID3DXEffect> m_effect;				// シャドウマップのシェーダー

	std::function<void()> m_setCameraAndLight;	// カメラとライトの設定関数

	Color m_bgColor;		// 背景の色
	Color m_destBgColor;	// 背景の目的の色
};

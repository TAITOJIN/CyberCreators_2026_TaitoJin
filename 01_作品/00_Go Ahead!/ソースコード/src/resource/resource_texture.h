//--------------------------------------------------------------------------------
// 
// テクスチャのリソース [resource_texture.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// テクスチャ情報のクラス
//===================================================
class ResourceTexture
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	ResourceTexture();
	~ResourceTexture() = default;

	void setTexture(Texture inTexture) { m_texture.Attach(inTexture); inTexture = nullptr; };	// テクスチャの設定処理
	void setU(const int& inU) { m_subdivisionCntU = inU; }										// 分割数 (U) の設定
	void setV(const int& inV) { m_subdivisionCntV = inV; }										// 分割数 (V) の設定
	void setAnimInterVal(const int& inInterval) { m_animInterval = inInterval; }				// アニメーションの更新間隔の設定

	Texture getTexture() const { return m_texture.Get(); }			// テクスチャの取得
	const int& getU() const { return m_subdivisionCntU; }			// 分割数 (U) の取得
	const int& getV() const { return m_subdivisionCntV; }			// 分割数 (V) の取得
	const int& getAnimInterVal() const { return m_animInterval; }	// アニメーションの更新間隔の取得

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirect3DTexture9> m_texture;	// テクスチャへのポインタ
	int m_subdivisionCntU;					// テクスチャの分割数 (U)
	int m_subdivisionCntV;					// テクスチャの分割数 (V)
	int m_animInterval;						// アニメーションの更新間隔
};

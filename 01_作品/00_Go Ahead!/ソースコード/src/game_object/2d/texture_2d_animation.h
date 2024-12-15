//--------------------------------------------------------------------------------
// 
// 2D テクスチャアニメーション [texture_2d_animation.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
#include "resource/resource_texture.h"

//===================================================
// 2D テクスチャアニメーションのクラス
//===================================================
class Tex2dAnim
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Tex2dAnim();										// デフォルトコンストラクタ
	Tex2dAnim(const Tex2dAnim& right);					// コピーコンストラクタ
	Tex2dAnim(Tex2dAnim&& right) noexcept;				// ムーブコンストラクタ
	~Tex2dAnim();										// デストラクタ
	Tex2dAnim& operator=(const Tex2dAnim& right);		// コピー代入演算子
	Tex2dAnim& operator=(Tex2dAnim&& right) noexcept;	// ムーブ代入演算子

	HRESULT init();	// 初期設定

	/**
	 * @brief 更新処理
	 *
	 * @param[in] inVtx 頂点情報
	 * @param[in] inResTex リソーステクスチャ
	 *
	 * @retval true アニメーション終了
	 * @retval false アニメーション終了していない
	 */
	bool update(const VertexBuffer& inVtx, const ResourceTexture& inResTex);

	void updateTexCoord(const VertexBuffer& inVtx, const Vec2& inMove);	// テクスチャ座標の更新 (移動)

	// テクスチャ座標の設定
	void setTexCoord(
		const VertexBuffer& inVtx,
		const Vec2& inPos1,
		const Vec2& inPos2,
		const Vec2& inPos3,
		const Vec2& inPos4);

	void setCounterAnim(int inCounter) { m_counterAnim = inCounter; }	// アニメーションカウンタの設定
	void setPatternAnimU(int inPattern) { m_patternAnimU = inPattern; }	// アニメーションパターン U の設定
	void setPatternAnimV(int inPattern) { m_patternAnimV = inPattern; }	// アニメーションパターン V の設定

	const int& getCounterAnim() const { return m_counterAnim; }		// アニメーションカウンタの取得
	const int& getPatternAnimU() const { return m_patternAnimU; }	// アニメーションパターン U の取得
	const int& getPatternAnimV() const { return m_patternAnimV; }	// アニメーションパターン V の取得

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	int m_counterAnim;	// アニメーションカウンタ
	int m_patternAnimU;	// パターン No. (U)
	int m_patternAnimV;	// パターン No. (V)
};

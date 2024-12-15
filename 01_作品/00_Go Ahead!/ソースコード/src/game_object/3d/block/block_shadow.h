//--------------------------------------------------------------------------------
// 
// 影ブロック [block_shadow.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_dummy.h"

//===================================================
// 影ブロックのクラス
//===================================================
class BlockShadow : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockShadow();
	~BlockShadow() override = default;
	BlockShadow(BlockShadow&& right) noexcept;
	BlockShadow& operator=(BlockShadow&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setScaleCounter(const float& inCnter) { m_scaleCounter = inCnter; }	// スケールカウンターの設定
	void setDoAdd(const bool& inDo) { m_doAdd = inDo; }							// 加算していくかどうかの設定
	void setFirstScale(const Vec3& inScale) { m_firstScale = inScale; }			// 最初のスケールの設定
	void setMtxParent(shared_ptr<Matrix> inMtx) { m_mtxParent = inMtx; }		// 親マトリックスの設定
	void setPosParent(const Vec3& inPos) { m_posParent = inPos; }				// 親の位置の設定
	void setIsAnim(const bool& inIsAnim) { m_isAnim = inIsAnim; }				// アニメーションをするか否かの設定処理

	const Vec3& getFirstScale() const { return m_firstScale; }				// 最初のスケールの取得
	shared_ptr<Matrix> getMtxParent() const { return m_mtxParent.lock(); }	// 親マトリックスの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockShadow(const BlockShadow&) = delete;
	void operator=(const BlockShadow&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_firstScale;				// 最初のスケール
	weak_ptr<Matrix> m_mtxParent;	// 親のマトリックス
	float m_scaleCounter;			// スケールカウンター
	bool m_doAdd;					// スケールカウンターを加算していく
	Vec3 m_posParent;				// 親の位置
	bool m_isAnim;					// アニメーションをするか否か
};

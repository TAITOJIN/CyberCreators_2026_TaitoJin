//--------------------------------------------------------------------------------
// 
// 音源ブロック [block_sound.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/game_object_3d.h"
// resource
#include "resource/sound.h"

//===================================================
// 音源ブロック生み出すクラス
//===================================================
class BlockSound : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockSound();
	~BlockSound() override = default;
	BlockSound(BlockSound&& right) noexcept;
	BlockSound& operator=(BlockSound&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setMtxParent(shared_ptr<Matrix> inMtx) { m_mtxParent = inMtx; }
	void setLabel(const CSoundManager::LABEL& inLabel) { m_label = inLabel; }

	shared_ptr<Matrix> getMtxParent() const { return m_mtxParent.lock(); }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockSound(const BlockSound&) = delete;
	void operator=(const BlockSound&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	float m_scale;					// スケール
	weak_ptr<Matrix> m_mtxParent;	// 親のマトリックス
	float m_volume;					// ボリューム
	bool m_isCollision;				// 範囲内か否か
	CSoundManager::LABEL m_label;	// ラベル
};

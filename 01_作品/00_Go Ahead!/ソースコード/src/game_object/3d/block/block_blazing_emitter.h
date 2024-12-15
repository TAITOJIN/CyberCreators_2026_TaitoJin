//--------------------------------------------------------------------------------
// 
// めらめらブロックを生み出す [block_blazing_emitter.h]
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

//===================================================
// めらめらブロックを生み出すクラス
//===================================================
class BlockBlazingEmitter : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockBlazingEmitter();
	~BlockBlazingEmitter() override = default;
	BlockBlazingEmitter(BlockBlazingEmitter&& right) noexcept;
	BlockBlazingEmitter& operator=(BlockBlazingEmitter&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setMtxParent(shared_ptr<Matrix> inMtx) { m_mtxParent = inMtx; }
	shared_ptr<Matrix> getMtxParent() const { return m_mtxParent.lock(); }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockBlazingEmitter(const BlockBlazingEmitter&) = delete;
	void operator=(const BlockBlazingEmitter&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	float m_setInterval;			// 生み出す間隔
	float m_counterInterval;		// 間隔の記録
	Vec3 m_createPos;				// 生み出す位置
	float m_scale;					// スケール
	float m_msMinLife;				// 最短寿命
	float m_msMaxLife;				// 最長寿命
	float m_gravity;				// 重力
	weak_ptr<Matrix> m_mtxParent;	// 親のマトリックス
};

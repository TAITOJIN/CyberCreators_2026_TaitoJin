//--------------------------------------------------------------------------------
// 
// 衝撃を発生させるブロック [block_impact.h]
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
#include "game_object/3d/block/block_base.h"
#include "game_object/3d/block/block_blazing_emitter.h"

//===================================================
// 衝撃を発生させるブロックのクラス
//===================================================
class BlockImpact : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockImpact();
	~BlockImpact() override = default;
	BlockImpact(BlockImpact&& right) noexcept;
	BlockImpact& operator=(BlockImpact&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsEnabled(const bool& inIs) { m_isEnabled = inIs; }	// 有効か否かの設定

	const bool& getIsEnabled() const { return m_isEnabled; }	// 有効か否かの取得
	auto& getEmitters() { return m_emitters; }					// めらめらの生成器の取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockImpact(const BlockImpact&) = delete;
	void operator=(const BlockImpact&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_isEnabled;	// 有効か否か
	float m_time;		// 時間
	std::list<weak_ptr<BlockBlazingEmitter>> m_emitters;	// めらめらの生成器たち
};

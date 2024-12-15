//--------------------------------------------------------------------------------
// 
// 複数のブロックの一部 [block_blocks.h]
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
// utility
#include "utility/flag_manager.h"

//===================================================
// 複数のブロックの一部のブロックのクラス
//===================================================
class BlockBlocks : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockBlocks();
	~BlockBlocks() override = default;
	BlockBlocks(BlockBlocks&& right) noexcept;
	BlockBlocks& operator=(BlockBlocks&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setMtxParent(shared_ptr<Matrix> inMatrix) { m_mtxParent = inMatrix; }	// 親マトリックスの設定
	shared_ptr<Matrix> getMtxParent() const { return m_mtxParent.lock(); }		// 親マトリックスの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockBlocks(const BlockBlocks&) = delete;
	void operator=(const BlockBlocks&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	weak_ptr<Matrix> m_mtxParent;	// 親のマトリックス
	bool m_wasSet;					// 既にトランスフォームの設定をした
};

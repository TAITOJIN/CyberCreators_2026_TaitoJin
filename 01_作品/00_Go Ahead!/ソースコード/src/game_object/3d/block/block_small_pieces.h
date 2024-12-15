//--------------------------------------------------------------------------------
// 
// 粉々なブロック [block_small_pieces.h]
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
// 粉々なブロックのクラス
//===================================================
class BlockSmallPieces : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockSmallPieces();
	~BlockSmallPieces() override = default;
	BlockSmallPieces(BlockSmallPieces&& right) noexcept;
	BlockSmallPieces& operator=(BlockSmallPieces&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockSmallPieces(const BlockSmallPieces&) = delete;
	void operator=(const BlockSmallPieces&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;		// 移動量
	float m_maxLife;	// 最大寿命
	float m_life;		// 寿命
	float m_firstScale;	// 最初のスケール (x, y, z で統一)
};

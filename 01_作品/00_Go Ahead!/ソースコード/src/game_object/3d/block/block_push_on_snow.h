//--------------------------------------------------------------------------------
// 
// 雪の上にある押せるブロック [block_push_on_snow.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_push.h"
// utility
#include "utility/collision.h"

//===================================================
// 押せるブロックのクラス
//===================================================
class BlockPushOnSnow : public BlockPush
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockPushOnSnow();
	~BlockPushOnSnow() override = default;
	BlockPushOnSnow(BlockPushOnSnow&& right) noexcept;
	BlockPushOnSnow& operator=(BlockPushOnSnow&& right) noexcept;

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
	BlockPushOnSnow(const BlockPushOnSnow&) = delete;
	void operator=(const BlockPushOnSnow&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_posOld;	// 過去の位置
};

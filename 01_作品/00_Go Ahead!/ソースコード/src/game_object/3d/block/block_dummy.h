//--------------------------------------------------------------------------------
// 
// ブロックのダミークラス [block_dummy.h]
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
// utility
#include "utility/flag_manager.h"

//===================================================
// ダミーブロックのクラス
//===================================================
class BlockDummy : public BlockBase
{
public:
	//---------------------------------------------------
	// フラグ
	//---------------------------------------------------
	enum class DUMMY_FLAGS : uint32_t
	{
		NONE = 0u,
		NO_COLLISION = 1 << 0,	// 当たり判定を行わない
		NO_DISP = 1 << 1,		// 表示しない
		DISP_WIRE = 1 << 2,		// ワイヤーを表示する
		SAVE = 1 << 3			// 保存する
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockDummy();
	~BlockDummy() override = default;
	BlockDummy(BlockDummy&& right) noexcept;
	BlockDummy& operator=(BlockDummy&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存
	void setForEachDummyFlag();					// フラグごとの初期設定

	void setDummyFlags(const FlagManager<DUMMY_FLAGS>& inFlags) { m_dummyFlags = inFlags; }	// フラグの設定処理
	FlagManager<DUMMY_FLAGS>& getDummyFlags() const { return m_dummyFlags; }				// フラグの取得処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockDummy(const BlockDummy&) = delete;
	void operator=(const BlockDummy&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	mutable FlagManager<DUMMY_FLAGS> m_dummyFlags;	// ダミーフラグ (MEMO: save で return するときに取得必要なので mutable)
};

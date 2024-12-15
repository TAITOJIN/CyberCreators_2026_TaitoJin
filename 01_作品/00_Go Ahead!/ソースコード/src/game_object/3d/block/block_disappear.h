//--------------------------------------------------------------------------------
// 
// 消えるブロック [block_disappear.h]
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

//===================================================
// 消えるブロックのクラス
//===================================================
class BlockDisappear : public BlockBase
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockDisappear();
	~BlockDisappear() override = default;
	BlockDisappear(BlockDisappear&& right) noexcept;
	BlockDisappear& operator=(BlockDisappear&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;				// インスペクター
	void save(json& outObject) const override;	// 保存

	const bool& getFirstAppearState() const { return m_firstAppearState; }	// 最初の出現状態の取得
	const bool& getIsAppear() const { return m_isAppear; }					// 出現しているかどうかの取得

	// 出現状態の切替
	//static void switchAppearState();

	// 全解放リストの初期化
	//static void initList();

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockDisappear(const BlockDisappear&) = delete;
	void operator=(const BlockDisappear&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_firstAppearState;	// 最初の出現状態
	bool m_isAppear;			// 出現しているかどうか

	//static std::list<BlockDisappear*> m_blockDisappearList;	// 消えるブロックのリスト
};

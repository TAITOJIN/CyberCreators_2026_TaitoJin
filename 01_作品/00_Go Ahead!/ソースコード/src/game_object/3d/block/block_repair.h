//--------------------------------------------------------------------------------
// 
// 修復されるブロック [block_repair.h]
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
// 修復されるブロックのクラス
//===================================================
class BlockRepair : public BlockDummy
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlockRepair();
	~BlockRepair() override = default;
	BlockRepair(BlockRepair&& right) noexcept;
	BlockRepair& operator=(BlockRepair&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsCollapse(const bool& inIsCollapse) { m_isCollapse = inIsCollapse; }			// 崩壊状態の設定
	void setCollapseTime(const float& inCollapseTime) { m_collapseTime = inCollapseTime; }	// 崩壊時間の設定

	const bool& getISCollapse() const { return m_isCollapse; }		// 崩壊状態の取得
	const bool& getCollapseTime() const { return m_collapseTime; }	// 崩壊時間の取得
	bool getIsRepair() const { return !m_transforms.empty(); }		// 修復中かどうかの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlockRepair(const BlockRepair&) = delete;
	void operator=(const BlockRepair&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_isCollapse;						// 崩壊中
	float m_collapseTime;					// 崩壊時間
	float m_collapseTimeKeep;				// 崩壊時間 (保持)
	std::list<btTransform> m_transforms;	// トランスフォーム
};

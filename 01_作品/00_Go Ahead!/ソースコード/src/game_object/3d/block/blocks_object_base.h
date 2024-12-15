//--------------------------------------------------------------------------------
// 
// 複数のブロックを組み合わせたオブジェクト [blocks_base.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/block/block_base.h"

//===================================================
// 複数のブロックを組み合わせたオブジェクトの基底クラス
//===================================================
class BlocksObjectBase : public GameObject3D
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	BlocksObjectBase();
	~BlocksObjectBase() override = default;
	BlocksObjectBase(BlocksObjectBase&& right) noexcept;
	BlocksObjectBase& operator=(BlocksObjectBase&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	std::vector<weak_ptr<BlockBase>>& getBlocks() { return m_blocks; }	// ブロックの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	BlocksObjectBase(const BlocksObjectBase&) = delete;
	void operator=(const BlocksObjectBase&) = delete;

	void loadBlocks(const std::string& inFilename);	// ブロックの読み込み処理

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::vector<weak_ptr<BlockBase>> m_blocks;	// ブロック
};

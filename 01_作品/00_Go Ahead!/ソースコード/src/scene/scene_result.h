//--------------------------------------------------------------------------------
// 
// リザルトのシーン [scene_result.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"
// game_object
// // 2d
#include "game_object/2d/game_object_2d.h"

//===================================================
// リザルトシーンのクラス
//===================================================
class SceneResult : public SceneBase
{
public:
	//---------------------------------------------------
	// 優先度
	//---------------------------------------------------
	enum class PRIORITY : uint32_t
	{
		BG = static_cast<uint32_t>(SceneBase::BASE_PRIORITY::MAX),	// 背景
		MAX
	};

	//---------------------------------------------------
	// オブジェクト 2D の種類
	//---------------------------------------------------
	enum class RESULT_OBJECT2D_TYPE : uint32_t
	{
		BG = 0U,
		BEST_TIME,
		BEST_COIN,
		MAX
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneResult();
	~SceneResult() override = default;

	HRESULT init() override;	// 初期設定
	void uninit() override;		// 終了処理
	void update() override;		// 更新処理
	void draw() const override;	// 描画処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneResult(const SceneResult&) = delete;
	void operator=(const SceneResult&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::unordered_map<RESULT_OBJECT2D_TYPE, weak_ptr<GameObject2D>> m_object2ds;
};

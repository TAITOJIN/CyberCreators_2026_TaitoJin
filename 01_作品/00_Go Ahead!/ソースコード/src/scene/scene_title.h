//--------------------------------------------------------------------------------
// 
// タイトルのシーン [scene_title.h]
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
// タイトルシーンのクラス
//===================================================
class SceneTitle : public SceneBase
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
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneTitle();
	~SceneTitle() override = default;

	HRESULT init() override;	// 初期設定
	void uninit() override;		// 終了処理
	void update() override;		// 更新処理
	void draw() const override;	// 描画処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneTitle(const SceneTitle&) = delete;
	void operator=(const SceneTitle&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	float m_timeCameraAnim;
	float m_intervalCameraAnim;
	weak_ptr<GameObject2D> m_player;

	Vec3 m_playerPos;
	Vec3 m_playerMove;
	bool m_isDispNormal;
	float m_playerAppearTimeCounter;
	float m_playerAnimTimeCounter;
};

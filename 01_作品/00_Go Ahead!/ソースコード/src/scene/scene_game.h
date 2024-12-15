//--------------------------------------------------------------------------------
// 
// ゲームのシーン [scene_game.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"
// resource
#include "resource/sound.h"

//===================================================
// ゲームシーンのクラス
//===================================================
class SceneGame : public SceneBase
{
public:
	//---------------------------------------------------
	// 優先度
	//---------------------------------------------------
	enum class PRIORITY : uint32_t
	{
		BULLET = static_cast<uint32_t>(SceneBase::BASE_PRIORITY::MAX),	// 弾
		ENEMY,			// 敵
		ITEM,			// アイテム
		EXPLOSION,		// 爆発
		EFFECT,			// エフェクト

		POLYGON_3D,		// ポリゴン
		MESHFIELD_3D,	// メッシュフィールド
		BILLBOARD,		// ビルボード
		MODEL,			// モデル
		MOTION,			// モーション

		NUMBER,			// 数字
		SCORE,			// スコア
		UI,				// UI
		MAX
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneGame();
	~SceneGame() override = default;

	HRESULT init() override;	// 初期設定
	void uninit() override;		// 終了処理
	void update() override;		// 更新処理
	void draw() const override;	// 描画処理

	void setSoundSourceInfo(const CSoundManager::LABEL& inLabel, const bool& inIsCollision, const float& inVolume);	// 音源情報の設定

	void setIsGoal(const bool& inIsGoal) { m_isGoal = inIsGoal; }	// ゴールしたかどうかの設定
	const bool& getIsGoal() const { return m_isGoal; }				// ゴールしたかどうかの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneGame(const SceneGame&) = delete;
	void operator=(const SceneGame&) = delete;

	// TODO: 基底クラスに純粋仮想を用意
	void registerEvent();

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::string m_stageFilename;	// ステージファイル名
	bool m_isGoal;					// ゴールしたかどうか

	//---------------------------------------------------
	// 音源
	//---------------------------------------------------
	struct SoundSource
	{
		bool m_isPlay;
		bool m_wasPlay;
		float m_volume;
	};

	std::unordered_map<CSoundManager::LABEL, SoundSource> m_soundSources;
};

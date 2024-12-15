//--------------------------------------------------------------------------------
// 
// Now Loading のシーン [scene_load.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"

//===================================================
// ロードシーンのクラス
//===================================================
class SceneLoad : public SceneBase
{
public:
	//---------------------------------------------------
	// 優先度
	//---------------------------------------------------
	enum class PRIORITY : uint32_t
	{
		FADE = static_cast<uint32_t>(SceneBase::BASE_PRIORITY::MAX),	// フェード
		MAX
	};

	//---------------------------------------------------
	// 状態
	//---------------------------------------------------
	enum class STATE : uint32_t
	{
		NONE = 0u,
		FADE_IN,
		FADE_OUT
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneLoad();
	~SceneLoad() override = default;

	HRESULT init() override;	// 初期設定
	void uninit() override;		// 終了処理
	void update() override;		// 更新処理
	void draw() const override;	// 描画処理

	void setEventName(const std::string& inEventName) { m_eventName = inEventName; }	// イベント名の設定処理
	void setFadeState(const STATE& inState) { m_fadeState = inState; };					// フェード状態の設定処理

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneLoad(const SceneLoad&) = delete;
	void operator=(const SceneLoad&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::string m_eventName;	// イベント名
	STATE m_fadeState;			// フェード状態
};

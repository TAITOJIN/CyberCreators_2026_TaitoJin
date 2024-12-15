//--------------------------------------------------------------------------------
// 
// ゴールエフェクト [goal_effect.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"
// // 2d
#include "game_object/2d/texture_2d_animation.h"
// scene
// // element
#include "scene/elements/time_manager.h"

//===================================================
// ゴールエフェクトのクラス
//===================================================
class GoalEffect : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	GoalEffect();
	~GoalEffect() override = default;
	GoalEffect(GoalEffect&& right) noexcept;
	GoalEffect& operator=(GoalEffect&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void setIsEffecting(const bool& inIs) { m_isEffectIng = inIs; }			// エフェクト中
	const bool& getIsEffecting() const { return m_isEffectIng; }		// エフェクト中

private:
	enum EFFECT_CIRCLE_TYPE : uint32_t
	{
		EFFECT_CIRCLE_TYPE_PINK_RED = 0U,
		EFFECT_CIRCLE_TYPE_BLUE_LIGHTBLUE,
		EFFECT_CIRCLE_TYPE_WHITE_0,
		EFFECT_CIRCLE_TYPE_MAX
	};

	struct EffectEllipse
	{
		Vec2 pos;
		Vec2 radius;
		float angle;
		Color col;
		float time;
	};

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<IDirect3DVertexBuffer9> m_vtxBuf;	// 頂点バッファへ
	Vec3 m_pos;									// 位置
	Vec3 m_rot;									// 向き
	Vec3 m_size;								// サイズ
	Color m_col;								// 色
	std::string m_texKey;						// 使用するテクスチャのキー

	ComPtr<IDirect3DIndexBuffer9> m_idxBuf;		// インデックスバッファ
	uint32_t m_vtxNumX;							// 頂点数 (x)
	uint32_t m_vtxNumY;							// 頂点数 (y)
	uint32_t m_nIdxNum;							// インデックスの数
	uint32_t m_nPrimNum;						// ポリゴンの数

	// TODO: flag_manager でフラグ管理

	bool m_isEffectIng;							// エフェクト中
	bool m_wasEffectIng;						// エフェクト中だった

	bool m_done;								// エフェクト完了

	TimeManager m_time;							// 時間
	float m_effectTime;							// エフェクトの時間

	float m_ellipseTime;
	std::array<EffectEllipse, EFFECT_CIRCLE_TYPE_MAX> m_ellipses;

	bool m_firstEffect;
	bool m_secondEffect;
	bool m_lastEffect;
	bool m_zoom;
	bool m_isGoResult;
};

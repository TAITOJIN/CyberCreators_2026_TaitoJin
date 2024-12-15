//--------------------------------------------------------------------------------
// 
// プレイヤー [player.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/model.h"
#include "game_object/3d/shadow.h"
#include "game_object/3d/block/block_shadow.h"

//===================================================
// プレイヤーのクラス
//===================================================
class Player : public Model
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Player();
	~Player() override = default;
	Player(Player&& right) noexcept;
	Player& operator=(Player&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void createCapsuleCharacter();	// カプセル形状のキャラクターの作成

	void setMove(const Vec3& inMove) { m_move = inMove; }										// 移動量の設定
	void setCharacter(btKinematicCharacterController* inChara);									// キャラの設定
	void setGravity(const Vec3& inGravity) { m_gravity = inGravity; }							// 重力の設定
	void setIsTouchWater(const bool& inIsTouchWater) { m_isTouchWater = inIsTouchWater; }		// 水に触れているかどうかの設定
	void setIsUnderWater(const bool& inIsUnderWater) { m_isUnderWater = inIsUnderWater; }		// 水中にいるかいないの設定
	void setWasTouchWater(const bool& inWasTouchWater) { m_wasTouchWater = inWasTouchWater; }	// 水に触れていたかどうかの設定
	void setRespawnPos(const Vec3& inPos) { m_respawnPos = inPos; }								// リスポーン地点の設定

	const Vec3& getMove() const { return m_move; }						// 移動量の取得
	auto& getCharacter() const { return *m_character; }					// キャラの取得
	const Vec3& getGravity() const { return m_gravity; }				// 重力の取得
	const float& getIsTouchWater() const { return m_isTouchWater; }		// 水に触れているかの取得
	const float& getIsUnderWater() const { return m_isUnderWater; }		// 水中にいるかいないかの取得
	const bool& getWasTouchWater() const { return m_wasTouchWater; }	// 水に触れていたかの取得
	const bool& getWasUnderWater() const { return m_wasUnderWater; }	// 水中にいたか否かの取得
	const Vec3& getRespawnPos() const { return m_respawnPos; }			// リスポーン地点の取得
	const Quaternion& getTargetRot() const { return m_targetRot; }		// 目的の向きの取得

private:
	//---------------------------------------------------
	// エイリアス宣言
	//---------------------------------------------------
	using CharacterDeleter = std::function<void(btKinematicCharacterController*)>;	// キャラクターのカスタムデリータ

	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Player(const Player&) = delete;
	void operator=(const Player&) = delete;

	void customDeleter(btKinematicCharacterController* ptr);	// カスタムデリータ (MEMO: ラムダ式のカスタムデリータ内で呼び出す) 親のとオーバーロード

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_move;							// 移動量
	Quaternion m_targetRot;					// 目的の向き
	float m_speed;							// 移動速度
	float m_atnSpeed;						// 速度の減衰係数
	float m_jumpStrength;					// ジャンプ力
	bool m_isJumping;						// ジャンプ中
	float m_maxSlopeDegree;					// 斜めの床に乗れる角度
	float m_stepHeight;						// 乗れる高さ
	float m_firstJumpSpeed;					// 最初のジャンプ力
	float m_fallSpeed;						// 落ちる速度
	Vec3 m_gravity;							// 重力
	unique_ptr<btKinematicCharacterController, CharacterDeleter> m_character;	// キャラクター
	bool m_isTouchWater;					// 水に触れている
	bool m_isUnderWater;					// 水中にいる (頭までしっかり入っているかどうか)
	bool m_wasTouchWater;					// 水に触れていた
	bool m_wasUnderWater;					// 水中にいた
	Vec3 m_respawnPos;						// リスポーン地点
	Vec3 m_respawnPosKeep;					// リスポーン地点 (save 用)
	unique_ptr<Shadow> m_shadow;			// 影
	bool m_isOnIce;							// 氷の上か否か
	float m_walkEffectIntervalCounter;		// 歩きエフェクトの間隔
	std::string m_effectMatKey;				// エフェクトマテリアルキー
	std::list<weak_ptr<BlockShadow>> m_shadowBlocks;	// 影ブロック
};

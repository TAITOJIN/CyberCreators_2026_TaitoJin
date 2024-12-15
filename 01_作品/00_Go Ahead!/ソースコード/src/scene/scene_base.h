//--------------------------------------------------------------------------------
// 
// シーンの基底 [scene_base.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/camera.h"
#include "scene/elements/light.h"
#include "scene/elements/physics_world.h"
#include "scene/elements/time_manager.h"
#include "scene/elements/shared_data.h"
#include "scene/elements/event_manager.h"
// resource
#include "resource/resource_manager.h"
// utility
#include "utility/flag_manager.h"
// game_object
#include "game_object/game_object.h"
#include "game_object/impact.h"
// 2d
#include "game_object/2d/goal_effect.h"
// // 3d
#include "game_object/3d/flame.h"
#include "game_object/3d/updraft.h"
#include "game_object/3d/updraft_effect_emitter.h"
#include "game_object/3d/water_wheel.h"
// // // block
#include "game_object/3d/block/block_push.h"
#include "game_object/3d/block/block_ice.h"
#include "game_object/3d/block/block_freeze_water_drop.h"
#include "game_object/3d/block/block_freeze_water_drop_emitter.h"
#include "game_object/3d/block/block_rotate.h"
#include "game_object/3d/block/block_move.h"
#include "game_object/3d/block/block_impact.h"
// // // character
#include "game_object/3d/character/player.h"

//===================================================
// シーンの基底クラス
//===================================================
class SceneBase : public std::enable_shared_from_this<SceneBase>
{
public:
	//---------------------------------------------------
	// 優先度
	//---------------------------------------------------
	enum class BASE_PRIORITY : uint32_t
	{
		NONE = 0u,
		CAMERA,		// カメラ
		LIGHT,		// ライト
		WORLD,		// 世界
		DEFAULT,	// デフォルト (設定しなかった場合)
		PLAYER,		// プレイヤー
		BLOCK,		// ブロック
		FADE,		// 
		LOADING,	// 
		MAX	// TODO: 各シーンの PRIORITY の一番最初の値をこれにする
	};

	//---------------------------------------------------
	// シーンのフラグ
	//---------------------------------------------------
	enum class SCENE_FLAGS : BYTE
	{
		NONE = 0u,
		IS_PAUSE = 1 << 1,	// ポーズ中か否か
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneBase();
	virtual ~SceneBase() = default;

	virtual HRESULT init();		// 初期設定
	virtual void uninit();		// 終了処理
	virtual void update();		// 更新処理
	virtual void draw() const;	// 描画処理
	virtual void pause();		// ポーズ
	virtual void resume();		// 再開

	void drawWithShadow() const;	// 描画

	void loadObjects(const char* inFilename);											// オブジェクトの読み込み処理
	void resizeObjects(const uint32_t& inSize) { m_objects.resize(inSize); }			// サイズの変更
	shared_ptr<GameObject> addObject(const std::string& inType, const json& inData);	// 追加

	void setSharedData(shared_ptr<SharedData> inSharedData) { m_sharedData = inSharedData; }	// 共有データの設定
	void setEventManager(shared_ptr<EventManager> inEvents) { m_eventManager = inEvents; }		// イベントの管理者の設定
	void setFlags(const FlagManager<SCENE_FLAGS>& inFlags) { m_flags = inFlags; }				// フラグの設定処理
	void setGoalEffect(shared_ptr<GoalEffect> inEffetc) { m_goalEffect = inEffetc; }			// ゴールエフェクト

	shared_ptr<TimeManager> getTime() { return m_time; }						// 時間の取得
	shared_ptr<ResourceManager> getResManager() { return m_resourceManager; }	// リソースの管理者の取得
	shared_ptr<SharedData> getSharedData() const { return m_sharedData; }		// 共有データの取得
	shared_ptr<EventManager> getEventmanager() const { return m_eventManager; }	// イベントの管理者の取得
	FlagManager<SCENE_FLAGS>& getFlags() { return m_flags; }					// フラグの取得処理
	std::list<shared_ptr<GameObject>>& getObjects(const uint32_t& inPriority) { return m_objects[inPriority]; }	// オブジェクトリストの取得
	shared_ptr<Camera> getCamera() { return m_objects[static_cast<uint32_t>(BASE_PRIORITY::CAMERA)].front()->downcast<Camera>(); }	// カメラの取得
	shared_ptr<Light3D> getLight() { return m_objects[static_cast<uint32_t>(BASE_PRIORITY::LIGHT)].front()->downcast<Light3D>(); }	// ライトの取得
	shared_ptr<PhysicsWorld> getWorld() { return m_objects[static_cast<uint32_t>(BASE_PRIORITY::WORLD)].front()->downcast<PhysicsWorld>(); }	// 世界の取得
	shared_ptr<Player> getPlayer() { return m_objects[static_cast<uint32_t>(BASE_PRIORITY::PLAYER)].front()->downcast<Player>(); }	// プレイヤーの取得
	const float& getDeltaTime() const { return m_time->getDeltaTime(); }		// デルタタイムの取得
	std::list<weak_ptr<BlockPush>>& getPushBlocks() { return m_pushBlocks; }	// 押せるブロックのリストの取得
	auto& getFlameAndUpdraftPairs() { return m_flameAndUpdraftPairs; }			// 炎と上昇気流のペアの取得
	auto& getUpdraftEffectEmitters() { return m_updraftEffectEmitters; }		// 上昇気流のエフェクトの生成器たちの取得
	auto& getIceBlocks() { return m_iceBlocks; }								// 氷ブロックたちの取得
	auto& getFreezeBlocks() { return m_freezeBlocks; }							// 氷結した水雫ブロックたちの取得
	auto& getFreezeEmitters() { return m_freezeEmitters; }						// 最初は氷結してる水雫ブロック生成器たちの取得
	auto& getRotateBlocks() { return m_rotateBlocks; }							// 回転するブロックたち
	auto& getMoveBlocks() { return m_moveBlocks; }								// 移動するブロックたち
	auto& getWaterWheels() { return m_waterWheels; }							// 水車たち
	std::vector<weak_ptr<Impact>>& getImpacts() { return m_impacts; }			// 衝撃たち
	shared_ptr<GoalEffect> getGoalEffect() { return m_goalEffect.lock(); }		// ゴールエフェクト

protected:
	//---------------------------------------------------
	// protected メンバ関数宣言
	//---------------------------------------------------
	std::vector<std::list<shared_ptr<GameObject>>>& getObjects() { return m_objects; }	// オブジェクトリストの取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneBase(const SceneBase&) = delete;
	void operator=(const SceneBase&) = delete;

	void releaseObjects();		// オブジェクトの解放
	void updateObjects();		// オブジェクトの更新
	void drawObjects() const;	// オブジェクトの描画

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	shared_ptr<TimeManager> m_time;					// 時間
	shared_ptr<ResourceManager> m_resourceManager;	// リソースの管理者
	shared_ptr<SharedData> m_sharedData;			// 共有データ
	shared_ptr<EventManager> m_eventManager;		// イベントの管理者
	FlagManager<SCENE_FLAGS> m_flags;				// シーンのフラグ

	// TODO: 各シーンで PRIORITY を用意し、初期設定で resize すること
	std::vector<std::list<shared_ptr<GameObject>>> m_objects;	// オブジェクトのリスト

	std::list<weak_ptr<BlockPush>> m_pushBlocks;	// 押せるブロックのリスト
	std::unordered_map<std::string, std::pair<weak_ptr<Flame>, weak_ptr<Updraft>>> m_flameAndUpdraftPairs;	// 炎と上昇気流のペア
	std::unordered_map<std::string, std::vector<weak_ptr<UpdraftEffectEmitter>>> m_updraftEffectEmitters;	// 上昇気流のエフェクトの生成器たち
	std::unordered_map<std::string, std::vector<weak_ptr<BlockIce>>> m_iceBlocks;							// 氷ブロックたち
	std::unordered_map<std::string, std::vector<weak_ptr<BlockFreezeWaterDrop>>> m_freezeBlocks;			// 氷結した水雫ブロックたち
	std::unordered_map<std::string, std::vector<weak_ptr<BlockFreezeWaterDropEmitter>>> m_freezeEmitters;	// 最初は氷結してる水雫ブロック生成器たち
	std::unordered_map<std::string, std::vector<weak_ptr<BlockRotate>>> m_rotateBlocks;						// 回転するブロックたち
	std::unordered_map<std::string, std::vector<weak_ptr<BlockMove>>> m_moveBlocks;							// 移動するブロックたち
	std::unordered_map<std::string, std::vector<weak_ptr<WaterWheel>>> m_waterWheels;						// 水車たち
	std::vector<weak_ptr<Impact>> m_impacts;																// 衝撃たち
	std::weak_ptr<GoalEffect> m_goalEffect;																	// ゴールエフェクト
};

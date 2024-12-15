//--------------------------------------------------------------------------------
// 
// ゲームオブジェクトのファクトリー [game_object_factory.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"
#include "game_object/snow_emitter_2d.h"
#include "game_object/spring_emitter_2d.h"
#include "game_object/impact.h"
// // 3d
#include "game_object/3d/model.h"
#include "game_object/3d/spawn_point.h"
#include "game_object/3d/event_point.h"
#include "game_object/3d/flame.h"
#include "game_object/3d/updraft.h"
#include "game_object/3d/updraft_effect_emitter.h"
// // // block
#include "game_object/3d/block/block_base.h"
#include "game_object/3d/block/block_move.h"
#include "game_object/3d/block/block_rotate.h"
#include "game_object/3d/block/block_fall.h"
#include "game_object/3d/block/block_disappear.h"
#include "game_object/3d/block/block_dummy.h"
#include "game_object/3d/block/block_goal.h"
#include "game_object/3d/block/blocks_object_base.h"
#include "game_object/3d/block/block_blocks.h"
#include "game_object/3d/block/block_physics.h"
#include "game_object/3d/block/block_wood.h"
#include "game_object/3d/block/block_water.h"
#include "game_object/3d/block/block_water_drop.h"
#include "game_object/3d/block/block_water_drop_emitter.h"
#include "game_object/3d/block/block_shadow.h"
#include "game_object/3d/block/block_push.h"
#include "game_object/3d/block/block_push_on_snow.h"
#include "game_object/3d/block/block_repair.h"
#include "game_object/3d/block/block_blazing.h"
#include "game_object/3d/block/block_blazing_emitter.h"
#include "game_object/3d/block/block_burnt.h"
#include "game_object/3d/block/block_ice.h"
#include "game_object/3d/block/block_freeze_water_drop.h"
#include "game_object/3d/block/block_change_state.h"
#include "game_object/3d/block/block_impact.h"
#include "game_object/3d/block/block_smoke.h"
#include "game_object/3d/block/block_small_pieces.h"
#include "game_object/3d/block/block_sound.h"
// // // item
#include "game_object/3d/item/coin.h"
// // // character
#include "game_object/3d/character/player.h"
// // 2d
#include "game_object/2d/game_object_2d.h"
#include "game_object/2d/snow_2d.h"
#include "game_object/2d/goal_effect.h"
// scene
// // elements
#include "scene/elements/camera.h"
#include "scene/elements/light.h"
#include "scene/elements/physics_world.h"

//===================================================
// ゲームオブジェクトのファクトリー (Factory Method)
//===================================================
class GameObjectFactory
{
public:
	//---------------------------------------------------
	// エイリアス宣言
	//---------------------------------------------------
	using CreateFunc = std::function<std::shared_ptr<GameObject>(const shared_ptr<SceneBase>&, const json&)>;

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	static void registerFactory(const std::string& inType, CreateFunc inFunc);	// 登録
	static std::shared_ptr<GameObject> create(const std::string& inType, const shared_ptr<SceneBase>& inScene, const json& inData);	// 生成

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	GameObjectFactory() = delete;
	~GameObjectFactory() = delete;

	static std::unordered_map<std::string, CreateFunc>& getFactories();	// ファクトリーメソッドの取得
};

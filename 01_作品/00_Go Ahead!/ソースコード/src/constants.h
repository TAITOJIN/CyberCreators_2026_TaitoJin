//--------------------------------------------------------------------------------
// 
// 定数 [constants.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// 初期値 (initial value => default value)
//---------------------------------------------------
const D3DXVECTOR2 DEF_VEC2 = D3DXVECTOR2(0.0f, 0.0f);
const D3DXVECTOR3 DEF_VEC3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
const D3DXQUATERNION DEF_ROT = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR DEF_COL = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXMATRIX DEF_MTX = D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
const D3DXVECTOR3 DEF_SCALE = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
const btTransform DEF_TRANSFORM = btTransform(btQuaternion(DEF_ROT.x, DEF_ROT.y, DEF_ROT.z, DEF_ROT.w), btVector3(DEF_VEC3.x, DEF_VEC3.y, DEF_VEC3.z));
const D3DMATERIAL9 DEF_MAT = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, 5.0f };

//---------------------------------------------------
// 軸
//---------------------------------------------------
const D3DXVECTOR3 AXIS_X = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
const D3DXVECTOR3 AXIS_Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
const D3DXVECTOR3 AXIS_Z = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
const btVector3 BT_AXIS_X = btVector3(1, 0, 0);
const btVector3 BT_AXIS_Y = btVector3(0, 1, 0);
const btVector3 BT_AXIS_Z = btVector3(0, 0, 1);

//---------------------------------------------------
// フラグ
//---------------------------------------------------

// object_3d
const std::string FLAG_POS = "POS";
const std::string FLAG_ROT = "ROT";
const std::string FLAG_SCALE = "SCALE";
const std::string FLAG_OFFSET = "OFFSET";
const std::string FLAG_KEY = "KEY";
const std::string FLAG_IS_STATIC = "IS_STATIC";
const std::string FLAG_MASS = "MASS";
const std::string FLAG_INERTIA = "INERTIA";

// model
const std::string FLAG_MAT = "MAT";
const std::string FLAG_MAT_NAME = "MAT_NAME";
const std::string FLAG_MAT_KEY = "MAT_KEY";

// character
const std::string FLAG_SPEED = "SPEED";
const std::string FLAG_ATN_SPEED = "ATN_SPEED";
const std::string FLAG_JUMP_STRENGTH = "JUMP_STRENGTH";
const std::string FLAG_GRAVITY = "GRAVITY";
const std::string FLAG_MAX_SLOPE_DEGREE = "MAX_SLOPE_DEGREE";
const std::string FLAG_STEP_HEIGHT = "STEP_HEIGHT";
const std::string FLAG_FIRST_SPEED_JUMP = "FIRST_SPEED_JUMP";
const std::string FLAG_FALL_SPEED = "FALL_SPEED";

// block
// // move
const std::string FLAG_POINT1 = "POINT1";
const std::string FLAG_POINT2 = "POINT2";
const std::string FLAG_TARGET_POS = "TARGET_POS";
// // rotate
const std::string FLAG_AXIS = "AXIS";
const std::string FLAG_SPEED_ROT = "SPEED_ROT";
// //　disappear
const std::string FLAG_FIRST_APPEAR = "FIRST_APPEAR";
// // dummy
const std::string FLAG_DUMMY_FLAGS = "DUMMY_FLAGS";

// texture
const std::string FLAG_TEX_FILENAME = "FILENAME";
const std::string FLAG_TEX_SUBDIVISION_COUNT_U = "SUBDIVISION_COUNT_U";
const std::string FLAG_TEX_SUBDIVISION_COUNT_V = "SUBDIVISION_COUNT_V";
const std::string FLAG_TEX_ANIMATION_INTERVAL = "ANIMATION_INTERVAL";

// model
const std::string FLAG_MODEL_FILENAME = "FILENAME";

// color
#if 1
const std::string MAT_COL_BLACK = "BLACK";
const std::string MAT_COL_GREEN = "GREEN";
const std::string MAT_COL_PINK = "PINK";
const std::string MAT_COL_YELLOW = "YELLOW";
const std::string MAT_COL_ORANGE = "ORANGE";
const std::string MAT_COL_LIGHT_BLUE = "LIGHT_BLUE";
const std::string MAT_COL_BLUE = "BLUE";
const std::string MAT_COL_TRANSLUCENT_RED = "TRANSLUCENT_RED";
const std::string MAT_COL_TRANSLUCENT_BLUE = "TRANSLUCENT_BLUE";
#endif

// ini
const std::string FLAG_INI_SYSTEM = "system";
const std::string INI_SYSTEM_TITLE = "class SceneTitle";
const std::string INI_SYSTEM_GAME = "class SceneGame";
const std::string INI_SYSTEM_RESULT = "class SceneResult";
const std::string INI_SYSTEM_DEBUG = "class SceneDebug";

const std::string FLAG_INI_DEBUG_SCENE = "debug_scene";
const std::string FLAG_INI_IS_FIRST_SCENE = "is_first_scene";

const std::string FLAG_INI_SAVE = "save";
const std::string FLAG_INI_LOAD = "load";
const std::string FLAG_INI_FILENAME = "filename";

const std::string FLAG_INI_STAGE = "stage";

// list
const std::string FLAG_LIST = "list";
const std::string LIST_TEX_FILENAME = "texture";
const std::string LIST_MODEL_FILENAME = "model";
const std::string LIST_MAT_FILENAME = "material";

//---------------------------------------------------
// その他
//---------------------------------------------------
const std::string CLASS_NAME = "WindowClass";												// ウィンドウクラスの名前
const std::string WINDOW_NAME = "Go Ahead!";												// ウィンドウの名前
const std::string RESOURCE_PATH = "res";													// リソースファイルのフォルダ
constexpr DWORD FVF_VERTEX_2D = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;				// 頂点フォーマット [2D]
constexpr DWORD FVF_VERTEX_3D = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// 頂点フォーマット [3D]
constexpr UINT TRISTRIP_VTX_NUM = 4U;														// 頂点の数 [TRIANGLESTRIP]
constexpr UINT TRISTRIP_POLYGON_NUM = 2U;													// ポリゴンの数 [TRIANGLESTRIP]
#if 1
constexpr UINT SCREEN_WIDTH = 1280U;														// スクリーンのサイズ (幅)
constexpr UINT SCREEN_HEIGHT = 720U;														// スクリーンのサイズ (高さ)
#else
constexpr UINT SCREEN_WIDTH = 1920U;														// スクリーンのサイズ (幅)
constexpr UINT SCREEN_HEIGHT = 1080U;														// スクリーンのサイズ (高さ)
#endif
constexpr int NONE = -1;																	// 何もなし
constexpr int BUTTON_PRESSED_MASK = 0x80;													// マウスボタンが押されていることを示すフラグ
constexpr float HALF = 0.5f;																// 半分 (/2 => *0.5f)
constexpr int MAX_WORD = 256;																// 最大文字数
constexpr int NUM_OF_BITS_IN_ONE_BYTE = 8;													// 1 バイトのビット数
constexpr float MIN_WORLD_Y = -500.0f;														// 世界の最低高度
constexpr const char* SCHEMA = "$schema";													// スキーマ
constexpr int ONLY_ONE = 1;																	// 唯一
constexpr float IMPACT_STRENGTH = 100.0f;													// 衝撃の強さ
#if 0
constexpr D3DCOLOR BG_COL = D3DCOLOR_RGBA(124, 125, 255, 255);				// 背景の色
#else
constexpr D3DCOLOR BG_COL = D3DCOLOR_RGBA(49, 161, 255, 255);				// 背景の色
//constexpr D3DCOLOR BG_COL = D3DCOLOR_RGBA(89, 179, 255, 255);				// 背景の色
#endif
constexpr D3DCOLOR BG_SUNSET_COL = D3DCOLOR_RGBA(255, 160, 0, 255);			// 背景の色 (夕焼け)
constexpr D3DCOLOR BG_EVENING_COL = D3DCOLOR_RGBA(124, 125, 255, 255);		// 背景の色 (夕方)

//---------------------------------------------------
// イベント名
//---------------------------------------------------
const std::string EVENTNAME_NEXT_SCENE_EVENT = "NEXT_SCENE_EVENT_";				// 次のシーンのイベント名
const std::string EVENTNAME_POP_NOW_LOADING = "POP_NOW_LOADING";				// Now Loading の終了
const std::string EVENTNAME_CAMERA_DISTANCE_600 = "CAMERA_DISTANCE_600";		// カメラの距離の設定 (600.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_500 = "CAMERA_DISTANCE_500";		// カメラの距離の設定 (500.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_550 = "CAMERA_DISTANCE_550";		// カメラの距離の設定 (550.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_400 = "CAMERA_DISTANCE_400";		// カメラの距離の設定 (400.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_450 = "CAMERA_DISTANCE_450";		// カメラの距離の設定 (450.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_300 = "CAMERA_DISTANCE_300";		// カメラの距離の設定 (300.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_350 = "CAMERA_DISTANCE_350";		// カメラの距離の設定 (350.0f)
const std::string EVENTNAME_CAMERA_DISTANCE_DEF = "CAMERA_DISTANCE_DEF";		// カメラの距離の設定 (デフォルト)
const std::string EVENTNAME_CAMERA_DISTANCE_SHORT = "CAMERA_DISTANCE_SHORT";	// カメラの距離の設定 (短い)
const std::string EVENTNAME_PLAY_BGM_000 = "PLAY_BGM_000";						// 音楽の再生
const std::string EVENTNAME_STOP_BGM_000 = "STOP_BGM_000";						// 音楽の停止
const std::string EVENTNAME_PLAY_BGM_001 = "PLAY_BGM_001";						// 音楽の再生
const std::string EVENTNAME_STOP_BGM_001 = "STOP_BGM_001";						// 音楽の停止
const std::string EVENTNAME_START_SNOW = "START_SNOW";							// 降雪開始
const std::string EVENTNAME_START_SPRING = "START_SPRING";						// 桜開始
const std::string EVENTNAME_END_SNOW = "END_SNOW";								// 降雪終了
const std::string EVENTNAME_END_SPRING = "END_SPRING";							// 桜終了
const std::string EVENTNAME_CHANGE_BGM_000_TO_001 = "000_TO_001";				// BGM を 000 から 001 に変更
const std::string EVENTNAME_CHANGE_BGM_001_TO_000 = "001_TO_000";				// BGM を 001 から 000 に変更
const std::string EVENTNAME_STAGE_05_TO_06 = "STAGE_05_TO_06";					// ステージ 05 から 06
const std::string EVENTNAME_STAGE_06_TO_05 = "STAGE_06_TO_05";					// ステージ 06 から 05
const std::string EVENTNAME_FIND_COIN_ZONE = "FIND_COIN_ZONE";					// コインのゾーンを見つけた否か
const std::string EVENTNAME_STAGE_H2_TO_007 = "H2_TO_007";						// 隠しルート 2 から 7 ステージへ
const std::string EVENTNAME_STAGE_007_TO_H2 = "007_TO_H2";						// 7 ステージから隠しルート 2 へ

//---------------------------------------------------
// パラメータの設定
//---------------------------------------------------
// GameObject
constexpr const char* GAMEOBJECT_PARAM_PRIORITY = "PRIORITY";
constexpr const char* GAMEOBJECT_PARAM_TAG = "TAG";
// カメラ
constexpr const char* CAMERA_PARAM_POSV = "POSV";
constexpr const char* CAMERA_PARAM_POSR = "POSR";
constexpr const char* CAMERA_PARAM_RIGHT_STICK_ROT_MOVE = "RIGHT_STICK_ROT_MOVE";					// 右スティックの回転量
constexpr const char* CAMERA_PARAM_FIRST_TARGET_IS_PLAYER = "FIRST_TARGET_IS_PLAYER";				// 最初のターゲットがプレイヤーか否か
constexpr const char* CAMERA_PARAM_DEST_DISTANCE = "DEST_DISTANCE";									// 目的の距離
constexpr const char* CAMERA_PARAM_FIRSTSTARTTARGETPLAYERTIME = "FIRST_START_TARGET_PLAYER_TIME";	// プレイヤーを追尾開始するまでの時間
// ライト
constexpr const char* LIGHT_PARAM_DIR = "DIR";
constexpr const char* LIGHT_PARAM_DIFFUSE = "DIFFUSE";
constexpr const char* LIGHT_PARAM_IDX = "IDX";
// Object2D
constexpr const char* OBJECT2D_PARAM_POS = "POS";
constexpr const char* OBJECT2D_PARAM_ROT = "ROT";
constexpr const char* OBJECT2D_PARAM_SIZE = "SIZE";
constexpr const char* OBJECT2D_PARAM_COL = "COL";
constexpr const char* OBJECT2D_PARAM_TEXKEY = "TEXKEY";
constexpr const char* OBJECT2D_PARAM_ISTEXANIM = "ISTEXANIM";
// BlocksObject
constexpr const char* BLOCKSOBJECT_PARAM_FILENAME = "FILENAME";
// polygon_3d
constexpr const char* POLYGON3D_PARAM_SIZE = "SIZE";
constexpr const char* POLYGON3D_PARAM_COLOR = "COLOR";
constexpr const char* POLYGON3D_PARAM_CENTER = "CENTER";
// respawn_point
constexpr const char* RESPAWNPOINT_PARAM_RESPAWNPOINT = "RESPAWN_POINT";
// block_water_drop_emitter
constexpr const char* WATERDROP_EMITTER_PARAM_SETINTERVAL = "SET_INTERVAL";
constexpr const char* WATERDROP_EMITTER_PARAM_POS = "DROP_POS";
constexpr const char* WATERDROP_EMITTER_PARAM_SCALE = "DROP_SCALE";
constexpr const char* WATERDROP_EMITTER_PARAM_MS_MIN_LIFE = "MS_MIN_LIFE";
constexpr const char* WATERDROP_EMITTER_PARAM_MS_MAX_LIFE = "MS_MAX_LIFE";
constexpr const char* WATERDROP_EMITTER_PARAM_GRAVITY = "DROP_GRAVITY";
// block_water_drop
constexpr const char* WATERDROP_PARAM_FIRST_SCALE = "FIRST_SCALE";
constexpr const char* WATERDROP_PARAM_MAX_LIFE = "MAX_LIFE";
constexpr const char* WATERDROP_PARAM_GRAVITY = "DROP_GRAVITY";
// block_repair
constexpr const char* WATERDROP_PARAM_COLLAPS_ETIME = "COLLAPSE_TIME";
// flame, updraft
constexpr const char* FLAMEANDUPDRAFT_PARAM_PAIR_KEY = "PAIR_KEY";
// block_burnt
constexpr const char* BURNT_BLOCK_PARAM_START_SMALL_SEC = "START_SMALL_SEC";
// block_ice
constexpr const char* ICE_BLOCK_PARAM_WATER_MAT_KEY = "WATER_MAT_KEY";
constexpr const char* ICE_BLOCK_PARAM_ICE_MAT_KEY = "ICE_MAT_KEY";
// enaled
constexpr const char* PARAM_IS_ENABLED = "IS_ENABLED";
// event_point
constexpr const char* PARAM_EVENT_NAME = "EVENT_NAME";
// snow_2d
constexpr const char* PARAM_SNOW_MOVE = "SNOW_PARAM_MOVE";
// snow_emitter_2d
constexpr const char* PARAM_SNOW_GENERATOR_INTERVAL_MIN = "INTERVAL_MIN";
constexpr const char* PARAM_SNOW_GENERATOR_INTERVAL_MAX = "INTERVAL_MAX";
constexpr const char* PARAM_SNOW_GENERATOR_NUM_MIN = "NUM_MIN";
constexpr const char* PARAM_SNOW_GENERATOR_NUM_MAX = "NUM_MAX";
// first respaen point (player)
constexpr const char* PARAM_PLAYER_FIRST_RESPAWN_POINT = "FIRST_RESPAWN_POINT";
// block_small_pieces
constexpr const char* PARAM_BLOCK_SMALL_PIECES = "MOVE";
// goal_effect
constexpr const char* PARAM_MESH_VTX_NUM_X = "VTX_NUM_X";
constexpr const char* PARAM_MESH_VTX_NUM_Y = "VTX_NUM_Y";
// sound_source
constexpr const char* PARAM_SOUND_SOURCE_LABEL = "LABEL";

//---------------------------------------------------
// factory の種類
//---------------------------------------------------
constexpr const char* FACTORY_MODEL = "Model";													// モデル
constexpr const char* FACTORY_PLAYER = "Player";												// プレイヤー
constexpr const char* FACTORY_NORMAL_BLOCK = "NormalBlock";										// 通常ブロック
constexpr const char* FACTORY_DUMMY_BLOCK = "DummyBlock";										// ダミーブロック
constexpr const char* FACTORY_DISAPPEAR_BLOCK = "DisappearBlock";								// 消えるブロック
constexpr const char* FACTORY_FALL_BLOCK = "FallBlock";											// 落ちるブロック
constexpr const char* FACTORY_GOAL_BLOCK = "GoalBlock";											// ゴールブロック
constexpr const char* FACTORY_MOVE_BLOCK = "MoveBlock";											// 動くブロック
constexpr const char* FACTORY_ROTATE_BLOCK = "RotateBlock";										// 回転するブロック
constexpr const char* FACTORY_CAMERA = "Camera";												// カメラ
constexpr const char* FACTORY_LIGHT = "Light";													// ライト
constexpr const char* FACTORY_WORLD = "World";													// 世界
constexpr const char* FACTORY_OBJECT2D = "Object2D";											// 2D オブジェクト
constexpr const char* FACTORY_BLOCKSOBJECT = "BlocksObject";									// 複数のブロックによるオブジェクト
constexpr const char* FACTORY_BLOCKSBLOCK = "BlocksBlock";										// 複数のブロックによるオブジェクトの一部
constexpr const char* FACTORY_COIN = "Coin";													// コイン
constexpr const char* FACTORY_PHYSICS_BLOCK = "PhysicsBlock";									// 物理挙動が特にあるブロック
constexpr const char* FACTORY_WOOD_BLOCK = "WoodBlock";											// 木ブロック
constexpr const char* FACTORY_WATER_BLOCK = "WaterBlock";										// 水ブロック
constexpr const char* FACTORY_3DPOLYGON_PLANE = "Plane3D";										// 3D ポリゴン (床)
constexpr const char* FACTORY_SPAWN_POINT = "SPAWN_POINT";										// スポーン地点
constexpr const char* FACTORY_3DPOLYGON_WALL = "Wall3D";										// 3D ポリゴン (壁)
constexpr const char* FACTORY_WATERDROP_BLOCK = "WaterDrop";									// 水雫ブロック
constexpr const char* FACTORY_WATERDROP_EMITTER = "WaterDropEmitter";							// 水雫ブロックを生み出す
constexpr const char* FACTORY_BLAZING_BLOCK = "Blazing";										// めらめらブロック
constexpr const char* FACTORY_BLAZING_EMITTER = "BlazingEmitter";								// めらめらを生み出す
constexpr const char* FACTORY_SHADOW_BLOCK = "ShadowBlock";										// 影ブロック
constexpr const char* FACTORY_PUSH_BLOCK = "PushBlock";											// 押せるブロック
constexpr const char* FACTORY_REPAIR_BLOCK = "RepairBlock";										// 修復されるブロック
constexpr const char* FACTORY_FLAME = "Flame";													// 炎
constexpr const char* FACTORY_UPDRAFT = "Updraft";												// 上昇気流
constexpr const char* FACTORY_UPDRAFT_EFFECT_EMITTER = "UpdraftEffectEmitter";					// 上昇気流のエフェクトの生成器
constexpr const char* FACTORY_BURNT_BLOCK = "BurntBlock";										// 焦げブロック
constexpr const char* FACTORY_ICE_BLOCK = "IceBlock";											// 氷ブロック
constexpr const char* FACTORY_FREEZE_WATER_DROP_BLOCK = "FreezeWaterDropBlock";					// 氷結した水雫ブロック
constexpr const char* FACTORY_FREEZE_WATER_DROP_BLOCK_EMITTER = "FreezeWaterDropBlockEmitter";	// 最初はフリーズしている水雫ブロックを生み出す
constexpr const char* FACTORY_EVENT_POINT = "EventPoint";										// イベントポイント
constexpr const char* FACTORY_WATER_WHEEL = "WaterWheel";										// 水車
constexpr const char* FACTORY_SNOW_EMITTER_2D = "SnowEmitter2D";								// 雪を生み出す
constexpr const char* FACTORY_SNOW_2D = "Snow2D";												// 雪 (2d)
constexpr const char* FACTORY_PUSH_ON_SNOW = "PushOnSnow";										// 雪の上にある押せるブロック
constexpr const char* FACTORY_IMPACT = "Impact";												// 衝撃
constexpr const char* FACTORY_IMPACT_BLOCK = "ImpactBlock";										// 衝撃を発生させるブロック
constexpr const char* FACTORY_CHANGE_STATE_BLOCK = "ChangeStateBlock";							// 状態変化するブロック
constexpr const char* FACTORY_IMPACT_SMOKE = "Smoke";											// 爆発の煙ブロック
constexpr const char* FACTORY_SMALL_PIECES = "SmallPieces";										// 粉々なブロック
constexpr const char* FACTORY_SPRING_EMITTER_2D = "SpringEmitter2D";							// 桜を生み出す
constexpr const char* FACTORY_GOAL_EFFECT = "GoalEffect";										// ゴールエフェクト
constexpr const char* FACTORY_SOUND_SOURCE = "SoundSource";										// 音源

//---------------------------------------------------
// マクロ定義
//---------------------------------------------------
#define ENABLED_SHADOW_MAP	(0)	// シャドウマップを使用する
#define USE_NEW_SHARED_DATA	(0)	// 新しく作成した共有データを使用する

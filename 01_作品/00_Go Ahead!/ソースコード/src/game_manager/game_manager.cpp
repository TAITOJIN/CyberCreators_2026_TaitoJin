//--------------------------------------------------------------------------------
// 
// ゲームのマネージャー (管理者) [game_manager.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_manager
#include "game_manager.h"
// game_object
#include "game_object/game_object_factory.h"
// scene
#include "scene/scene_title.h"
#include "scene/scene_game.h"
#include "scene/scene_result.h"
#include "scene/scene_debug.h"
// resource
#include "resource/sound.h"
#include "resource/font.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr auto FILENAME_SYSETM_INI = "res\\system.ini";	// システム INI ファイル名

} // namespace /* anonymous */

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT GameManager::init(const HINSTANCE& inInstanceHandle, const HWND& inWindowHandle, const BOOL& isWindowed)
{
	m_instanceHandle = inInstanceHandle;
	m_windowHandle = inWindowHandle;

	// ランダムの種を設定する
	srand(static_cast<UINT>(time(nullptr)));

	// ゲームオブジェクトの登録
	registerFactoryMethod();

	// クリティカルセクションの生成
	m_cs = std::make_unique<CriticalSection>();
	if (!m_cs)
	{
		return E_FAIL;
	}

	// サウンドの管理者の初期設定
	HRESULT hr = CSoundManager::GetInstance().Init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 入力処理の管理者の生成
	m_inputManager = std::make_unique<InputManager>();
	if (!m_inputManager)
	{
		return E_FAIL;
	}

	// 入力処理の管理者の初期設定
	hr = m_inputManager->init(m_instanceHandle, m_windowHandle);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// レンダラーの生成
	m_renderer = std::make_unique<Renderer>();
	if (!m_renderer)
	{
		return E_FAIL;
	}

	// レンダラーの初期設定
	hr = m_renderer->init(m_windowHandle, isWindowed);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// フォントの初期設定
	hr = FontManager::getInstance().init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// シーンの管理者の生成
	m_sceneManager = std::make_unique<SceneManager>();
	if (!m_sceneManager)
	{
		return E_FAIL;
	}

	// シーンの管理者の初期設定
	hr = m_sceneManager->init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// シーンの設定
	json config;
	{
		std::ifstream ifs("res\\debug_config.json");
		if (ifs)
		{
			ifs >> config;
		}
	}
	bool isDebug = config["IsDebug"];

	if (isDebug)
	{
		m_sceneManager->changeScene<SceneDebug>();
	}
	else
	{
		m_sceneManager->changeScene<SceneTitle>();
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void GameManager::uninit()
{
	// シーンの管理者の終了処理
	if (m_sceneManager)
	{
		m_sceneManager->uninit();
	}

	// フォントの終了処理
	FontManager::getInstance().uninit();

	// レンダラーの終了処理
	if (m_renderer)
	{
		m_renderer->uninit();
	}

	// 入力処理の管理者の終了処理
	if (m_inputManager)
	{
		m_inputManager->uninit();
	}

	// サウンドの管理者の終了処理
	CSoundManager::GetInstance().Uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void GameManager::update()
{
	// 入力処理の管理者の更新
	m_inputManager->update();

	// レンダラーの更新
	m_renderer->update();

	// シーンの管理者の更新
	m_sceneManager->update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void GameManager::draw() const
{
	m_cs->lock();

	// レンダラーの描画
	m_renderer->draw(
		[this]()
		{
			// シーンの管理者の描画
			this->m_sceneManager->draw();
		},
		[this]()
		{
			this->m_sceneManager->drawWithShadow();
		}
	);

	m_cs->unlock();
}

//---------------------------------------------------
// クリティカルセクション
//---------------------------------------------------
void GameManager::cs(const std::function<void()>& inFunc)
{
	m_cs->lock();

	inFunc();

	m_cs->unlock();
}

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
GameManager::GameManager()
	: m_windowHandle(nullptr)
	, m_instanceHandle(nullptr)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ゲームオブジェクトの生成メソッドの登録
//---------------------------------------------------
void GameManager::registerFactoryMethod()
{
	// TODO: シーンごとに登録 (シーンが各々)



	// モデル
	GameObjectFactory::registerFactory(FACTORY_MODEL, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto model = make_shared<Model>();	// 生成
		model->setScene(inScene);			// シーンの設定
		model->init(inData);				// 初期設定
		model->createBlockRigidBody();		// ブロック形状の剛体の作成
		return model;
	});

	// プレイヤー
	GameObjectFactory::registerFactory(FACTORY_PLAYER, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto player = make_shared<Player>();	// 生成
		player->setScene(inScene);				// シーンの設定
		player->init(inData);					// 初期設定
		player->createCapsuleCharacter();		// カプセル形状のキャラクターの作成
		return player;
	});

	// 通常ブロック
	GameObjectFactory::registerFactory(FACTORY_NORMAL_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockBase>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		return block;
	});

	// ダミーブロック
	GameObjectFactory::registerFactory(FACTORY_DUMMY_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockDummy>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		block->setForEachDummyFlag();			// ダミーフラグごとの処理
		return block;
	});

	// 消えるブロック
	GameObjectFactory::registerFactory(FACTORY_DISAPPEAR_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockDisappear>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		return block;
	});

	// 落ちるブロック
	GameObjectFactory::registerFactory(FACTORY_FALL_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockFall>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		return block;
	});

	// ゴールブロック
	GameObjectFactory::registerFactory(FACTORY_GOAL_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockGoal>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		block->setForEachDummyFlag();			// ダミーフラグごとの処理
		return block;
	});

	// 動くブロック
	GameObjectFactory::registerFactory(FACTORY_MOVE_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockMove>();							// 生成
		block->setScene(inScene);										// シーンの設定
		block->init(inData);											// 初期設定
		block->createBlockRigidBody();									// ブロック形状の剛体の作成
		block->getRigitBody().setGravity(btVector3(0.0f, 0.0f, 0.0f));	// 重力をなくす (MEMO: 動くので dynamic だけど、重力に影響してしまうので)

		// ペアに格納
		inScene->getMoveBlocks()[block->getPairKey()].push_back(block);
		return block;
	});

	// 回転するブロック
	GameObjectFactory::registerFactory(FACTORY_ROTATE_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockRotate>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成

		// ペアに格納
		inScene->getRotateBlocks()[block->getPairKey()].push_back(block);
		return block;
	});

	// カメラ
	GameObjectFactory::registerFactory(FACTORY_CAMERA, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto camera = make_shared<Camera>();	// 生成
		camera->setScene(inScene);				// シーンの設定
		camera->init(inData);					// 初期設定
		return camera;
	});

	// ライト
	GameObjectFactory::registerFactory(FACTORY_LIGHT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto light = make_shared<Light3D>();	// 生成
		light->setScene(inScene);				// シーンの設定
		light->init(inData);					// 初期設定
		return light;
	});

	// 世界
	GameObjectFactory::registerFactory(FACTORY_WORLD, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto world = make_shared<PhysicsWorld>();	// 生成
		world->setScene(inScene);					// シーンの設定
		world->init(inData);						// 初期設定
		return world;
	});

	// 2D オブジェクト
	GameObjectFactory::registerFactory(FACTORY_OBJECT2D, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto object2d = make_shared<GameObject2D>();	// 生成
		object2d->setScene(inScene);					// シーンの設定
		object2d->init(inData);							// 初期設定
		object2d->initVtx(0, 0);						// 頂点の初期設定
		return object2d;
	});

	// 複数のブロックによるオブジェクト
	GameObjectFactory::registerFactory(FACTORY_BLOCKSOBJECT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto blocksObject = make_shared<BlocksObjectBase>();	// 生成
		blocksObject->setScene(inScene);						// シーンの設定
		blocksObject->init(inData);								// 初期設定
		return blocksObject;
	});

	// 複数のブロックによるオブジェクトに使用されるブロック
	GameObjectFactory::registerFactory(FACTORY_BLOCKSBLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockBlocks>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		block->setForEachDummyFlag();				// ダミーフラグごとの処理
		return block;
	});

	// コイン
	GameObjectFactory::registerFactory(FACTORY_COIN, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto coin = make_shared<Coin>();	// 生成
		coin->setScene(inScene);			// シーンの設定
		coin->init(inData);					// 初期設定
		coin->createBlockRigidBody();		// ブロック形状の剛体の作成
		coin->getRigitBody().setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);	// 当たり判定を無効化
		return coin;
	});

	// 物理挙動が特にあるブロック
	GameObjectFactory::registerFactory(FACTORY_PHYSICS_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockPhysics>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		return block;
	});

	// 木ブロック
	GameObjectFactory::registerFactory(FACTORY_WOOD_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockWood>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		return block;
	});

	// 押せるブロック
	GameObjectFactory::registerFactory(FACTORY_PUSH_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockPush>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成

		// リストに追加
		inScene->getPushBlocks().push_back(block);
		return block;
	});

	// 雪の上にある押せるブロック
	GameObjectFactory::registerFactory(FACTORY_PUSH_ON_SNOW, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockPushOnSnow>();	// 生成
		block->setScene(inScene);						// シーンの設定
		block->init(inData);							// 初期設定
		block->createBlockRigidBody();					// ブロック形状の剛体の作成

		// リストに追加
		inScene->getPushBlocks().push_back(block);
		return block;
	});

	// 水ブロック
	GameObjectFactory::registerFactory(FACTORY_WATER_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockWater>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		block->setForEachDummyFlag();			// ダミーフラグごとの処理
		return block;
	});

	// 水雫ブロック
	GameObjectFactory::registerFactory(FACTORY_WATERDROP_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockWaterDrop>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		block->setForEachDummyFlag();				// ダミーフラグごとの処理
		return block;
	});

	// 水雫ブロックの生成器
	GameObjectFactory::registerFactory(FACTORY_WATERDROP_EMITTER, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<BlockWaterDropEmitter>();	// 生成
		emitter->setScene(inScene);								// シーンの設定
		emitter->init(inData);									// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "ORANGE",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(emitter->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + emitter->getTag());					// タグの設定
		}

		return emitter;
	});

	// 影ブロック
	GameObjectFactory::registerFactory(FACTORY_SHADOW_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockShadow>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
	#if 0
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		block->setForEachDummyFlag();				// ダミーフラグごとの処理
		#endif
	return block;
	});

	// 修復されるブロック
	GameObjectFactory::registerFactory(FACTORY_REPAIR_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockRepair>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		block->setForEachDummyFlag();				// ダミーフラグごとの処理
		return block;
	});

	// スポーン地点
	GameObjectFactory::registerFactory(FACTORY_SPAWN_POINT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto spawnPoint = make_shared<SpawnPoint>();	// 生成
		spawnPoint->setScene(inScene);					// シーンの設定
		spawnPoint->init(inData);						// 初期設定
		spawnPoint->setPlain();							// 平面の作成
		return spawnPoint;
	});

	// イベント地点
	GameObjectFactory::registerFactory(FACTORY_EVENT_POINT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto eventPoint = make_shared<EventPoint>();	// 生成
		eventPoint->setScene(inScene);					// シーンの設定
		eventPoint->init(inData);						// 初期設定
		eventPoint->setPlain();							// 平面の作成
		return eventPoint;
	});

	// めらめらブロック
	GameObjectFactory::registerFactory(FACTORY_BLAZING_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockBlazing>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		block->setForEachDummyFlag();				// ダミーフラグごとの処理
		return block;
	});

	// 炎 (めらめらの生成器)
	GameObjectFactory::registerFactory(FACTORY_BLAZING_EMITTER, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<BlockBlazingEmitter>();	// 生成
		emitter->setScene(inScene);							// シーンの設定
		emitter->init(inData);								// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WIRE_RED",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(emitter->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + emitter->getTag());					// タグの設定
		}

		return emitter;
	});

	// 炎
	GameObjectFactory::registerFactory(FACTORY_FLAME, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto flame = make_shared<Flame>();	// 生成
		flame->setScene(inScene);			// シーンの設定
		flame->init(inData);				// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WIRE_BLUE",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(flame->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + flame->getTag());						// タグの設定
		}

		// ペアに格納
		inScene->getFlameAndUpdraftPairs()[flame->getPairKey()].first = flame;

		return flame;
	});

	// 上昇気流
	GameObjectFactory::registerFactory(FACTORY_UPDRAFT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto updraft = make_shared<Updraft>();	// 生成
		updraft->setScene(inScene);				// シーンの設定
		updraft->init(inData);					// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "ORANGE",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(updraft->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + updraft->getTag());					// タグの設定
		}

		// ペアに格納
		inScene->getFlameAndUpdraftPairs()[updraft->getPairKey()].second = updraft;
		return updraft;
	});

	// 上昇気流のエフェクトの生成器
	GameObjectFactory::registerFactory(FACTORY_UPDRAFT_EFFECT_EMITTER, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<UpdraftEffectEmitter>();	// 生成
		emitter->setScene(inScene);							// シーンの設定
		emitter->init(inData);								// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WIRE_WHITE",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(emitter->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + emitter->getTag());					// タグの設定
		}

		// ペアに格納
		inScene->getUpdraftEffectEmitters()[emitter->getPairKey()].push_back(emitter);
		return emitter;
	});

	// 焦げブロック
	GameObjectFactory::registerFactory(FACTORY_BURNT_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto coin = make_shared<BlockBurnt>();	// 生成
		coin->setScene(inScene);				// シーンの設定
		coin->init(inData);						// 初期設定
		coin->createBlockRigidBody();			// ブロック形状の剛体の作成
		return coin;
	});

	// 氷ブロック
	GameObjectFactory::registerFactory(FACTORY_ICE_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockIce>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		block->setForEachDummyFlag();			// ダミーフラグごとの処理

		// ペアに格納
		inScene->getIceBlocks()[block->getPairKey()].push_back(block);
		return block;
	});

	// 氷結した水雫ブロック
	GameObjectFactory::registerFactory(FACTORY_FREEZE_WATER_DROP_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockFreezeWaterDrop>();	// 生成
		block->setScene(inScene);							// シーンの設定
		block->init(inData);								// 初期設定
		block->createBlockRigidBody();						// ブロック形状の剛体の作成
		block->setForEachDummyFlag();						// ダミーフラグごとの処理

		// ペアに格納
		inScene->getFreezeBlocks()[block->getPairKey()].push_back(block);
		return block;
	});

	// 最初はフリーズしている水雫ブロックの生成器
	GameObjectFactory::registerFactory(FACTORY_FREEZE_WATER_DROP_BLOCK_EMITTER, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<BlockFreezeWaterDropEmitter>();	// 生成
		emitter->setScene(inScene);									// シーンの設定
		emitter->init(inData);										// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "ORANGE",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(emitter->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + emitter->getTag());					// タグの設定
		}

		// ペアに格納
		inScene->getFreezeEmitters()[emitter->getPairKey()].push_back(emitter);
		return emitter;
	});

	// 水車
	GameObjectFactory::registerFactory(FACTORY_WATER_WHEEL, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto waterWheel = make_shared<WaterWheel>();	// 生成
		waterWheel->setScene(inScene);					// シーンの設定
		waterWheel->init(inData);						// 初期設定
		waterWheel->createBlockRigidBody();				// ブロック形状の剛体の作成

		// ペアに格納
		inScene->getWaterWheels()[waterWheel->getPairKey()].push_back(waterWheel);
		return waterWheel;
	});

	// 雪を生み出す
	GameObjectFactory::registerFactory(FACTORY_SNOW_EMITTER_2D, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<SnowEmitter2D>();	// 生成
		emitter->setScene(inScene);						// シーンの設定
		emitter->init(inData);							// 初期設定
		return emitter;
	});

	// 雪 2D
	GameObjectFactory::registerFactory(FACTORY_SNOW_2D, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto object2d = make_shared<Snow2D>();	// 生成
		object2d->setScene(inScene);			// シーンの設定
		object2d->init(inData);					// 初期設定
		object2d->initVtx(0, 0);				// 頂点の初期設定
		return object2d;
	});

	// 衝撃
	GameObjectFactory::registerFactory(FACTORY_IMPACT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<Impact>();	// 生成
		emitter->setScene(inScene);				// シーンの設定
		emitter->init(inData);					// 初期設定
		return emitter;
	});

	// 状態が変化するブロック
	GameObjectFactory::registerFactory(FACTORY_CHANGE_STATE_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockChangeState>();	// 生成
		block->setScene(inScene);						// シーンの設定
		block->init(inData);							// 初期設定
		block->createBlockRigidBody();					// ブロック形状の剛体の作成
		return block;
	});

	// 衝撃を発生させるブロック
	GameObjectFactory::registerFactory(FACTORY_IMPACT_BLOCK, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockImpact>();	// 生成
		block->setScene(inScene);					// シーンの設定
		block->init(inData);						// 初期設定
		block->createBlockRigidBody();				// ブロック形状の剛体の作成
		return block;
	});

	// 爆発の煙ブロック
	GameObjectFactory::registerFactory(FACTORY_IMPACT_SMOKE, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockSmoke>();	// 生成
		block->setScene(inScene);				// シーンの設定
		block->init(inData);					// 初期設定
		block->createBlockRigidBody();			// ブロック形状の剛体の作成
		block->setForEachDummyFlag();			// ダミーフラグごとの処理
		return block;
	});

	// 粉々なブロック
	GameObjectFactory::registerFactory(FACTORY_SMALL_PIECES, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto block = make_shared<BlockSmallPieces>();	// 生成
		block->setScene(inScene);						// シーンの設定
		block->init(inData);							// 初期設定
		block->createBlockRigidBody();					// ブロック形状の剛体の作成
		block->setForEachDummyFlag();					// ダミーフラグごとの処理
		return block;
	});

	// 桜を生み出す
	GameObjectFactory::registerFactory(FACTORY_SPRING_EMITTER_2D, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto emitter = make_shared<SpringEmitter2D>();	// 生成
		emitter->setScene(inScene);						// シーンの設定
		emitter->init(inData);							// 初期設定
		return emitter;
	});

	// ゴールエフェクト
	GameObjectFactory::registerFactory(FACTORY_GOAL_EFFECT, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto effect = make_shared<GoalEffect>();	// 生成
		effect->setScene(inScene);					// シーンの設定
		effect->init(inData);						// 初期設定

		// 追加
		inScene->setGoalEffect(effect);
		return effect;
	});

	// 音源
	GameObjectFactory::registerFactory(FACTORY_SOUND_SOURCE, [](const shared_ptr<SceneBase>& inScene, const json& inData) {
		auto source = make_shared<BlockSound>();	// 生成
		source->setScene(inScene);					// シーンの設定
		source->init(inData);						// 初期設定

		// デバッグシーンなら表示する
		if (auto debug = std::dynamic_pointer_cast<SceneDebug>(inScene))
		{
			// パラメータ
			json j = json::parse(R"({
				"TAG": "wire_",
				"MASS": 0.0,
				"INERTIA": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"DUMMY_FLAGS": "0b101",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "BLACK",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"PRIORITY": 6,
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 1.0,
					"y": 1.0,
					"z": 1.0
				}
			})");
			auto wire = debug->addObject(FACTORY_BLOCKSBLOCK, j);				// シーンに追加
			wire->downcast<BlockBlocks>()->setMtxParent(source->getMatrix());	// 親のマトリックスの設定
			wire->setTag(wire->getTag() + source->getTag());					// タグの設定
		}

		return source;
	});
}

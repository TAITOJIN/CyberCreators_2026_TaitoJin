//--------------------------------------------------------------------------------
// 
// デバッグのシーン [scene_debug.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_debug.h"
#include "scene/scene_game.h"
// game_manager
#include "game_manager/game_manager.h"
// game_object
#include "game_object/game_object_factory.h"
// utility
#include "utility/calculate.h"
// resource
#include "resource/sound.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr auto DEF_SAVE_FILENAME = "res\\stage\\out\\stage000.json";	// デフォルトの保存ファイル名
	constexpr auto DEF_LOAD_FILENAME = "res\\stage\\out\\stage000.json";	// デフォルトの読み込みファイル名
	constexpr auto DEF_MATERIAL_SAVE_LOAD_FILENAME = "res\\list\\material_list.json";	// デフォルトのマテリアルの読み込み書き出しファイル名
	constexpr ImVec2 CREATEBLE_OBJECT_IMAGE_SIZE = ImVec2(100, 100);		// 一覧の画像サイズ
	constexpr ImVec2 DRAG_IMAGE_SIZE = ImVec2(100, 100);					// ドラッグ中の画像サイズ
	constexpr ImVec2 DRAG_WINDOW_SIZE = ImVec2(DRAG_IMAGE_SIZE.x + 16.0f, DRAG_IMAGE_SIZE.y + 16.0f);	// ドラッグ中のウィンドウサイズ

	const std::string NORMAL_BLOCK_NAME = "Nromal Block";
	const std::string NORMAL_BLOCK_PATH = "res\\texture\\img_block_normal.png";
	const std::string MOVE_BLOCK_NAME = "Move Block";
	const std::string MOVE_BLOCK_PATH = "res\\texture\\img_block_move.png";
	const std::string ROT_BLOCK_NAME = "Rot Block";
	const std::string ROT_BLOCK_PATH = "res\\texture\\img_block_rot.png";
	const std::string FALL_BLOCK_NAME = "Fall Block";
	const std::string FALL_BLOCK_PATH = "res\\texture\\img_block_fall.png";
	const std::string DISAPPEAR_BLOCK_NAME = "Disappear Block";
	const std::string DISAPPEAR_BLOCK_PATH = "res\\texture\\img_block_disappear.png";
	const std::string DUMMY_BLOCK_NAME = "Dummy Block";
	const std::string DUMMY_BLOCK_PATH = "res\\texture\\no_image.png";
	const std::string GOAL_BLOCK_NAME = "Goal Block";
	const std::string GOAL_BLOCK_PATH = "res\\texture\\no_image.png";
	const std::string PLAYER_NAME = "Player";
	const std::string PLAYER_PATH = "res\\texture\\no_image.png";
	const std::string COIN_NAME = "Coin";
	const std::string COIN_PATH = "res\\texture\\no_image.png";
	const std::string PHYSICS_NAME = "Physics Block";
	const std::string WOOD_NAME = "Wood Block";
	const std::string PUSH_BLOCK_NAME = "Push Block";
	const std::string REPAIR_BLOCK_NAME = "Repair Block";
	const std::string SOUND_SOURCE_NAME = "Sound Source";

	const std::string TREE000_NAME = "tree000";
	const std::string TREE001_NAME = "tree001";
	const std::string TREE002_NAME = "tree002";
	const std::string TREE003_NAME = "tree003";
	const std::string TREE004_NAME = "tree004";
	const std::string TREE005_NAME = "tree005";
	const std::string BONFIRE_NAME = "bonfire000";
	const std::string GROUND000_NAME = "ground000";
	const std::string GRASS000_NAME = "grass000";
	const std::string WATER_BLOCK_NAME = "Water Block";
	const std::string WATER_DROP_EMITTER_NAME = "Water Drop Emitter";
	const std::string BLAZING_EMITTER_NAME = "Blazing Emitter";
	const std::string SPAWN_POINT_NAME = "Spawn Point";
	const std::string EVENT_POINT_NAME = "Event Point";
	const std::string FLAME_NAME = "Flame";
	const std::string UPDRAFT_NAME = "Updraft";
	const std::string UPDRAFT_EFFECT_EMITTER_NAME = "Updraft Effect Emitter";
	const std::string ICE_BLOCK_NAME = "Ice Block";
	const std::string FREEZE_WATER_DROP_BLOCK_NAME = "Freeze Water Drop";
	const std::string FREEZE_WATER_DROP_BLOCK_EMITTER_NAME = "Freeze Water Drop Emitter";
	const std::string WATER_WHEEL_NAME = "Water Wheel";
	const std::string PUSH_ON_SNOW_NAME = "Push On Snow";
	const std::string CHANGE_STATE_BLOCK_NAME = "ChangeStateBlock";
	const std::string IMPACT_BLOCK_NAME = "Impact Block";

	constexpr const char* NO_IMAGE_PATH = "res\\texture\\no_image.png";

	constexpr float MIGRATION_LENGTH = 1.0f;		// 移動幅
	constexpr float MOUSE_OPERATION_SPEED = 30.0f;	// 移動速度

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneDebug::SceneDebug()
	: SceneBase()
	, m_isSelected(false)
	, m_isEdit(false)
	, m_saveFilename()
	, m_loadFilename()
	, m_creatbleObjectList()
	, m_isListImageHovered(false)
	, m_selectedImage(nullptr)
	, m_selectedCreateName()
	, m_mouseOperationSpeed(0.0f)
{
	strcpy(&m_saveFilename[0], "\0");
	strcpy(&m_loadFilename[0], "\0");

	m_mouseOperationSpeed = MOUSE_OPERATION_SPEED;
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneDebug::init()
{
	// 親クラスの処理
	HRESULT hr = SceneBase::init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// サイズの変更
	resizeObjects(static_cast<uint32_t>(PRIORITY::MAX));

	// ini ファイルの読み込み
	loadIni("res\\ini\\debug.ini");

	// リソースの読み込み
	getResManager()->loadTextures("res\\list\\texture_list.json");
	getResManager()->loadmaterials("res\\list\\material_list.json");
	getResManager()->loadModels("res\\list\\model_list.json");

	// オブジェクトの配置
	loadObjects(&m_loadFilename[0]);

	// 生成可能なオブジェクトの一覧の読み込み
	loadCreatbleObjectList();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneDebug::uninit()
{
	// テクスチャの破棄
	for (auto& texture : m_creatbleObjectList)
	{
		if (texture.second != nullptr)
		{
			texture.second->Release();
			texture.second = nullptr;
		}
	}
	m_creatbleObjectList.clear();

	// 親クラスの処理
	SceneBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneDebug::update()
{
	// 親クラスの処理
	SceneBase::update();

	// コインの回転
	Coin::updateRot();

	auto& gameManager = GM;								// ゲームの管理者
	auto& inputManager = gameManager.getInputManager();	// 入力処理の管理者
	auto& keyboard = inputManager.getKeyboard();		// キーボード
	auto& mouse = inputManager.getMouse();				// マウス
	auto world = getWorld();							// 世界
	auto dynamicsWorld = world->getDynamicsWorld();		// 動力学世界

	if (!IAO_IS_WND_HOVERED &&
		mouse.getTrigger(InputMouse::BUTTON::LEFT) &&
		!mouse.getPress(InputMouse::BUTTON::RIGHT) &&
		!keyboard.getPress(DIK_LSHIFT))
	{ // ウィンドウが他のウィンドウにフォーカスされていない，かつ左クリックした，かつ右クリックをしていない

		// 選択していない状態にする
		m_isSelected = false;

		// 原点と方向ベクトル
		Vec3 rayOrigin, rayDir;
		rayOrigin = rayDir = DEF_VEC3;

		// レイキャスト
		Camera::raycastMouse(&rayOrigin, &rayDir);

		btVector3 rayFrom(rayOrigin.x, rayOrigin.y, rayOrigin.z);
		btVector3 rayTo(rayDir.x, rayDir.y, rayDir.z);
		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

		dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

		if (rayCallback.hasHit())
		{
			// 当たったオブジェクトの取得
			btCollisionObject* hit = const_cast<btCollisionObject*>(rayCallback.m_collisionObject);

			auto objectslist = getObjects();
			for (const auto& objects : objectslist)
			{
				for (const auto& object : objects)
				{
					if(auto chara = std::dynamic_pointer_cast<Player>(object))
					{
						if (chara->getCharacter().getGhostObject() != hit)
						{
							continue;
						}

						m_selected = object;

						// 選択している状態にする
						m_isSelected = true;

						break;
					}

					if (auto object3d = std::dynamic_pointer_cast<GameObject3D>(object))
					{
						if (object3d->getRigitBodyPtr() != hit)
						{
							continue;
						}

						m_selected = object3d;

						// 選択している状態にする
						m_isSelected = true;

						break;
					}
				}
			}
		}
	}

	// 破棄
	if (keyboard.getTrigger(DIK_DELETE) && m_isSelected && m_selected.lock() && m_isEdit)
	{
		m_selected.lock()->setIsDestroy(true);

		m_selected.reset();

		// 選択していない状態にする
		m_isSelected = false;
	}

	// ツール
	bool isReload = false;
	tool(isReload);

	if (m_isEdit)
	{
		// オブジェクトの一覧
		objectList();

		// マテリアルの追加
		addMat();

		// インスペクター
		inspector();

		// ギズモ
		guizmo();

		// 操作
		operation();

		// パネル
		panel();
	}

	// 通知の更新処理
	ImGuiAddOns::UpdateNotify();

	// カメラとライトの設定
	gameManager.getRenderer().setCameraAndLight([this]() {
		getCamera()->setCamera();
		getObjects(static_cast<uint32_t>(SceneBase::BASE_PRIORITY::LIGHT)).front()->downcast<Light3D>()->setLightMatrix();
	});

	// 画面遷移
	if ((keyboard.getPress(DIK_RSHIFT) || isReload))
	{ // 右 SHIFT が押されている
		if (keyboard.getTrigger(DIK_RETURN) || isReload)
		{ // ENTER キーが押された
			if (m_isEdit)
			{
				ImGuiAddOns::NotifyWarning("Editing in progress.");
			}
			else
			{
				GM.getSceneManager().changeScene<SceneDebug>();	// デバッグへ
			}
		}
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneDebug::draw() const
{
	// 親クラスの処理
	SceneBase::draw();
}

//---------------------------------------------------
// 配置しているオブジェクトの一覧
//---------------------------------------------------
void SceneDebug::objectList()
{
	ImGui::Begin("object list");

	static bool isOpen = false;

	if (GM.getInputManager().getKeyboard().getTrigger(DIK_F2))
	{
		isOpen = !isOpen;
	}

	if (isOpen)
	{
		// 選択インデックスの変数
		static int selectedIndex = -1;

		// オブジェクトにそれぞれインデックスを振る
		std::vector<const char*> objectTags;
		auto objectsList = getObjects();
		for (const auto& objects : objectsList)
		{
			for (const auto& object : objects)
			{
				objectTags.push_back(object->getTag().c_str());
			}
		}

		if (ImGui::ListBox("##Object List", &selectedIndex, objectTags.data(), objectTags.size(), 18))
		{
			// 古い情報を削除
			m_selected.reset();

			// 選択したオブジェクトを探す (インデックスが一致したらそれ)
			bool isBreak = false;
			int j = 0;
			for (const auto& objects : objectsList)
			{
				for (const auto& object : objects)
				{
					if (j == selectedIndex)
					{
						m_selected = object;

						// 選択している状態にする
						m_isSelected = true;
						isBreak = true;
						break;
					}
					j++;
				}

				if (isBreak)
				{
					break;
				}
			}
		}
	}

	ImGui::End();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void SceneDebug::save(const std::string& inFilename)
{
	json saveData;

	saveData[SCHEMA] = "../schema/blocks_object_schema.json";

	json& normalBlocks = saveData[FACTORY_NORMAL_BLOCK];
	json& moveBlocks = saveData[FACTORY_MOVE_BLOCK];
	json& rotBlocks = saveData[FACTORY_ROTATE_BLOCK];
	json& fallBlocks = saveData[FACTORY_FALL_BLOCK];
	json& disappearBlocks = saveData[FACTORY_DISAPPEAR_BLOCK];
	json& dummyBlocks = saveData[FACTORY_DUMMY_BLOCK];
	json& goalBlocks = saveData[FACTORY_GOAL_BLOCK];
	json& player = saveData[FACTORY_PLAYER];

	json& waterBlock = saveData[FACTORY_WATER_BLOCK];

	json& models = saveData[FACTORY_MODEL];
	json& camelas = saveData[FACTORY_CAMERA];
	json& lights = saveData[FACTORY_LIGHT];
	json& worlds = saveData[FACTORY_WORLD];
	json& object2ds = saveData[FACTORY_OBJECT2D];

	json& coin = saveData[FACTORY_COIN];

	json& blocksObject = saveData[FACTORY_BLOCKSOBJECT];
	json& physicsBlock = saveData[FACTORY_PHYSICS_BLOCK];
	json& woodBlock = saveData[FACTORY_WOOD_BLOCK];
	json& repairBlock = saveData[FACTORY_REPAIR_BLOCK];
	json& pushBlock = saveData[FACTORY_PUSH_BLOCK];

	json& waterDropEmitter = saveData[FACTORY_WATERDROP_EMITTER];
	json& waterDropBlock = saveData[FACTORY_WATER_BLOCK];

	json& blazingEmitter = saveData[FACTORY_BLAZING_EMITTER];

	json& spawnPoint = saveData[FACTORY_SPAWN_POINT];
	json& eventPoint = saveData[FACTORY_EVENT_POINT];
	json& flame = saveData[FACTORY_FLAME];
	json& updraft = saveData[FACTORY_UPDRAFT];
	json& updraftEffectEmitter = saveData[FACTORY_UPDRAFT_EFFECT_EMITTER];
	json& iceBlock = saveData[FACTORY_ICE_BLOCK];
	json& freezeWaterDropBlock = saveData[FACTORY_FREEZE_WATER_DROP_BLOCK];
	json& freezeWaterDropBlockEmitter = saveData[FACTORY_FREEZE_WATER_DROP_BLOCK_EMITTER];

	json& waterWheel = saveData[FACTORY_WATER_WHEEL];

	json& snowEmitter2d = saveData[FACTORY_SNOW_EMITTER_2D];
	json& springEmitter2d = saveData[FACTORY_SPRING_EMITTER_2D];

	json& goalEffect = saveData[FACTORY_GOAL_EFFECT];

	json& pushOnSnow = saveData[FACTORY_PUSH_ON_SNOW];

	json& changeStateBlock = saveData[FACTORY_CHANGE_STATE_BLOCK];
	json& impactBlock = saveData[FACTORY_IMPACT_BLOCK];

	json& soundSource = saveData[FACTORY_SOUND_SOURCE];

	for (const auto& objects : getObjects())
	{
		for (const auto& object : objects)
		{
			json indivisual;
			object->save(indivisual);

			json* set = nullptr;
			if (typeid(*object) == typeid(BlockBase))
			{ // 通常ブロック
				set = &normalBlocks;
			}
			else if (typeid(*object) == typeid(BlockMove))
			{ // 動くブロック
				set = &moveBlocks;
			}
			else if (typeid(*object) == typeid(BlockRotate))
			{ // 回転ブロック
				set = &rotBlocks;
			}
			else if (typeid(*object) == typeid(BlockFall))
			{ // 落下ブロック
				set = &fallBlocks;
			}
			else if (typeid(*object) == typeid(BlockDisappear))
			{ // 消えるブロック
				set = &disappearBlocks;
			}
			else if (typeid(*object) == typeid(BlockDummy))
			{ // ダミーブロック
				if (auto block = std::dynamic_pointer_cast<BlockDummy>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &dummyBlocks;
			}
			else if (typeid(*object) == typeid(BlockGoal))
			{ // ゴールブロック
				if (auto block = std::dynamic_pointer_cast<BlockGoal>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &goalBlocks;
			}
			else if (typeid(*object) == typeid(Player))
			{ // プレイヤー
				set = &player;
			}
			else if (typeid(*object) == typeid(Model))
			{ // モデル
				set = &models;
			}
			else if (typeid(*object) == typeid(Camera))
			{ // カメラ
				set = &camelas;
			}
			else if (typeid(*object) == typeid(Light3D))
			{ // ライト
				set = &lights;
			}
			else if (typeid(*object) == typeid(PhysicsWorld))
			{ // 世界
				set = &worlds;
			}
			else if (typeid(*object) == typeid(GameObject2D))
			{ // 2D オブジェクト
				set = &object2ds;
			}
			else if (typeid(*object) == typeid(BlocksObjectBase))
			{ // 複数のブロックによるオブジェクト
				set = &blocksObject;
			}
			else if (typeid(*object) == typeid(Coin))
			{ // コイン
				set = &coin;
			}
			else if (typeid(*object) == typeid(BlockPhysics))
			{ // 物理挙動が特にあるブロック
				set = &physicsBlock;
			}
			else if (typeid(*object) == typeid(BlockWood))
			{ // 木ブロック
				set = &woodBlock;
			}
			else if (typeid(*object) == typeid(BlockPush))
			{ // 押せるブロック
				set = &pushBlock;
			}
			else if (typeid(*object) == typeid(BlockWater))
			{ // 水ブロック
				if (auto block = std::dynamic_pointer_cast<BlockWater>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &waterBlock;
			}
			else if (typeid(*object) == typeid(BlockWaterDrop))
			{ // 水雫ブロック
				if (auto block = std::dynamic_pointer_cast<BlockWaterDrop>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &waterDropBlock;
			}
			else if (typeid(*object) == typeid(BlockWaterDropEmitter))
			{ // 水雫生成器
				set = &waterDropEmitter;
			}
			else if (typeid(*object) == typeid(BlockSound))
			{ // 音源
				set = &soundSource;
			}
			else if (typeid(*object) == typeid(BlockBlazingEmitter))
			{ // めらめら生成器
				if (auto block = std::dynamic_pointer_cast<BlockBlazingEmitter>(object))
				{
					if (block->getMtxParent())
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &blazingEmitter;
			}
			else if (typeid(*object) == typeid(BlockRepair))
				{ // 修復されるブロック
				if (auto block = std::dynamic_pointer_cast<BlockRepair>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &repairBlock;
			}
			else if (typeid(*object) == typeid(SpawnPoint))
			{ // スポーン地点
				set = &spawnPoint;
			}
			else if (typeid(*object) == typeid(EventPoint))
			{ // イベント地点
				set = &eventPoint;
			}
			else if (typeid(*object) == typeid(Flame))
			{ // 炎
				set = &flame;
			}
			else if (typeid(*object) == typeid(Updraft))
			{ // 上昇気流
				set = &updraft;
			}
			else if (typeid(*object) == typeid(UpdraftEffectEmitter))
			{ // 上昇気流のエフェクトの生成器
				set = &updraftEffectEmitter;
			}
			else if (typeid(*object) == typeid(BlockIce))
			{ // 氷ブロック
				if (auto block = std::dynamic_pointer_cast<BlockIce>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
				set = &iceBlock;
			}
			else if (typeid(*object) == typeid(BlockFreezeWaterDrop))
				{ // 氷結した水雫ブロック
				if (auto block = std::dynamic_pointer_cast<BlockFreezeWaterDrop>(object))
				{
					if (!block->getDummyFlags().isFlagSet(BlockDummy::DUMMY_FLAGS::SAVE))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
					set = &freezeWaterDropBlock;
				}
			else if (typeid(*object) == typeid(BlockFreezeWaterDropEmitter))
			{ // 最初はフリーズしている水雫生成器
				set = &freezeWaterDropBlockEmitter;
			}
			else if (typeid(*object) == typeid(WaterWheel))
			{ // 水車
				set = &waterWheel;
			}
			else if (typeid(*object) == typeid(SnowEmitter2D))
			{ // 雪を生み出す
				set = &snowEmitter2d;
			}
			else if (typeid(*object) == typeid(SpringEmitter2D))
			{ // 桜を生み出す
				set = &springEmitter2d;
			}
			else if (typeid(*object) == typeid(BlockPushOnSnow))
			{ // 雪の上にある押せるブロック
				set = &pushOnSnow;
			}
			else if (typeid(*object) == typeid(BlockChangeState))
			{ // 状態変化するブロック
				set = &changeStateBlock;
			}
			else if (typeid(*object) == typeid(BlockImpact))
			{ // 衝撃を発生させるブロック
				set = &impactBlock;
			}
			else if (typeid(*object) == typeid(GoalEffect))
			{ // ゴールエフェクト
				set = &goalEffect;
			}
			else
			{
				continue;
			}

			set->push_back(indivisual);
		}
	}

	// 外部ファイルに保存
	std::ofstream ofs(inFilename);
	if (!ofs)
	{
		assert(false);
		std::cout << "ファイルを開けません！ステージ情報をセーブできませんでした。\n";
		ImGuiAddOns::NotifyError("Couldn't open file.");
		return;
	}
	ofs << saveData.dump(4) << std::endl;
	ofs.close();
	ImGuiAddOns::NotifySuccess("Stage information saved.");
}

//---------------------------------------------------
// 生成可能なオブジェクトの一覧の読み込み
//---------------------------------------------------
void SceneDebug::loadCreatbleObjectList()
{
	auto& gm = GM;								// ゲームの管理者
	Device device = nullptr;					// デバイス
	static const int LOCAL_LOAD_INTERVAL = 10;	// 読み込みの間隔 (これを設けないとメインスレッドが長い時間止まってしまう)

	// デバイスの取得
	gm.cs([&]() { device = gm.getDevice(); });

	// 通常ブロック
	gm.cs([this, &device]() {
		Texture texture;
		std::string name = NORMAL_BLOCK_NAME;
		std::string path = NORMAL_BLOCK_PATH;
		D3DXCreateTextureFromFile(device, path.c_str(), &texture);
		m_creatbleObjectList[name] = texture;
	});

	// 少し待つ
	std::this_thread::sleep_for(std::chrono::milliseconds(LOCAL_LOAD_INTERVAL));

	// 動くブロック
	gm.cs([this, &device]() {
		Texture texture;
		std::string name = MOVE_BLOCK_NAME;
		std::string path = MOVE_BLOCK_PATH;
		D3DXCreateTextureFromFile(device, path.c_str(), &texture);
		m_creatbleObjectList[name] = texture;
	});

	// 少し待つ
	std::this_thread::sleep_for(std::chrono::milliseconds(LOCAL_LOAD_INTERVAL));

	// 回転するブロック
	gm.cs([this, &device]() {
		Texture texture;
		std::string name = ROT_BLOCK_NAME;
		std::string path = ROT_BLOCK_PATH;
		D3DXCreateTextureFromFile(device, path.c_str(), &texture);
		m_creatbleObjectList[name] = texture;
	});

	// 少し待つ
	std::this_thread::sleep_for(std::chrono::milliseconds(LOCAL_LOAD_INTERVAL));

	// 落下ブロック
	gm.cs([this, &device]() {
		Texture texture;
		std::string name = FALL_BLOCK_NAME;
		std::string path = FALL_BLOCK_PATH;
		D3DXCreateTextureFromFile(device, path.c_str(), &texture);
		m_creatbleObjectList[name] = texture;
	});

	// 少し待つ
	std::this_thread::sleep_for(std::chrono::milliseconds(LOCAL_LOAD_INTERVAL));

	// 消えるブロック
	gm.cs([this, &device]() {
		Texture texture;
		std::string name = DISAPPEAR_BLOCK_NAME;
		std::string path = DISAPPEAR_BLOCK_PATH;
		D3DXCreateTextureFromFile(device, path.c_str(), &texture);
		m_creatbleObjectList[name] = texture;
	});

	// no image
	Texture noImage = nullptr;

	gm.cs([this, &device, &noImage]() {
		D3DXCreateTextureFromFile(device, NO_IMAGE_PATH, &noImage);
	});

	// ダミーブロック
	m_creatbleObjectList[DUMMY_BLOCK_NAME] = noImage;

	// ゴールブロック
	m_creatbleObjectList[GOAL_BLOCK_NAME] = noImage;

	// プレイヤー
	m_creatbleObjectList[PLAYER_NAME] = noImage;

	// コイン
	m_creatbleObjectList[COIN_NAME] = noImage;

	// 物理挙動が特にある
	m_creatbleObjectList[PHYSICS_NAME] = noImage;
}

//---------------------------------------------------
// ツール
//---------------------------------------------------
void SceneDebug::tool(bool& outResult)
{
	auto& keyboard = GM.getInputManager().getKeyboard();	// キーボード

	ImGui::Begin("##tool");

	// 編集モードのオンオフ
	if (keyboard.getTrigger(DIK_F1))
	{
		m_isEdit = !m_isEdit;
	}
	{
		ImGuiAddOns::ToggleButton("editor", &m_isEdit);
		ImGui::SameLine();
		ImGui::Text("edit");
	}

	ImGui::Separator();

	// 保存
	{
		ImGui::InputText("##SaveFilename", &m_saveFilename[0], sizeof(m_saveFilename));
		ImGui::SameLine();

		// 保存ボタン
		static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

		bool isRelease = ImGui::Button("SAVE");
		ImGui::PopStyleColor(3);

		if (isRelease)
		{
			if (m_saveFilename[0] == '\0')
			{
				ImGuiAddOns::NotifyError("Please specify file name.");
			}
			else
			{
				m_ini[FLAG_INI_SAVE][FLAG_INI_FILENAME] = &m_saveFilename[0];
				m_ini.save(m_iniFilename);

				// 保存処理
				save(&m_saveFilename[0]);
			}
		}
	}

	// 読み込み
	{
		ImGui::InputText("##LoadFilename", &m_loadFilename[0], sizeof(m_loadFilename));
		ImGui::SameLine();

		// 読み込みボタン
		static constexpr float LOCAL_LOAD_COLOR_R = 1.f / 4.f;
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_LOAD_COLOR_R, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_LOAD_COLOR_R, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_LOAD_COLOR_R, 0.9f, 0.9f));

		bool isRelease = ImGui::Button("LOAD");
		ImGui::PopStyleColor(3);

		if (isRelease)
		{
			if (m_loadFilename[0] == '\0')
			{
				ImGuiAddOns::NotifyError("Please specify file name.");
			}
			else
			{
				m_ini[FLAG_INI_LOAD][FLAG_INI_FILENAME] = &m_loadFilename[0];
				m_ini.save(m_iniFilename);

				// 再読み込みフラグを真にする
				outResult = true;
			}
		}
	}

	// マウス操作でのオブジェクの移動速度の設定
	{
		ImGui::Separator();
		ImGui::InputFloat("Mouse Operation", &m_mouseOperationSpeed);
	}

	// 時間の表示
	{
		std::string time;
		{ // デルタタイム
			std::stringstream ss;
			ss << getTime()->getDeltaTime();
			time += ss.str() + "\n";
		}
		
		{ // FPS
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << getTime()->getFPS();
			time += ss.str();
		}

		ImGui::Text(time.c_str());
	}

	ImGui::End();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void SceneDebug::inspector()
{
	// インスペクター
	ImGui::Begin("inspector");

	if (m_selected.lock() && m_isSelected && m_isEdit)
	{
		// インスペクター
		m_selected.lock()->inspector();
	}

	ImGui::End();
}

//---------------------------------------------------
// ギズモ
//---------------------------------------------------
void SceneDebug::guizmo()
{
	if (!m_selected.lock())
	{
		return;
	}
}

//---------------------------------------------------
// 操作
//---------------------------------------------------
void SceneDebug::operation()
{
	if (!m_selected.lock() || !m_isSelected)
	{
		return;
	}

	auto object3d = std::dynamic_pointer_cast<GameObject3D>(m_selected.lock());
	if (!object3d)
	{
		return;
	}

	auto& inputManager = GM.getInputManager();		// 入力処理の管理者
	float deltaTime = getTime()->getDeltaTime();	// デルタタイム
	Vec3 pos = object3d->getPos();					// 位置
	auto& keyboard = inputManager.getKeyboard();	// キーボード

	// マウス操作
	{
		Vec3 move = DEF_VEC3;
		auto& mouse = inputManager.getMouse();
		Vec2 mouseMove = mouse.getMove() * deltaTime;

		if (keyboard.getPress(DIK_F))
		{
			Vec2 cameraRot = getCamera()->getRot();
			float y = atan2f(mouseMove.y, mouseMove.x) - cameraRot.y - Camera::REVISION_ANGLE_Y;

			move.x = -sinf(D3DX_PI - y) * m_mouseOperationSpeed * fabsf(mouseMove.x);
			move.z = cosf(D3DX_PI - y) * m_mouseOperationSpeed * fabsf(mouseMove.y);
		}

		if (keyboard.getPress(DIK_SPACE))
		{
			move.y = -mouseMove.y * m_mouseOperationSpeed;
		}

		pos += move;
	}

	// 位置の設定
	object3d->setPos(pos);

	// 剛体にも反映
	if (object3d->getRigitBodyPtr() != nullptr)
	{
		auto trans = object3d->getRigitBody().getWorldTransform();
		btVector3 newPos(pos.x, pos.y, pos.z);
		trans.setOrigin(newPos);
		object3d->getRigitBody().setWorldTransform(trans);
	}
}

//---------------------------------------------------
// パネル
//---------------------------------------------------
void SceneDebug::panel()
{
	// ウィンドウのフラグ
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	if (m_isListImageHovered)
	{
		windowFlags |= ImGuiWindowFlags_NoMove;
	}

	bool isOpen = true;
	ImGui::Begin("Panel", &isOpen, windowFlags);

	ImVec2 panelPos = ImGui::GetWindowPos();
	ImVec2 panelSize = ImGui::GetWindowSize();

	m_isListImageHovered = false;	// 被さっていない状態にする

	// 通常ブロック
	setPanelObject(NORMAL_BLOCK_NAME);

	// 動くブロック
	setPanelObject(MOVE_BLOCK_NAME);

	// 回転するブロック
	setPanelObject(ROT_BLOCK_NAME);

	// 落下ブロック
	setPanelObject(FALL_BLOCK_NAME);

	// 消えるブロック
	setPanelObject(DISAPPEAR_BLOCK_NAME);

	// ダミーブロック
	setPanelObject(DUMMY_BLOCK_NAME);

	// ゴールブロック
	setPanelObject(GOAL_BLOCK_NAME);

	// プレイヤー
	setPanelObject(PLAYER_NAME);

	// コイン
	setPanelObject(COIN_NAME);

	// 物理挙動が特にあるブロック
	setPanelObject(PHYSICS_NAME);

	// 木ブロック
	setPanelObject(WOOD_NAME);

	// 修復されるブロック
	setPanelObject(REPAIR_BLOCK_NAME);

	// 押せるブロック
	setPanelObject(PUSH_BLOCK_NAME);

	// 雪の上にある押せるブロック
	setPanelObject(PUSH_ON_SNOW_NAME);

	// 水ブロック
	setPanelObject(WATER_BLOCK_NAME);

	// 複数のブロックによるオブジェクト
	setPanelObject(TREE000_NAME);
	setPanelObject(TREE001_NAME);
	setPanelObject(TREE002_NAME);
	setPanelObject(TREE003_NAME);
	setPanelObject(TREE004_NAME);
	setPanelObject(TREE005_NAME);
	setPanelObject(BONFIRE_NAME);
	setPanelObject(GROUND000_NAME);
	setPanelObject(GRASS000_NAME);

	// 水雫ブロックの生成器
	setPanelObject(WATER_DROP_EMITTER_NAME);

	// めらめらブロックの生成器
	setPanelObject(BLAZING_EMITTER_NAME);

	// スポーン地点
	setPanelObject(SPAWN_POINT_NAME);

	// イベント地点
	setPanelObject(EVENT_POINT_NAME);

	// 炎
	setPanelObject(FLAME_NAME);

	// 上昇気流
	setPanelObject(UPDRAFT_NAME);

	// 上昇気流のエフェクトの生成器
	setPanelObject(UPDRAFT_EFFECT_EMITTER_NAME);

	// 氷結した水雫ブロック
	setPanelObject(FREEZE_WATER_DROP_BLOCK_NAME);

	// 最初はフリーズしている水雫の生成器
	setPanelObject(FREEZE_WATER_DROP_BLOCK_EMITTER_NAME);

	// 氷ブロック
	setPanelObject(ICE_BLOCK_NAME);

	// 水車
	setPanelObject(WATER_WHEEL_NAME);

	// 状態が変化するブロック
	setPanelObject(CHANGE_STATE_BLOCK_NAME);

	// 衝撃を発生させるブロック
	setPanelObject(IMPACT_BLOCK_NAME);

	// 音源
	setPanelObject(SOUND_SOURCE_NAME);

	ImGui::End();

	if (m_selectedImage != nullptr)
	{
		// ウィンドウのフラグ
		ImGuiWindowFlags dragWindowFlags = ImGuiWindowFlags_None;

		dragWindowFlags |= ImGuiWindowFlags_NoTitleBar;
		dragWindowFlags |= ImGuiWindowFlags_NoResize;
		dragWindowFlags |= ImGuiWindowFlags_NoMove;
		dragWindowFlags |= ImGuiWindowFlags_NoScrollbar;
		dragWindowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
		dragWindowFlags |= ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("##drag", &isOpen, dragWindowFlags);

		ImGui::SetWindowSize(DRAG_WINDOW_SIZE);

		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos -= DRAG_WINDOW_SIZE * HALF;
		ImGui::SetWindowPos(mousePos);

		ImGui::Image(reinterpret_cast<void*>(*m_selectedImage), DRAG_IMAGE_SIZE);

		ImVec2 dragPos = ImGui::GetWindowPos();
		ImVec2 dragSize = ImGui::GetWindowSize();

		// 配置
		if (!Collision::isCollisionRectVsRect2D(panelPos, panelSize, dragPos, dragSize) &&
			!ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{ // 生成可能なオブジェクト一覧のパネル外でマウスのクリックを離した

			json j{};
			std::string factoryName;

			if (m_selectedCreateName == NORMAL_BLOCK_NAME)
			{ // 通常ブロック
				factoryName = FACTORY_NORMAL_BLOCK;
				j = json::parse(R"({"TAG": "normal_0", "PRIORITY": 6, "MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "IS_STATIC": true, "KEY": "res\\model\\block000.x", "MAT": [{"MAT_KEY": "ORANGE", "MAT_NAME": 0}],
"OFFSET": { "x": 0.0, "y": 0.0, "z": 0.0 }, "POS": { "x": 0.0, "y": 0.0, "z": 0.0 }, "ROT": { "x": -0.0, "y": 0.0, "z": 0.0 }, "SCALE": { "x": 2.0, "y": 2.0, "z": 2.0 }})");
			}
			else if (m_selectedCreateName == MOVE_BLOCK_NAME)
			{ // 動くブロック
				factoryName = FACTORY_MOVE_BLOCK;
				j = json::parse(R"({"TAG": "move_0", "PAIR_KEY": "A0_todo_change", "PRIORITY": 6, "MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "IS_STATIC": true, "KEY": "res\\model\\block000.x", "MAT": [{"MAT_KEY": "YELLOW", "MAT_NAME": 0}],
"OFFSET": { "x": 0.0, "y": 0.0, "z": 0.0 }, "POS": { "x": 0.0, "y": 0.0, "z": 0.0 }, "ROT": { "x": -0.0, "y": 0.0, "z": 0.0 }, "SCALE": { "x": 2.0, "y": 2.0, "z": 2.0 }, "POINT1": {"x": -100.0, "y": 0.0, "z": 0.0}, "POINT2": {"x": 100.0, "y": 0.0, "z": 0.0}, "TARGET_POS": {"x": -100.0, "y": 0.0, "z": 0.0}, "ATN_SPEED": 1.0})");
			}
			else if (m_selectedCreateName == ROT_BLOCK_NAME)
			{ // 回転ブロック
				factoryName = FACTORY_ROTATE_BLOCK;
				j = json::parse(R"({
					"TAG": "rotate_0",
					"PAIR_KEY": "A0_todo_change",
					"IS_ENABLED": false,
					"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
					"PRIORITY": 6,
					"AXIS": {
						"x": 0.0,
						"y": 0.0,
						"z": 1.0
					},
					"IS_STATIC": true,
					"KEY": "res\\model\\block000.x",
					"MAT": [
						{
							"MAT_KEY": "GREEN",
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
					"ROT": {
						"x": -0.0,
						"y": 0.0,
						"z": 0.0
					},
					"SCALE": {
						"x": 2.0,
						"y": 2.0,
						"z": 2.0
					},
					"SPEED_ROT": -0.30000001192092896
				})");

			}
			else if (m_selectedCreateName == FALL_BLOCK_NAME)
			{ // 落下ブロック

				factoryName = FACTORY_FALL_BLOCK;
				j = json::parse(R"({"TAG": "fall_0", "MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "PRIORITY": 6, "IS_STATIC": true, "KEY": "res\\model\\block000.x", "MAT": [{"MAT_KEY": "LIGHT_BLUE", "MAT_NAME": 0}],
"OFFSET": { "x": 0.0, "y": 0.0, "z": 0.0 }, "POS": { "x": 0.0, "y": 0.0, "z": 0.0 }, "ROT": { "x": -0.0, "y": 0.0, "z": 0.0 }, "SCALE": { "x": 2.0, "y": 2.0, "z": 2.0 }})");

			}
			else if (m_selectedCreateName == DISAPPEAR_BLOCK_NAME)
			{ // 消えるブロック
				factoryName = FACTORY_DISAPPEAR_BLOCK;
				j = json::parse(R"({
				"TAG": "dissappear_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"FIRST_APPEAR": true,
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "TRANSLUCENT_RED",
						"MAT_NAME": 0
					}
				],
				"OFFSET": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"POS": {
					"x": -40.0,
					"y": 0.0,
					"z": 0.0
				},
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == DUMMY_BLOCK_NAME)
			{ // ダミーブロック
			factoryName = FACTORY_DUMMY_BLOCK;
			j = json::parse(R"({
				"TAG": "dummy_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");

			}
			else if (m_selectedCreateName == GOAL_BLOCK_NAME)
			{ // ゴールブロック
				factoryName = FACTORY_GOAL_BLOCK;
				j = json::parse(R"({
				"TAG": "goal_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1001",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "YELLOW",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");

			}
			else if (m_selectedCreateName == PLAYER_NAME)
			{ // プレイヤー
#if 0
				factoryName = FACTORY_PLAYER;

				bool existPlayer = false;
				for (const auto& object : m_objectList)
				{
					if (auto player = dynamic_cast<Player*>(object))
					{
						existPlayer = true;
						break;
					}
				}

				// 既にプレイヤーが存在していたら配置しない
				if (!existPlayer)
				{
					j = json::parse(R"()");


					const std::string key = MAT_COL_BLACK;
					auto black = GM->getScene()->getResManager()->getResMaterial(key)->getMat();
					std::unordered_map<int, Model::ModelMaterial> mat;
					mat[0].mat = black;
					mat[0].key = key;

					// 作成
					m_objectList.emplace_back(Player::create(
						Vec3(0.f, 0.f, 0.f),					// 位置
						Vec3(0.f, D3DXToRadian(180.0f), 0.0f),	// 向き
						Vec3(0.5f, 0.5f, 0.5f),					// スケール
						"model\\capsule000.x",					// キー
						Vec3(0.0f, 0.0f, 0.0f),					// オフセット
						mat,					// マテリアル
						10.f,					// 速さ
						0.2f,					// 減速度
						75.f,					// ジャンプ力
						Vec3(0.0f, -100.0f, 0.0f),	// 重力
						45.1f,					// 乗れる斜め角度
						0.35f,					// 乗れる高さ
						0.f,					// ジャンプの初速度
						55.f					// 落下速度
					));
				}
#endif
			}
			else if (m_selectedCreateName == COIN_NAME)
			{ // コイン
			factoryName = FACTORY_COIN;
			j = json::parse(R"({"TAG": "coin_0", "MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "PRIORITY": 4, "IS_STATIC": true, "KEY": "res\\model\\coin000.x", "MAT": [{"MAT_KEY": "YELLOW", "MAT_NAME": 0}],
"OFFSET": { "x": 0.0, "y": 0.0, "z": 0.0 }, "POS": { "x": 0.0, "y": 0.0, "z": 0.0 }, "ROT": { "x": -0.0, "y": 0.0, "z": 0.0 }, "SCALE": { "x": 0.75, "y": 0.75, "z": 0.75 }})");
			}
			else if (m_selectedCreateName == PHYSICS_NAME)
			{ // 物理挙動が特にあるブロック
			factoryName = FACTORY_PHYSICS_BLOCK;
			j = json::parse(R"({
				"TAG": "physics_block_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": false,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "TREE_WHITE",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == WOOD_NAME)
			{ // 木ブロック
			factoryName = FACTORY_WOOD_BLOCK;
			j = json::parse(R"({
				"TAG": "wood_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": false,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WOOD_BLOCK_0",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 0.75,
					"y": 0.75,
					"z": 0.75
				}
			})");
			}
			else if (m_selectedCreateName == REPAIR_BLOCK_NAME)
			{ // 修復されるブロック
			factoryName = FACTORY_REPAIR_BLOCK;
			j = json::parse(R"({
				"TAG": "repair_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				},
				"COLLAPSE_TIME" : 2.0
			})");

			}
			else if (m_selectedCreateName == PUSH_BLOCK_NAME)
			{ // 押せるブロック
			factoryName = FACTORY_PUSH_BLOCK;
			j = json::parse(R"({
				"TAG": "push_block_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": false,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "TREE_WHITE",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == TREE000_NAME)
			{ // 木 000
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree000.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "tree_short_0"
		})");
			}
			else if (m_selectedCreateName == TREE001_NAME)
			{ // 木 001
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree001.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "tree_long_0"
		})");
			}
			else if (m_selectedCreateName == TREE002_NAME)
			{ // 木 002
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree002.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "snow_tree_0"
		})");
			}
			else if (m_selectedCreateName == TREE003_NAME)
			{ // 木 003
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree003.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "spring_tree_0"
		})");
			}
			else if (m_selectedCreateName == TREE004_NAME)
			{ // 木 004
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree004.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "snow_long_tree_0"
		})");
			}
			else if (m_selectedCreateName == TREE005_NAME)
			{ // 木 005
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"({
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\tree005.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "spring_long_tree_0"
		})");
			}
			else if (m_selectedCreateName == GROUND000_NAME)
			{ // 地面 000
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"(        {
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\ground000.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "ground000_0"
		})");
			}
			else if (m_selectedCreateName == BONFIRE_NAME)
			{ // 焚火 000
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"({
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\bonfire000.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "bonfire_0"
		})");
			}
			else if (m_selectedCreateName == GRASS000_NAME)
			{ // 草 000
			factoryName = FACTORY_BLOCKSOBJECT;
			j = json::parse(R"({
			"IS_STATIC": false,
			"KEY": "res\\blocks_object\\grass000.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "grass000_0"
		})");
			}
			else if (m_selectedCreateName == WATER_BLOCK_NAME)
			{ // 水
			factoryName = FACTORY_WATER_BLOCK;
			j = json::parse(R"({
				"TAG": "water_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1001",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "WATER_BLOCK",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == WATER_DROP_EMITTER_NAME)
			{ // 水雫生成器
			factoryName = FACTORY_WATERDROP_EMITTER;
			j = json::parse(R"({
			"IS_STATIC": true,
			"KEY": "res\\none.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "water_drop_emitter_0",
			"SET_INTERVAL": 3.0,
			"DROP_POS": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"DROP_SCALE": 1.0,
			"MS_MIN_LIFE": 60.0,
			"MS_MAX_LIFE": 120.0,
			"DROP_GRAVITY": -10.0
				})");
			}
			else if (m_selectedCreateName == SOUND_SOURCE_NAME)
			{ // 音源
			factoryName = FACTORY_SOUND_SOURCE;
			j = json::parse(R"({
			"IS_STATIC": true,
			"KEY": "res\\none.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"DROP_SCALE": 1.0,
			"TAG": "sound_source_0",
			"LABEL": "BONFIRE"
				})");
			}
			else if (m_selectedCreateName == BLAZING_EMITTER_NAME)
			{ // めらめら生成器
			factoryName = FACTORY_BLAZING_EMITTER;
			j = json::parse(R"({
			"IS_STATIC": true,
			"KEY": "res\\none.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "blazing_emitter_0",
			"SET_INTERVAL": 1.0,
			"DROP_POS": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"DROP_SCALE": 1.0,
			"MS_MIN_LIFE": 60.0,
			"MS_MAX_LIFE": 120.0,
			"DROP_GRAVITY": 10.0
				})");
			}
			else if (m_selectedCreateName == SPAWN_POINT_NAME)
			{ // スポーン地点
				factoryName = FACTORY_SPAWN_POINT;

				j = json::parse(R"(
				{
					"SIZE": {
						"x": 50.0,
						"y": 50.0
					},
					"COLOR": {
						"r": 1.0,
						"g": 1.0,
						"b": 1.0,
						"a": 1.0
					},
					"CENTER": {
						"x": 25.0,
						"y": 25.0
					},
					"RESPAWN_POINT": {
						"x": 0.0,
						"y": 0.0,
						"z": 0.0
					},
					"INERTIA": {
						"x": 0.0,
						"y": 0.0,
						"z": 0.0
					},
					"IS_STATIC": true,
					"KEY": "res\\texture\\spawn_point.png",
					"MASS": 0.0,
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
					},
					"TAG": "spawn_point_0"
				}
				)");
			}
			else if (m_selectedCreateName == EVENT_POINT_NAME)
			{ // イベント地点
				factoryName = FACTORY_EVENT_POINT;

				j = json::parse(R"(
					{
						"SIZE": {
							"x": 50.0,
							"y": 50.0
						},
						"COLOR": {
							"r": 1.0,
							"g": 1.0,
							"b": 1.0,
							"a": 1.0
						},
						"CENTER": {
							"x": 25.0,
							"y": 25.0
						},
						"RESPAWN_POINT": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"INERTIA": {
							"x": 0.0,
							"y": 0.0,
							"z": 0.0
						},
						"IS_STATIC": true,
						"EVENT_NAME": "event_name_todo_change",
						"KEY": "res\\texture\\event_point.png",
						"MASS": 0.0,
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
						},
						"TAG": "event_point_0"
					}
					)");
			}
			else if (m_selectedCreateName == FLAME_NAME)
			{ // 炎
				factoryName = FACTORY_FLAME;
				j = json::parse(R"({
					"IS_STATIC": true,
					"KEY": "res\\model\\block000.x",
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
						"x": 0.0,
						"y": -0.0,
						"z": -0.0
					},
					"SCALE": {
						"x": 1.0,
						"y": 1.0,
						"z": 1.0
					},
					"MASS": 0.0,
					"INERTIA": {
						"x": 0.0,
						"y": 0.0,
						"z": 0.0
					},
					"TAG": "flame_0",
					"PAIR_KEY": "A0_todo_change"
				})");
			}
			else if (m_selectedCreateName == UPDRAFT_NAME)
			{ // 上昇気流
				factoryName = FACTORY_UPDRAFT;
				j = json::parse(R"({
					"IS_STATIC": true,
					"KEY": "res\\model\\block000.x",
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
						"x": 0.0,
						"y": -0.0,
						"z": -0.0
					},
					"SCALE": {
						"x": 1.0,
						"y": 1.0,
						"z": 1.0
					},
					"MASS": 0.0,
					"INERTIA": {
						"x": 0.0,
						"y": 0.0,
						"z": 0.0
					},
					"TAG": "updraft_0",
					"PAIR_KEY": "A0_todo_change"
				})");
			}
			else if (m_selectedCreateName == UPDRAFT_EFFECT_EMITTER_NAME)
			{ // 上昇気流のエフェクトの生成器
			factoryName = FACTORY_UPDRAFT_EFFECT_EMITTER;
			j = json::parse(R"({
			"IS_STATIC": true,
			"KEY": "res\\none.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "updraft_effect_emitter_0",
			"SET_INTERVAL": 1.5,
			"DROP_POS": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"DROP_SCALE": 1.0,
			"MS_MIN_LIFE": 60.0,
			"MS_MAX_LIFE": 120.0,
			"DROP_GRAVITY": 10.0,
					"PAIR_KEY": "A0_todo_change"
				})");
			}
			else if (m_selectedCreateName == ICE_BLOCK_NAME)
			{ // 氷
			factoryName = FACTORY_ICE_BLOCK;
			j = json::parse(R"({
				"TAG": "ice_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "ICE_BLOCK",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				},
				"WATER_MAT_KEY": "WATER_BLOCK",
				"ICE_MAT_KEY": "ICE_BLOCK",
				"PAIR_KEY": "A0_todo_change"
			})");
			}
			else if (m_selectedCreateName == FREEZE_WATER_DROP_BLOCK_NAME)
			{ // 氷結した水雫ブロック
				factoryName = FACTORY_FREEZE_WATER_DROP_BLOCK;
				j = json::parse(R"({
				"TAG": "freeze_block_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "ICE_BLOCK",
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
				"ROT": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 0.5,
					"y": 0.5,
					"z": 0.5
				},
				"FIRST_SCALE": 0.5,
				"MAX_LIFE": 60.0,
				"DROP_GRAVITY": -10.0,
				"PAIR_KEY": "A0_todo_change"
			})");
			}
			else if (m_selectedCreateName == FREEZE_WATER_DROP_BLOCK_EMITTER_NAME)
			{ // 最初はフリーズしている水雫生成器
			factoryName = FACTORY_FREEZE_WATER_DROP_BLOCK_EMITTER;
			j = json::parse(R"({
			"IS_STATIC": true,
			"KEY": "res\\none.json",
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
				"x": 0.0,
				"y": -0.0,
				"z": -0.0
			},
			"SCALE": {
				"x": 1.0,
				"y": 1.0,
				"z": 1.0
			},
			"MASS": 0.0,
			"INERTIA": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"TAG": "freeze_emitter_0",
			"SET_INTERVAL": 3.0,
			"DROP_POS": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"DROP_SCALE": 1.0,
			"MS_MIN_LIFE": 60.0,
			"MS_MAX_LIFE": 120.0,
			"DROP_GRAVITY": -10.0,
				"PAIR_KEY": "A0_todo_change"
				})");
			}
			else if (m_selectedCreateName == WATER_WHEEL_NAME)
			{ // 水車
				factoryName = FACTORY_WATER_WHEEL;
				j = json::parse(R"({
					"TAG": "water_wheel_0",
					"PAIR_KEY": "A0_todo_change",
					"IS_ENABLED": false,
					"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
					"PRIORITY": 6,
					"AXIS": {
						"x": 0.0,
						"y": 0.0,
						"z": 1.0
					},
					"IS_STATIC": true,
					"KEY": "res\\model\\water_wheel000.x",
					"MAT": [
						{
							"MAT_KEY": "WOOD_BLOCK_0",
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
					"ROT": {
						"x": -0.0,
						"y": 0.0,
						"z": 0.0
					},
					"SCALE": {
						"x": 1.0,
						"y": 1.0,
						"z": 1.0
					},
					"SPEED_ROT": -0.30000001192092896
				})");

			}
			else if (m_selectedCreateName == PUSH_ON_SNOW_NAME)
			{ // 雪の上にある押せるブロック
			factoryName = FACTORY_PUSH_ON_SNOW;
			j = json::parse(R"({
				"TAG": "on_snow_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b1000",
				"IS_STATIC": false,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "TREE_WHITE",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == CHANGE_STATE_BLOCK_NAME)
			{ // 状態が変化するブロック
			factoryName = FACTORY_CHANGE_STATE_BLOCK;
			j = json::parse(R"({
				"TAG": "state_block_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "TREE_WHITE",
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				}
			})");
			}
			else if (m_selectedCreateName == IMPACT_BLOCK_NAME)
			{ // 衝撃を発生させるブロック
			factoryName = FACTORY_IMPACT_BLOCK;
			j = json::parse(R"({
				"TAG": "impact_block_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"IS_STATIC": false,
				"KEY": "res\\model\\barrel000.x",
				"MAT": [
				    {
				        "MAT_KEY": "WOOD_BLOCK_0",
				        "MAT_NAME": 0
				    },
				    {
				        "MAT_KEY": "FIRECRACKERS_BLACK",
				        "MAT_NAME": 1
				    },
				    {
				        "MAT_KEY": "FIRECRACKERS_ROPE",
				        "MAT_NAME": 2
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
				"ROT": {
					"x": -0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 0.75,
					"y": 0.75,
					"z": 0.75
				}
			})");
			}

			// 注視点に配置
			j[FLAG_POS] = getCamera()->getPosR();

			if (!j.empty())
			{
				// オブジェクトの追加 (今配置したオブジェクトを選択状態にする)
				m_selected = addObject(factoryName, j);
				m_isSelected = true;
			}
		}

		ImGui::End();
	}

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		m_selectedImage = nullptr;
	}
}

//---------------------------------------------------
// パネルに表示するオブジェクの設定
//---------------------------------------------------
void SceneDebug::setPanelObject(const std::string& inName)
{
	ImGui::Image(reinterpret_cast<void*>(m_creatbleObjectList[inName]), CREATEBLE_OBJECT_IMAGE_SIZE);
	if (ImGui::IsItemHovered())
	{
		m_isListImageHovered = true;

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			m_selectedCreateName = inName;
			m_selectedImage = &m_creatbleObjectList[inName];
		}
	}
	ImGui::Text(inName.c_str());
	ImGui::Separator();
}

//---------------------------------------------------
// マテリアルの追加
//---------------------------------------------------
void SceneDebug::addMat()
{
	ImGui::Begin("Material Management");

	static bool isOpen = false;
	if (GM.getInputManager().getKeyboard().getTrigger(DIK_F3))
	{
		isOpen = !isOpen;
	}

	if (isOpen)
	{
		auto rm = getResManager();
		static GraphicsMaterial newMaterial = DEF_MAT;
		static char key[MAX_WORD] = "HOGE";

		// マテリアルの設定
		if (ImGui::CollapsingHeader("Create Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// マテリアルの種類
			{
				// マテリアルのリスト
				auto& materialList = rm->getMaterials();

				static std::vector<const char*> items;
				items.clear();
				for (const auto& resMat : materialList)
				{
					items.emplace_back(resMat.first.c_str());
				}

				// 現在のマテリアルのキーの取得
				static int selectMatKey = 0;

				if (ImGui::Combo("Select Material", &selectMatKey, items.data(), items.size()))
				{
					// マテリアルのキーの取得
					int j = 0;
					for (auto itr = items.begin(); itr != items.end(); ++itr)
					{
						if (selectMatKey != j)
						{
							++j;
							continue;
						}
						break;
					}

					// マテリアルのキーの設定
					strcpy(&key[0], items[j]);

					// マテリアルの設定
					newMaterial = materialList.find(items[j])->second->getMaterial();
				}
			}

			// カラーピッカーを表示
			ImGuiAddOns::Material("Diffuse", newMaterial.Diffuse);

			ImGui::Separator();
			ImGuiAddOns::Material("Ambient", newMaterial.Ambient);

			ImGui::Separator();
			ImGuiAddOns::Material("Specular", newMaterial.Specular);

			ImGui::Separator();
			ImGuiAddOns::Material("Emissive", newMaterial.Emissive);

			ImGui::Separator();
			ImGui::SliderFloat("Power", &newMaterial.Power, 0.f, 128.f);
		}

		json j;

		for (const auto& material : rm->getMaterials())
		{
			json mat;
			std::unordered_map<int, Model::ModelMaterial> matInfo;

			{
				matInfo[0].mat = material.second->getMaterial();
				matInfo[0].key = material.first;
			}

			mat[FLAG_MAT] = matInfo[0].mat;
			mat[FLAG_MAT_KEY] = matInfo[0].key;

			j.emplace_back(mat);
		}

		static char filename[MAX_WORD] = "res\\list\\material_list.json";
		ImGui::InputText("mat save filename", &filename[0], sizeof(filename));

		ImGui::InputText("mat key", &key[0], sizeof(key));

		// 書き出しボタン
		static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

		bool isRelease = ImGui::Button("SAVE");
		ImGui::PopStyleColor(3);

		if (isRelease)
		{
			// 既にそのキーが存在するかどうかの確認
			if (rm->getMaterials().count(&key[0]) != 0)
			{ // 既に存在する
				ImGuiAddOns::NotifyError("That key already exists.");
			}
			else
			{
				json mat;
				mat[FLAG_MAT] = newMaterial;
				mat[FLAG_MAT_KEY] = key;

				j.emplace_back(mat);

				// 外部ファイルに書き出す
				{
					std::ofstream ofs(&filename[0]);

					if (!ofs)
					{
						std::cout << "ファイルを開けませんでした！" << std::endl;
						ImGuiAddOns::NotifyError("Couldn't open file.");
						return;
					}

					ofs << j.dump(4) << std::endl;

					ofs.close();
				}

				// マテリアルの追加
				{
					// マテリアルの読み込み
					ResourceMaterial* resMat = new ResourceMaterial();
					resMat->setMaterial(newMaterial);

					// 追加
					rm->addMaterial(key, resMat);

					// 成功通知
					ImGuiAddOns::NotifySuccess("Saved.");
				}
			}
		}
	}

	ImGui::End();
}

//---------------------------------------------------
// ini ファイルの読み込み
//---------------------------------------------------
void SceneDebug::loadIni(const char* inFilename)
{
	// ファイル名の設定
	m_iniFilename = inFilename;

	// 読み込み
	m_ini.load(m_iniFilename);

	// 各メンバの設定
	strcpy(&m_saveFilename[0], m_ini[FLAG_INI_SAVE][FLAG_INI_FILENAME].as<std::string>().c_str());
	strcpy(&m_loadFilename[0], m_ini[FLAG_INI_LOAD][FLAG_INI_FILENAME].as<std::string>().c_str());
}

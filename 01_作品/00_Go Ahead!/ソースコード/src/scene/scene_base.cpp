//--------------------------------------------------------------------------------
// 
// シーンの基底 [scene_base.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"
#include "scene/scene_game.h"
#include "scene/scene_debug.h"
// game_manager
#include "game_manager/game_manager.h"
// game_object
#include "game_object/game_object_factory.h"
// utility
#include "utility/file.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneBase::SceneBase()
	: m_time(nullptr)
	, m_resourceManager(nullptr)
	, m_sharedData(nullptr)
	, m_eventManager(nullptr)
{
	m_objects.clear();
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneBase::init()
{
	// 時間の生成
	m_time = make_shared<TimeManager>();
	if (!m_time)
	{
		return E_FAIL;
	}

	// 時間計測の開始
	m_time->reset();

	// リソースの管理者の生成
	m_resourceManager = make_shared<ResourceManager>();
	if (!m_resourceManager)
	{
		return E_FAIL;
	}

	m_flameAndUpdraftPairs.clear();
	m_pushBlocks.clear();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneBase::uninit()
{
	m_impacts.clear();
	m_waterWheels.clear();
	m_rotateBlocks.clear();
	m_freezeEmitters.clear();
	m_freezeBlocks.clear();
	m_iceBlocks.clear();
	m_updraftEffectEmitters.clear();
	m_flameAndUpdraftPairs.clear();
	m_pushBlocks.clear();

	// オブジェクトの解放
	releaseObjects();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneBase::update()
{
	if (m_flags.isFlagSet(SCENE_FLAGS::IS_PAUSE))
	{
		return;
	}

	// 時間の更新
	m_time->update();

	// オブジェクトの更新
	updateObjects();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneBase::draw() const
{
#if ENABLED_SHADOW_MAP
	auto game = std::dynamic_pointer_cast<SceneGame>(std::const_pointer_cast<SceneBase>(shared_from_this()));
	if (!game)
	{
		auto debug = std::dynamic_pointer_cast<SceneDebug>(std::const_pointer_cast<SceneBase>(shared_from_this()));
		if (!debug)
		{
			for (const auto& objetcs : m_objects)
			{
				for (const auto& object : objetcs)
				{
					// 描画処理
					object->draw();
}
			}
			return;
		}
	}

	auto& gm = GM;
	auto device = gm.getDevice();
	auto effect = gm.getRenderer().getEffect();

	// ライトのマトリックスを取得
	Matrix mLight = m_objects[static_cast<uint32_t>(SceneBase::BASE_PRIORITY::LIGHT)].front()->downcast<Light3D>()->getMatrix();

	// プロジェクションマトリックスの情報を取得
	Matrix mtxProjecttion;
	device->GetTransform(D3DTS_PROJECTION, &mtxProjecttion);

	for (const auto& objetcs : m_objects)
	{
		for (const auto& object : objetcs)
		{
			if (auto model = object->downcast<Model>())
			{
				effect->BeginPass(0);

				D3DXMATRIX mtxWLP = DEF_MTX;
				D3DXMatrixMultiply(&mtxWLP, model->getMatrix().get(), &mLight);
				D3DXMatrixMultiply(&mtxWLP, &mtxWLP, &mtxProjecttion);
				effect->SetMatrix("mWLP", &mtxWLP);
				effect->CommitChanges();

				// 描画処理
				object->draw();

				effect->EndPass();

			}
		}
	}
#else
	for (const auto& objetcs : m_objects)
	{
		for (const auto& object : objetcs)
		{
			// 描画処理
			object->draw();
		}
	}
#endif
}

//---------------------------------------------------
// 描画
//---------------------------------------------------
void SceneBase::drawWithShadow() const
{
	auto game = std::dynamic_pointer_cast<SceneGame>(std::const_pointer_cast<SceneBase>(shared_from_this()));
	if (!game)
	{
		auto debug = std::dynamic_pointer_cast<SceneDebug>(std::const_pointer_cast<SceneBase>(shared_from_this()));
		if (!debug)
		{
			return;
		}
	}

	auto& gm = GM;
	auto& renderer = gm.getRenderer();
	auto device = renderer.getDevice();
	auto effect = renderer.getEffect();

	// 視点
	Vec3 posV =m_objects[static_cast<uint32_t>(BASE_PRIORITY::CAMERA)].front()->downcast<Camera>()->getPosV();

	// ライトのマトリックスを取得
	auto light = m_objects[static_cast<uint32_t>(SceneBase::BASE_PRIORITY::LIGHT)].front()->downcast<Light3D>();
	Matrix mLight = light->getMatrix();

	// ビューマトリックスの情報を取得
	Matrix mtxView;
	device->GetTransform(D3DTS_VIEW, &mtxView);

	// プロジェクションマトリックスの情報を取得
	Matrix mtxProjecttion;
	device->GetTransform(D3DTS_PROJECTION, &mtxProjecttion);

	Matrix mtxTexSpace;
	ZeroMemory(&mtxTexSpace, sizeof(Matrix));
	mtxTexSpace._11 = 0.5;
	mtxTexSpace._22 = -0.5;
	mtxTexSpace._33 = 1;
	mtxTexSpace._41 = 0.5;
	mtxTexSpace._42 = 0.5;
	mtxTexSpace._44 = 1;

	/*device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	device->SetRenderTarget(0, renderer.getBackBuf());
	device->SetDepthStencilSurface(renderer.getZBuf());

	effect->SetTechnique("tecRenderScene");
	effect->SetTexture("DepthTex", renderer.getDepthTexture());
	effect->Begin(NULL, 0);*/

	effect->BeginPass(0U);

	for (const auto& objetcs : m_objects)
	{
		for (const auto& object : objetcs)
		{
			if (auto object3d = object->downcast<Model>())
			{
				Matrix* mtxWorld = object3d->getMatrix().get();

				D3DXMATRIX mtxWCP = DEF_MTX;
				D3DXMatrixMultiply(&mtxWCP, mtxWorld, &mtxView);
				D3DXMatrixMultiply(&mtxWCP, &mtxWCP, &mtxProjecttion);
				effect->SetMatrix("mWCP", &mtxWCP);

				D3DXMATRIX mtxWLP = DEF_MTX;
				D3DXMatrixMultiply(&mtxWLP, mtxWorld, &mLight);
				D3DXMatrixMultiply(&mtxWLP, &mtxWLP, &mtxProjecttion);
				effect->SetMatrix("mWLP", &mtxWLP);

				D3DXMATRIX mtxWLPT = DEF_MTX;
				D3DXMatrixMultiply(&mtxWLPT, mtxWorld, &mLight);
				D3DXMatrixMultiply(&mtxWLPT, &mtxWLPT, &mtxProjecttion);
				D3DXMatrixMultiply(&mtxWLPT, &mtxWLPT, &mtxTexSpace);
				effect->SetMatrix("mWLPT", &mtxWLPT);

				effect->SetMatrix("mW", mtxWorld);

				Vec3 dir = light->getVLight() - light->getVLook();
				D3DXVec3Normalize(&dir, &dir);
				effect->SetVector("vLightDir", (D3DXVECTOR4*)&dir);

				effect->SetVector("vEye", (D3DXVECTOR4*)&posV);

				object3d->drawWithShadow();
			}

			// 描画処理
			//object->draw();
		}
	}

	effect->EndPass();

	//effect->End();
}

//---------------------------------------------------
// ポーズ
//---------------------------------------------------
void SceneBase::pause()
{
	m_flags.setFlag(SCENE_FLAGS::IS_PAUSE);
}

//---------------------------------------------------
// 再開
//---------------------------------------------------
void SceneBase::resume()
{
	m_flags.clearFlag(SCENE_FLAGS::IS_PAUSE);
}

//---------------------------------------------------
// ステージの読み込み処理
//---------------------------------------------------
void SceneBase::loadObjects(const char* inFilename)
{
	// 読み込む
	json j = File::Json::load(inFilename);

	// オブジェクトの追加
	auto addObjects = [this, &j](const std::string& inFactoryName)
	{
		auto it = j.find(inFactoryName);
		if (it == j.end())
		{
			GM.cs([&]() { std::cout << "SceneBase Set => " << inFactoryName << ": Nothing" << std::endl; });
			return;
		}

		const json& objects = it.value();
		for (const auto& object : objects)
		{
			this->addObject(inFactoryName, object);
		}
	};

	// MEMO: プレイヤーの配置は 3d オブジェクトの中で一番最初にすること。でなければ、ブロック法線ベクトルが真逆になる。
	// MEMO: 透明度のあるオブジェクトは、最後に配置すること。描画順の関係で。

	// 配置
	addObjects(FACTORY_CAMERA);								// カメラ
	addObjects(FACTORY_LIGHT);								// ライト
	addObjects(FACTORY_WORLD);								// 世界
	addObjects(FACTORY_SNOW_EMITTER_2D);					// 雪を生み出す
	addObjects(FACTORY_SPRING_EMITTER_2D);					// 桜を生み出す

	addObjects(FACTORY_GOAL_EFFECT);						// ゴールエフェクト

	addObjects(FACTORY_OBJECT2D);							// 2D オブジェクト
	addObjects(FACTORY_SPAWN_POINT);						// スポーン地点
	addObjects(FACTORY_EVENT_POINT);						// イベント地点

	addObjects(FACTORY_PLAYER);								// プレイヤー
	addObjects(FACTORY_MODEL);								// モデル

	addObjects(FACTORY_NORMAL_BLOCK);						// 通常ブロック
	addObjects(FACTORY_DUMMY_BLOCK);						// ダミーブロック
	addObjects(FACTORY_FALL_BLOCK);							// 落ちるブロック
	addObjects(FACTORY_GOAL_BLOCK);							// ゴールブロック
	addObjects(FACTORY_PHYSICS_BLOCK);						// 物理挙動が特にあるブロック
	addObjects(FACTORY_CHANGE_STATE_BLOCK);					// 状態が変化するブロック
	addObjects(FACTORY_IMPACT_BLOCK);						// 衝撃を発生させるブロック
	addObjects(FACTORY_WOOD_BLOCK);							// 木ブロック
	addObjects(FACTORY_REPAIR_BLOCK);						// 修復されるブロック
	addObjects(FACTORY_PUSH_BLOCK);							// 押せるブロック
	addObjects(FACTORY_MOVE_BLOCK);							// 動くブロック
	addObjects(FACTORY_ROTATE_BLOCK);						// 回転するブロック	// static で動くオブジェクトは dynamic なオブジェクトよりも後に配置
	addObjects(FACTORY_WATER_WHEEL);						// 水車
	addObjects(FACTORY_BLAZING_EMITTER);					// めらめらブロックの生成器
	addObjects(FACTORY_FLAME);								// 炎
	addObjects(FACTORY_UPDRAFT);							// 上昇気流
	addObjects(FACTORY_UPDRAFT_EFFECT_EMITTER);				// 上昇気流のエフェクトの生成器
	addObjects(FACTORY_COIN);								// コイン
	addObjects(FACTORY_DISAPPEAR_BLOCK);					// 消えるブロック
	addObjects(FACTORY_FREEZE_WATER_DROP_BLOCK);			// 氷結した水雫ブロック
	addObjects(FACTORY_ICE_BLOCK);							// 氷ブロック
	addObjects(FACTORY_WATER_BLOCK);						// 水ブロック
	addObjects(FACTORY_WATERDROP_EMITTER);					// 水雫ブロックの生成器
	addObjects(FACTORY_FREEZE_WATER_DROP_BLOCK_EMITTER);	// 最初はフリーズしている水雫ブロックの生成器
	addObjects(FACTORY_SOUND_SOURCE);	// 音源

	addObjects(FACTORY_BLOCKSOBJECT);						// 複数のブロックによるオブジェクト
}

//---------------------------------------------------
// 追加
//---------------------------------------------------
shared_ptr<GameObject> SceneBase::addObject(const std::string& inType, const json& inData)
{
	// 生成
	auto object = GameObjectFactory::create(inType, shared_from_this(), inData);
	if (!object)
	{
		assert(false);
		return nullptr;
	}

	// プライオリティの取得
	uint32_t priority = object->getPriority();

	// 追加
	m_objects[priority].push_back(object);

	return object;
}

//---------------------------------------------------
// オブジェクトの解放
//---------------------------------------------------
void SceneBase::releaseObjects()
{
	// 逆順に解放
	for (auto objects = m_objects.rbegin(); objects != m_objects.rend(); ++objects)
	{
		for (auto object = objects->rbegin(); object != objects->rend();)
		{
			object->get()->uninit();
			object = std::list<std::shared_ptr<GameObject>>::reverse_iterator(objects->erase(std::next(object).base()));
		}
	}

	m_objects.clear();
}

//---------------------------------------------------
// オブジェクトの更新
//---------------------------------------------------
void SceneBase::updateObjects()
{
	// 各オブジェクトの更新
	for (const auto& objects : m_objects)
	{
		for (const auto& object : objects)
		{
			if (!object->getIsDestroy())
			{ // 破棄しない
				// 更新処理
				object->update();
			}
		}
	}

	// 破棄
	for (auto& objects : m_objects)
	{
		for (auto it = objects.begin(); it != objects.end();)
		{
			auto object = *it;
			if (object->getIsDestroy())
			{ // 破棄する
				// 終了処理
				object->uninit();

				// 破棄
				it = objects.erase(it);
			}
			else
			{ // 破棄しない
				++it;
			}
		}
	}
}

//---------------------------------------------------
// オブジェクトの描画
//---------------------------------------------------
void SceneBase::drawObjects() const
{
	for (const auto& objetcs : m_objects)
	{
		for (const auto& object : objetcs)
		{
			// 描画処理
			object->draw();
		}
	}
}

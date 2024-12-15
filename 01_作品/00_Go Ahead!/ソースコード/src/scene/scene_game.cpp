//--------------------------------------------------------------------------------
// 
// ゲームのシーン [scene_game.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_game.h"
#include "scene/scene_result.h"
// game_manager
#include "game_manager/game_manager.h"
// game_object
#include "game_object/snow_emitter_2d.h"
#include "game_object/spring_emitter_2d.h"
// // 3d
// // // item
#include "game_object/3d/item/coin.h"
// resource
#include "resource/sound.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneGame::SceneGame()
	: SceneBase()
	, m_isGoal(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneGame::init()
{
	// 親クラスの処理
	HRESULT hr = SceneBase::init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_stageFilename = "res\\stage\\stage000.json";
	}

	// サイズの変更
	resizeObjects(static_cast<uint32_t>(PRIORITY::MAX));

	// リソースの読み込み
	getResManager()->loadTextures("res\\list\\texture_list.json");
	getResManager()->loadmaterials("res\\list\\material_list.json");
	getResManager()->loadModels("res\\list\\model_list.json");

	// イベントの登録
	registerEvent();

	// オブジェクトの配置
	loadObjects(m_stageFilename.c_str());

	// 小鳥のさえずりの再生
	CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_000);
	CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_000_ADD);

	for (auto& source : m_soundSources)
	{
		source.second.m_isPlay = false;
		source.second.m_wasPlay = false;
		source.second.m_volume = 0.0f;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneGame::uninit()
{
	// 音声の停止
	CSoundManager::GetInstance().Stop(/*CSoundManager::LABEL::BGM_GAME*/);

	// 親クラスの処理
	SceneBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneGame::update()
{
	auto& inputManager = GM.getInputManager();
	auto& gamepad = inputManager.getGamepad();
	auto& keyboard = inputManager.getKeyboard();

	if (gamepad.getButtonPress(InputGamepad::BUTTON_TYPE::START) ||
		keyboard.getTrigger(DIK_P))
	{
		getFlags().changeFlag(SceneBase::SCENE_FLAGS::IS_PAUSE);
	}

	// 親クラスの処理
	SceneBase::update();
	if (getFlags().isFlagSet(SCENE_FLAGS::IS_PAUSE))
	{
		return;
	}

	// 音源の更新
	for (auto& source : m_soundSources)
	{
		if (!source.second.m_wasPlay && source.second.m_isPlay)
		{ // 前回未再生，かつ今回再生する
			auto& sm = CSoundManager::GetInstance();		// サウンドの管理者の取得
			sm.Play(source.first);							// 再生
		}
		else if (!source.second.m_wasPlay && !source.second.m_isPlay)
		{ // 前回も今回も未再生
			CSoundManager::GetInstance().Stop(source.first);	// 停止
		}

		source.second.m_wasPlay = source.second.m_isPlay;	// 前回の結果を更新
		source.second.m_isPlay = false;						// 今回の結果をリセット
		source.second.m_volume = 0.0f;						// 音量のリセット
	}

	// カメラとライトの設定
	GM.getRenderer().setCameraAndLight([this]() {
		getCamera()->setCamera();
		getObjects(static_cast<uint32_t>(SceneBase::BASE_PRIORITY::LIGHT)).front()->downcast<Light3D>()->setLightMatrix();
	});

	// コインの回転
	Coin::updateRot();

	auto& gm = GM;	// ゲームの管理者
	if (m_isGoal)
	{
		gm.cs([this]() { this->getSharedData()->setTime(getTime()->getTime()); });	// 時間の設定
		gm.getSceneManager().changeScene<SceneResult>();							// リザルトへ
	}

#ifdef _DEBUG
	// 画面遷移
	if (keyboard.getTrigger(DIK_RETURN))
	{ // ENTER キーが押された
		gm.cs([this]() { this->getSharedData()->setTime(getTime()->getTime()); });	// 時間の設定
		gm.getSceneManager().changeScene<SceneResult>();	// リザルトへ
	}
#endif // _DEBUG
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneGame::draw() const
{
	// 親クラスの処理
	SceneBase::draw();
}

//---------------------------------------------------
// 音源情報の設定
//---------------------------------------------------
void SceneGame::setSoundSourceInfo(const CSoundManager::LABEL& inLabel, const bool& inIsCollision, const float& inVolume)
{
	SoundSource& source = m_soundSources[inLabel];

	if (inIsCollision)
	{
		source.m_isPlay = true;
	}

	if (source.m_volume < inVolume)
	{
		source.m_volume = inVolume;
		CSoundManager::GetInstance().SetVolume(inLabel, source.m_volume);	// 音量の設定
	}
}

//---------------------------------------------------
// イベントの登録
//---------------------------------------------------
void SceneGame::registerEvent()
{
	// 毎シーンやらないと、登録したときの scene は既に破棄されている (修正済み: 引数にシーンを渡すようにした)

	auto& gm = GM;
	auto eventManager = getEventmanager();

	// カメラの距離
	{
		// カメラの距離が 600.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_600,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(600.0f); }
		);

		// カメラの距離が 560.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_550,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(550.0f); }
		);

		// カメラの距離が 500.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_500,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(500.0f); }
		);

		// カメラの距離が 450.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_450,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(450.0f); }
		);

		// カメラの距離が 400.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_400,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(400.0f); }
		);

		// カメラの距離が 350.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_350,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(350.0f); }
		);

		// カメラの距離が 300.0f になるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_300,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(300.0f); }
		);

		// カメラの距離がデフォルトになるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_DEF,
			[this](std::any inData)
			{
				auto camera = std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera();
				camera->setDestDistance(camera->getDestDistanceKeep());
			}
		);

		// カメラの距離が短くなるイベント
		eventManager->registerEvent(
			EVENTNAME_CAMERA_DISTANCE_SHORT,
			[this](std::any inData) { std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(50.0f); }
		);
	}

	// BGM 切替
	{
		// 音楽の再生イベント
		eventManager->registerEvent(
			EVENTNAME_CHANGE_BGM_000_TO_001,
			[this, &gm](std::any)
			{
				//CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001_ADD);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_000);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_000_ADD);

				gm.getRenderer().setDestBgColor(BG_EVENING_COL);
			}
		);

		// 音楽の停止イベント
		eventManager->registerEvent(
			EVENTNAME_CHANGE_BGM_001_TO_000,
			[this, &gm](std::any)
			{
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_000);
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_000_ADD);
				//CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001_ADD);

				gm.getRenderer().setDestBgColor(BG_COL);
			}
		);
	}

	// 降雪
	{
		// 開始
		eventManager->registerEvent(
			EVENTNAME_START_SNOW,
			[this](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				auto game = std::dynamic_pointer_cast<SceneGame>(scene);
				if (!game)
				{
					return;
				}

				auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
				if (objects.empty())
				{
					return;
				}

				for (auto& element : objects)
				{
					auto emitter = element->downcast<SnowEmitter2D>();
					if (emitter)
					{
						if (!emitter->getIsEnabled())
						{
							emitter->setIsEnabled(true);
						}
						break;
					}
				}
			}
		);

		// 終了
		eventManager->registerEvent(
			EVENTNAME_END_SNOW,
			[this](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				auto game = std::dynamic_pointer_cast<SceneGame>(scene);
				if (!game)
				{
					return;
				}

				auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
				if (objects.empty())
				{
					return;
				}

				for (auto& element : objects)
				{
					auto emitter = element->downcast<SnowEmitter2D>();
					if (emitter)
					{
						if (emitter->getIsEnabled())
						{
							emitter->setIsEnabled(false);
						}
						break;
					}
				}
			}
		);
	}

	// 桜
	{
		// 開始
		eventManager->registerEvent(
			EVENTNAME_START_SPRING,
			[this](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				auto game = std::dynamic_pointer_cast<SceneGame>(scene);
				if (!game)
				{
					return;
				}

				auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
				if (objects.empty())
				{
					return;
				}

				for (auto& element : objects)
				{
					auto emitter = element->downcast<SpringEmitter2D>();
					if (emitter)
					{
						if (!emitter->getIsEnabled())
						{
							emitter->setIsEnabled(true);
						}
						break;
					}
				}
			}
		);

		// 終了
		eventManager->registerEvent(
			EVENTNAME_END_SPRING,
			[this](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				auto game = std::dynamic_pointer_cast<SceneGame>(scene);
				if (!game)
				{
					return;
				}

				auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
				if (objects.empty())
				{
					return;
				}

				for (auto& element : objects)
				{
					auto emitter = element->downcast<SpringEmitter2D>();
					if (emitter)
					{
						if (emitter->getIsEnabled())
						{
							emitter->setIsEnabled(false);
						}
						break;
					}
				}
			}
		);
	}

	// ステージ変更
	{
		// 05 to 06
		eventManager->registerEvent(
			EVENTNAME_STAGE_05_TO_06,
			[this, &gm, &eventManager](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_002);
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_002_ADD);
				//CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001_ADD);

				gm.getRenderer().setDestBgColor(BG_COL);

				{
					auto game = std::dynamic_pointer_cast<SceneGame>(scene);
					if (!game)
					{
						return;	// 下の MEMO 必読
					}

					auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
					if (objects.empty())
					{
						return;	// 下の MEMO 必読
					}

					// 雪終了
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SnowEmitter2D>();
						if (emitter)
						{
							if (emitter->getIsEnabled())
							{
								emitter->setIsEnabled(false);
							}
							break;
						}
					}

					// 桜開始
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SpringEmitter2D>();
						if (emitter)
						{
							if (!emitter->getIsEnabled())
							{
								emitter->setIsEnabled(true);
							}
							break;
						}
					}
				}

				// MEMO: ここ以降の処理通らない可能性あり
				//       処理を追加するなら上の処理を return じゃなくてネスト深くしていくこと
			}
		);

		// 06 to 05
		eventManager->registerEvent(
			EVENTNAME_STAGE_06_TO_05,
			[this, &gm, &eventManager](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				//CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001_ADD);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_002);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_002_ADD);

				gm.getRenderer().setDestBgColor(BG_EVENING_COL);

				{
					auto game = std::dynamic_pointer_cast<SceneGame>(scene);
					if (!game)
					{
						return;	// 下の MEMO 必読
					}

					auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
					if (objects.empty())
					{
						return;	// 下の MEMO 必読
					}

					// 桜終了
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SpringEmitter2D>();
						if (emitter)
						{
							if (emitter->getIsEnabled())
							{
								emitter->setIsEnabled(false);
							}
							break;
						}
					}

					// 雪開始
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SnowEmitter2D>();
						if (emitter)
						{
							if (!emitter->getIsEnabled())
							{
								emitter->setIsEnabled(true);
							}
							break;
						}
					}
				}

				// MEMO: ここ以降の処理通らない可能性あり
				//       処理を追加するなら上の処理を return じゃなくてネスト深くしていくこと
			}
		);

		// winter to autumn
		eventManager->registerEvent(
			EVENTNAME_STAGE_H2_TO_007,
			[this, &gm, &eventManager](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				// カメラの距離を離す
				std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera()->setDestDistance(400.0f);

				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_HALLOWEEN);
				//CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_GAME_001_ADD);

				gm.getRenderer().setDestBgColor(BG_SUNSET_COL);

				{
					auto game = std::dynamic_pointer_cast<SceneGame>(scene);
					if (!game)
					{
						return;	// 下の MEMO 必読
					}

					auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
					if (objects.empty())
					{
						return;	// 下の MEMO 必読
					}

					// 雪終了
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SnowEmitter2D>();
						if (emitter)
						{
							if (emitter->getIsEnabled())
							{
								emitter->setIsEnabled(false);
							}
							break;
						}
					}
				}

				// MEMO: ここ以降の処理通らない可能性あり
				//       処理を追加するなら上の処理を return じゃなくてネスト深くしていくこと
			}
		);

		// autumn to winter
		eventManager->registerEvent(
			EVENTNAME_STAGE_007_TO_H2,
			[this, &gm, &eventManager](std::any inData)
			{
				auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
				if (!scene)
				{
					return;
				}

				// カメラの距離をデフォルトにする
				auto camera = std::any_cast<shared_ptr<SceneBase>>(inData)->getCamera();
				camera->setDestDistance(camera->getDestDistanceKeep());

				//CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001);
				CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_001_ADD);
				CSoundManager::GetInstance().Stop(CSoundManager::LABEL::BGM_HALLOWEEN);

				gm.getRenderer().setDestBgColor(BG_EVENING_COL);

				{
					auto game = std::dynamic_pointer_cast<SceneGame>(scene);
					if (!game)
					{
						return;	// 下の MEMO 必読
					}

					auto objects = game->getObjects(static_cast<uint32_t>(SceneGame::PRIORITY::EFFECT));
					if (objects.empty())
					{
						return;	// 下の MEMO 必読
					}

					// 雪開始
					for (auto& element : objects)
					{
						auto emitter = element->downcast<SnowEmitter2D>();
						if (emitter)
						{
							if (!emitter->getIsEnabled())
							{
								emitter->setIsEnabled(true);
							}
							break;
						}
					}
				}

				// MEMO: ここ以降の処理通らない可能性あり
				//       処理を追加するなら上の処理を return じゃなくてネスト深くしていくこと
			}
		);
	}

	// コインゾーンを見つけたか否か
	eventManager->registerEvent(
		EVENTNAME_FIND_COIN_ZONE,
		[this, &gm, &eventManager](std::any inData)
		{
			auto scene = std::any_cast<shared_ptr<SceneBase>>(inData);
			if (!scene)
			{
				return;
			}
#if USE_NEW_SHARED_DATA
			gm.lockCS();

			auto data = scene->getSharedData();
			data->m_isFindCoinZone = true;

			gm.unlockCS();
#endif
		}
	);
}

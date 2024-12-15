//--------------------------------------------------------------------------------
// 
// リザルトのシーン [scene_result.h\cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_result.h"
#include "scene/scene_title.h"
// game_manager
#include "game_manager/game_manager.h"
// game_object
// // 2d
#include "game_object/2d/game_object_2d.h"
// // 3d
// // // item
#include "game_object/3d/item/coin.h"
// resource
#include "resource/font.h"
// utlility
#include "utility/enum.h"
#include "utility/string.h"
#include "utility/file.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const std::string PARAM_OBJECT2D = R"({
			"TAG": "text_title",
			"PRIORITY": 9,
			"POS": {
				"x": 1000.0,
				"y": 200.0,
				"z": 0.0
			},
			"ROT": {
				"x": 0.0,
				"y": 0.0,
				"z": 0.0
			},
			"SIZE": {
				"x": 512.0,
				"y": 256.0,
				"z": 0.0
			},
			"COL": {
				"r": 1.0,
				"g": 1.0,
				"b": 1.0,
				"a": 1.0
			},
			"TEXKEY": "res\\texture\\todo_change.png",
			"ISTEXANIM": false
		})";

	const Color ANON_COL_PINK = Color(1.0f, 0.4196f, 0.9529f, 1.0f);
	const Color ANON_COL_YELLOW = Color(1.0f, 0.8039f, 0.3803f, 1.0f);

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneResult::SceneResult() : SceneBase()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneResult::init()
{
	// 親クラスの処理
	HRESULT hr = SceneBase::init();
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// サイズの変更
	resizeObjects(static_cast<uint32_t>(PRIORITY::MAX));

	// リソースの読み込み
	getResManager()->loadTextures("res\\list\\texture_list_result.json");

	// オブジェクトの配置
	loadObjects("res\\list\\result_set.json");
	getResManager()->loadmaterials("res\\list\\material_list.json");
	getResManager()->loadModels("res\\list\\model_list.json");

	auto& gm = GM;
	gm.lockCS();
	gm.getRenderer().setBgColor(BG_SUNSET_COL);
	gm.getRenderer().setDestBgColor(BG_SUNSET_COL);
	gm.unlockCS();

	// ベストタイムと最高コイン数の読み込み
	float bestTime = 0.0f;
	int bestCoin = 0;
	{
		json j = File::Json::load("res\\result\\best_result.json");
		bestTime = j["BEST_TIME"];
		bestCoin = j["BEST_COIN"];
	}

	// テキストの設定
	{ // 今回のタイムの設定
		gm.lockCS();

		// 時間の取得
		float time = getSharedData()->getTime();
		int min = 0;	// 分
		int sec = 0;	// 秒
		int firstDecimalPlace = 0;	// 小数点第 1 位
		int secondDecimalPlace = 0;	// 小数点第 2 位
		{
			int integer = static_cast<int>(time);
			min = integer / 60;
			sec = integer % 60;

			float diff = time - integer;
			diff *= 10.0f;
			firstDecimalPlace = static_cast<int>(diff);

			diff -= firstDecimalPlace;
			diff *= 10.0f;
			secondDecimalPlace = static_cast<int>(diff);

			if (firstDecimalPlace >= 6)
			{
				firstDecimalPlace -= 6;
				sec += 1;
			}

			if (min > 99)
			{
				min = 99;
				sec = 59;
				firstDecimalPlace = 5;
				secondDecimalPlace = 9;
			}
		}

		auto& fm = FontManager::getInstance();

		DWORD fmt = Enum::from(Font::FMT_FLAG::CENTER);

		std::string text = String::print(0, min, "：");
		if (sec < 10)
		{
			text = String::print(text, 0, sec);
		}
		else
		{
			text = String::print(text, sec);
		}

		auto& currentTime = fm.getFont(FontManager::FONT_TYPE::CURRENT_TIME);
		currentTime.setText(
			Vec2(920.0f, 450.0f),
			150,
			Color(1.0f, 1.0f, 1.0f, 1.0f),
			fmt,
			text);

		std::string ms = String::print("：", firstDecimalPlace, secondDecimalPlace);
		fm.getFont(FontManager::FONT_TYPE::CURRENT_TIME_MS).
			setText(
				Vec2(920.0f + 228.0f, 450.0f + 20.0f),
				75,
				Color(1.0f, 1.0f, 1.0f, 1.0f),
				fmt,
				ms);

		gm.unlockCS();

		// ベストタイムの更新
		if (time < bestTime)
		{
			bestTime = time;
		}
	}
	{ // ベストタイムの設定
		// 時間の取得
		float time = bestTime;
		int min = 0;	// 分
		int sec = 0;	// 秒
		int firstDecimalPlace = 0;	// 小数点第 1 位
		int secondDecimalPlace = 0;	// 小数点第 2 位
		{
			int integer = static_cast<int>(time);
			min = integer / 60;
			sec = integer % 60;

			float diff = time - integer;
			diff *= 10.0f;
			firstDecimalPlace = static_cast<int>(diff);
			if (firstDecimalPlace >= 6)
			{
				firstDecimalPlace -= 6;
				sec += 1;
			}

			diff -= firstDecimalPlace;
			diff *= 10.0f;
			secondDecimalPlace = static_cast<int>(diff);
		}

		auto& fm = FontManager::getInstance();

		DWORD fmt = Enum::from(Font::FMT_FLAG::CENTER);

		std::string text = String::print(0, min, "：");
		if (sec < 10)
		{
			text = String::print(text, 0, sec);
		}
		else
		{
			text = String::print(text, sec);
		}

		gm.lockCS();

		fm.getFont(FontManager::FONT_TYPE::BEST_TIME).
			setText(
				Vec2(1020.0f, 215.0f),
				80,
				ANON_COL_PINK,
				fmt,
				text);

		std::string ms = String::print("：", firstDecimalPlace, secondDecimalPlace);
		fm.getFont(FontManager::FONT_TYPE::BEST_TIME_MS).
			setText(
				Vec2(1020.0f + 120.0f, 215.0f + 16.0f),
				40,
				ANON_COL_PINK,
				fmt,
				ms);

		gm.unlockCS();
	}

	// コイン
	{
		gm.lockCS();

		int numCoin = getSharedData()->getNumCoin();

		auto& fm = FontManager::getInstance();

		DWORD fmt = Enum::from(Font::FMT_FLAG::CENTER);

		// ベストコインの枚数を更新
		if (numCoin > bestCoin)
		{
			bestCoin = numCoin;
		}

		fm.getFont(FontManager::FONT_TYPE::CURRENT_COIN).
			setText(
				Vec2(300.0f, 300.0f),
				150,
				Color(1.0f, 1.0f, 1.0f, 1.0f),
				fmt,
				String::print(numCoin, "枚"));

		fm.getFont(FontManager::FONT_TYPE::BEST_COIN).
			setText(
				Vec2(150.0f, 540.0f),
				75,
				ANON_COL_YELLOW,
				fmt,
				String::print(bestCoin, "枚"));

		gm.unlockCS();
	}

	// ベストタイムと最高コイン数の書き出し
	{
		json j;
		j["BEST_TIME"] = bestTime;
		j["BEST_COIN"] = bestCoin;
		File::Json::save("res\\result\\best_result.json", j);
	}

	{
		json param = json::parse(PARAM_OBJECT2D);

		param[OBJECT2D_PARAM_TEXKEY] = "res\\texture\\result_wave.png";
		param[FLAG_POS] = Vec3(SCREEN_WIDTH * HALF, SCREEN_HEIGHT * HALF, 0.0f);
		param[OBJECT2D_PARAM_SIZE] = Vec3(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f);
		m_object2ds[RESULT_OBJECT2D_TYPE::BG] = addObject(FACTORY_OBJECT2D, param)->downcast<GameObject2D>();

		m_object2ds[RESULT_OBJECT2D_TYPE::BG].lock()->setTexCoord(
			Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 0.5625f), Vec2(1.0f, 0.5625f));

		param[OBJECT2D_PARAM_TEXKEY] = "res\\texture\\best_time.png";
		param[FLAG_POS] = Vec3(1050.0f, 200.0f, 0.0f);
		param[OBJECT2D_PARAM_SIZE] = Vec3(359.0f, 311.0f, 0.0f);
		m_object2ds[RESULT_OBJECT2D_TYPE::BEST_TIME] = addObject(FACTORY_OBJECT2D, param)->downcast<GameObject2D>();

		param[OBJECT2D_PARAM_TEXKEY] = "res\\texture\\best_coin.png";
		param[FLAG_POS] = Vec3(150.0f, 500.0f, 0.0f);
		param[OBJECT2D_PARAM_SIZE] = Vec3(264.0f, 230.0f, 0.0f);
		m_object2ds[RESULT_OBJECT2D_TYPE::BEST_COIN] = addObject(FACTORY_OBJECT2D, param)->downcast<GameObject2D>();
	}

	// 共有データの書き出し
	{
#if USE_NEW_SHARED_DATA
		json j = File::Json::load("res\\shared_data\\000_num.json");
		int num = j["NUM"];

		j["NUM"] = num + 1;
		File::Json::save("res\\shared_data\\000_num.json", j);

		{
			gm.lockCS();
			auto data = getSharedData();

			json k = File::Json::load("res\\shared_data\\shared_data.json");
			json& l = k["SharedData"];

			json m;
			m["IDX"] = num;
			m["NUM_IMPACT"] = data->m_numImpact;
			m["TIME_FLAME_PLAYER"] = data->m_timeFlamePlayer;
			m["IS_BREAK_SNOWMAN"] = data->m_isBreakSnowman;
			m["TIME_ON_ICE"] = data->m_timeOnIce;
			m["TIME_UNDER_WATER"] = data->m_timeUnderWater;
			m["TIME_TOUCH_PUSH"] = data->m_timeTouchPush;
			m["MAX_ROT_VELOCITY"] = Vec3(data->m_maxRotVelocity.getX(), data->m_maxRotVelocity.getY(), data->m_maxRotVelocity.getZ());
			m["TIME_GOAL"] = data->m_time;
			m["FIND_COIN_ZONE"] = data->m_isFindCoinZone;
			m["JUMP_NUM"] = data->m_numJump;

			l.push_back(m);

			File::Json::save("res\\shared_data\\shared_data.json", k);

			gm.unlockCS();
		}
#endif

		// 共有データのリセット
		getSharedData()->reset();
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneResult::uninit()
{
	// 親クラスの処理
	SceneBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneResult::update()
{
	// 親クラスの処理
	SceneBase::update();

	auto& gm = GM;									// ゲームの管理者
	auto& inputManager = gm.getInputManager();		// 入力処理の管理者
	auto& keyboard = inputManager.getKeyboard();	// キーボード
	auto& gamePad = inputManager.getGamepad();		// ゲームパッド

	// コインの回転
	Coin::updateRot();

	if (auto bg = m_object2ds[RESULT_OBJECT2D_TYPE::BG].lock())
	{
		bg->updateTexCoord(Vec2(0.0f, 0.005f));
	}

	// 画面遷移
	gm.lockCS();
	if (gm.getSceneManager().getSceneNum() == ONLY_ONE)
	{ // 現在のシーンのみ
		if (keyboard.getTrigger(DIK_RETURN) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::A) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::B) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::START))
		{ // ENTER キーが押された
			gm.getSceneManager().changeScene<SceneTitle>();	// タイトルへ
		}
	}
	gm.unlockCS();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneResult::draw() const
{
	// 親クラスの処理
	SceneBase::draw();

	// テキストの描画
	FontManager::getInstance().draw();
}

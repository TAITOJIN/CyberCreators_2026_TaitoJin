//--------------------------------------------------------------------------------
// 
// タイトルのシーン [scene_title.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_title.h"
#include "scene/scene_game.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/random_generator.h"
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
	constexpr float APPEAR_TIME = 5.0f;
	constexpr float APPEAR_SPEED = 400.0f;

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
SceneTitle::SceneTitle()
	: SceneBase()
	, m_timeCameraAnim(0.0f)
	, m_intervalCameraAnim(0.0f)
	, m_playerPos(DEF_VEC3)
	, m_playerMove(DEF_VEC3)
	, m_isDispNormal(true)
	, m_playerAppearTimeCounter(0.0f)
	, m_playerAnimTimeCounter(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT SceneTitle::init()
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
	getResManager()->loadTextures("res\\list\\texture_list_title.json");
	getResManager()->loadmaterials("res\\list\\material_list.json");
	getResManager()->loadModels("res\\list\\model_list.json");

	// オブジェクトの配置
	loadObjects("res\\list\\title_set.json");

	auto& gm = GM;
	gm.lockCS();
	gm.getRenderer().setBgColor(BG_COL);
	gm.getRenderer().setDestBgColor(BG_COL);
	gm.unlockCS();

	m_playerPos = Vec3(SCREEN_WIDTH + 200.0f, SCREEN_HEIGHT + 200.0f, 0.0f);

	{
		json param = json::parse(PARAM_OBJECT2D);
		param[OBJECT2D_PARAM_ROT] = Vec3(0.0f, 0.0f, D3DX_PI * HALF * HALF);
		param[OBJECT2D_PARAM_SIZE] = Vec3(150.0f, 327.0f, 0.0f);
		param[FLAG_POS] = m_playerPos;

		param[OBJECT2D_PARAM_TEXKEY] = "res\\texture\\normal_player.png";
		m_player = addObject(FACTORY_OBJECT2D, param)->downcast<GameObject2D>();
	}

	CSoundManager::GetInstance().Play(CSoundManager::LABEL::BGM_GAME_000);

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void SceneTitle::uninit()
{
	CSoundManager::GetInstance().Stop();

	// 親クラスの処理
	SceneBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void SceneTitle::update()
{
	// 親クラスの処理
	SceneBase::update();

	auto& gm = GM;									// ゲームの管理者
	auto& inputManager = gm.getInputManager();		// 入力処理の管理者
	auto& keyboard = inputManager.getKeyboard();	// キーボード
	auto& gamePad = inputManager.getGamepad();		// ゲームパッド
	//float deltaTime = getDeltaTime();				// デルタタイム

	m_timeCameraAnim += getDeltaTime();
	if (m_timeCameraAnim >= m_intervalCameraAnim)
	{
		float distance = RandomGenerator::get(304.0f, 334.0f);
		m_timeCameraAnim = 0.0f;
		m_intervalCameraAnim = RandomGenerator::get(3.0f, 5.0f);

		getCamera()->setDestDistance(distance);
	}

	// 画面遷移
	gm.lockCS();
	if (gm.getSceneManager().getSceneNum() == ONLY_ONE)
	{ // 現在のシーンのみ
		if (keyboard.getTrigger(DIK_RETURN) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::A) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::B) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::START))
		{ // ENTER キーが押された，もしくは左クリックされた
			gm.getSceneManager().changeScene<SceneGame>();	// ゲームへ
		}
	}
	gm.unlockCS();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void SceneTitle::draw() const
{
	// 親クラスの処理
	SceneBase::draw();
}

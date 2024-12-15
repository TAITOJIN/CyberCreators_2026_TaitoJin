//--------------------------------------------------------------------------------
// 
// 衝撃を発生させるブロック [block_impact.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_impact.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"
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
	constexpr float RESTART_HEIGHT = 100.0f;	// リスタートになる高さ (MIN_WORLD_Y + この数値)
	constexpr float DO_IMPACT_TIME = 1.5f;		// 爆発する時間
	constexpr const char* PARAM_IMPACT = R"({
			"PRIORITY": 1,
			"TAG": "impact_0"
		})";
	constexpr const char* PARAM_SMOKE = R"({
				"TAG": "smoke_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0001",
				"IS_STATIC": true,
				"KEY": "res\\model\\block000.x",
				"MAT": [
					{
						"MAT_KEY": "SMOKE",
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
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				},
				"DROP_GRAVITY": 3.0
			})";
	constexpr const char* PARAM_SMALL_PIECES = R"({
				"TAG": "pieces_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0001",
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
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				},
				"SCALE": {
					"x": 2.0,
					"y": 2.0,
					"z": 2.0
				},
				"MOVE": {
					"x": 0.0,
					"y": 0.0,
					"z": 0.0
				}
			})";

	constexpr int NUM_SMOKE = 20;				// 煙の数
	constexpr float RAND_POS_MIN = -10.0f;		// 位置の補正 (最小値)
	constexpr float RAND_POS_MAX = 10.0f;		// 位置の補正 (最大値)
	constexpr float RAND_SCALE_MIN = 0.5f;		// スケール (最小値)
	constexpr float RAND_SCALE_MAX = 1.0f;		// スケール (最大値)
	constexpr float RAND_LIFE_MIN = 30.0f;		// 寿命 (最小値)
	constexpr float RAND_LIFE_MAX = 60.0f;		// 寿命 (最大値)
	constexpr float ADJUST_MILLSECOND = 0.01f;	// ミリ秒に補正
	constexpr float RAND_GRAVITY_MIN = 1.0f;	// 重力 (最小値)
	constexpr float RAND_GRAVITY_MAX = 10.0f;	// 重力 (最大値)

	constexpr int NUM_PIECES = 50;						// 粉々の数
	constexpr float PIECES_RAND_SCALE_MIN = 0.1f;		// スケール (最小値)
	constexpr float PIECES_RAND_SCALE_MAX = 0.25f;		// スケール (最大値)
	constexpr float PIECES_RAND_LIFE_MIN = 60.0f;		// 寿命 (最小値)
	constexpr float PIECES_RAND_LIFE_MAX = 120.0f;		// 寿命 (最大値)
	constexpr float PIECES_ADJUST_MILLSECOND = 0.01f;	// ミリ秒に補正
	constexpr float PIECES_RAND_MOVE_MIN = -5.0f;		// 移動量 (最小値)
	constexpr float PIECES_RAND_MOVE_MAX = 5.0f;		// 移動量 (最大値)
	constexpr float PIECES_RAND_MOVE_Y_MIN = -1.0f;		// 移動量 (最小値)
	constexpr float PIECES_RAND_MOVE_Y_MAX = 1.0f;		// 移動量 (最大値)

	constexpr const char* PARAM_BLAZING_EMITTER = R"({
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
			"TAG": "barrel_blazing_emitter_0",
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
				})";

	constexpr int NUM_EMITTER = 5;				// 粉々の数
	constexpr float BLOCK_SIZE = 20.0f;			// ブロックのサイズ
	constexpr float ADJUST_BLAZING_POS = 1.5f;	// 煙の位置の調整 (倍率)

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockImpact::BlockImpact()
	: BlockBase()
	, m_isEnabled(false)
	, m_time(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockImpact::BlockImpact(BlockImpact&& right) noexcept
	: BlockBase(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockImpact::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		auto scene = getScene();							// シーン
		json param = json::parse(PARAM_BLAZING_EMITTER);	// パラメータ

		// オフセット
		Vec3 offset = DEF_VEC3;
		{
			// サイズ
			auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
			Vec3 scale = getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;

			offset = Vec3(0.0f, size.y + BLOCK_SIZE * PIECES_RAND_SCALE_MAX * ADJUST_BLAZING_POS, 0.0f);
		}

		// めらめらの生成器の設定
		for (int i = 0; i < NUM_EMITTER; i++)
		{
			// オフセットの調整
			Vec3 adjustOffset = DEF_VEC3;
			static constexpr float ADJUST_OFFSET_MIN = -5.0f;
			static constexpr float ADJUST_OFFSET_MAX = 5.0f;
			adjustOffset.x = RandomGenerator::get(ADJUST_OFFSET_MIN, ADJUST_OFFSET_MAX);
			adjustOffset.y = RandomGenerator::get(ADJUST_OFFSET_MIN, ADJUST_OFFSET_MAX);
			adjustOffset.z = RandomGenerator::get(ADJUST_OFFSET_MIN, ADJUST_OFFSET_MAX);

			// 間隔
			static constexpr float INTERVAL_MIN = 0.2f;
			static constexpr float INTERVAL_MAX = 0.8f;
			float interval = RandomGenerator::get(INTERVAL_MIN, INTERVAL_MAX);

			// スケール
			static constexpr float SCALE_MIN = 0.1f;
			static constexpr float SCALE_MAX = 0.3f;
			float scale = RandomGenerator::get(SCALE_MIN, SCALE_MAX);

			// 寿命
			static constexpr float LIFE_MIN = 50.0f;
			static constexpr float LIFE_MAX = 100.0f;

			// 移動速度
			static constexpr float GRAVITY_MIN = 2.5f;
			static constexpr float GRAVITY_MAX = 3.0f;
			float gravity = RandomGenerator::get(GRAVITY_MIN, GRAVITY_MAX);

			// パラメータの設定
			param[FLAG_OFFSET] = offset + adjustOffset;
			param[WATERDROP_EMITTER_PARAM_SETINTERVAL] = interval;
			param[WATERDROP_EMITTER_PARAM_SCALE] = scale;
			param[WATERDROP_EMITTER_PARAM_MS_MIN_LIFE] = LIFE_MIN;
			param[WATERDROP_EMITTER_PARAM_MS_MAX_LIFE] = LIFE_MAX;
			param[WATERDROP_EMITTER_PARAM_GRAVITY] = gravity;

			// オブジェクトの追加
			auto emitter = scene->addObject(FACTORY_BLAZING_EMITTER, param);

			// 親マトリックスの設定
			emitter->downcast<BlockBlazingEmitter>()->setMtxParent(getMatrix());

			// 追加
			m_emitters.push_back(emitter->downcast<BlockBlazingEmitter>());
		}
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockImpact::uninit()
{
	// めらめらの生成器の破棄
	for (auto& element : m_emitters)
	{
		auto emitter = element.lock();
		if (!emitter)
		{
			continue;
		}
		emitter->setIsDestroy(true);
	}

	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockImpact::update()
{
	if (m_isEnabled)
	{ // 有効だった
		auto scene = getScene<SceneBase>();
		float deltaTime = scene->getDeltaTime();

		m_time += deltaTime;
		if (m_time >= DO_IMPACT_TIME)
		{
			CSoundManager::GetInstance().Play(CSoundManager::LABEL::SE_EXPLOSION);

			// 自分自身の破棄
			setIsDestroy(true);

#if USE_NEW_SHARED_DATA
			// 衝撃を起こした回数を加算
			auto& gm = GM;
			gm.lockCS();
			auto data = scene->getSharedData();
			data->m_numImpact++;
			gm.unlockCS();
#endif

			// 衝撃を発生させる
			{
				json param = json::parse(PARAM_IMPACT);
				param[FLAG_POS] = getPos();
				scene->addObject(FACTORY_IMPACT, param);
			}

			// 煙の生成
			for (int i = 0; i < NUM_SMOKE; i++)
			{
				Vec3 pos = getPos();
				float x = RandomGenerator::get(RAND_POS_MIN, RAND_POS_MAX);
				float y = RandomGenerator::get(RAND_POS_MIN, RAND_POS_MAX);
				float z = RandomGenerator::get(RAND_POS_MIN, RAND_POS_MAX);
				pos.x += x;
				pos.y += y;
				pos.z += z;

				float scale = RandomGenerator::get(RAND_SCALE_MIN, RAND_SCALE_MAX);

				json modelMaterial;
				modelMaterial[FLAG_MAT_NAME] = 0;
				modelMaterial[FLAG_MAT_KEY] = "SMOKE";

				// パラメータの設定
				json j = json::parse(PARAM_SMOKE);
				j[FLAG_POS] = pos;
				j[WATERDROP_PARAM_FIRST_SCALE] = scale;
				j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(RAND_LIFE_MIN, RAND_LIFE_MAX) * ADJUST_MILLSECOND;
				j[WATERDROP_PARAM_GRAVITY] = RandomGenerator::get(RAND_GRAVITY_MIN, RAND_GRAVITY_MAX);
				j[FLAG_MAT].push_back(modelMaterial);

				// オブジェクトの追加
				scene->addObject(FACTORY_IMPACT_SMOKE, j);
			}

			// 粉々の生成
			for (int i = 0; i < NUM_PIECES; i++)
			{
				Vec3 pos = getPos();

				float scale = RandomGenerator::get(PIECES_RAND_SCALE_MIN, PIECES_RAND_SCALE_MAX);

				Vec3 move = DEF_VEC3;
				move.x = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);
				move.y = RandomGenerator::get(PIECES_RAND_MOVE_Y_MIN, PIECES_RAND_MOVE_Y_MAX);
				move.z = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);

				// パラメータの設定
				json j = json::parse(PARAM_SMALL_PIECES);
				j[FLAG_POS] = pos;
				j[WATERDROP_PARAM_FIRST_SCALE] = scale;
				j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(PIECES_RAND_LIFE_MIN, PIECES_RAND_LIFE_MAX) * ADJUST_MILLSECOND;
				j[PARAM_BLOCK_SMALL_PIECES] = move;

				std::string a = j.dump();

				// オブジェクトの追加
				scene->addObject(FACTORY_SMALL_PIECES, j);
			}
		}
	}

#if 1
	if (getPos().y <= MIN_WORLD_Y + RESTART_HEIGHT)
	{ // 最低高度に到達した
		// 位置
		Vec3 firstPos = getFirstPos();
		setPos(firstPos);
		setRot(getFirstRot());
		createBlockRigidBody();
		//rigidBody->getWorldTransform().setOrigin(btVector3(firstPos.x, firstPos.y, firstPos.z));
	}
#endif	

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockImpact::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockImpact::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockImpact::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockImpact& BlockImpact::operator=(BlockImpact&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));
	}

	return *this;
}

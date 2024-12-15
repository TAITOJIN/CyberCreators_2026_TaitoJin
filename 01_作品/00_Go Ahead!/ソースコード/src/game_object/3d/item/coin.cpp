//--------------------------------------------------------------------------------
// 
// コイン [coin.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/item/coin.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_game.h"
// utility
#include "utility/calculate.h"
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
	constexpr float ROT_SPEED = 0.025f;					// コインの回転速度
	constexpr float ADJUST_MILLSECOND = 0.01f;			// ミリ秒に調整
	constexpr float WALK_EFFECT_INTERVAL = 0.2f;		// 間隔

	constexpr int NUM_PIECES = 20;						// 粉々の数
	constexpr float PIECES_RAND_SCALE_MIN = 0.1f;		// スケール (最小値)
	constexpr float PIECES_RAND_SCALE_MAX = 0.25f;		// スケール (最大値)
	constexpr float PIECES_RAND_LIFE_MIN = 40.0f;		// 寿命 (最小値)
	constexpr float PIECES_RAND_LIFE_MAX = 60.0f;		// 寿命 (最大値)
	constexpr float PIECES_ADJUST_MILLSECOND = 0.01f;	// ミリ秒に補正
	constexpr float PIECES_RAND_MOVE_MIN = -0.6f;		// 移動量 (最小値)
	constexpr float PIECES_RAND_MOVE_MAX = 0.6f;		// 移動量 (最大値)

	constexpr const char* PARAM_SMALL_PIECES = R"({
				"TAG": "pieces_0",
				"MASS": 0.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0001",
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

	constexpr const char* PARAM_SHADOW_BLOCK = R"({"TAG": "shadow_block_0","MASS": 0.0,"INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "PRIORITY": 6,"DUMMY_FLAGS": "0b0001","IS_STATIC": true,"KEY": "res\\model\\block000.x","MAT": [{"MAT_KEY": "GOAL_YELLOW","MAT_NAME": 0}],"OFFSET": {"x": 0.0,"y": 0.0,"z": 0.0},"POS": {"x": 0.0,"y": 0.0,"z": 0.0},"ROT": {"x": 0.0,"y": 0.0,"z": 0.0},"SCALE": {"x": 0.25,"y": 0.01,"z": 0.25}})";

	constexpr int NUM_SHADOW_PARTICLE = 5;

} // namespace /* anonymous */

//---------------------------------------------------
// 静的メンバの初期化
//---------------------------------------------------
Quaternion Coin::m_q = DEF_ROT;	// みんな同じ回転角

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Coin::Coin()
	: Model()
	, m_walkEffectIntervalCounter(0.0f)
	, m_isHidden(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Coin::Coin(Coin&& right) noexcept
	: Model(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Coin::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = Model::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Coin::uninit()
{
	// 親クラスの処理
	Model::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Coin::update()
{
	// 回転を合わせる
	setRot(m_q);
	getRigitBody().getWorldTransform().setRotation(btQuaternion(m_q.x, m_q.y, m_q.z, m_q.w));

	// プレイヤーとの当たり判定

	auto scene = getScene<SceneGame>();
	if (!scene)
	{
		if (auto body = getRigitBodyPtr())
		{
			body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);	// 当たり判定を無効化
		}

		// MEMO: デバッグ時などは当たり判定は行わないが、エディターなどで状態の更新を行うため親クラスの処理は呼ぶ
		Model::update();
		return;
	}

	const Player& player = *scene->getPlayer();								// プレイヤーの取得
	Collision::Sphere spherePlayer = { player.getPos(), 10.0f };			// プレイヤーの球状の当たり判定情報
	auto& aabb = scene->getResManager()->getResModel(getKey()).getAABB();	// AABB を取得
	Vec3 halfExtents = Collision::getHalfExtents(aabb, getScale());			// 各辺の半径を求める
	float radius = (halfExtents.x + halfExtents.y + halfExtents.z) / 3.0f;	// 半径を求める
	Collision::Sphere sphereCoin = { getPos(), radius };					// 球状の当たり判定情報

	if (Collision::isCollisionSphereVsSphere(spherePlayer, sphereCoin))
	{ // 衝突している
		// TODO: 取得エフェクト
		// コイン破棄
		setIsDestroy(true);

		// 取得音再生
		CSoundManager::GetInstance().Play(CSoundManager::LABEL::SE_GAME_COIN);

		// コインの数を加算
		GM.cs([&]()
			{
				auto sharedData = scene->getSharedData();
				sharedData->setNumCoin(sharedData->getNumCoin() + 1);
			}
		);

		// 粉々の生成
		for (int i = 0; i < NUM_PIECES; i++)
		{
			Vec3 pos = getPos();

			float scale = RandomGenerator::get(PIECES_RAND_SCALE_MIN, PIECES_RAND_SCALE_MAX);

			Vec3 move = DEF_VEC3;
			move.x = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);
			move.y = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);
			move.z = RandomGenerator::get(PIECES_RAND_MOVE_MIN, PIECES_RAND_MOVE_MAX);

			// パラメータの設定
			json j = json::parse(PARAM_SMALL_PIECES);
			j[FLAG_POS] = pos;
			j[WATERDROP_PARAM_FIRST_SCALE] = scale;
			j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(PIECES_RAND_LIFE_MIN, PIECES_RAND_LIFE_MAX) * ADJUST_MILLSECOND;
			j[PARAM_BLOCK_SMALL_PIECES] = move;

			// オブジェクトの追加
			scene->addObject(FACTORY_SMALL_PIECES, j);
		}

		// 影の破棄
		for (auto& shadow : m_shadowBlocks)
		{
			if (auto shadowBlock = shadow.lock())
			{
				shadowBlock->setIsDestroy(true);
			}
		}
	}

	// 影用のパーティクル更新処理
	Vec3 pos = getPos();
	{
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		pos.y -= size.y;
	}
	float deltaTime = scene->getDeltaTime();
	m_walkEffectIntervalCounter += deltaTime;
	if (m_walkEffectIntervalCounter >= WALK_EFFECT_INTERVAL)
	{
		//Vec3 pos = getPos();
		Vec3 adjustPos = pos;
		adjustPos.y -= 5.0f;

		m_walkEffectIntervalCounter = 0.0f;

		// 粉々の生成
		for (int i = 0; i < NUM_SHADOW_PARTICLE; i++)
		{
			float scale = RandomGenerator::get(PIECES_RAND_SCALE_MIN, PIECES_RAND_SCALE_MAX);

			Vec3 move = DEF_VEC3;
			move.x = RandomGenerator::get(-0.15f, 0.15f);
			move.y = RandomGenerator::get(0.f, 0.1f);
			move.z = RandomGenerator::get(-0.15f, 0.15f);

			// パラメータの設定
			json j = json::parse(PARAM_SMALL_PIECES);

			json modelMaterial;
			modelMaterial[FLAG_MAT_NAME] = 0;
			modelMaterial[FLAG_MAT_KEY] = "GOAL_YELLOW";

			j[FLAG_MAT].push_back(modelMaterial);

			j[FLAG_POS] = adjustPos;
			j[WATERDROP_PARAM_FIRST_SCALE] = scale;
			j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(120.0f, 180.0f) * ADJUST_MILLSECOND;
			j[PARAM_BLOCK_SMALL_PIECES] = move;

			// オブジェクトの追加
			scene->addObject(FACTORY_SMALL_PIECES, j);
		}
	}

#if 1
	// 影の更新
	for (auto& shadow : m_shadowBlocks)
	{
		//Vec3 pos = getPos();
		Vec3 hitPos = pos, nor = DEF_VEC3;
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		hitPos = pos;
		hitPos.y -= size.y;
		//hitPos.y -= 0.1f;

		if (auto shadowBlock = shadow.lock())
		{
			shadowBlock->setPosParent(hitPos);
		}
	}
#endif

	// 親クラスの処理
	Model::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Coin::draw() const
{
	// 親クラスの処理
	Model::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Coin::inspector()
{
	// 親クラスの処理
	Model::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Coin::save(json& outObject) const
{
	// 親クラスの処理
	Model::save(outObject);
}

//---------------------------------------------------
// コインの回転の更新処理
//---------------------------------------------------
void Coin::updateRot()
{
	Calculate::mulRot(&m_q, AXIS_Y, ROT_SPEED);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Coin& Coin::operator=(Coin&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		Model::operator=(std::move(right));
	}

	return *this;
}

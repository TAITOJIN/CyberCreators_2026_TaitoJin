//--------------------------------------------------------------------------------
// 
// 水ブロック [block_water.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_water.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/collision.h"
#include "utility/calculate.h"
#include "utility/random_generator.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float ADJUST_MILLSECOND = 0.01f;			// ミリ秒に調整

	constexpr int NUM_PIECES = 5;						// 粉々の数
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

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockWater::BlockWater()
	: BlockDummy()
	, m_wasCollision(false)
	, m_isSetCoolTime(false)
	, m_coolTime(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockWater::BlockWater(BlockWater&& right) noexcept
	: BlockDummy(std::move(right))
	, m_wasCollision(std::move(m_wasCollision))
	, m_isSetCoolTime(std::move(m_isSetCoolTime))
	, m_coolTime(std::move(m_coolTime))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockWater::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockWater::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockWater::update()
{
	// シーン情報の取得
	auto scene = getScene<SceneBase>();

	bool isTouch = false;	// 触れているか否か
	bool isUnder = false;	// 水面下

	// 水の当たり判定情報
	Collision::Rectangle waterRect;	// 水の矩形情報
	float waterY = 0.0f;			// 高さの原点
	float waterHeight = 0.0f;		// 高さ
	{
		// 中心位置
		Vec3 pos = getPos();
		waterRect.center = Vec2(pos.x, pos.z);

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		waterRect.halfExtend = Vec2(size.x, size.z);

		// 回転値
		Quaternion rot = getRot();
		Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
		waterRect.rot = euler.y;

		// 高さ (位置)
		waterY = pos.y;

		// 高さ (大きさ)
		waterHeight = size.y;
	}

	// プレイヤーとの当たり判定
	{
		// プレイヤーの当たり判定情報
		Collision::Rectangle playerRect;	// プレイヤーの矩形の情報
		auto player = scene->getPlayer();	// プレイヤー
		float playerY = 0.0f;				// 高さの原点
		float playerHeight = 0.0f;			// 高さ
		{
			// 中心位置
			Vec3 pos = player->getPos();
			playerRect.center = Vec2(pos.x, pos.z);

			// 半径
			auto aabb = scene->getResManager()->getResModel(player->getKey()).getAABB();
			Vec3 scale = player->getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;
			playerRect.halfExtend = Vec2(size.x, size.z);

			// 回転値
			Quaternion rot = player->getRot();
			Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
			playerRect.rot = euler.y;

			// 高さ (位置)
			playerY = pos.y;

			// 高さ (大きさ)
			playerHeight = size.y * HALF;
		}
		bool wasTouch = player->getWasTouchWater();
		//bool wasUnder = player->getWasUnderWater();

		// TODO: 中心点じゃなくて原点渡して高さで調整しているが、本来は原点ではなく原点を高さで補正して中心点を渡すべき

		// プレイヤーと水との当たり判定 (触れているかどうか)
		if (Collision::isCollisionBlockVsBlock(waterRect, playerRect, waterY, playerY, waterHeight, playerHeight))
		{ // プレイヤーの原点 (中心点) が水ブロック内にある
			isTouch = true;
			player->setIsTouchWater(true);
		}

		// MEMO: こっちは中心点に調整済み？

		// プレイヤーが水にがっつり浸っているか (高さを少し余裕を持たせる)
		if (Collision::isCollisionBlockVsBlock(waterRect, playerRect, waterY, playerY + playerHeight, waterHeight, playerHeight - 1.0f))
		{ // プレイヤーの頭の先まで水に浸かっている
			isUnder = true;
			player->setIsUnderWater(true);
		}

		if (!wasTouch && isTouch)
		{ // 着水
			// 水しぶき
			splashOfWater();
		}

#if 0
		if (wasTouch && !isTouch && !isUnder)
		{ // 離水
			// 水しぶき
			splashOfWater();
		}
#endif
	}

	// 押せるブロックとの当たり判定
	for(auto& weakBlock : scene->getPushBlocks())
	{
		auto block = weakBlock.lock();
		if (!block)
		{ // 空
			continue;
		}

		// 押せるブロックの矩形の当たり判定情報
		Collision::Rectangle pushRect = block->getRect();	// 矩形情報
		float pushY = block->getPosY();						// 高さの原点
		float pushHeight = block->getHeight();				// 高さ

		// TODO: 中心点じゃなくて原点渡して高さで調整しているが、本来は原点ではなく原点を高さで補正して中心点を渡すべき

		// ブロックと水との当たり判定 (触れているかどうか)
		if (Collision::isCollisionBlockVsBlock(waterRect, pushRect, waterY, pushY, waterHeight, pushHeight))
		{
			isTouch = true;
			block->setIsTouchWater(true);
		}

		// MEMO: こっちは中心点に調整済み？

		// ブロックが水にがっつり浸っているか (高さを少し余裕を持たせる)
		if (Collision::isCollisionBlockVsBlock(waterRect, pushRect, waterY, pushY + pushHeight, pushHeight, pushHeight - 1.0f))
		{
			block->setIsUnderWater(true);
		}
	}

	// 当たり判定情報を更新
	m_wasCollision = isTouch;

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockWater::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockWater::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockWater::save(json& outObject) const
{
	// 親クラスの処理
	BlockDummy::save(outObject);
}

//---------------------------------------------------
// 水しぶき
//---------------------------------------------------
void BlockWater::splashOfWater()
{
	auto scene = getScene();
	Vec3 pos = scene->getPlayer()->getPos();

	// 粉々の生成
	for (int i = 0; i < NUM_PIECES; i++)
	{
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
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockWater& BlockWater::operator=(BlockWater&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));

		m_wasCollision = std::move(right.m_wasCollision);
		m_isSetCoolTime = std::move(right.m_isSetCoolTime);
		m_coolTime = std::move(right.m_coolTime);
	}

	return *this;
}

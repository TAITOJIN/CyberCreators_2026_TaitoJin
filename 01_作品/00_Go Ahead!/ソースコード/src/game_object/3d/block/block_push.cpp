//--------------------------------------------------------------------------------
// 
// 押せるブロック [block_push.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_push.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_debug.h"
// utility
#include "utility/calculate.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float RESTART_HEIGHT = 100.0f;			// リスタートになる高さ (MIN_WORLD_Y + この数値)
	constexpr float UP_SPEED = 100.0f;					// 浮かび上がる速さ
	constexpr float ATN_UP_SPEED = 0.02f;				// 浮かび上がる速さの減衰率
	constexpr float UNDERWATER_JUMP_STRENGTH = 15.0f;	// 水中のジャンプ力
	constexpr float ADJUST_BUOYANCY_UNDERWATER = 0.1f;	// 水中の浮力の補正
	constexpr float ADJUST_GRAVITY_UNDERWATER = 10.25f;	// 水中の重力の補正
	constexpr float ATN_GRAVITY_UNDERWATER = 0.01f;		// 水中の重力、浮力の減衰率
	constexpr float SUB_SCALE = 2.0f;					// スケールの減算
	constexpr float ATN_UPDRAFT = 0.25f;				// 上昇気流の補正

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockPush::BlockPush()
	: BlockBase()
	, m_firstScale(DEF_VEC3)
	, m_move(DEF_VEC3)
	, m_isTouchWater(false)
	, m_isUnderWater(false)
	, m_wasTouchWater(false)
	, m_isTouchFlame(false)
	, m_oldPos(DEF_VEC3)
	, m_sphere()
	, m_rect()
	, m_height(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockPush::BlockPush(BlockPush&& right) noexcept
	: BlockBase(std::move(right))
	, m_move(std::move(right.m_move))
	, m_isTouchWater(std::move(right.m_isTouchWater))
	, m_isUnderWater(std::move(right.m_isUnderWater))
	, m_wasTouchWater(std::move(right.m_wasTouchWater))
	, m_oldPos(std::move(right.m_oldPos))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockPush::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		// シーン情報の取得
		auto scene = getScene<SceneBase>();

		// 中心位置
		Vec3 pos = getPos();
		m_sphere.pos = pos;
		m_rect.center = Vec2(pos.x, pos.z);

		// y 座標
		m_posY = pos.y;

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		m_firstScale = scale;
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		m_sphere.radius = (size.x + size.y + size.z) * 0.333f;
		m_rect.halfExtend = Vec2(size.x, size.z);

		// 向き
		Quaternion rot = getRot();
		Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
		m_rect.rot = euler.y;

		// 高さ (大きさ)
		m_height = size.y;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockPush::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockPush::update()
{
	float deltaTime = getDeltaTime();					// デルタタイム
	auto scene = getScene<SceneBase>();					// シーン
	auto world = scene->getWorld();						// 世界
	auto dynamicsWorldPtr = world->getDynamicsWorld();	// 動力学世界のポインタ

	// 剛体の取得
	btRigidBody* rigidBody = getRigitBodyPtr();
	if (!rigidBody)
	{
		assert(false);
		return;
	}

	// 重力
	btVector3 gravity = scene->getWorld()->getDynamicsWorld()->getGravity();

	// TODO: deltaTime 掛ける？？ dynamicsWorld の更新でデルタタイム反映されるから大丈夫？
	if (m_isTouchWater)
	{ // 水に触れている
		gravity *= -1.0f * ADJUST_BUOYANCY_UNDERWATER;

		if (getPos().y - m_oldPos.y <= 0)
		{ // 下に移動中
			gravity *= ADJUST_GRAVITY_UNDERWATER;
		}
	}

	// 球，矩形の当たり判定の情報の更新
	{
		// 中心位置
		Vec3 pos = getPos();
		m_sphere.pos = pos;
		m_rect.center = Vec2(pos.x, pos.z);

		// 半径
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		m_rect.halfExtend = Vec2(size.x, size.z);

		// 回転値
		Quaternion rot = getRot();
		Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
		m_rect.rot = euler.y;

		// 高さ (位置)
		m_posY = pos.y;

		// 高さ (大きさ)
		m_height = size.y;
	}

	// 衝撃との当たり判定
	{
		auto impacts = scene->getImpacts();
		for (const auto& element : impacts)
		{
			auto impact = element.lock();
			if (!impact)
			{
				continue;
			}

			// 剛体を有効にする
			rigidBody->activate(true);

			if (Collision::isCollisionSphereVsSphere(impact->getSphere(), m_sphere))
			{
				setIsStatic(false);
				createBlockRigidBody();

				btRigidBody* rigidbody = getRigitBodyPtr();
				if (rigidbody)
				{
					Vec3 diff = getPos() - impact->getSphere().pos;
					D3DXVec3Normalize(&diff, &diff);
					diff *= IMPACT_STRENGTH;
					rigidbody->applyImpulse(btVector3(diff.x, diff.y, diff.z), btVector3(0.0f, -1.0f, 0.0f));
				}

				break;
			}
		}
	}

	// 炎や上昇気流との当たり判定
	{
		auto pairs = scene->getFlameAndUpdraftPairs();
		for (auto& pair : pairs)
		{
			auto flame = pair.second.first.lock();
			if (!flame)
			{
				continue;
			}

			// 炎との当たり判定
			if (!m_isTouchFlame)
			{
				if (Collision::isCollisionSphereVsSphere(m_sphere, flame->getSphere()))
				{ // 炎と衝突
					m_isTouchFlame = true;
					break;	// 炎との当たり判定を優先
				}
			}

			auto updraft = pair.second.second.lock();
			if (!updraft)
			{
				continue;
			}

			if (!updraft->getIsEnabled())
			{
				continue;
			}

			// 上昇気流との当たり判定
			if (Collision::isCollisionBlockVsBlock(
				updraft->getRect(), m_rect,
				updraft->getPosY(), m_posY,
				updraft->getHeight(), m_height))
			{
				if (gravity.getY() < 0.0f)
				{
					gravity *= -1.0f;
				}

				break;
			}
		}
	}

	// 重力の設定
	rigidBody->setGravity(gravity);
	
	if (m_isTouchFlame)
	{ // 炎に触れた
		// スケールの変更
		Vec3 scale = getScale();
		if (scale.z >= 0.0f)
		{
			scale.z -= SUB_SCALE * deltaTime;
			if (scale.z <= 0.0f)
			{
				scale.z = 0.0f;
				reset();
			}
			else
			{
				scale.x = scale.y = scale.z;
				setScale(scale);
				createBlockRigidBody();
			}
		}
	}

#if USE_NEW_SHARED_DATA
	// 最高回転速度の更新
	float angularVelocity = rigidBody->getAngularVelocity().length2();
	auto& gm = GM;
	gm.lockCS();
	auto data = scene->getSharedData();
	if (data->m_maxRotVelocity.length2() < angularVelocity)
	{
		data->m_maxRotVelocity = rigidBody->getAngularVelocity();
	}
	gm.unlockCS();
#endif

	m_move.y = (0.0f - m_move.y) * ATN_UPDRAFT;

	// 過去の位置を保持
	m_oldPos = getPos();

	// リセット
	if (getPos().y <= MIN_WORLD_Y + RESTART_HEIGHT)
	{ // 下に行ってしまった
		reset();
	}

	rigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);

	// 親クラスの処理
	BlockBase::update();

	// 過去の状態を更新
	m_wasTouchWater = m_isTouchWater;

	// 状態のリセット
	m_isTouchWater = false;
	m_isUnderWater = false;
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockPush::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockPush::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();

	// Push Config の設定
	if (ImGui::CollapsingHeader("Push Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// スケール適応ボタン
		static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

		bool isRelease = ImGui::Button("APPLY SCALE##push");
		ImGui::PopStyleColor(3);

		if (isRelease)
		{
			// シーン情報の取得
			auto scene = getScene<SceneBase>();

			// 中心位置
			Vec3 pos = getPos();
			m_sphere.pos = pos;
			m_rect.center = Vec2(pos.x, pos.z);

			// y 座標
			m_posY = pos.y;

			// 半径
			auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
			Vec3 scale = getScale();
			Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
			size.x *= scale.x;
			size.y *= scale.y;
			size.z *= scale.z;
			m_sphere.radius = (size.x + size.y + size.z) * 0.333f;
			m_rect.halfExtend = Vec2(size.x, size.z);

			// 向き
			Quaternion rot = getRot();
			Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
			m_rect.rot = euler.y;

			// 高さ (大きさ)
			m_height = size.y;
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockPush::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);
}

//---------------------------------------------------
// リセット
//---------------------------------------------------
void BlockPush::reset()
{
	btRigidBody* rigidBody = getRigitBodyPtr();

	// 位置
	Vec3 firstPos = getFirstPos();
	setPos(firstPos);
	rigidBody->getWorldTransform().setOrigin(btVector3(firstPos.x, firstPos.y, firstPos.z));

	// スケール
	setScale(m_firstScale);
	createBlockRigidBody();

	m_isTouchFlame = false;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockPush& BlockPush::operator=(BlockPush&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));

		m_move = std::move(right.m_move);
		m_isTouchWater = std::move(right.m_isTouchWater);
		m_isUnderWater = std::move(right.m_isUnderWater);
		m_wasTouchWater = std::move(right.m_wasTouchWater);
		m_oldPos = std::move(right.m_oldPos);
	}

	return *this;
}

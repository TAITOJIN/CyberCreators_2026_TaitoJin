//--------------------------------------------------------------------------------
// 
// 木ブロック [block_wood.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_wood.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
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
	constexpr float RESTART_HEIGHT = 100.0f;			// リスタートになる高さ (MIN_WORLD_Y + この数値)
	constexpr float ADJUST_BUOYANCY_UNDERWATER = 0.1f;	// 水中の浮力の補正
	constexpr uint32_t NUM_BURNT_BLOCK = 10;		 	// 放出する焦げブロックの数
	constexpr const char* BURNT_PARAM = R"({
				"TAG": "burnt_0",
				"MASS": 1.0, "INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, 
				"PRIORITY": 6,
				"DUMMY_FLAGS": "0b0",
				"IS_STATIC": false,
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
				"START_SMALL_SEC": 1.0
			})";

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockWood::BlockWood()
	: BlockPhysics()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockWood::BlockWood(BlockWood&& right) noexcept
	: BlockPhysics(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockWood::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockPhysics::init(inParam);
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
void BlockWood::uninit()
{
	// 親クラスの処理
	BlockPhysics::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockWood::update()
{
	// シーン情報の取得
	auto scene = getScene<SceneBase>();

	// 球の当たり判定の位置の更新
	m_sphere.pos = getPos();

	// 重力
	btVector3 gravity = scene->getWorld()->getDynamicsWorld()->getGravity();

	auto pairs = scene->getFlameAndUpdraftPairs();
	for (auto& pair : pairs)
	{
		auto flame = pair.second.first.lock();
		if (!flame)
		{
			continue;
		}

		auto updraft = pair.second.second.lock();

		// 炎との当たり判定
		if (Collision::isCollisionSphereVsSphere(m_sphere, flame->getSphere()))
		{ // 炎と衝突
			Vec3 pos = getPos();

			// 焦げを放出
			for (uint32_t i = 0; i < NUM_BURNT_BLOCK; i++)
			{
				json j = json::parse(BURNT_PARAM);

				// 位置
				Vec3 adjustPos = DEF_VEC3;
				adjustPos.x = RandomGenerator::get<float>(-7.5f, 7.5f);
				adjustPos.y = RandomGenerator::get<float>(-7.5f, 7.5f);
				adjustPos.z = RandomGenerator::get<float>(-7.5f, 7.5f);
				j[FLAG_POS] = pos + adjustPos;

				// スケール
				float scale = RandomGenerator::get<float>(0.1f, 0.25f);
				j[FLAG_SCALE] = Vec3(scale, scale, scale);

				// 縮小開始の時間
				j[BURNT_BLOCK_PARAM_START_SMALL_SEC] = RandomGenerator::get<float>(1.0f, 1.5f);

				// オブジェクトの追加
				scene->addObject(FACTORY_BURNT_BLOCK, j);
			}

			if (updraft)
			{
				// 上昇気流を有効にする
				if (!updraft->getIsEnabled())
				{
					updraft->setIsEnabled(true);
					auto emitters = scene->getUpdraftEffectEmitters()[updraft->getPairKey()];
					for (auto& element : emitters)
					{
						auto emitter = element.lock();
						if (!emitter)
						{
							continue;
						}

						emitter->setIsEnabled(true);
					}
				}
			}

			// 氷を有効にする
			auto iceBlocks = scene->getIceBlocks()[flame->getPairKey()];
			for (auto& element : iceBlocks)
			{
				auto iceBlock = element.lock();
				if (!iceBlock)
				{
					continue;
				}

				if (iceBlock->getIceState() != BlockIce::ICE_STATE::ICE)
				{ // 既に状態変化後
					continue;
				}

				// 氷から水
				iceBlock->setIceState(BlockIce::ICE_STATE::ICE_TO_WATER);
			}

			// 氷結した水雫ブロックの氷結解除
			auto freezeBlocks = scene->getFreezeBlocks()[flame->getPairKey()];
			for (auto& element : freezeBlocks)
			{
				auto freezeBlock = element.lock();
				if (!freezeBlock)
				{
					continue;
				}

				if (!freezeBlock->getIsFreeze())
				{ // 既に解除済み
					continue;
				}

				// 氷結解除
				freezeBlock->setIsFreeze(false);
			}

			// 最初はフリーズしている水雫ブロックの生成器の有効化
			auto freezeEmitters = scene->getFreezeEmitters()[flame->getPairKey()];
			for (auto& element : freezeEmitters)
			{
				auto freezeEmitter = element.lock();
				if (!freezeEmitter)
				{
					continue;
				}

				if (!freezeEmitter->getIsFreeze())
				{ // 既に解除済み
					continue;
				}

				// 氷結解除
				freezeEmitter->setIsFreeze(false);
			}

			// 回転するブロック
			auto rotBlocks = scene->getRotateBlocks()[flame->getPairKey()];
			for (auto& element : rotBlocks)
			{
				auto rotBlock = element.lock();
				if (!rotBlock)
				{
					continue;
				}

				if (rotBlock->getIsEnabled())
				{ // 既に有効済み
					continue;
				}

				// 有効にする
				rotBlock->setIsEnabled(true);
			}

			// 移動するブロック
			auto moveBlocks = scene->getMoveBlocks()[flame->getPairKey()];
			for (auto& element : moveBlocks)
			{
				auto moveBlock = element.lock();
				if (!moveBlock)
				{
					continue;
				}

				if (moveBlock->getIsEnabled())
				{ // 既に有効済み
					continue;
				}

				// 有効にする
				moveBlock->setIsEnabled(true);
			}

			// 水車
			auto waterWheels = scene->getWaterWheels()[flame->getPairKey()];
			for (auto& element : waterWheels)
			{
				auto waterWheel = element.lock();
				if (!waterWheel)
				{
					continue;
				}

				if (waterWheel->getIsEnabled())
				{ // 既に有効済み
					continue;
				}

				// 有効にする
				waterWheel->setIsEnabled(true);
			}

			setIsDestroy(true);
			break;	// 炎優先
		}

		// 上昇気流との当たり判定
		if (updraft)
		{
			if (!updraft->getIsEnabled())
			{
				continue;
			}

			// 中心位置
			Vec3 pos = getPos();
			m_rect.center = Vec2(pos.x, pos.z);

			// y 座標
			m_posY = pos.y;

			// 向き
			Quaternion rot = getRot();
			Vec3 euler = Calculate::quaternionToEulerYXZ(rot);
			m_rect.rot = euler.y;

			if (Collision::isCollisionBlockVsBlock(
				updraft->getRect(), m_rect,
				updraft->getPosY(), m_posY,
				updraft->getHeight(), m_height))
			{
				gravity *= -1.0f * ADJUST_BUOYANCY_UNDERWATER;
			}
		}
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
			auto rigidBody = getRigitBodyPtr();
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

	// 重力の設定
	btRigidBody* rigidBody = getRigitBodyPtr();
	rigidBody->setGravity(gravity);

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

#if 0
	// physics 側でも上昇気流との当たり判定を行っているのでこっちは呼ばずに、直接親の親を呼ぶ

	// 親クラスの処理
	BlockPhysics::update();
#else
	// 親クラスの処理
	BlockBase::update();
#endif
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockWood::draw() const
{
	// 親クラスの処理
	BlockPhysics::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockWood::inspector()
{
	// 親クラスの処理
	BlockPhysics::inspector();

	// Flame Config の設定
	if (ImGui::CollapsingHeader("Wood Config", ImGuiTreeNodeFlags_DefaultOpen))
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
void BlockWood::save(json& outObject) const
{
	// 親クラスの処理
	BlockPhysics::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockWood& BlockWood::operator=(BlockWood&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockPhysics::operator=(std::move(right));
	}

	return *this;
}

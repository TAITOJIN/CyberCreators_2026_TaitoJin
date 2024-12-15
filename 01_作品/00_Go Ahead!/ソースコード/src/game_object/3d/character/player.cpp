//--------------------------------------------------------------------------------
// 
// プレイヤー [player.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// マクロ定義
//---------------------------------------------------
#define USE_SHADOW_PLAYER	(0)

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // character
#include "game_object/3d/character/player.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_debug.h"
#include "scene/scene_game.h"
// // elements
#include "scene/elements/camera.h"
#include "scene/elements/light.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"
#include "utility/random_generator.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float RESTART_HEIGHT = -200.0f;					// リスタートになる高さ
	constexpr float UP_SPEED = 100.0f;							// 浮かび上がる速さ
	constexpr float ATN_UP_SPEED = 0.02f;						// 浮かび上がる速さの減衰率
	constexpr float UNDERWATER_JUMP_STRENGTH = 15.0f;			// 水中のジャンプ力
	constexpr float RAY_END = 1000.0f;							// レイの終点
	//const Vec3 SHADOW_BLOCK_SCALE = Vec3(0.5f, 0.1f, 0.5f);		// 影ブロックのスケール
	constexpr float ADJUST_UNDERWATER_JUMP = 0.25f;				// 水中のジャンプ力の補正
	constexpr float ADJUST_FIRST_POS_Y = 100.0f;				// 最初の高さを調整する
	constexpr float ATN_SPEED_ON_ICE = 0.4f;					// 氷の上

	constexpr float ADJUST_MILLSECOND = 0.01f;			// ミリ秒に調整

	constexpr int NUM_PIECES = 2;						// 粉々の数
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
					"y": -20.0,
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

	constexpr float WALK_EFFECT_INTERVAL = 0.2f;

	constexpr const char* PARAM_SHADOW_BLOCK = R"({"TAG": "shadow_block_0","MASS": 0.0,"INERTIA": { "x": 0.0, "y": 0.0, "z": 0.0 }, "PRIORITY": 6,"DUMMY_FLAGS": "0b0001","IS_STATIC": true,"KEY": "res\\model\\block000.x","MAT": [{"MAT_KEY": "BLACK","MAT_NAME": 0}],"OFFSET": {"x": 0.0,"y": 0.0,"z": 0.0},"POS": {"x": 0.0,"y": 0.0,"z": 0.0},"ROT": {"x": 0.0,"y": 0.0,"z": 0.0},"SCALE": {"x": 0.2,"y": 0.01,"z": 0.2}})";

	constexpr float ADJUST_SHADOW = 0.5f;

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Player::Player()
	: Model()
	, m_move(DEF_VEC3)
	, m_targetRot(DEF_VEC3)
	, m_speed(0.0f)
	, m_atnSpeed(0.0f)
	, m_jumpStrength(0.0f)
	, m_isJumping(false)
	, m_maxSlopeDegree(0.0f)
	, m_stepHeight(0.0f)
	, m_firstJumpSpeed(0.0f)
	, m_fallSpeed(0.0f)
	, m_gravity(DEF_VEC3)
	, m_isTouchWater(false)
	, m_isUnderWater(false)
	, m_wasTouchWater(false)
	, m_wasUnderWater(false)
	, m_respawnPos(DEF_VEC3)
	, m_isOnIce(false)
	, m_walkEffectIntervalCounter(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Player::Player(Player&& right) noexcept
	: Model(std::move(right))
	, m_move(std::move(right.m_move))
	, m_targetRot(std::move(right.m_targetRot))
	, m_speed(std::move(right.m_speed))
	, m_atnSpeed(std::move(right.m_atnSpeed))
	, m_isJumping(std::move(right.m_isJumping))
	, m_jumpStrength(std::move(right.m_jumpStrength))
	, m_maxSlopeDegree(std::move(right.m_maxSlopeDegree))
	, m_stepHeight(std::move(right.m_stepHeight))
	, m_firstJumpSpeed(std::move(right.m_firstJumpSpeed))
	, m_fallSpeed(std::move(right.m_fallSpeed))
	, m_gravity(std::move(right.m_gravity))
	, m_character(std::move(right.m_character))
	, m_isTouchWater(std::move(right.m_isTouchWater))
	, m_isUnderWater(std::move(right.m_isUnderWater))
	, m_wasTouchWater(std::move(right.m_wasTouchWater))
	, m_respawnPos(std::move(right.m_respawnPos))
	, m_shadow(std::move(right.m_shadow))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Player::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = Model::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_targetRot = getRot();
		m_speed = inParam[FLAG_SPEED];
		m_atnSpeed = inParam[FLAG_ATN_SPEED];
		m_jumpStrength = inParam[FLAG_JUMP_STRENGTH];
		m_gravity = inParam[FLAG_GRAVITY];
		m_maxSlopeDegree = inParam[FLAG_MAX_SLOPE_DEGREE];
		m_stepHeight = inParam[FLAG_STEP_HEIGHT];
		m_firstJumpSpeed = inParam[FLAG_FIRST_SPEED_JUMP];
		m_fallSpeed = inParam[FLAG_FALL_SPEED];

		m_respawnPos = inParam[PARAM_PLAYER_FIRST_RESPAWN_POINT];
		m_respawnPosKeep = m_respawnPos;

		json param = json::parse(PARAM_SHADOW_BLOCK);
		{
			param[FLAG_SCALE] = Vec3(0.2f, 0.01f, 0.2f) * ADJUST_SHADOW;
			param[FLAG_OFFSET] = Vec3(-2.0f, 0.0f, 2.0f) * ADJUST_SHADOW;
			auto block = getScene()->addObject(FACTORY_SHADOW_BLOCK, param)->downcast<BlockShadow>();
			block->setIsAnim(false);
			m_shadowBlocks.push_back(block);
		}
		{
			param[FLAG_SCALE] = Vec3(0.2f, 0.01f, 0.2f) * ADJUST_SHADOW;
			param[FLAG_OFFSET] = Vec3(2.0f, 0.0f, 2.0f) * ADJUST_SHADOW;
			auto block = getScene()->addObject(FACTORY_SHADOW_BLOCK, param)->downcast<BlockShadow>();
			block->setIsAnim(false);
			m_shadowBlocks.push_back(block);
		}
		{
			param[FLAG_SCALE] = Vec3(0.2f, 0.01f, 0.2f) * ADJUST_SHADOW;
			param[FLAG_OFFSET] = Vec3(-2.0f, 0.0f, -2.0f) * ADJUST_SHADOW;
			auto block = getScene()->addObject(FACTORY_SHADOW_BLOCK, param)->downcast<BlockShadow>();
			block->setIsAnim(false);
			m_shadowBlocks.push_back(block);
		}
		{
			param[FLAG_SCALE] = Vec3(0.2f, 0.01f, 0.2f) * ADJUST_SHADOW;
			param[FLAG_OFFSET] = Vec3(2.0f, 0.0f, -2.0f) * ADJUST_SHADOW;
			auto block = getScene()->addObject(FACTORY_SHADOW_BLOCK, param)->downcast<BlockShadow>();
			block->setIsAnim(false);
			m_shadowBlocks.push_back(block);
		}

		for (auto& shadow : m_shadowBlocks)
		{
			if (auto shadowBlock = shadow.lock())
			{
				shadowBlock->setMtxParent(getMatrix());
			}
		}

#if USE_SHADOW_PLAYER
		m_shadow = make_unique<Shadow>(getMatrix());
#endif
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Player::uninit()
{
	m_shadow.reset();
	m_character.reset();

	// 親クラスの処理
	Model::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Player::update()
{
	bool can = true;
	auto game = getScene<SceneGame>();
	if (game)
	{
		auto effect = game->getGoalEffect();
		if (effect)
		{
			if (effect->getIsEffecting())
			{
				can = false;
			}
		}
	}

	if (!can)
	{
		return;
	}

	auto& inputManager = GM.getInputManager();			// 入力処理の管理者
	auto& keyboard = inputManager.getKeyboard();		// キーボード
	auto& gamePad = inputManager.getGamepad();			// ゲームパッド
	float deltaTime = getDeltaTime();					// デルタタイム
	auto scene = getScene<SceneBase>();					// シーン
	auto world = scene->getWorld();						// 世界
	auto dynamicsWorldPtr = world->getDynamicsWorld();	// 動力学世界のポインタ
	bool isEdit = false;								// 編集中かどうか

	m_effectMatKey = "BLACK";

	// 編集中かどうかの設定
	if (auto sceneDebug = getScene<SceneDebug>())
	{
		isEdit = sceneDebug->getIsEdit();
	}

	// 浮力
	if (m_isTouchWater)
	{
		m_move.y += UP_SPEED * deltaTime;
	}
	m_move.y += (0.0f - m_move.y) * ATN_UP_SPEED;

	// 重力やジャンプ力、落下速度の設定
	m_character->setGravity(btVector3(m_gravity.x, m_gravity.y, m_gravity.z));
	m_character->setJumpSpeed(m_jumpStrength);
	m_character->setFallSpeed(m_fallSpeed);
	if (isEdit)
	{ // 編集中
		// 動かさない
		m_character->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		m_character->setJumpSpeed(0.0f);
		m_character->setFallSpeed(0.0f);
	}

	// キャラクターの更新開始
	m_character->preStep(dynamicsWorldPtr.get());

	// 移動処理
	if (!isEdit)
	{ // 編集中でない
		// 移動
		bool isMove = false;
		// キーボード操作
		{
			Vec2 move(0.0f, 0.0f);

			if (keyboard.getPress(DIK_A))
			{
				move.x += -1.0f;
			}

			if (keyboard.getPress(DIK_D))
			{
				move.x += 1.0f;
			}

			if (keyboard.getPress(DIK_W))
			{
				move.y += -1.0f;
			}

			if (keyboard.getPress(DIK_S))
			{
				move.y += 1.0f;
			}

			if ((move.x != 0.0f) || (move.y != 0.0f))
			{ // 移動している
				isMove = true;

				// 目的の向きの設定
				float y = atan2f(move.y, move.x) - scene->getCamera()->getRot().y - Camera::REVISION_ANGLE_Y;
				D3DXQuaternionRotationAxis(&m_targetRot, &AXIS_Y, y);
			}
		}

		// ゲームパッド操作
		{
			if (gamePad.isDir(InputGamepad::STICK_TYPE::LEFT))
			{
				isMove = true;

				// 目的の向きの設定
				float y = gamePad.getStickAngle(InputGamepad::STICK_TYPE::LEFT) - scene->getCamera()->getRot().y/* - Camera::REVISION_ANGLE_Y*/;
				D3DXQuaternionRotationAxis(&m_targetRot, &AXIS_Y, y);
			}
		}

		// 向きの更新，設定
		Quaternion curtRot = GameObject3D::getRot();

		// 差を求める
		Quaternion diffRot;
		D3DXQuaternionInverse(&diffRot, &curtRot);
		D3DXQuaternionMultiply(&diffRot, &m_targetRot, &diffRot);

		// 半分にする
		Quaternion halfDiffRot;
		D3DXQuaternionSlerp(&halfDiffRot, &DEF_ROT, &diffRot, HALF);

		// 現在の向きに加算
		D3DXQuaternionMultiply(&curtRot, &curtRot, &halfDiffRot);

		// 反映
		GameObject3D::setRot(curtRot);

		// 回転
		Quaternion q = getRot();
		btQuaternion deltaRot(q.x, q.y, q.z, q.w);
		m_character->getGhostObject()->getWorldTransform().setRotation(deltaRot);

		// 移動量の更新
		if (isMove)
		{ // 移動する
			Quaternion rot = m_targetRot;
			Matrix mtxRot;
			D3DXMatrixRotationQuaternion(&mtxRot, &rot);

			// 方向ベクトル
			Vec3 forwardVector(mtxRot._31, mtxRot._32, mtxRot._33);
			Vec3 move = forwardVector * m_speed * -1.0f;

			// 移動量の更新 (加算)
			m_move.x += move.x;
			m_move.z += move.z;
		}

		// 移動量の更新 (減衰)
		float atnSpeed = m_atnSpeed;
		if (m_isOnIce)
		{
			atnSpeed *= ATN_SPEED_ON_ICE;
		}
		m_move.x += (0.0f - m_move.x) * atnSpeed;
		m_move.z += (0.0f - m_move.z) * atnSpeed;

		// 着地処理
		Vec3 move = DEF_VEC3;	// 移動ブロックに触れているときなど、調整用移動量
		{
			bool isWaterBlock = false;	// 水ブロックか否か
			bool isOnGround = false;	// 着地しているか否か
			bool isCollision = false;	// 衝突しているか否か
			bool isOnPhycics = false;	// 物理ブロックに乗っているか否か
			world->isCollisionCharacter(m_character.get(), move, isWaterBlock, m_isOnIce, isOnGround, isOnPhycics, isCollision, m_effectMatKey);
			if (m_isUnderWater)
			{
				m_isJumping = false;	// 非ジャンプ状態
			}

			if (isOnGround)
			{ // 着地している
				if (!isWaterBlock)
				{
					m_isJumping = false;	// 非ジャンプ状態

					if (isMove)
					{ // 移動中
						Vec3 pos = DEF_VEC3;
						{
							// 中心位置
							pos = getPos();

							// 半径
							auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
							Vec3 scale = getScale();
							Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
							size.x *= scale.x;
							size.y *= scale.y;
							size.z *= scale.z;

							pos.y -= size.y;
						}

						m_walkEffectIntervalCounter += deltaTime;
						if (m_walkEffectIntervalCounter >= WALK_EFFECT_INTERVAL)
						{
							m_walkEffectIntervalCounter = 0.0f;

							// エフェクトの生成
							for (int i = 0; i < NUM_PIECES; i++)
							{
								float scale = RandomGenerator::get(PIECES_RAND_SCALE_MIN, PIECES_RAND_SCALE_MAX);

								Vec3 moveRand = m_move * -1.0f;
								D3DXVec3Normalize(&moveRand, &moveRand);
								moveRand.x *= RandomGenerator::get(0.1f, 1.0f);
								moveRand.y = RandomGenerator::get(0.0f, 0.5f);
								moveRand.z *= RandomGenerator::get(0.1f, 1.0f);

								// パラメータの設定
								json j = json::parse(PARAM_SMALL_PIECES);

								json modelMaterial;
								modelMaterial[FLAG_MAT_NAME] = 0;
								modelMaterial[FLAG_MAT_KEY] = m_effectMatKey;

								j[FLAG_MAT].push_back(modelMaterial);
								j[FLAG_POS] = pos;
								j[WATERDROP_PARAM_FIRST_SCALE] = scale;
								j[WATERDROP_PARAM_MAX_LIFE] = RandomGenerator::get(PIECES_RAND_LIFE_MIN, PIECES_RAND_LIFE_MAX) * ADJUST_MILLSECOND;
								j[PARAM_BLOCK_SMALL_PIECES] = moveRand;

								// オブジェクトの追加
								scene->addObject(FACTORY_SMALL_PIECES, j);
							}
						}
					}
				}
			}

			// 衝撃との当たり判定
			if (isOnPhycics)
			{ // 物理ブロックに乗っている
				auto impacts = scene->getImpacts();
				for (const auto& element : impacts)
				{
					auto impact = element.lock();
					if (!impact)
					{
						continue;
					}

					Collision::Sphere sphere;
					{
						Vec3 pos = getPos();
						auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
						Vec3 scale = getScale();
						Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
						size.x *= scale.x;
						size.y *= scale.y;
						size.z *= scale.z;

						sphere.pos = pos;
						sphere.radius = (size.x + size.y + size.z) * 0.333f;
					}

					if (Collision::isCollisionSphereVsSphere(impact->getSphere(), sphere))
					{
						setIsStatic(false);
						createBlockRigidBody();

						if (m_character)
						{
							Vec3 diff = getPos() - impact->getSphere().pos;
							D3DXVec3Normalize(&diff, &diff);
							diff *= IMPACT_STRENGTH;
							m_character->applyImpulse(btVector3(diff.x, diff.y, diff.z));
						}

						break;
					}
				}
			}
		}

		// ジャンプ処理
		// MEMO: 位置更新が最後なので、着地後にジャンプ処理しないと、ジャンプ押した瞬間に着地判定される
		if (keyboard.getTrigger(DIK_SPACE) ||
			gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::A) || gamePad.getButtonTrigger(InputGamepad::BUTTON_TYPE::B))
		{
			if (m_isUnderWater)
			{ // 水中にいる
				//m_move.y = UNDERWATER_JUMP_STRENGTH;

#if USE_NEW_SHARED_DATA
				// ジャンプ回数を加算
				auto& gm = GM;
				gm.lockCS();
				auto data = scene->getSharedData();
				data->m_numJump++;
				gm.unlockCS();
#endif

				// ジャンプしている状態にする
				m_isJumping = true;

				// ジャンプ
				m_character->jump(btVector3(0.0f, m_jumpStrength * ADJUST_UNDERWATER_JUMP, 0.0f));
			}
			else
			{ // 地面もしくは水面
				if (!m_isJumping)
				{
#if USE_NEW_SHARED_DATA
					// ジャンプ回数を加算
					auto& gm = GM;
					gm.lockCS();
					auto data = scene->getSharedData();
					data->m_numJump++;
					gm.unlockCS();
#endif

					// ジャンプしている状態にする
					m_isJumping = true;

					// ジャンプ
					m_character->jump(btVector3(0.0f, m_jumpStrength, 0.0f));
				}
			}
		}

		// 速度の設定
		btVector3 velocity = btVector3(m_move.x, m_move.y, m_move.z);
		m_character->setWalkDirection(velocity * deltaTime + btVector3(move.x, move.y, move.z));
	}

	// 位置の反映
	btVector3 p = m_character->getGhostObject()->getWorldTransform().getOrigin();
	setPos(Vec3(p.getX(), p.getY(), p.getZ()));

	// プレイヤーの更新
	m_character->playerStep(dynamicsWorldPtr.get(), deltaTime);

	// 位置のリセット
	if (getPos().y <= RESTART_HEIGHT)
	{ // 下に行ってしまった
		setPos(m_respawnPos);
		m_character->getGhostObject()->getWorldTransform().setOrigin(btVector3(m_respawnPos.x, m_respawnPos.y, m_respawnPos.z));
	}

	// マトリックスの更新
	GameObject3D::updateMtx();

#if USE_NEW_SHARED_DATA
	if (m_isUnderWater)
	{
		// 水中にいた時間を加算
		auto& gm = GM;
		gm.lockCS();
		auto data = scene->getSharedData();
		data->m_timeUnderWater += deltaTime;
		gm.unlockCS();
	}
#endif

#if 0
	// 状態の追加
	auto& gm = GM;
	gm.lockCS();
	auto data = scene->getSharedData();
	SharedData::ReplayStransform trans;
	trans.pos = getPos();
	trans.rot = getRot();
	trans.deltaTime = deltaTime;
	trans.currentTime = scene->getTime()->getTime();
	data->m_transformations.push_back(trans);
	gm.unlockCS();
#endif

#if USE_SHADOW_PLAYER
	// 影の更新
	{
		Vec3 pos = getPos();
		Vec3 hitPos = pos, nor = DEF_VEC3;
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
		Vec3 scale = getScale();
		Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
		size.x *= scale.x;
		size.y *= scale.y;
		size.z *= scale.z;
		hitPos = pos;
		hitPos.y -= size.y;
		
		btVector3 rayFrom = btVector3(pos.x, pos.y - size.y - 1.0f, pos.z);
		btVector3 rayTo = btVector3(pos.x, pos.y - RAY_END, pos.z);
		
		PhysicsWorld::PerformRaycast(getScene<SceneBase>()->getWorld()->getDynamicsWorld().get(), rayFrom, rayTo, hitPos, nor);
		
		hitPos.y += 0.1f;
		
		m_shadow->setPos(hitPos);
		m_shadow->setRot(nor);
		m_shadow->update();
	}
#endif

	// 影の更新
	for (auto& shadow : m_shadowBlocks)
	{
		Vec3 pos = getPos();
		Vec3 hitPos = pos, nor = DEF_VEC3;
		auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
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

	// 過去の状態を更新
	m_wasTouchWater = m_isTouchWater;
	m_wasUnderWater = m_isUnderWater;

	// 状態のリセット
	m_isTouchWater = false;
	m_isUnderWater = false;
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Player::draw() const
{
#if USE_SHADOW_PLAYER
	// 影の描画
	const ResourceModel& resModel = getScene<SceneBase>()->getResManager()->getResModel(getKey());
	m_shadow->draw(resModel);
#endif

	// 親クラスの処理
	Model::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Player::inspector()
{
	// Transform の設定
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 位置の設定
		{
			Vec3 pos = getPos();
			bool isReturn = ImGuiAddOns::Transform("Position", &pos);

			if (isReturn)
			{
				// 反映
				setPos(pos);

				// 剛体にも反映
				auto trans = m_character->getGhostObject()->getWorldTransform();
				pos = getPos();
				btVector3 newPos(pos.x, pos.y, pos.z);
				trans.setOrigin(newPos);
				m_character->getGhostObject()->setWorldTransform(trans);
			}
		}

		// 向きの設定
		{
			auto rot = getRot();

			// クォータニオンからオイラー角に変換
			Vec3 euler = DEF_VEC3;
#if 0
			m_character->getGhostObject()->getWorldTransform().getRotation().getEulerZYX(euler.z, euler.y, euler.x);
#else
			euler = Calculate::quaternionToEulerZYX(rot);
#endif

			// radian から degree に変換
			euler.x = D3DXToDegree(euler.x);
			euler.y = D3DXToDegree(euler.y);
			euler.z = D3DXToDegree(euler.z);

			bool isReturn = ImGuiAddOns::Transform("Rotation", &euler);

			if (isReturn)
			{
				// degree から radian に変換
				Calculate::calculateRot(&rot, AXIS_Y, D3DXToRadian(euler.y));
				Calculate::mulRot(&rot, AXIS_X, D3DXToRadian(euler.x));
				Calculate::mulRot(&rot, AXIS_Z, D3DXToRadian(euler.z));

				// 剛体に反映
				btQuaternion newRot(rot.x, rot.y, rot.z, rot.w);
				m_character->getGhostObject()->getWorldTransform().setRotation(newRot);

				// 反映
				setFirstRot(rot);
				setRot(rot);
			}
		}

		// スケールの設定
		{
			Vec3 scale = getScale();
			bool isReturn = ImGuiAddOns::Transform("Scale", &scale);

			if (isReturn)
			{
				// 反映
				setScale(scale);

				// 新しくキャラクターを作成
				createCapsuleCharacter();
			}
		}

		// オフセット
		{
			Vec3 offset = getOffset();
			bool isReturn = ImGuiAddOns::Transform("Offset", &offset);

			if (isReturn)
			{
				// 反映
				setOffset(offset);
			}
		}
	}

#if 0
	// マテリアルの設定
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& mat : getMat())
		{
			ImGui::Text("material name: %d", mat.first);

			auto& material = mat.second.mat;

			// カラーピッカーを表示
			ImGuiAddOns::Material("Diffuse", material.Diffuse);

			ImGui::Separator();
			ImGuiAddOns::Material("Ambient", material.Ambient);

			ImGui::Separator();
			ImGuiAddOns::Material("Specular", material.Specular);

			ImGui::Separator();
			ImGuiAddOns::Material("Emissive", material.Emissive);

			ImGui::Separator();
			ImGui::SliderFloat("Power", &material.Power, 0.0f, 128.0f);
		}
	}
#endif

	// 情報の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 速度の設定
		{
			float speed = m_speed;
			bool isReturn = ImGuiAddOns::Float("Speed", &speed);

			if (isReturn)
			{
				m_speed = speed;
			}
		}

		// 速度の調整
		{
			float atnSpeed = m_atnSpeed;
			bool isReturn = ImGuiAddOns::Float("Atn Speed", &atnSpeed);

			if (isReturn)
			{
				m_atnSpeed = atnSpeed;
			}
		}

		// ジャンプ力の設定
		{
			float jumpStrength = m_jumpStrength;
			bool isReturn = ImGuiAddOns::Float("Jump Strength", &jumpStrength);

			if (isReturn)
			{
				m_jumpStrength = jumpStrength;
			}
		}

		// 重力の設定
		{
			Vec3 gravity= m_gravity;
			bool isReturn = ImGuiAddOns::Transform("Gravity", &gravity);

			if (isReturn)
			{
				m_gravity = gravity;
			}
		}

		// 乗れる最大角度の設定
		{
			float maxSlopeDegree = D3DXToDegree(m_character->getMaxSlope());
			bool isReturn = ImGuiAddOns::Float("Max Slope", &maxSlopeDegree);

			if (isReturn)
			{
				m_maxSlopeDegree = maxSlopeDegree;
				m_character->setMaxSlope(D3DXToRadian(m_maxSlopeDegree));
			}
		}

		// 落ちる速度の設定
		{
			float fallSpeed = m_fallSpeed;
			bool isReturn = ImGuiAddOns::Float("Fall Speed", &fallSpeed);

			if (isReturn)
			{
				m_fallSpeed = fallSpeed;
			}
		}
	}

	// その他の設定
	if (ImGui::CollapsingHeader("Others", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 最初の位置の設定
		{
			Vec3 pos = getFirstPos();
			bool isReturn = ImGuiAddOns::Transform("First Position", &pos);

			if (isReturn)
			{
				// 反映
				setFirstPos(pos);
				setPos(pos);

				// 剛体にも反映
				auto trans = m_character->getGhostObject()->getWorldTransform();
				pos = getPos();
				btVector3 newPos(pos.x, pos.y, pos.z);
				trans.setOrigin(newPos);
				m_character->getGhostObject()->setWorldTransform(trans);
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Player::save(json& outObject) const
{
	// 親クラスの処理
	Model::save(outObject);

	outObject[FLAG_SPEED] = m_speed;
	outObject[FLAG_ATN_SPEED] = m_atnSpeed;
	outObject[FLAG_JUMP_STRENGTH] = m_jumpStrength;
	outObject[FLAG_GRAVITY] = m_gravity;
	outObject[FLAG_MAX_SLOPE_DEGREE] = m_maxSlopeDegree;
	outObject[FLAG_STEP_HEIGHT] = m_stepHeight;
	outObject[FLAG_FIRST_SPEED_JUMP] = m_firstJumpSpeed;
	outObject[FLAG_FALL_SPEED] = m_fallSpeed;
	outObject[PARAM_PLAYER_FIRST_RESPAWN_POINT] = m_respawnPosKeep;
}

//---------------------------------------------------
// カプセル形状のキャラクターの作成
//---------------------------------------------------
void Player::createCapsuleCharacter()
{
	auto scene = getScene<SceneBase>();	// シーンの取得
	auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();	// aabb の取得
	Vec2 capsule = Collision::getCapsule(aabb, getScale());	// カプセル形状の取得

	// キャラクターの作成
	btKinematicCharacterController* chara = PhysicsWorld::createCharacter(
		*scene->getWorld(),
		PhysicsWorld::createCollisionCapsuleShape(capsule),
		PhysicsWorld::createTransform(getPos(), getRot(), getOffset()),
		m_gravity,
		m_maxSlopeDegree,
		m_stepHeight,
		m_firstJumpSpeed);

	// キャラクターの設定
	setCharacter(chara);
}

//---------------------------------------------------
// キャラクターの設定
//---------------------------------------------------
void Player::setCharacter(btKinematicCharacterController* inChara)
{
	auto characterDeleter = [this](btKinematicCharacterController* inPtr) { this->customDeleter(inPtr); };
	unique_ptr<btKinematicCharacterController, CharacterDeleter> chara(inChara, characterDeleter);
	m_character = std::move(chara);
}

//---------------------------------------------------
// カスタムデリータ
//---------------------------------------------------
void Player::customDeleter(btKinematicCharacterController* ptr)
{
	if (!ptr)
	{
		return;
	}

	auto dynamicsWorld = getScene<SceneBase>()->getWorld()->getDynamicsWorld();
	if (dynamicsWorld)
	{
		dynamicsWorld->removeAction(ptr);

		auto collisionObject = ptr->getGhostObject();
		if (collisionObject)
		{
			auto collisionShape = collisionObject->getCollisionShape();
			if (collisionShape)
			{
				delete collisionShape;
				collisionShape = nullptr;
			}

			dynamicsWorld->removeCollisionObject(collisionObject);
			delete collisionObject;
			collisionObject = nullptr;
		}

		delete ptr;
		ptr = nullptr;
	}
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Player& Player::operator=(Player&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		Model::operator=(std::move(right));

		m_move = std::move(right.m_move);
		m_targetRot = std::move(right.m_targetRot);
		m_speed = std::move(right.m_speed);
		m_atnSpeed = std::move(right.m_atnSpeed);
		m_jumpStrength = std::move(right.m_jumpStrength);
		m_isJumping = std::move(right.m_isJumping);
		m_maxSlopeDegree = std::move(right.m_maxSlopeDegree);
		m_stepHeight = std::move(right.m_stepHeight);
		m_firstJumpSpeed = std::move(right.m_firstJumpSpeed);
		m_fallSpeed = std::move(right.m_fallSpeed);
		m_gravity = std::move(right.m_gravity);
		m_character = std::move(right.m_character);
		m_isTouchWater = std::move(right.m_isTouchWater);
		m_isUnderWater = std::move(right.m_isUnderWater);
		m_wasTouchWater = std::move(right.m_wasTouchWater);
		m_respawnPos = std::move(right.m_respawnPos);
		m_shadow = std::move(right.m_shadow);
	}

	return *this;
}

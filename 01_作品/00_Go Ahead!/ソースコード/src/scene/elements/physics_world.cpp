//--------------------------------------------------------------------------------
// 
// 物理演算を使用した世界 [physics_world.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // selements
#include "scene/elements/physics_world.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_debug.h"
// game_object
// // 3d
#include "game_object/3d/game_object_3d.h"
// // // block
#include "game_object/3d/block/block_base.h"
#include "game_object/3d/block/block_move.h"
#include "game_object/3d/block/block_rotate.h"
#include "game_object/3d/block/block_fall.h"
#include "game_object/3d/block/block_water.h"
#include "game_object/3d/block/block_physics.h"
#include "game_object/3d/block/block_push.h"
#include "game_object/3d/block/block_repair.h"
#include "game_object/3d/block/block_ice.h"
#include "game_object/3d/block/block_wood.h"
#include "game_object/3d/block/block_impact.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float WORLD_GRAVITY_STRENGTH = -100.f;	// 重力の強さ
	constexpr int MAX_SUB_STEP = 10;					// 最大サブステップ
	constexpr float DETECTION_RADIUS = 3000.0f;			// プレイヤー周辺検出範囲 (当たり判定)
	constexpr float UPDATE_DETECTION = 1.0f;			// 周辺検出範囲の更新頻度 (秒)
	constexpr float ADJUST_PHYSICS_IMPACT = 8.0f;		// 衝撃や力の補正 (物理挙動が特にあるブロックに対して)
	constexpr float ADJUST_PUSH_IMPACT = 3.0f;			// 衝撃や力の補正 (押せるブロックに対して)
	constexpr float ADJUST_ICE_MOVE = 1.2f;				// 氷ブロック着地中の移動量の補正
	constexpr float LIMIT_PHYSICS_Y = 20.0f;			// 制限
	constexpr float LIMIT_PHYSICS_XZ = 100.0f;			// 制限

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
PhysicsWorld::PhysicsWorld()
	: GameObject()
	, m_time(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT PhysicsWorld::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	// 衝突構成設定の作成と設定
	m_collisionConfig = make_shared<btDefaultCollisionConfiguration>();
	if (!m_collisionConfig)
	{
		return E_FAIL;
	}

	// 衝突ディスパッチャの作成と設定
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(m_collisionConfig.get());
	shared_ptr<btCollisionDispatcher> uniqueDispatcher(dispatcher);
	m_dispatcher = std::move(uniqueDispatcher);
	if (!m_dispatcher)
	{
		return E_FAIL;
	}

	// 広域位相衝突判定の作成と設定
	m_overlappingPairCache = make_shared<btDbvtBroadphase>();
	if (!m_overlappingPairCache)
	{
		return E_FAIL;
	}

	{
		float detectionRadius = DETECTION_RADIUS;
		btVector3 worldMin = -btVector3(detectionRadius, detectionRadius, detectionRadius);
		btVector3 worldMax = btVector3(detectionRadius, detectionRadius, detectionRadius);
		auto sweepBP = make_shared<btAxisSweep3>(worldMin, worldMax);
		if (!sweepBP)
		{
			return E_FAIL;
		}

		m_overlappingPairCache = std::move(sweepBP);
	}

	// 拘束条件ソルバの作成と設定
	m_solver = make_shared<btSequentialImpulseConstraintSolver>();
	if (!m_solver)
	{
		return E_FAIL;
	}

	// 動力学世界の作成と設定
	btDiscreteDynamicsWorld* dynamicWorld = new btDiscreteDynamicsWorld(m_dispatcher.get(), m_overlappingPairCache.get(), m_solver.get(), m_collisionConfig.get());
	shared_ptr<btDiscreteDynamicsWorld> uniqueDynamicsWorld(dynamicWorld);
	m_dynamicsWorld = std::move(uniqueDynamicsWorld);
	if (!m_dynamicsWorld)
	{
		return E_FAIL;
	}

	// 重力の設定
	m_dynamicsWorld->setGravity(btVector3(0, WORLD_GRAVITY_STRENGTH, 0));

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void PhysicsWorld::uninit()
{
#if 1
	// コールバックの削除
	for (auto& callback : m_callbacks)
	{
		callback.reset();
	}
	m_callbacks.clear();

	// クリーンアップ
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
			m_dynamicsWorld->removeRigidBody(body);
		}
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	m_collisionObjects.clear();

	// 生成した形状を削除
	for (int i = 0; i < m_collisionShapes.size(); i++)
	{
		//btCollisionShape* shape = m_collisionShapes[i];
		//delete shape;
		m_collisionShapes[i].reset();
	}
	m_collisionShapes.clear();

	// 物理演算を使用した世界の破棄
	m_dynamicsWorld.reset();

	// 拘束ソルバの破棄
	m_solver.reset();

	// ブロードフェーズの破棄
	m_overlappingPairCache.reset();

	// ディスパッチャーの破棄
	m_dispatcher.reset();

	// 当たり判定の情報の破棄
	m_collisionConfig.reset();
#endif
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void PhysicsWorld::update()
{
	float deltaTime = getDeltaTime();

	if (auto debug = getScene<SceneDebug>())
	{
		if (debug->getIsEdit())
		{
			deltaTime = 0.0f;
		}
	}

	// 物理世界の時間を進める
	m_dynamicsWorld->stepSimulation(deltaTime, MAX_SUB_STEP);
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void PhysicsWorld::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void PhysicsWorld::inspector()
{
	// 親クラスの処理
	GameObject::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void PhysicsWorld::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);
}

//---------------------------------------------------
// 形状の追加
//---------------------------------------------------
uint32_t PhysicsWorld::addCollisionShape(shared_ptr<btCollisionShape> inShape)
{
	m_collisionShapes.push_back(inShape);
	uint32_t idx = m_collisionShapes.size() - 1;
	return idx;
}

//---------------------------------------------------
// 当たり判定オブジェクトの追加
//---------------------------------------------------
void PhysicsWorld::addCollisionObject(std::shared_ptr<GameObject3D> object)
{
	// MEMO: GameObject3D::createBlockRigidBody() などで呼ぶこと

	if (object && object->getRigitBodyPtr())
	{
		m_collisionObjects[object->getRigitBodyPtr()] = object;
	}
}

//---------------------------------------------------
// 当たり判定オブジェクトの削除
//---------------------------------------------------
void PhysicsWorld::deleteCollisionObject(btCollisionObject* inCollisionObject)
{
	if (inCollisionObject)
	{
		assert(m_collisionObjects.count(inCollisionObject) != 0);
		m_collisionObjects.erase(inCollisionObject);
	}
}

//---------------------------------------------------
// キャラクターがオブジェクトに衝突しているか判定 (ペア)
//---------------------------------------------------
bool PhysicsWorld::isCollisionCharacter(
	btKinematicCharacterController* inCharacter,
	Vec3& outMove,
	bool& outIsWaterBlock,
	bool& outIsOnIce,
	bool& outIsOnGround,
	bool& outIsOnPhycics,
	bool& outIsCollision,
	std::string& outMatKey)
{
	bool isCollision = false;
	outIsOnIce = false;	// 氷の上でない
	outMatKey = "BLACK";

	Vec3 resultMove = DEF_VEC3;			// 移動量
	float deltaTime = getDeltaTime();	// デルタタイム

	float radian = inCharacter->getMaxSlope();	// 乗れる角度を degree => radian に変換する
	float groundThreshold = cos(radian);		// 着地可能な面に対する法線ベクトルの閾値

	// プレイヤーの衝突オーバーラップペアのリストを取得
	const btBroadphasePairArray& pairArray = inCharacter->getGhostObject()->getOverlappingPairCache()->getOverlappingPairArray();
	int numPairs = pairArray.size();
	outIsCollision = (numPairs > 0);	// 衝突しているか否か

	for (int i = 0; i < numPairs; i++)
	{
		const btBroadphasePair& pair = pairArray[i];
		btCollisionObject* objA = static_cast<btCollisionObject*>(pair.m_pProxy0->m_clientObject);
		btCollisionObject* objB = static_cast<btCollisionObject*>(pair.m_pProxy1->m_clientObject);
		if (!objA || !objB)
		{ // 空
			continue;
		}

		// プレイヤーのオブジェクトを特定
		btCollisionObject* playerObject = inCharacter->getGhostObject();
		btCollisionObject* collidedObject = nullptr;
		if (objA == playerObject)
		{
			collidedObject = objB;
		}
		else if (objB == playerObject)
		{
			collidedObject = objA;
		}

		if (!collidedObject)
		{ // 空
			continue;
		}

		if (m_collisionObjects.count(collidedObject) == 0)
		{ // ハッシュマップに存在しない
			continue;
		}

		// 衝突アルゴリズムを取得してチェック
		btManifoldArray manifoldArray;
		btCollisionAlgorithm* algorithm = m_dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1)->m_algorithm;
		if (algorithm)
		{
			algorithm->getAllContactManifolds(manifoldArray);

			// 衝突マニフォールドごとに衝突情報を処理
			for (int j = 0; j < manifoldArray.size(); ++j)
			{
				btPersistentManifold* manifold = manifoldArray[j];
				int numContacts = manifold->getNumContacts();

				// 衝突面の法線を取得し、移動量やブロックの種類を判定
				for (int k = 0; k < numContacts; ++k)
				{
					btManifoldPoint& pt = manifold->getContactPoint(k);
					btVector3 normal = pt.m_normalWorldOnB;

					// 3D オブジェクト
					auto object3d = m_collisionObjects[collidedObject].lock();
					if (!object3d)
					{
						continue;
					}

					// 一致している確認
					assert(object3d->getRigitBodyPtr() == collidedObject);

					// ブロックに応じて、衝突した際の処理を変える
#if 0
					if (auto blockPhysics = object3d->downcast<BlockPhysics>())
					{ // 物理挙動が特にあるブロック
						float averageScale = (blockPhysics->getScale().x + blockPhysics->getScale().y + blockPhysics->getScale().z) / 3.0f;
						btVector3 velocity = inCharacter->getLinearVelocity() * ADJUST_PHYSICS_IMPACT * (1.0f + averageScale);
						btVector3 pos = pt.getPositionWorldOnB();
						btRigidBody* rigidbody = blockPhysics->getRigitBodyPtr();
						if (rigidbody)
						{
							applyImpact(rigidbody, pos, velocity);
						}
					}
#else
					if (auto blockWood = object3d->downcast<BlockWood>())
					{ // 木ブロック
						outIsOnPhycics = true;
						float averageScale = (blockWood->getScale().x + blockWood->getScale().y + blockWood->getScale().z) / 3.0f;
						btVector3 velocity = inCharacter->getLinearVelocity() * ADJUST_PHYSICS_IMPACT * (1.0f + averageScale);
						float velocityY = velocity.getY();
						if (velocityY >= LIMIT_PHYSICS_Y)
						{
							velocity.setY(LIMIT_PHYSICS_Y);
						}
						else if (velocityY <= -LIMIT_PHYSICS_Y)
						{
							velocity.setY(-LIMIT_PHYSICS_Y);
						}

						float velocityX = velocity.getX();
						if (velocityX >= LIMIT_PHYSICS_XZ)
						{
							velocityX = LIMIT_PHYSICS_XZ;
						}
						else if (velocityX <= -LIMIT_PHYSICS_XZ)
						{
							velocityX = -LIMIT_PHYSICS_XZ;
						}

						float velocityZ = velocity.getX();
						if (velocityZ >= LIMIT_PHYSICS_XZ)
						{
							velocityZ = LIMIT_PHYSICS_XZ;
						}
						else if (velocityZ <= -LIMIT_PHYSICS_XZ)
						{
							velocityZ = -LIMIT_PHYSICS_XZ;
						}

						btVector3 pos = pt.getPositionWorldOnB();
						btRigidBody* rigidbody = blockWood->getRigitBodyPtr();
						if (rigidbody)
						{
							rigidbody->activate(true);
							applyImpact(rigidbody, pos, velocity);
						}
					}
#endif

					if (auto blockPush = object3d->downcast<BlockPush>())
					{ // 押せるブロック
						outIsOnPhycics = true;
						btVector3 velocity = inCharacter->getLinearVelocity() * ADJUST_PUSH_IMPACT;
						velocity.setY(0.0f);
#if 0
						btVector3 pos = pt.getPositionWorldOnB();
#else
						btVector3 pos = btVector3(0.0f, -1.0f, 0.0f);
#endif
						btRigidBody* rigidbody = blockPush->getRigitBodyPtr();
						if (rigidbody)
						{
							rigidbody->activate(true);
							rigidbody->applyImpulse(velocity, pos);
						}

#if USE_NEW_SHARED_DATA
						// 触れていた時間を加算
						auto& gm = GM;
						gm.lockCS();
						auto scene = getScene();
						auto data = scene->getSharedData();
						data->m_timeTouchPush += scene->getDeltaTime();
						gm.unlockCS();
#endif
					}

					if (auto blockImpact = object3d->downcast<BlockImpact>())
					{ // 衝撃を与えるブロック
						outIsOnPhycics = true;
						btVector3 velocity = inCharacter->getLinearVelocity() * ADJUST_PUSH_IMPACT;
						velocity.setY(0.0f);
#if 0
						btVector3 pos = pt.getPositionWorldOnB();
#else
						btVector3 pos = btVector3(0.0f, 0.0f, 0.0f);
#endif
						btRigidBody* rigidbody = blockImpact->getRigitBodyPtr();
						if (rigidbody)
						{
							rigidbody->activate(true);
							rigidbody->applyImpulse(velocity, pos);
						}

						if (!blockImpact->getIsEnabled())
						{
							blockImpact->setIsEnabled(true);
						}
					}

					// 着地しているか判定
					if (normal.y() > groundThreshold)
					{
						if (auto block = object3d->downcast<BlockBase>())
						{
							outMatKey = block->getMat()[0].key;
						}

						// ブロックに応じて、着地しているときの挙動を変える
						if (auto blockMove = object3d->downcast<BlockMove>())
						{ // 移動ブロック
							resultMove = blockMove->getMove() * deltaTime * HALF;
						}

						if (auto blockFall = object3d->downcast<BlockFall>())
						{ // 落下ブロック
							if (blockFall->getIsStatic())
							{
								blockFall->setIsStatic(false);		// 静的ではない状態にする
								blockFall->createBlockRigidBody();	// 剛体も作り直す
							}
						}

						if (auto blockWater = object3d->downcast<BlockWater>())
						{ // 水ブロック
							outIsWaterBlock = true;
						}

						if (auto blockRepair = object3d->downcast<BlockRepair>())
						{ // 修復されるブロック
							outIsOnPhycics = true;
							if (!blockRepair->getISCollapse() && !blockRepair->getIsRepair())
							{ // 崩壊，修復していない
								blockRepair->setIsCollapse(true);		// 崩壊開始
								blockRepair->setIsStatic(false);		// 静的ではない状態にする
								blockRepair->createBlockRigidBody();	// 剛体も作り直す
							}
						}

						if (auto blockIce = object3d->downcast<BlockIce>())
						{ // 氷ブロック
							BlockIce::ICE_STATE state = blockIce->getIceState();
							switch (state)
							{
							case BlockIce::ICE_STATE::ICE:
							{
								outIsOnIce = true;

#if USE_NEW_SHARED_DATA
								// 乗っていた時間を加算
								auto& gm = GM;
								gm.lockCS();
								auto scene = getScene();
								auto data = scene->getSharedData();
								data->m_timeOnIce += scene->getDeltaTime();
								gm.unlockCS();
#endif

							} break;
							case BlockIce::ICE_STATE::ICE_TO_WATER:
								break;
							case BlockIce::ICE_STATE::WATER:
								outIsWaterBlock = true;
								break;
							default:
								assert(false);
								break;
							}
						}

						if (auto blockPhysics = object3d->downcast<BlockPhysics>())
						{ // 物理挙動が特にあるブロック
							outIsOnPhycics = true;
							btVector3 velocity = btVector3(0.0f, -2.0f, 0.0f);
							btVector3 pos = pt.getPositionWorldOnB();
							btRigidBody* rigidbody = blockPhysics->getRigitBodyPtr();
							if (rigidbody)
							{
								applyImpact(rigidbody, pos, velocity);
							}
						}

						btRigidBody* rigidBody = object3d->getRigitBodyPtr();
						if (rigidBody)
						{
							if (!outIsWaterBlock && rigidBody->getCollisionFlags() != btCollisionObject::CF_NO_CONTACT_RESPONSE)
							{
								outIsOnGround = true;	// 着地している
							}
						}
						else
						{
							outIsOnGround = true;	// 着地している
						}

						isCollision = true;
					}

					isCollision = true;
				}
			}
		}
	}

	// 未着地
	outMove = resultMove;
	return isCollision;
}

//---------------------------------------------------
// 剛体の作成
//---------------------------------------------------
btRigidBody* PhysicsWorld::createRigidBody(
	PhysicsWorld& inOutWorld,
	btCollisionShape* inCollisionShape,
	const btTransform& inStartTransform,
	const bool& inIsStatic,
	const float& inMass,
	const Vec3& inInertia)
{
	if (!inCollisionShape)
	{
		assert(false);
		return nullptr;
	}

	// 当たり判定の形状の追加
	uint32_t idx = 0;
	{
		shared_ptr<btCollisionShape> shape(inCollisionShape);
		idx = inOutWorld.addCollisionShape(shape);
	}
	btCollisionShape* shape = inOutWorld.getCollisionShape(idx);

	// モーション状態の作成
	btDefaultMotionState* motionState = new btDefaultMotionState(inStartTransform);

	// 剛体の構成情報の作成
	btScalar mass = inMass;	// 質量
	if (inIsStatic)
	{
		mass = 0.0f;
	}
	btVector3 inertia(inInertia.x, inInertia.y, inInertia.z);	// 慣性
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);

	// 剛体の作成
	btRigidBody* rigidBody = new btRigidBody(rbInfo);

#if 0
	// 剛体の設定
	rigidBody->getBroadphaseProxy()->m_collisionFilterGroup = MY_COLLISION_GROUP;
	rigidBody->getBroadphaseProxy()->m_collisionFilterMask = MY_COLLISION_MASK;
#endif

	// TODO: 呼び出し元で、dynamicsWorld->addRigidBody() すること！

	return rigidBody;
}

//---------------------------------------------------
// キャラクターの作成
//---------------------------------------------------
btKinematicCharacterController* PhysicsWorld::createCharacter(
	PhysicsWorld& inOutWorld,
	btConvexShape* inCollisionShape,
	const btTransform& inStartTransform,
	Vec3 inGravity,
	float inMaxSlopeDegree,
	float inStepHeight,
	float inFirstSpeedJump)
{
	if (!inCollisionShape)
	{
		assert(false);
		return nullptr;
	}

	// ゴーストオブジェクトを作成
	btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
	if (!ghostObject)
	{
		assert(false);
		return nullptr;
	}

	auto dynamicsWorld = inOutWorld.getDynamicsWorld();	// 動力学世界

	// 当たり判定の形状の追加
	ghostObject->setCollisionShape(inCollisionShape);

	// 当たり判定のフラグを設定
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	// 状態の設定
	ghostObject->setWorldTransform(inStartTransform);

	// コールバック関数の設定
	auto callback = make_shared<btGhostPairCallback>();
	inOutWorld.getCallBacks().push_back(callback);
	inOutWorld.getBroadphaseInterface().getOverlappingPairCache()->setInternalGhostPairCallback(callback.get());

	// 乗れる段差の大きさの設定
	btScalar stepHeight = btScalar(inStepHeight);

	// キャラクターの作成
	btKinematicCharacterController* chara = new btKinematicCharacterController(
		ghostObject, inCollisionShape, stepHeight);
	if (!chara)
	{
		assert(false);
		return nullptr;
	}

	// 当たり判定オブジェクトの追加
	int filter = btBroadphaseProxy::DefaultFilter;
	filter |= btBroadphaseProxy::StaticFilter;
	filter |= btBroadphaseProxy::KinematicFilter;
	filter |= btBroadphaseProxy::SensorTrigger;
	dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, filter);

	// アクションの追加
	dynamicsWorld->addAction(chara);

	// 重力の適応
	chara->setGravity(btVector3(inGravity.x, inGravity.y, inGravity.z));

	// 閾値の設定 (斜めの上に乗れる最大角度)
	chara->setMaxSlope(D3DXToRadian(inMaxSlopeDegree));

	// ジャンプの初速度の設定
	chara->setJumpSpeed(inFirstSpeedJump);

	return chara;
}

//---------------------------------------------------
// 箱形状の当たり判定の作成
//---------------------------------------------------
btCollisionShape* PhysicsWorld::createCollisionBoxShape(const Vec3& inHalfExtents)
{
	// 箱形状の作成
	btCollisionShape* boxShape = new btBoxShape(btVector3(inHalfExtents.x, inHalfExtents.y, inHalfExtents.z));
	assert(boxShape != nullptr);

	return boxShape;
}

//---------------------------------------------------
// カプセル形状の当たり判定の作成
//---------------------------------------------------
btConvexShape* PhysicsWorld::createCollisionCapsuleShape(const Vec2& inRadiusAndHalfHeight)
{
	// カプセル形状の作成
	btConvexShape* capsuleShape = new btCapsuleShape(inRadiusAndHalfHeight.x, inRadiusAndHalfHeight.y);
	assert(capsuleShape != nullptr);

	return capsuleShape;
}

//---------------------------------------------------
// transform の取得
//---------------------------------------------------
btTransform PhysicsWorld::createTransform(const Vec3& inPos, const Quaternion& inRot, [[maybe_unused]] const Vec3& inOffset)
{
	// 初期化
	btTransform startTransform;
	startTransform.setIdentity();

	// 位置の設定
	{
		Vec3 adjustedPos = inPos;

		btVector3 pos = btVector3(adjustedPos.x, adjustedPos.y, adjustedPos.z);
		startTransform.setOrigin(pos);
	}

	// 向きの設定
	{
		btQuaternion quaternion(inRot.x, inRot.y, inRot.z, inRot.w);
		startTransform.setRotation(quaternion);
	}

	return startTransform;
}

//---------------------------------------------------
// 衝撃や力の適用
//---------------------------------------------------
void PhysicsWorld::applyImpact(btRigidBody* body, const btVector3& collisionPoint, const btVector3& impulse)
{
	btVector3 relativePosition = collisionPoint - body->getCenterOfMassPosition();
	body->applyImpulse(impulse, relativePosition);
}

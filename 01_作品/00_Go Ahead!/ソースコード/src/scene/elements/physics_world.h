//--------------------------------------------------------------------------------
// 
// 物理演算を使用した世界 [physics_world.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"
#include "game_object/3d/game_object_3d.h"

//===================================================
// 世界のクラス
//===================================================
class PhysicsWorld : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	PhysicsWorld();
	~PhysicsWorld() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	uint32_t addCollisionShape(shared_ptr<btCollisionShape> inShape);										// 形状の追加
	btCollisionShape* getCollisionShape(const uint32_t& inIdx) { return m_collisionShapes[inIdx].get(); }	// 形状の取得

	// TODO: flag_manager でフラグ管理
	// キャラクターがオブジェクトに衝突しているか判定 (ペア)
	bool isCollisionCharacter(
		btKinematicCharacterController* inCharacter,
		Vec3& outMove,
		bool& outIsWaterBlock,
		bool& outIsOnIce,
		bool& outIsOnGround,
		bool& outIsOnPhycics,
		bool& outIsCollision,
		std::string& outMatKey);

	void addCollisionObject(shared_ptr<GameObject3D> object);			// 当たり判定オブジェクトの追加
	void deleteCollisionObject(btCollisionObject* inCollisionObject);	// 当たり判定オブジェクトの削除

	shared_ptr<btDiscreteDynamicsWorld> getDynamicsWorld() { return m_dynamicsWorld; }		// 動力学世界の取得
	btBroadphaseInterface& getBroadphaseInterface() { return *m_overlappingPairCache; }		// 広域位相衝突判定の取得
	std::vector<shared_ptr<btGhostPairCallback>>& getCallBacks() { return m_callbacks; }	// コールバックの取得

	// 剛体の作成
	static btRigidBody* createRigidBody(
		PhysicsWorld& inOutWorld,
		btCollisionShape* inCollisionShape,
		const btTransform& inStartTransform,
		const bool& inIsStatic,
		const float& inMass,
		const Vec3& inInertia);

	// キャラクターの作成
	static btKinematicCharacterController* createCharacter(
		PhysicsWorld& inOutWorld,
		btConvexShape* inCollisionShape,
		const btTransform& inStartTransform,
		Vec3 inGravity,
		float inMaxSlopeDegree,
		float inStepHeight,
		float inFirstSpeedJump);

	static btCollisionShape* createCollisionBoxShape(const Vec3& inHalfExtents);							// 箱形状の当たり判定の作成
	static btConvexShape* createCollisionCapsuleShape(const Vec2& inRadiusAndHalfHeight);					// カプセル形状の当たり判定の作成
	static btTransform createTransform(const Vec3& inPos, const Quaternion& inRot, const Vec3& inOffset);	// transform の作成
	static void applyImpact(btRigidBody* body, const btVector3& collisionPoint, const btVector3& impulse);	// 衝撃や力の適用

private:
	//---------------------------------------------------
	// privaet メンバ関数宣言
	//---------------------------------------------------
	PhysicsWorld(const PhysicsWorld&) = delete;
	void operator=(const PhysicsWorld&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	shared_ptr<btDefaultCollisionConfiguration> m_collisionConfig;						// 衝突構成設定
	shared_ptr<btCollisionDispatcher> m_dispatcher;										// 衝突ディスパッチャ
	shared_ptr<btBroadphaseInterface> m_overlappingPairCache;							// 広域位相衝突判定
	shared_ptr<btSequentialImpulseConstraintSolver> m_solver;							// 拘束条件ソルバ
	shared_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;								// 動力学世界
	btAlignedObjectArray<shared_ptr<btCollisionShape>> m_collisionShapes;				// 形状を保持するための配列
	std::unordered_map<btCollisionObject*, weak_ptr<GameObject3D>> m_collisionObjects;	// 当たり判定オブジェクト
	std::vector<shared_ptr<btGhostPairCallback>> m_callbacks;							// コールバック
	float m_time;	// 時間
};

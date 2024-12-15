//--------------------------------------------------------------------------------
// 
// 3D オブジェクト [game_object_3d.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"

//===================================================
// オブジェクト 3D のクラス
//===================================================
class GameObject3D : public GameObject
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	GameObject3D();
	~GameObject3D() override = default;
	GameObject3D(GameObject3D&& right) noexcept;
	GameObject3D& operator=(GameObject3D&& right) noexcept;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override = 0;				// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void updateMtx();					// マトリックスの更新処理
	void createBlockRigidBody();		// ブロックの剛体の作成
	void deleteRigidBody();				// 剛体の削除 MEMO: 非推奨
	void mulMtx(const Matrix& inMat);	// マトリックスを掛ける
	void updateNotDestroy();			// 高度による破棄をしない更新処理

	void setFirstPos(const Vec3& inFirstPos) { m_firstPos = inFirstPos; }		// 最初の位置の設定
	void setPos(const Vec3& inPos) { m_pos = inPos; }							// 位置の設定
	void setFirstRot(const Quaternion& inFirstRot) { m_firstRot = inFirstRot; }	// 最初の向きの設定
	void setRot(const Quaternion& inRot) { m_rot = inRot; }						// 向きの設定
	void setScale(const Vec3& inScale) { m_scale = inScale; }					// スケールの設定
	void setMatrix(const Matrix& inMtx) { *m_mtxWorld = inMtx; }				// マトリックスの設定
	void setKey(const std::string& inKey) { m_key = inKey; }					// キーの設定
	void setRigidBody(btRigidBody* inRigidBody);								// 剛体の設定
	void setOffset(const Vec3& inOffset) { m_offset = inOffset; }				// オフセットの設定
	void setIsStatic(const bool& inIsStatic) { m_isStatic = inIsStatic; }		// 静的かどうかの設定
	void setMass(const float& inMass) { m_mass = inMass; }						// 静的かどうかの設定
	void setInertia(const Vec3& inIternia) { m_inertia = inIternia; }			// 静的かどうかの設定

	const Vec3& getFirstPos() const { return m_firstPos; }			// 最初の位置の取得
	const Vec3& getPos() const { return m_pos; }					// 位置の取得
	const Quaternion& getFirstRot() const { return m_firstRot; }	// 最初の向きの取得
	const Quaternion& getRot() const { return m_rot; }				// 向きの取得
	const Vec3& getScale() const { return m_scale; }				// スケールの取得
	shared_ptr<Matrix> getMatrix() const { return m_mtxWorld; }		// マトリックスの取得
	const std::string& getKey() const { return m_key; }				// キーの取得
	btRigidBody& getRigitBody() { return *m_rigidBody; }			// 剛体の取得
	btRigidBody* getRigitBodyPtr() { return m_rigidBody.get(); }	// 剛体の取得 (ポインタ)
	const Vec3& getOffset() const { return m_offset; }				// オフセットの取得
	const bool& getIsStatic() const { return m_isStatic; }			// 静的かどうかの取得
	const float& getMass() const { return m_mass; }					// 質量の取得
	const Vec3& getInertia() const { return m_inertia; }			// 慣性の取得

private:
	//---------------------------------------------------
	// エイリアス宣言
	//---------------------------------------------------
	using RigidBodyDeleter = std::function<void(btRigidBody*)>;	// 剛体のカスタムデリータ

	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	GameObject3D(const GameObject3D&) = delete;
	void operator=(const GameObject3D&) = delete;

	void customDeleter(btRigidBody* ptr);	// カスタムデリータ (MEMO: ラムダ式のカスタムデリータ内で呼び出す)

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_firstPos;											// 最初の位置
	Vec3 m_pos;													// 現在の位置
	Quaternion m_firstRot;										// 最初の向き
	Quaternion m_rot;											// 現在の向き
	Vec3 m_scale;												// スケール
	shared_ptr<Matrix> m_mtxWorld;								// ワールドマトリックス
	std::string m_key;											// テクスチャ or モデル or モーションのキー
	std::unique_ptr<btRigidBody, RigidBodyDeleter> m_rigidBody;	// 剛体
	btTransform m_transform;									// トランスフォーム
	Vec3 m_offset;												// 当たり判定のオフセット
	bool m_isStatic;											// 静的かどうか [true: 質量 => 0.0f / false: 質量 => 1.0f]
	float m_mass;												// 質量
	Vec3 m_inertia;												// 慣性
};

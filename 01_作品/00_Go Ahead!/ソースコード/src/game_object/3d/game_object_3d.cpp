//--------------------------------------------------------------------------------
// 
// 3D オブジェクト [game_object_3d.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/game_object_3d.h"
// // // character
#include "game_object/3d/character/player.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/file.h"

//---------------------------------------------------
// デフォルトコンストラクタ
//---------------------------------------------------
GameObject3D::GameObject3D()
	: GameObject()
	, m_firstPos(DEF_VEC3)
	, m_pos(DEF_VEC3)
	, m_firstRot(DEF_VEC3)
	, m_rot(DEF_VEC3)
	, m_scale(DEF_SCALE)
	, m_mtxWorld(make_shared<Matrix>(DEF_MTX))
	, m_transform(DEF_TRANSFORM)
	, m_offset(DEF_VEC3)
	, m_isStatic(false)
	, m_mass(0.0f)
	, m_inertia(DEF_VEC3)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
GameObject3D::GameObject3D(GameObject3D&& right) noexcept
	: GameObject(std::move(right))
	, m_firstPos(std::move(right.m_firstPos))
	, m_pos(std::move(right.m_pos))
	, m_firstRot(std::move(right.m_firstRot))
	, m_rot(std::move(right.m_rot))
	, m_scale(std::move(right.m_scale))
	, m_mtxWorld(std::move(right.m_mtxWorld))
	, m_key(std::move(right.m_key))
	, m_rigidBody(std::move(right.m_rigidBody))
	, m_transform(std::move(right.m_transform))
	, m_offset(std::move(right.m_offset))
	, m_isStatic(std::move(right.m_isStatic))
	, m_mass(std::move(right.m_mass))
	, m_inertia(std::move(right.m_inertia))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT GameObject3D::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	// 初期パラメータの設定
	{
		m_firstPos = inParam[FLAG_POS];
		m_pos = m_firstPos;
		{
			Vec3 rot = inParam[FLAG_ROT];
#if 0
			rot.x = D3DXToRadian(rot.x);
			rot.y = D3DXToRadian(rot.y);
			rot.z = D3DXToRadian(rot.z);
			Calculate::calculateQuaternionByEularYawPitchRoll(&m_firstRot, rot);
#else
			Calculate::calculateRot(&m_firstRot, AXIS_Y, D3DXToRadian(rot.y));
			Calculate::mulRot(&m_firstRot, AXIS_X, D3DXToRadian(rot.x));
			Calculate::mulRot(&m_firstRot, AXIS_Z, D3DXToRadian(rot.z));
#endif
			m_rot = m_firstRot;
		}
		m_scale = inParam[FLAG_SCALE];
		m_offset = inParam[FLAG_OFFSET];
		m_key = inParam[FLAG_KEY];
		m_isStatic = inParam[FLAG_IS_STATIC];
		m_mass = inParam[FLAG_MASS];
		m_inertia = inParam[FLAG_INERTIA];
	}

	// マトリックスの更新処理
	updateMtx();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void GameObject3D::uninit()
{
	m_rigidBody.reset();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void GameObject3D::update()
{
	if (!m_rigidBody)
	{
		// トランスフォームの更新
		m_transform.setOrigin(btVector3(m_pos.x, m_pos.y, m_pos.z));
		m_transform.setRotation(btQuaternion(m_rot.x, m_rot.y, m_rot.z, m_rot.w));
		return;
	}

	// トランスフォームの取得
	auto trans = m_rigidBody->getWorldTransform();

	// 位置の反映
	auto pos = trans.getOrigin();
	m_pos = Vec3(pos.getX(), pos.getY(), pos.getZ());

	// 向きの反映
	auto rot = trans.getRotation();
	m_rot = Quaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW());

	// トランスフォームの更新
	m_transform.setOrigin(pos);
	m_transform.setRotation(rot);

	if (m_pos.y <= MIN_WORLD_Y)
	{ // 最低高度に到達した
		setIsDestroy(true);
	}
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void GameObject3D::inspector()
{
	// 親クラスの処理
	GameObject::inspector();

	// 静的か動的かの設定
	{
		bool prevIsStatic = m_isStatic;
		ImGuiAddOns::ToggleButton("##isStatic", &m_isStatic);
		ImGui::SameLine();
		ImGui::Text("is static");

		if (m_isStatic != prevIsStatic)
		{
			// 静的状態の変更処理
			createBlockRigidBody();
		}
	}

	// Transform の設定
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 位置の設定
		{
			Vec3 pos = m_pos;
			bool isReturn = ImGuiAddOns::Transform("Position", &pos);

			if (isReturn)
			{
				// 反映
				m_firstPos = pos;
				m_pos = pos;

				// 剛体にも反映
				if (m_rigidBody)
				{
					auto trans = m_rigidBody->getWorldTransform();
					btVector3 newPos(m_pos.x, m_pos.y, m_pos.z);
					trans.setOrigin(newPos);
					m_rigidBody->setWorldTransform(trans);
				}
			}
		}

		// 向きの設定
		{
			auto rot = m_rot;

			// クォータニオンからオイラー角に変換
			Vec3 euler = DEF_VEC3;
			euler = Calculate::quaternionToEulerYXZ(rot);

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
				if (m_rigidBody)
				{
					btQuaternion newRot(rot.x, rot.y, rot.z, rot.w);
					m_rigidBody->getWorldTransform().setRotation(newRot);
				}

				// 反映
				m_firstRot = rot;
				m_rot = rot;
			}

			// リセットボタン 現在の位置を最初の位置として設定するか否か
			static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

			ImGui::SameLine();
			bool isRelease = ImGui::Button("RESET##rot");
			ImGui::PopStyleColor(3);

			if (isRelease)
			{
				m_rot = DEF_ROT;
				btQuaternion newRot(rot.x, rot.y, rot.z, rot.w);
				m_rigidBody->getWorldTransform().setRotation(newRot);
			}
		}

		// スケールの設定
		{
			// 個別
			{
				Vec3 scale = m_scale;
				bool isReturn = ImGuiAddOns::Transform("Scale", &scale);

				if (isReturn)
				{
					// 反映
					m_scale = scale;

					if (m_rigidBody)
					{ // もともと剛体が存在していたら
						// 新しく剛体を作成
						createBlockRigidBody();
					}
				}
			}

			// 一括
			{
				float scale = 0.0f;
				if (m_scale.x == m_scale.y &&
					m_scale.x == m_scale.z)
				{
					scale = m_scale.x;
				}

				bool isReturn = ImGuiAddOns::Float("Scale indivisual", &scale);
				if (isReturn)
				{
					// 反映
					m_scale.x = scale;
					m_scale.y = scale;
					m_scale.z = scale;

					if (m_rigidBody)
					{ // もともと剛体が存在していたら
						// 新しく剛体を作成
						createBlockRigidBody();
					}
				}
			}
		}

		// オフセット
		{
			Vec3 offset = m_offset;
			bool isReturn = ImGuiAddOns::Transform("Offset", &offset);

			if (isReturn)
			{
				// 反映
				m_offset = offset;

				if (m_rigidBody)
				{ // もともと剛体が存在していたら
					// 新しく剛体を作成
					createBlockRigidBody();
				}
			}
		}
	}

	// 最初の Transform の設定
	if (ImGui::CollapsingHeader("First Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 位置の設定
		{
			Vec3 pos = m_firstPos;
			bool isReturn = ImGuiAddOns::Transform("First Position", &pos);

			if (isReturn)
			{
				// 反映
				m_firstPos = pos;
			}

			// コピーボタン 現在の位置を最初の位置として設定するか否か
			static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

			bool isRelease = ImGui::Button("PASTE##pos");
			ImGui::PopStyleColor(3);

			if (isRelease)
			{
				m_firstPos = m_pos;
			}
		}

		// 向きの設定
		{
			auto rot = m_firstRot;

			// クォータニオンからオイラー角に変換
			Vec3 euler = DEF_VEC3;
			euler = Calculate::quaternionToEulerYXZ(rot);

			// radian から degree に変換
			euler.x = D3DXToDegree(euler.x);
			euler.y = D3DXToDegree(euler.y);
			euler.z = D3DXToDegree(euler.z);

			bool isReturn = ImGuiAddOns::Transform("FIrst Rotation", &euler);

			if (isReturn)
			{
				// degree から radian に変換
				Calculate::calculateRot(&rot, AXIS_Y, D3DXToRadian(euler.y));
				Calculate::mulRot(&rot, AXIS_X, D3DXToRadian(euler.x));
				Calculate::mulRot(&rot, AXIS_Z, D3DXToRadian(euler.z));

				// 反映
				m_firstRot = rot;
			}

			// コピーボタン 現在の位置を最初の位置として設定するか否か
			static constexpr float LOCAL_SAVE_COLOR_R = 1.f / 4.f;
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.8f, 0.8f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(LOCAL_SAVE_COLOR_R, 0.9f, 0.9f));

			bool isRelease = ImGui::Button("PASTE##rot");
			ImGui::PopStyleColor(3);

			if (isRelease)
			{
				m_firstRot = m_rot;
			}
		}
	}

	// Transform の設定
	if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 質量の設定
		{
			float mass = m_mass;
			bool isReturn = ImGuiAddOns::Float("mass", &mass);

			if (isReturn)
			{
				// 反映
				m_mass = mass;
				createBlockRigidBody();
			}
		}

		// 慣性の設定
		{
			Vec3 inertia = m_inertia;
			bool isReturn = ImGuiAddOns::Transform("inertia", &inertia);

			if (isReturn)
			{
				// 反映
				m_inertia = inertia;
				createBlockRigidBody();
			}
		}
	}

	// 当たり判定フラグ
	if (ImGui::CollapsingHeader("Collision Flag"/*, ImGuiTreeNodeFlags_DefaultOpen*/))
	{
		int selected = 3;

		btRigidBody* rigidBody = getRigitBodyPtr();
		int collisionFlag = rigidBody->getCollisionFlags();
		switch (collisionFlag)
		{
		case btCollisionObject::CF_DYNAMIC_OBJECT:
			selected = 0;
			break;
		case btCollisionObject::CF_STATIC_OBJECT:
			selected = 1;
			break;
		case btCollisionObject::CF_NO_CONTACT_RESPONSE:
			selected = 2;
			break;
		default:
			selected = 3;
			break;
		}

		if (ImGui::RadioButton("Dynamic", &selected, 0))
		{
			rigidBody->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		}
		if (ImGui::RadioButton("Static", &selected, 1))
		{
			rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		}
		if (ImGui::RadioButton("No Contact", &selected, 2))
		{
			rigidBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
		if (ImGui::RadioButton("Other", &selected, 3))
		{
			// DO_NOTHING
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void GameObject3D::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);

	outObject[FLAG_POS] = m_firstPos;
	{
		Vec3 rot = Calculate::quaternionToEulerYXZ(m_firstRot);
		rot.x = D3DXToDegree(rot.x);
		rot.y = D3DXToDegree(rot.y);
		rot.z = D3DXToDegree(rot.z);
		outObject[FLAG_ROT] = rot;
	}
	outObject[FLAG_SCALE] = m_scale;
	outObject[FLAG_OFFSET] = m_offset;
	outObject[FLAG_KEY] = m_key;
	outObject[FLAG_IS_STATIC] = m_isStatic;
	outObject[FLAG_MASS] = m_mass;
	outObject[FLAG_INERTIA] = m_inertia;
}

//---------------------------------------------------
// マトリックスの更新処理
//---------------------------------------------------
void GameObject3D::updateMtx()
{
	// デバイスの取得
	Device device = GM.getDevice();

	// 計算用マトリックス
	Matrix mtxScale, mtxRot, mtxTrans;	

	// ワールドマトリックスの初期化
	*m_mtxWorld = DEF_MTX;

	// スケールの反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(m_mtxWorld.get(), m_mtxWorld.get(), &mtxScale);

	// 向きを反映
	D3DXMatrixRotationQuaternion(&mtxRot, &m_rot);
	D3DXMatrixMultiply(m_mtxWorld.get(), m_mtxWorld.get(), &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(m_mtxWorld.get(), m_mtxWorld.get(), &mtxTrans);

	// オフセットを反映
	{
		Matrix mtxOffset = DEF_MTX;

		D3DXMatrixTranslation(&mtxTrans, m_offset.x, m_offset.y, m_offset.z);
		D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxTrans);

		D3DXMatrixMultiply(m_mtxWorld.get(), m_mtxWorld.get(), &mtxOffset);
	}

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, m_mtxWorld.get());
}

//---------------------------------------------------
// ブロックの剛体の作成
//---------------------------------------------------
void GameObject3D::createBlockRigidBody()
{
	auto scene = getScene<SceneBase>();
	auto world = scene->getWorld();

	// 剛体の作成と設定
	{
		auto& aabb = scene->getResManager()->getResModel(m_key).getAABB();
		Vec3 halfExtents = Collision::getHalfExtents(aabb, m_scale);
		auto shape = PhysicsWorld::createCollisionBoxShape(halfExtents);
		auto transform = PhysicsWorld::createTransform(m_pos, m_rot, m_offset);
		auto rigidBody = PhysicsWorld::createRigidBody(*world, shape, transform, m_isStatic, m_mass, m_inertia);
		setRigidBody(rigidBody);
	}

	// 世界に剛体を追加
	world->getDynamicsWorld()->addRigidBody(getRigitBodyPtr());

	// 当たり判定オブジェクトの追加
	world->addCollisionObject(shared_from_this()->downcast<GameObject3D>());
}

//---------------------------------------------------
// 剛体の削除
//---------------------------------------------------
void GameObject3D::deleteRigidBody()
{
	// MEMO: 非推奨 使用しない

	btRigidBody* rawPtr = m_rigidBody.release();

	if (!rawPtr)
	{
		return;
	}

	btCollisionObject* obj = rawPtr;
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		delete body->getMotionState();
		body->setMotionState(nullptr);
	}

	getScene<SceneBase>()->getWorld()->getDynamicsWorld()->removeCollisionObject(obj);

	delete obj;
	obj = nullptr;
}

//---------------------------------------------------
// マトリックスを掛ける
//---------------------------------------------------
void GameObject3D::mulMtx(const Matrix& inMat)
{
	D3DXMatrixMultiply(m_mtxWorld.get(), m_mtxWorld.get(), &inMat);
}

//---------------------------------------------------
// 高度による破棄をしない更新処理
//---------------------------------------------------
void GameObject3D::updateNotDestroy()
{
	if (!m_rigidBody)
	{
		// トランスフォームの更新
		m_transform.setOrigin(btVector3(m_pos.x, m_pos.y, m_pos.z));
		m_transform.setRotation(btQuaternion(m_rot.x, m_rot.y, m_rot.z, m_rot.w));
		return;
	}

	// トランスフォームの取得
	auto trans = m_rigidBody->getWorldTransform();

	// 位置の反映
	auto pos = trans.getOrigin();
	m_pos = Vec3(pos.getX(), pos.getY(), pos.getZ());

	// 向きの反映
	auto rot = trans.getRotation();
	m_rot = Quaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW());

	// トランスフォームの更新
	m_transform.setOrigin(pos);
	m_transform.setRotation(rot);
}

//---------------------------------------------------
// 剛体の設定
//---------------------------------------------------
void GameObject3D::setRigidBody(btRigidBody* inRigidBody)
{
	std::unique_ptr<btRigidBody, RigidBodyDeleter> body(inRigidBody, [this](btRigidBody* inPtr) { this->customDeleter(inPtr); });
	m_rigidBody = std::move(body);
}

//---------------------------------------------------
// カスタムデリータ
//---------------------------------------------------
void GameObject3D::customDeleter(btRigidBody* ptr)
{
	if (!ptr)
	{
		return;
	}

	btCollisionObject* obj = ptr;
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		delete body->getMotionState();
		body->setMotionState(nullptr);
	}

	auto world = getScene<SceneBase>()->getWorld();
	world->getDynamicsWorld()->removeCollisionObject(obj);
	world->deleteCollisionObject(obj);

	delete obj;
	obj = nullptr;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
GameObject3D& GameObject3D::operator=(GameObject3D&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject::operator=(std::move(right));

		m_firstPos = std::move(right.m_firstPos);
		m_pos = std::move(right.m_pos);
		m_firstRot = std::move(right.m_firstRot);
		m_rot = std::move(right.m_rot);
		m_scale = std::move(right.m_scale);
		m_mtxWorld = std::move(right.m_mtxWorld);
		m_key = std::move(right.m_key);
		m_rigidBody = std::move(right.m_rigidBody);
		m_transform = std::move(right.m_transform);
		m_offset = std::move(right.m_offset);
		m_isStatic = std::move(right.m_isStatic);
		m_mass = std::move(right.m_mass);
		m_inertia = std::move(right.m_inertia);
	}

	return *this;
}

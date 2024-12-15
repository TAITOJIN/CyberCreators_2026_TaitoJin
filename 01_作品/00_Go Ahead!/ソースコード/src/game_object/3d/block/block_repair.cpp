//--------------------------------------------------------------------------------
// 
// 修復されるブロック [block_repair.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_repair.h"
// // // character
#include "game_object/3d/character/player.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_game.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockRepair::BlockRepair()
	: BlockDummy()
	, m_isCollapse(false)
	, m_collapseTime(0.0f)
	, m_collapseTimeKeep(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockRepair::BlockRepair(BlockRepair&& right) noexcept
	: BlockDummy(std::move(right))
	, m_isCollapse(std::move(right.m_isCollapse))
	, m_collapseTime(std::move(right.m_collapseTime))
	, m_collapseTimeKeep(std::move(right.m_collapseTimeKeep))
	, m_transforms(std::move(right.m_transforms))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockRepair::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	m_collapseTime = inParam[WATERDROP_PARAM_COLLAPS_ETIME];
	m_collapseTimeKeep = m_collapseTime;

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockRepair::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockRepair::update()
{
	float deltaTime = getScene<SceneBase>()->getDeltaTime();	// デルタタイムの取得
	auto rigidBody = getRigitBodyPtr();							// 剛体の取得

	if (m_isCollapse)
	{ // 崩壊中
		// トランスフォームの追加
		m_transforms.push_back(rigidBody->getWorldTransform());

		// 崩壊時間の更新
		m_collapseTime -= deltaTime;
		if (m_collapseTime <= 0.0f)
		{ // 崩壊終了
			m_isCollapse = false;

			// 静的
			setIsStatic(true);

			// 剛体を作り直す
			createBlockRigidBody();
		}
	}
	else
	{ // 崩壊中じゃない
		if (m_transforms.empty())
		{ // 何もしていない状態
			if (getIsStatic())
			{
				// シーン情報の取得
				auto scene = getScene<SceneBase>();

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

					Collision::Sphere sphere;
					{
						// 中心位置
						sphere.pos = getPos();

						// 半径
						auto aabb = scene->getResManager()->getResModel(getKey()).getAABB();
						Vec3 scale = getScale();
						Vec3 size = (aabb.maxPos - aabb.minPos) * HALF;
						size.x *= scale.x;
						size.y *= scale.y;
						size.z *= scale.z;
						sphere.radius = (size.x + size.y + size.z) * 0.333f;
					}

					if (Collision::isCollisionSphereVsSphere(impact->getSphere(), sphere))
					{
						// トランスフォームの追加
						m_transforms.push_back(rigidBody->getWorldTransform());

						// 動的にして作り直す
						setIsStatic(false);
						createBlockRigidBody();

						btRigidBody* rigidbody = getRigitBodyPtr();
						if (rigidbody)
						{
							// 飛ばす方向の更新
							Vec3 diff = getPos() - impact->getSphere().pos;
							D3DXVec3Normalize(&diff, &diff);
							diff *= IMPACT_STRENGTH;
							rigidbody->applyImpulse(btVector3(diff.x, diff.y, diff.z), btVector3(0.0f, -1.0f, 0.0f));

							// 崩壊開始
							m_isCollapse = true;
						}

						break;
					}
				}
			}
		}
		else
		{ // 修復中
			// 剛体にトランスフォームを反映
			btTransform transform = m_transforms.back();
			rigidBody->setWorldTransform(m_transforms.back());

			// 位置の設定
			btVector3 pos = transform.getOrigin();
			setPos(Vec3(pos.getX(), pos.getY(), pos.getZ()));

			// 向きの設定
			btQuaternion rot = transform.getRotation();
			setRot(Quaternion(rot.getX(), rot.getY(), rot.getZ(), rot.getW()));

			// トランスフォームを削除
			m_transforms.pop_back();

			if (m_transforms.empty())
			{ // 空
				// 時間リセット
				m_collapseTime = m_collapseTimeKeep;
			}
		}
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockRepair::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockRepair::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();

	// Transform の設定
	if (ImGui::CollapsingHeader("Repair Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 崩壊時間の設定
		{
			float time = m_collapseTimeKeep;
			bool isReturn = ImGuiAddOns::Float("Collapse Time", &time);

			if (isReturn)
			{
				// 反映
				m_collapseTimeKeep = time;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockRepair::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[WATERDROP_PARAM_COLLAPS_ETIME] = m_collapseTime;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockRepair& BlockRepair::operator=(BlockRepair&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));

		m_isCollapse = std::move(right.m_isCollapse);
		m_collapseTime = std::move(right.m_collapseTime);
		m_collapseTimeKeep = std::move(right.m_collapseTimeKeep);
		m_transforms = std::move(right.m_transforms);
	}

	return *this;
}

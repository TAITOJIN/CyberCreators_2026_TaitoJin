//--------------------------------------------------------------------------------
// 
// 複数のブロックの一部 [block_blocks.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_blocks.h"
// game_manager
#include "game_manager/game_manager.h"
// renderer
#include "renderer/renderer.h"
// utility
#include "utility/calculate.h"
// scene
#include "scene/scene_debug.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockBlocks::BlockBlocks()
	: BlockDummy()
	, m_wasSet(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockBlocks::BlockBlocks(BlockBlocks&& right) noexcept
	: BlockDummy(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockBlocks::init(const json& inParam)
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
void BlockBlocks::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockBlocks::update()
{
	if (!getScene<SceneDebug>())
	{ // デバッグシーンじゃない
		if (m_wasSet)
		{
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

					// 剛体の取得
					btRigidBody* rigidBody = getRigitBodyPtr();
					//if (rigidBody->getMass() != 0.0f)
					{
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
			}

			// 親クラスの処理
			BlockDummy::update();

			return;
		}
		m_wasSet = true;
	}

	if (!m_mtxParent.lock())
	{
		// 親クラスの処理
		BlockDummy::update();
		return;
	}

	const Vec3& scale = getScale();
	const Quaternion& rot = getRot();
	const Vec3& pos = getPos();
	const Vec3& offset = getOffset();

	// デバイスの取得
	Device device = GM.getDevice();

	// 計算用マトリックス
	Matrix mtxScale, mtxRot, mtxTrans;
	auto mtxWorld = getMatrix();

	// ワールドマトリックスの初期化
	*mtxWorld = DEF_MTX;

	// スケールの反映
	D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxScale);

	// 向きを反映
	D3DXMatrixRotationQuaternion(&mtxRot, &rot);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxTrans);

	// オフセットを反映
	{
		Matrix mtxOffset = DEF_MTX;

		D3DXMatrixTranslation(&mtxTrans, offset.x, offset.y, offset.z);
		D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxTrans);

		D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxOffset);
	}

	// 親のマトリックスの反映
	D3DXMatrixMultiply(
		mtxWorld.get(),
		mtxWorld.get(),
		m_mtxParent.lock().get());

	// ワールドマトリックスの設定
	GM.getDevice()->SetTransform(
		D3DTS_WORLD,
		mtxWorld.get());

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, mtxWorld.get());

	// 剛体に反映
	if (auto body = getRigitBodyPtr())
	{
		Vec3 _scale, _pos;
		Quaternion _rot;
		D3DXMatrixDecompose(&_scale, &_rot, &_pos, getMatrix().get());

		body->getWorldTransform().setOrigin(btVector3(_pos.x, _pos.y, _pos.z));
		body->getWorldTransform().setRotation(btQuaternion(_rot.x, _rot.y, _rot.z, _rot.w));
	}
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockBlocks::draw() const
{
	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockBlocks::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockBlocks::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockBlocks& BlockBlocks::operator=(BlockBlocks&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));
	}

	return *this;
}

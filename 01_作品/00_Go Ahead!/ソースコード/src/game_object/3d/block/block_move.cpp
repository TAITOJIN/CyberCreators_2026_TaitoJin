//--------------------------------------------------------------------------------
// 
// 回転するブロック [block_rotate.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_move.h"
// scene
#include "scene/scene_debug.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockMove::BlockMove()
	: BlockBase()
	, m_point1(DEF_VEC3)
	, m_point2(DEF_VEC3)
	, m_targetPoint(DEF_VEC3)
	, m_move(DEF_VEC3)
	, m_atnSpeed(1.0f)
	, m_isEnabled(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockMove::BlockMove(BlockMove&& right) noexcept
	: BlockBase(std::move(right))
	, m_point1(std::move(right.m_point1))
	, m_point2(std::move(right.m_point2))
	, m_targetPoint(std::move(right.m_targetPoint))
	, m_move(std::move(right.m_move))
	, m_atnSpeed(std::move(right.m_atnSpeed))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockMove::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_point1 = inParam[FLAG_POINT1];
		m_point2 = inParam[FLAG_POINT2];
		m_targetPoint = inParam[FLAG_TARGET_POS];
		m_atnSpeed = inParam[FLAG_ATN_SPEED];
		m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockMove::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockMove::update()
{
	if (m_isEnabled)
	{
		float deltaTime = getDeltaTime();	// デルタタイム
		auto scene = getScene<SceneBase>();	// シーン
		bool isEdit = false;				// 編集中かどうか

		// 編集中かどうかの設定
		if (auto debug = getScene<SceneDebug>())
		{
			isEdit = debug->getIsEdit();
		}

		if (!isEdit && getIsStatic())
		{
			Vec3 distance = DEF_VEC3;
			if (m_targetPoint == m_point1)
			{
				distance = m_targetPoint - m_point2;
			}
			else
			{
				distance = m_targetPoint - m_point1;
			}

			// 移動量の計算
			m_move = distance * m_atnSpeed;

			// 位置の反映
			setPos(getPos() + m_move * deltaTime);
			getRigitBody().getWorldTransform().setOrigin(btVector3(getPos().x, getPos().y, getPos().z));

			// 目的の位置に到達したかどうかを判定
			{
				auto& aabb = scene->getResManager()->getResModel(getKey()).getAABB();
				Vec3 halfExtents = Collision::getHalfExtents(aabb, getScale());

				Collision::Sphere curt = { getPos(), 2.0f };
				Collision::Sphere target = { m_targetPoint, D3DXVec3Length(&halfExtents) };

				if (Collision::isCollisionSphereVsSphere(curt, target))
				{ // 到達
					// 目標地点を設定
					if (m_targetPoint == m_point1)
					{
						m_targetPoint = m_point2;
					}
					else if (m_targetPoint == m_point2)
					{
						m_targetPoint = m_point1;
					}
				}
			}
		}
	}

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockMove::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockMove::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 位置その 1
		{
			Vec3 point1 = m_point1;
			if (ImGuiAddOns::Transform("Point1", &point1))
			{
				if (m_targetPoint == m_point1)
				{
					m_targetPoint = point1;
				}
				m_point1 = point1;
			}
		}

		// 位置その 2
		{
			Vec3 point2 = m_point2;
			if (ImGuiAddOns::Transform("Point2", &point2))
			{
				if (m_targetPoint == m_point2)
				{
					m_targetPoint = point2;
				}
				m_point2 = point2;
			}
		}

		// ターゲットの設定
		{
			static std::vector<const char*> items = { "Point1", "Point2" };
			int currentItem = 0;	// 選択
			if (m_targetPoint == m_point1)
			{
				currentItem = 0;
			}
			else
			{
				currentItem = 1;
			}

			if (ImGui::Combo("Target Pos", &currentItem, items.data(), items.size()))
			{
				if (currentItem == 0)
				{
					m_targetPoint = m_point1;
				}
				else
				{
					m_targetPoint = m_point2;
				}

				// 現在の位置をリセット
				Vec3 firstPos = getFirstPos();
				setPos(firstPos);	// 現在の位置に反映
				getRigitBody().getWorldTransform().setOrigin(btVector3(firstPos.x, firstPos.y, firstPos.z));	// 剛体にも反映
			}
		}

		// 速度の調整
		{
			float atnSpeed = m_atnSpeed;
			if (ImGuiAddOns::Float("Atn Speed", &atnSpeed))
			{
				m_atnSpeed = atnSpeed;
			}
		}

		// ペアキーの設定
		{
			// 変数宣言
			static char inputBuf[256] = {};

			strncpy(&inputBuf[0], m_pairKey.c_str(), sizeof(inputBuf) - 1);
			inputBuf[sizeof(inputBuf) - 1] = '\0';

			if (ImGui::InputText("pair key", inputBuf, sizeof(inputBuf)))
			{
				// もともとの所属から削除
				{
					auto blocks = getScene<SceneBase>()->getMoveBlocks()[m_pairKey];

					auto it = blocks.begin();
					while (it != blocks.end())
					{
						if (auto block = it->lock())
						{
							if (block == shared_from_this())
							{ // 自分自身
								it = blocks.erase(it);
								break;
							}
							else
							{
								++it;
							}
						}
						else
						{
							it = blocks.erase(it);
						}
					}
				}

				// ペアキーを更新して新しく追加
				m_pairKey = inputBuf;
				auto blocks = getScene<SceneBase>()->getMoveBlocks()[m_pairKey];
				blocks.push_back(shared_from_this()->downcast<BlockMove>());
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockMove::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);

	outObject[FLAG_POINT1] = m_point1;
	outObject[FLAG_POINT2] = m_point2;
	outObject[FLAG_TARGET_POS] = m_targetPoint;
	outObject[FLAG_ATN_SPEED] = m_atnSpeed;
	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockMove& BlockMove::operator=(BlockMove&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));

		m_point1 = std::move(right.m_point1);
		m_point2 = std::move(right.m_point2);
		m_targetPoint = std::move(right.m_targetPoint);
		m_move = std::move(right.m_move);
		m_atnSpeed = std::move(right.m_atnSpeed);
	}

	return *this;
}

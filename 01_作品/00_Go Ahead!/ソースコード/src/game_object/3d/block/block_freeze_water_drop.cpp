//--------------------------------------------------------------------------------
// 
// 氷結した水雫ブロック [block_freeze_water_drop.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_freeze_water_drop.h"
// game_manager
#include "game_manager/game_manager.h"
// scene
#include "scene/scene_game.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//===================================================
	// 定数
	//===================================================
	constexpr float GRAVITY_STRENGTH = -10.0f;	// 重力の強さ

} // namespace /* anonymous */
//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockFreezeWaterDrop::BlockFreezeWaterDrop()
	: BlockDummy()
	, m_move(DEF_VEC3)
	, m_maxLife(0.0f)
	, m_life(0.0f)
	, m_firstScale(0.0f)
	, m_gravity(0.0f)
	, m_isFreeze(true)
	, m_keep(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockFreezeWaterDrop::BlockFreezeWaterDrop(BlockFreezeWaterDrop&& right) noexcept
	: BlockDummy(std::move(right))
	, m_move(std::move(right.m_move))
	, m_maxLife(std::move(right.m_maxLife))
	, m_life(std::move(right.m_life))
	, m_firstScale(std::move(right.m_firstScale))
	, m_gravity(std::move(right.m_gravity))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockFreezeWaterDrop::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockDummy::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_firstScale = inParam[WATERDROP_PARAM_FIRST_SCALE];
		m_maxLife = inParam[WATERDROP_PARAM_MAX_LIFE];
		m_life = m_maxLife;
		m_gravity = inParam[WATERDROP_PARAM_GRAVITY];
		m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];

		// スケールの反映
		setScale(Vec3(m_firstScale, m_firstScale, m_firstScale));
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockFreezeWaterDrop::uninit()
{
	// 親クラスの処理
	BlockDummy::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockFreezeWaterDrop::update()
{
	if (!m_isFreeze && !m_keep)
	{ // 氷結中でない
		// デルタタイムの取得
		float deltaTime = getScene<SceneBase>()->getDeltaTime();

		// 落下していく
		{
			m_move.y += m_gravity * deltaTime;
			setPos(getPos() + m_move);
			Vec3 pos = getPos();
			getRigitBodyPtr()->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
		}

		// 寿命を減らしていく
		m_life -= deltaTime;
		if (m_life <= 0.0f)
		{ // 寿命がなくなった
			m_life = 0.0f;	// 保持
			m_keep = true;	// 保持
		}

		// スケールの算出と反映
		{
			float scale = (m_firstScale * m_life) / m_maxLife;	// 現在のスケールの計算
			setScale(Vec3(scale, scale, scale));				// スケールの反映
		}
	}

	// 親クラスの処理
	BlockDummy::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockFreezeWaterDrop::draw() const
{
	if (m_keep)
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockFreezeWaterDrop::inspector()
{
	// 親クラスの処理
	BlockDummy::inspector();

	// Freeze Config の設定
	if (ImGui::CollapsingHeader("Freeze Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 大きさの設定
		{
			float scale = m_firstScale;
			bool isReturn = ImGuiAddOns::Float("drop scale", &scale);

			if (isReturn)
			{
				m_firstScale = scale;
				setScale(Vec3(scale, scale, scale));
			}
		}

		// 重力
		{
			float gravity = m_gravity;
			bool isReturn = ImGuiAddOns::Float("drop gravity", &gravity);

			if (isReturn)
			{
				m_gravity = gravity;
			}
		}

		// 最長寿命
		{
			float life = m_maxLife;
			bool isReturn = ImGuiAddOns::Float("ms max life", &life);

			if (isReturn)
			{
				m_maxLife = life;
				m_life = m_maxLife;
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
					auto blocks = getScene<SceneBase>()->getFreezeBlocks()[m_pairKey];

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
				auto blocks = getScene<SceneBase>()->getFreezeBlocks()[m_pairKey];
				blocks.push_back(shared_from_this()->downcast<BlockFreezeWaterDrop>());
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockFreezeWaterDrop::save(json& outObject) const
{
	if (!getDummyFlags().isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockDummy::save(outObject);

	outObject[WATERDROP_PARAM_FIRST_SCALE] = m_firstScale;
	outObject[WATERDROP_PARAM_MAX_LIFE] = m_maxLife;
	outObject[WATERDROP_PARAM_GRAVITY] = m_gravity;
	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockFreezeWaterDrop& BlockFreezeWaterDrop::operator=(BlockFreezeWaterDrop&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockDummy::operator=(std::move(right));

		m_move = std::move(right.m_move);
		m_maxLife = std::move(right.m_maxLife);
		m_life = std::move(right.m_life);
		m_firstScale = std::move(right.m_firstScale);
		m_gravity = std::move(right.m_gravity);
	}

	return *this;
}

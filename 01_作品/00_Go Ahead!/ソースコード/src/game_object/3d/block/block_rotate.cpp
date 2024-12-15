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
#include "game_object/3d/block/block_rotate.h"
// scene
#include "scene/scene_debug.h"
// utility
#include "utility/calculate.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockRotate::BlockRotate()
	: BlockBase()
	, m_axis(DEF_VEC3)
	, m_speedRot(0.0f)
	, m_isEnabled(false)
	, m_firstIsEnabled(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockRotate::BlockRotate(BlockRotate&& right) noexcept
	: BlockBase(std::move(right))
	, m_axis(std::move(right.m_axis))
	, m_speedRot(std::move(right.m_speedRot))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockRotate::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_axis = inParam[FLAG_AXIS];
		m_speedRot = inParam[FLAG_SPEED_ROT];
		m_isEnabled = inParam[PARAM_IS_ENABLED];
		m_firstIsEnabled = m_isEnabled;
		m_pairKey = inParam[FLAMEANDUPDRAFT_PARAM_PAIR_KEY];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockRotate::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockRotate::update()
{
	if (m_isEnabled)
	{
		float deltaTime = getDeltaTime();	// デルタタイム

		// 編集中かどうかの設定
		bool isEdit = false;	// 編集中かどうか
		if (auto debug = getScene<SceneDebug>())
		{
			isEdit = debug->getIsEdit();
		}

		if (!isEdit)
		{
			// 回転
			Quaternion rot = getRot();
			Calculate::mulRot(&rot, m_axis, m_speedRot * deltaTime);
			setRot(rot);

			// 当たり判定に反映
			btQuaternion deltaRot(rot.x, rot.y, rot.z, rot.w);
			getRigitBody().getWorldTransform().setRotation(deltaRot);
		}
	}

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockRotate::draw() const
{
	// 親クラスの処理
	BlockBase::draw();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockRotate::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Rotate Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 最初回転しているか否か
		{
			bool isRot = m_firstIsEnabled;
			ImGuiAddOns::ToggleButton("##isRot", &m_firstIsEnabled);
			ImGui::SameLine();
			ImGui::Text("is first rot");

			if (m_firstIsEnabled != isRot)
			{
				m_isEnabled = m_firstIsEnabled;
				setRot(getFirstRot());
			}
		}

		// 最初の向き
		{
			// クォータニオンからオイラー角に変換
			auto firstRot = getFirstRot();
			Vec3 euler = Calculate::quaternionToEulerZXY(firstRot);

			// radian から degree に変換
			euler.x = D3DXToDegree(euler.x);
			euler.y = D3DXToDegree(euler.y);
			euler.z = D3DXToDegree(euler.z);

			bool isReturn = ImGuiAddOns::Transform("First Rot", &euler);

			if (isReturn)
			{
				// degree から radian に変換
				Calculate::calculateRot(&firstRot, AXIS_X, D3DXToRadian(euler.x));
				Calculate::mulRot(&firstRot, AXIS_Y, D3DXToRadian(euler.y));
				Calculate::mulRot(&firstRot, AXIS_Z, D3DXToRadian(euler.z));

#if 0
				// 剛体に反映
				btQuaternion newRot(firstRot.x, firstRot.y, firstRot.z, firstRot.w);
				getRigitBody()->getWorldTransform().setRotation(newRot);
#endif

				// 反映
				setFirstRot(firstRot);
			}
		}

		// 回転する軸の設定
		{
			Vec3 axis = m_axis;
			bool isReturn = ImGuiAddOns::Transform("Axis", &axis);

			if (isReturn)
			{
				m_axis = axis;
			}
		}

		// 回転する速度の設定
		{
			float speedRot = m_speedRot;
			bool isReturn = ImGuiAddOns::Float("Speed Rot", &speedRot);

			if (isReturn)
			{
				m_speedRot = speedRot;
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
					auto blocks = getScene<SceneBase>()->getRotateBlocks()[m_pairKey];

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
				auto blocks = getScene<SceneBase>()->getRotateBlocks()[m_pairKey];
				blocks.push_back(shared_from_this()->downcast<BlockRotate>());
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockRotate::save(json& outObject) const
{
	// 親クラスの処理
	BlockBase::save(outObject);

	outObject[FLAG_AXIS] = m_axis;
	outObject[FLAG_SPEED_ROT] = m_speedRot;
	outObject[PARAM_IS_ENABLED] = m_firstIsEnabled;
	outObject[FLAMEANDUPDRAFT_PARAM_PAIR_KEY] = m_pairKey;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockRotate& BlockRotate::operator=(BlockRotate&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));

		m_axis = std::move(right.m_axis);
		m_speedRot = std::move(right.m_speedRot);
	}

	return *this;
}

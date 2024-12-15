//--------------------------------------------------------------------------------
// 
// ブロックのダミークラス [block_dummy.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/block_dummy.h"
// renderer
#include "renderer/renderer.h"
// utility
#include "utility/calculate.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlockDummy::BlockDummy()
	: BlockBase()
	, m_dummyFlags()
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlockDummy::BlockDummy(BlockDummy&& right) noexcept
	: BlockBase(std::move(right))
	, m_dummyFlags(std::move(right.m_dummyFlags))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlockDummy::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = BlockBase::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		std::string flag = inParam[FLAG_DUMMY_FLAGS];
		m_dummyFlags.setFlagsFromBinString(flag);
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlockDummy::uninit()
{
	// 親クラスの処理
	BlockBase::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlockDummy::update()
{
	// 当たり判定の更新
	if (auto body = getRigitBodyPtr())
	{
		int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		if (!m_dummyFlags.isFlagSet(DUMMY_FLAGS::NO_COLLISION))
		{
			if (getIsStatic())
			{ // 静的
				collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
			}
			else
			{ // 動的
				collisionFlags = btCollisionObject::CF_DYNAMIC_OBJECT;
			}
		}
		body->setCollisionFlags(collisionFlags);
	}

	// 親クラスの処理
	BlockBase::update();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlockDummy::draw() const
{
	if (m_dummyFlags.isFlagSet(BlockDummy::DUMMY_FLAGS::NO_DISP))
	{ // 描画しない
		return;
	}

	if (m_dummyFlags.isFlagSet(BlockDummy::DUMMY_FLAGS::DISP_WIRE))
	{
		// ワイヤーフレームを表示
		Renderer::onWireFrame();
	}

	// 親クラスの処理
	BlockBase::draw();

	if (m_dummyFlags.isFlagSet(BlockDummy::DUMMY_FLAGS::DISP_WIRE))
	{
		// ワイヤーフレームを非表示
		Renderer::offWireFrame();
	}
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlockDummy::inspector()
{
	// 親クラスの処理
	BlockBase::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Dummy Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 当たり判定の有無
		{
			bool curt = m_dummyFlags.isFlagSet(DUMMY_FLAGS::NO_COLLISION);
			bool prev = curt;
			ImGuiAddOns::ToggleButton("##noCollision", &curt);
			ImGui::SameLine();
			ImGui::Text("no collision");

			if (curt != prev)
			{
				// 切替
				m_dummyFlags.changeFlag(DUMMY_FLAGS::NO_COLLISION);

				int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
				if (!m_dummyFlags.isFlagSet(DUMMY_FLAGS::NO_COLLISION))
				{ // 当たり判定を行う
					// 剛体の作成
					createBlockRigidBody();

					if (getIsStatic())
					{
						collisionFlags = btCollisionObject::CF_STATIC_OBJECT;
					}
					else
					{
						collisionFlags = btCollisionObject::CF_DYNAMIC_OBJECT;
					}
				}

				// 当たり判定フラグの設定
				getRigitBody().setCollisionFlags(collisionFlags);
			}
		}

		// 表示の有無
		{
			bool curt = m_dummyFlags.isFlagSet(DUMMY_FLAGS::NO_DISP);
			bool prev = curt;
			ImGuiAddOns::ToggleButton("##noDisp", &curt);
			ImGui::SameLine();
			ImGui::Text("no disp");

			if (curt != prev)
			{
				m_dummyFlags.changeFlag(DUMMY_FLAGS::NO_DISP);
			}
		}

		// ワイヤー表示の有無
		{
			bool curt = m_dummyFlags.isFlagSet(DUMMY_FLAGS::DISP_WIRE);
			bool prev = curt;
			ImGuiAddOns::ToggleButton("##dispWire", &curt);
			ImGui::SameLine();
			ImGui::Text("disp wire");

			if (curt != prev)
			{
				m_dummyFlags.changeFlag(DUMMY_FLAGS::DISP_WIRE);
			}
		}

		// 保存するか否か
		{
			bool curt = m_dummyFlags.isFlagSet(DUMMY_FLAGS::SAVE);
			bool prev = curt;
			ImGuiAddOns::ToggleButton("##save", &curt);
			ImGui::SameLine();
			ImGui::Text("save");

			if (curt != prev)
			{
				m_dummyFlags.changeFlag(DUMMY_FLAGS::SAVE);
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlockDummy::save(json& outObject) const
{
	if (!m_dummyFlags.isFlagSet(DUMMY_FLAGS::SAVE))
	{
		return;
	}

	// 親クラスの処理
	BlockBase::save(outObject);

	outObject[FLAG_DUMMY_FLAGS] = m_dummyFlags.toBinString();
}

//---------------------------------------------------
// フラグごとの初期設定
//---------------------------------------------------
void BlockDummy::setForEachDummyFlag()
{
	if (m_dummyFlags.isFlagSet(DUMMY_FLAGS::NO_COLLISION))
	{ // 当たり判定をしない
		// 当たり判定を無効化
		int collisionFlags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
		getRigitBody().setCollisionFlags(collisionFlags);
	}
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlockDummy& BlockDummy::operator=(BlockDummy&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		BlockBase::operator=(std::move(right));

		m_dummyFlags = std::move(right.m_dummyFlags);
	}

	return *this;
}

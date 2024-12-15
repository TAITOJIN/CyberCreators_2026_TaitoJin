//--------------------------------------------------------------------------------
// 
// 3D ポリゴン [polygon_3d.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
#include "game_object/3d/polygon_3d.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/collision.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Polygon3D::Polygon3D()
	: GameObject3D()
	, m_size(DEF_VEC2)
	, m_col(DEF_COL)
	, m_center(DEF_COL)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
Polygon3D::Polygon3D(Polygon3D&& right) noexcept
	: GameObject3D(std::move(right))
	, m_size(std::move(right.m_size))
	, m_col(std::move(right.m_col))
	, m_center(std::move(right.m_center))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Polygon3D::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 初期パラメータの設定
	{
		m_size = inParam[POLYGON3D_PARAM_SIZE];
		m_col = inParam[POLYGON3D_PARAM_COLOR];
		m_center = inParam[POLYGON3D_PARAM_CENTER];
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Polygon3D::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Polygon3D::update()
{
	// 親クラスの処理
	GameObject3D::update();

	// マトリックスの更新
	GameObject3D::updateMtx();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Polygon3D::draw() const
{
	// デバイスの取得
	Device device = GM.getDevice();

	// 法線ベクトルを正規化
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// テクスチャの取得
	Texture texture = getScene<SceneBase>()->getResManager()->getResTexture(getKey()).getTexture();

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, getMatrix().get());

	// 頂点バッファをデータストリームに設定
	device->SetStreamSource(0U, m_vtxBuf.Get(), 0U, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	device->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	device->SetTexture(0U, texture);

	// ポリゴンの描画
	device->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,
		0U,
		TRISTRIP_POLYGON_NUM);

	// 法線ベクトルを元に戻す
	device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Polygon3D::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();

	// 情報の設定
	if (ImGui::CollapsingHeader("Polygon3D Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// サイズの設定
		{
			Vec2 size = m_size;
			bool isReturn = ImGuiAddOns::TransformVec2("size", &size);

			if (isReturn)
			{
				m_size = size;

				// 頂点バッファをロックし，頂点情報へのポインタを取得
				VERTEX_3D* vtx;
				m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0U);

				// 頂点情報の設定
				for (int i = 0; i < TRISTRIP_VTX_NUM; i++, vtx++)
				{
					int x = i % 2;
					int y = i / 2;

					vtx[0].pos = Vec3(-m_center.x + m_size.x * x, 0.f, m_center.y - m_size.y * y);
				}

				// 頂点バッファをアンロック
				m_vtxBuf->Unlock();
			}
		}

		// 中心点の設定
		{
			Vec2 center = m_center;
			bool isReturn = ImGuiAddOns::TransformVec2("center", &center);

			if (isReturn)
			{
				m_center = center;

				// 頂点バッファをロックし，頂点情報へのポインタを取得
				VERTEX_3D* vtx;
				m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0U);

				// 頂点情報の設定
				for (int i = 0; i < TRISTRIP_VTX_NUM; i++, vtx++)
				{
					int x = i % 2;
					int y = i / 2;

					vtx[0].pos = Vec3(-m_center.x + m_size.x * x, 0.f, m_center.y - m_size.y * y);
				}

				// 頂点バッファをアンロック
				m_vtxBuf->Unlock();
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Polygon3D::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);

	outObject[POLYGON3D_PARAM_SIZE] = m_size;
	outObject[POLYGON3D_PARAM_COLOR] = m_col;
	outObject[POLYGON3D_PARAM_CENTER] = m_center;
}

//---------------------------------------------------
// 床の生成
//---------------------------------------------------
void Polygon3D::setPlain()
{
	// 頂点の作成
	if (!createVtx())
	{
		return;
	}

	// 頂点バッファをロックし，頂点情報へのポインタを取得
	VERTEX_3D* vtx;
	m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0U);

	// 頂点情報の設定
	for (int i = 0; i < TRISTRIP_VTX_NUM; i++, vtx++)
	{
		int x = i % 2;
		int y = i / 2;

		vtx[0].pos = Vec3(-m_center.x + m_size.x * x, 0.f, m_center.y - m_size.y * y);
		vtx[0].nor = Vec3(0.0f, 1.0f, 0.0f);
		vtx[0].col = m_col;
		vtx[0].tex = Vec2(1.0f * x, 1.0f * y);
	}

	// 頂点バッファをアンロック
	m_vtxBuf->Unlock();
}

//---------------------------------------------------
// 壁の生成
//---------------------------------------------------
void Polygon3D::setWall()
{
	// 頂点の作成
	if (!createVtx())
	{
		return;
	}
#if 0
	// ポリゴン，テクスチャのサイズ
	Vec2 texSize = Vec2(m_size.x / m_blockNum.x, m_size.y / m_blockNum.y);
	Vec2 texCoord = Vec2(1.f / m_blockNum.x, 1.f / m_blockNum.y);

	// 縦，横のカウント
	int x_i, y_i;
	x_i = y_i = 0;

	// 頂点バッファをロックし，頂点情報へのポインタを取得
	VERTEX_3D* pVtx;
	m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&pVtx), 0);

	// 頂点情報の設定
	for (UINT i_vtx = 0; i_vtx < m_allVtxNum; i_vtx++, pVtx++)
	{
		x_i = i_vtx % (m_blockNum.x + 1);
		y_i = i_vtx / (m_blockNum.x + 1);

		pVtx[0].pos = Vec3(-inOrigin.x + texSize.x * x_i, inOrigin.y - texSize.y * y_i, 0.f);
		pVtx[0].nor = DEF_NOR_WALL;
		pVtx[0].col = m_col;
		pVtx[0].tex = Vec2(texCoord.x * x_i, texCoord.y * y_i);
	}

	// 頂点バッファをアンロック
	m_vtxBuf->Unlock();
#endif
}

//---------------------------------------------------
// 頂点の生成
//---------------------------------------------------
bool Polygon3D::createVtx()
{
	Device device = GM.getDevice();

	// 頂点バッファの生成
	HRESULT hr = device->CreateVertexBuffer(
		sizeof(VERTEX_3D) * TRISTRIP_VTX_NUM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		m_vtxBuf.GetAddressOf(),
		nullptr);
	if (FAILED(hr))
	{
		assert(false);
		return false;
	}

	return true;
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
Polygon3D& Polygon3D::operator=(Polygon3D&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));

		m_size = std::move(right.m_size);
		m_col = std::move(right.m_col);
		m_center = std::move(right.m_center);
	}

	return *this;
}

//--------------------------------------------------------------------------------
// 
// 2D オブジェクト [game_object_2d.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 2d
#include "game_object/2d/game_object_2d.h"
// game_manager
#include "game_manager/game_manager.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
GameObject2D::GameObject2D()
	: GameObject()
	, m_vtxBuf(nullptr)
	, m_pos(DEF_VEC3)
	, m_rot(DEF_VEC3)
	, m_size(DEF_VEC3)
	, m_col(DEF_COL)
	, m_texAnim(nullptr)
	, m_isTexAnim(false)
{
	// DO_NOTHING
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
GameObject2D::GameObject2D(GameObject2D&& right) noexcept 
	: GameObject(std::move(right))
	, m_vtxBuf(std::move(right.m_vtxBuf))
	, m_pos(std::move(right.m_pos))
	, m_rot(std::move(right.m_rot))
	, m_size(std::move(right.m_size))
	, m_col(std::move(right.m_col))
	, m_texAnim(std::move(right.m_texAnim))
	, m_isTexAnim(std::move(right.m_isTexAnim))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT GameObject2D::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	// 初期パラメータの設定
	{
		m_pos = inParam[OBJECT2D_PARAM_POS];
		m_rot = inParam[OBJECT2D_PARAM_ROT];
		m_size = inParam[OBJECT2D_PARAM_SIZE];
		m_col = inParam[OBJECT2D_PARAM_COL];
		m_texKey = inParam[OBJECT2D_PARAM_TEXKEY];
		m_isTexAnim = inParam[OBJECT2D_PARAM_ISTEXANIM];

		m_texAnim = make_unique<Tex2dAnim>();
		if (!m_texAnim)
		{
			return E_FAIL;
		}
	}

	// 頂点情報の作成
	{
		auto& gm = GM;	// ゲームの管理者の取得

		// クリティカルセクションに入る
		gm.lockCS();

		// デバイスの取得
		Device device = gm.getDevice();

		// 頂点バッファの生成
		hr = device->CreateVertexBuffer(
			sizeof(VERTEX_2D) * TRISTRIP_VTX_NUM,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_2D,
			D3DPOOL_MANAGED,
			m_vtxBuf.GetAddressOf(),
			nullptr);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// クリティカルセクションから離れる
		gm.unlockCS();
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void GameObject2D::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void GameObject2D::update()
{
	// テクスチャアニメーション
	updateTexAnim();

	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	// 対角線の長さを計算
	const float length = sqrtf(m_size.x * m_size.x + m_size.y * m_size.y) * HALF;

	// 対角線の角度を計算
	const float angle = atan2f(m_size.x, m_size.y);

	// 頂点座標の設定
	{ // 左上
		float rot = m_rot.z - (D3DX_PI - angle);
		vtx[0].pos.x = m_pos.x + sinf(rot) * length;
		vtx[0].pos.y = m_pos.y + cosf(rot) * length;
		vtx[0].pos.z = 0.0f;
	}
	{ // 右上
		float rot = m_rot.z + (D3DX_PI - angle);
		vtx[1].pos.x = m_pos.x + sinf(rot) * length;
		vtx[1].pos.y = m_pos.y + cosf(rot) * length;
		vtx[1].pos.z = 0.0f;
	}
	{ // 左下
		
		float rot = m_rot.z - angle;
		vtx[2].pos.x = m_pos.x + sinf(rot) * length;
		vtx[2].pos.y = m_pos.y + cosf(rot) * length;
		vtx[2].pos.z = 0.0f;
	}
	{ // 右下
		float rot = m_rot.z + angle;
		vtx[3].pos.x = m_pos.x + sinf(rot) * length;
		vtx[3].pos.y = m_pos.y + cosf(rot) * length;
		vtx[3].pos.z = 0.0f;
	}

	// 頂点カラーの設定
	for (int i = 0; i < TRISTRIP_VTX_NUM; i++)
	{
		vtx[i].col = m_col;
	}

	// 頂点バッファをアンロック
	hr = m_vtxBuf->Unlock();
	assert(SUCCEEDED(hr));
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void GameObject2D::draw() const
{
	// デバイスの取得
	Device device = GM.getDevice();

	// テクスチャの取得
	Texture texture = getScene<SceneBase>()->getResManager()->getResTexture(m_texKey).getTexture();

	// 頂点バッファをデータストリームに設定
	device->SetStreamSource(0U, m_vtxBuf.Get(), 0U, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	device->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	device->SetTexture(0, texture);

	// ポリゴンの描画
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0U, TRISTRIP_POLYGON_NUM);
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void GameObject2D::inspector()
{
	// 親クラスの処理
	GameObject::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void GameObject2D::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);

	outObject[OBJECT2D_PARAM_POS] = m_pos;
	outObject[OBJECT2D_PARAM_ROT] = m_rot;
	outObject[OBJECT2D_PARAM_SIZE] = m_size;
	outObject[OBJECT2D_PARAM_COL] = m_col;
	outObject[OBJECT2D_PARAM_TEXKEY] = m_texKey;
	outObject[OBJECT2D_PARAM_ISTEXANIM] = m_isTexAnim;
}

//---------------------------------------------------
// テクスチャアニメーション
//---------------------------------------------------
void GameObject2D::updateTexAnim()
{
	if (m_isTexAnim && m_texAnim)
	{
		m_texAnim->update(m_vtxBuf.Get(), getScene<SceneBase>()->getResManager()->getResTexture(m_texKey));
	}
}

//---------------------------------------------------
// テクスチャ座標の設定
//---------------------------------------------------
void GameObject2D::setTexCoord(const Vec2& inPos1, const Vec2& inPos2, const Vec2& inPos3, const Vec2& inPos4)
{
	if (m_texAnim)
	{
		m_texAnim->setTexCoord(m_vtxBuf.Get(), inPos1, inPos2, inPos3, inPos4);
	}
}

//---------------------------------------------------
// 頂点の初期設定
//---------------------------------------------------
void GameObject2D::initVtx(const int& inPatternNumU, const int& inPatternNumV)
{
	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	// テクスチャ情報の取得
	auto resTex = getScene<SceneBase>()->getResManager()->getResTexture(m_texKey);

	// 対角線の長さを計算
	const float length = sqrtf(m_size.x * m_size.x + m_size.y * m_size.y) * HALF;

	// 対角線の角度を計算
	const float angle = atan2f(m_size.x, m_size.y);

	// 座標の計算
	float u = 1.0f / static_cast<float>(resTex.getU());
	float u1 = static_cast<float>(inPatternNumU) * u;
	float u2 = static_cast<float>(inPatternNumU + 1) * u;
	float v = 1.0f / static_cast<float>(resTex.getV());
	float v1 = static_cast<float>(inPatternNumV) * v;
	float v2 = static_cast<float>(inPatternNumV + 1) * v;

	// 頂点の設定
	{ // 左上
		// 頂点座標の設定
		float rot = m_rot.z - (D3DX_PI - angle);
		vtx[0].pos.x = m_pos.x + sinf(rot) * length;
		vtx[0].pos.y = m_pos.y + cosf(rot) * length;
		vtx[0].pos.z = 0.0f;

		// テクスチャ座標の設定
		vtx[0].tex = Vec2(u1, v1);
	}
	{ // 右上
		// 頂点座標の設定
		float rot = m_rot.z + (D3DX_PI - angle);
		vtx[1].pos.x = m_pos.x + sinf(rot) * length;
		vtx[1].pos.y = m_pos.y + cosf(rot) * length;
		vtx[1].pos.z = 0.0f;

		// テクスチャ座標の設定
		vtx[1].tex = Vec2(u2, v1);
	}
	{ // 左下
		// 頂点座標の設定
		float rot = m_rot.z - angle;
		vtx[2].pos.x = m_pos.x + sinf(rot) * length;
		vtx[2].pos.y = m_pos.y + cosf(rot) * length;
		vtx[2].pos.z = 0.0f;

		// テクスチャ座標の設定
		vtx[2].tex = Vec2(u1, v2);
	}
	{ // 右下
		// 頂点座標の設定
		float rot = m_rot.z + angle;
		vtx[3].pos.x = m_pos.x + sinf(rot) * length;
		vtx[3].pos.y = m_pos.y + cosf(rot) * length;
		vtx[3].pos.z = 0.0f;

		// テクスチャ座標の設定
		vtx[3].tex = Vec2(u2, v2);
	}

	for (int i = 0; i < TRISTRIP_VTX_NUM; i++)
	{
		// rhw の設定
		vtx[i].rhw = 1.0f;

		// 頂点カラーの設定
		vtx[i].col = m_col;
	}

	// 頂点バッファをアンロック
	hr = m_vtxBuf->Unlock();
	assert(SUCCEEDED(hr));
}

//---------------------------------------------------
// テクスチャ座標の更新 (移動)
//---------------------------------------------------
void GameObject2D::updateTexCoord(const Vec2& inMove)
{
	// 頂点バッファをロック
	VERTEX_2D* vtx;
	HRESULT hr = m_vtxBuf->Lock(0U, 0U, reinterpret_cast<void**>(&vtx), 0);
	assert(SUCCEEDED(hr));

	// テクスチャ座標の設定
	vtx[0].tex += inMove;
	vtx[1].tex += inMove;
	vtx[2].tex += inMove;
	vtx[3].tex += inMove;

	// 頂点バッファをアンロック
	hr = m_vtxBuf->Unlock();
	assert(SUCCEEDED(hr));
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
GameObject2D& GameObject2D::operator=(GameObject2D&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject::operator=(std::move(right));

		m_vtxBuf = std::move(right.m_vtxBuf);
		m_pos = std::move(right.m_pos);
		m_rot = std::move(right.m_rot);
		m_size = std::move(right.m_size);
		m_col = std::move(right.m_col);
		m_texAnim = std::move(right.m_texAnim);
		m_isTexAnim = std::move(right.m_isTexAnim);
	}

	return *this;
}

//--------------------------------------------------------------------------------
// 
// レンダーラー [renderer.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// renderer
#include "renderer/renderer.h"
// game_manager
#include "game_manager/game_manager.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const std::string FACE_NAME = "Terminal";									// フォント名
	constexpr int FONT_HEIGHT = 18;												// フォントサイズ (高さ)
	constexpr int FPS_DECIMAL_PRECISION = 2;									// FPS の小数点以下の桁数
	constexpr int DELTATIME_DECIMAL_PRECISION = 3;								// デルタタイムの小数点以下の桁数
	constexpr D3DCOLOR FONT_COL = D3DCOLOR_RGBA(255, 255, 255, 255);			// フォントの色
	constexpr const char* FILENAME_SHADOW_MAP = "res\\shader\\shadow_map.fx";	// シャドウマップのファイル名
	constexpr float ATN_CHANGE_COL = 0.01f;										// 色変化の減衰率

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Renderer::Renderer()
	: m_bgColor(BG_COL)
	, m_destBgColor(m_bgColor)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Renderer::init(const HWND& inWndHandle, const BOOL& inIsWindowed)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// Direct3D オブジェクトの生成
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_d3d)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	HRESULT hr = m_d3d->GetAdapterDisplayMode(
		D3DADAPTER_DEFAULT,
		&d3ddm);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// パラメータのゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ (幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ (高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え (映像信号に同期)
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファとステンシルバッファを作成
#if 0
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして 16bit を使う
#else
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;				// デプスバッファとして 24bit を使う
#endif
	d3dpp.Windowed = inIsWindowed;								// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// Direct3D デバイスの生成 (描画処理と頂点処理をハードウェアで行う)
	hr = m_d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		inWndHandle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		m_d3dDevice.GetAddressOf());
	if (FAILED(hr))
	{
		// Direct3D デバイスの生成 (描画処理はハードウェア，頂点処理は CPU で行う)
		hr = m_d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			inWndHandle,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			m_d3dDevice.GetAddressOf());
		if (FAILED(hr))
		{
			// Direct3D デバイスの生成 (描画処理と頂点処理をCPUで行う)
			hr = m_d3d->CreateDevice(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				inWndHandle,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				m_d3dDevice.GetAddressOf());
			if (FAILED(hr))
			{
				return E_FAIL;
			}
		}
	}

#if ENABLED_SHADOW_MAP

	// Zバッファー処理を有効にする
	m_d3dDevice->SetRenderState(D3DRS_ZENABLE, true);

	// スペキュラ（鏡面反射）を無効にする
	m_d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);

	// シャドウマップの初期設定
	hr = initShadowMap();
	if (FAILED(hr))
	{
		assert(false);
		return E_FAIL;
	}
#else
	// レンダーステートの設定
	m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	m_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
#endif

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Renderer::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Renderer::update()
{
	// 背景の色の更新
	m_bgColor += (m_destBgColor - m_bgColor) * ATN_CHANGE_COL;
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Renderer::draw(const std::function<void()>& inDrawFunc, const std::function<void()>& /*inDrawWithShadowFunc*/)
{
#if ENABLED_SHADOW_MAP
	// 通常時のバックバッファ，Ｚバッファを保存していく (後でもとのレンダリングターゲットに戻す際に必要)
	m_d3dDevice->GetRenderTarget(0, &m_backBuf);
	m_backBuf->Release();
	m_d3dDevice->GetDepthStencilSurface(&m_zBuf);
	m_zBuf->Release();

	// カメラとライトの設定
	if (m_setCameraAndLight)
	{
		m_setCameraAndLight();
	}

	// シャドウマップのサイズを取得
	D3DSURFACE_DESC desc;
	m_depthTexture->GetLevelDesc(0, &desc);
	m_effect->SetFloat("TexWidth", static_cast<float>(desc.Width));
	m_effect->SetFloat("TexHeight", static_cast<float>(desc.Height));

	// 画面クリア (バックバッファ && Z バッファのクリア)
	m_d3dDevice->Clear(0, nullptr,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		BG_COL, 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(m_d3dDevice->BeginScene()))
	{ // 描画開始が成功した場合

		// シェーダーで、シーンを深度用テクスチャ上にレンダリング
		{
			ComPtr<IDirect3DSurface9> texSurf;
			m_depthTexture->GetSurfaceLevel(0U, texSurf.GetAddressOf());
			m_d3dDevice->SetRenderTarget(0U, texSurf.Get());
			m_d3dDevice->SetDepthStencilSurface(m_texZBuf.Get());
			texSurf.Reset();

			m_d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
			m_d3dDevice->Clear(0, nullptr,
				(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
				BG_COL, 1.0f, 0U);
			m_effect->SetTechnique("tecMakeDepthTexture");

			D3DXMATRIX mtxView;
			m_d3dDevice->GetTransform(D3DTS_VIEW, &mtxView);
			D3DXMATRIX mtxProj;
			m_d3dDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

			m_effect->Begin(nullptr, 0U);

			// 描画
			inDrawFunc();

			m_effect->End();
		}

		// 通常のフレームバッファにシーンをレンダリング (シェーダー内では、同時にシャドウマップをカメラ空間にマッピング)
		{
			m_d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
			m_d3dDevice->SetRenderTarget(0U, m_backBuf.Get());
			m_d3dDevice->SetDepthStencilSurface(m_zBuf.Get());

			m_effect->SetTechnique("tecRenderScene");
			m_effect->SetTexture("DepthTex", m_depthTexture.Get());

			m_effect->Begin(nullptr, 0U);

			if (inDrawWithShadowFunc)
			{
				inDrawWithShadowFunc();
			}

			m_effect->End();
		}

		// ImGui の描画処理
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		// 描画終了
		m_d3dDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

	// リセット
	m_setCameraAndLight = nullptr;
#else
	// 画面クリア (バックバッファ && Z バッファのクリア)
	m_d3dDevice->Clear(0U, nullptr,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		m_bgColor, 1.0F, 0U);

	// 描画開始
	if (SUCCEEDED(m_d3dDevice->BeginScene()))
	{ // 描画開始が成功した場合

		// 描画
		inDrawFunc();

		// ImGui の描画処理
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		// 描画終了
		m_d3dDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
#endif
}

//---------------------------------------------------
// α ブレンドを加算合成にする
//---------------------------------------------------
void Renderer::setAlphaBlendAdd()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

//---------------------------------------------------
// α ブレンドを元に戻す
//---------------------------------------------------
void Renderer::resetAlphaBlend()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//---------------------------------------------------
// ワイヤーフレームの表示
//---------------------------------------------------
void Renderer::onWireFrame()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//---------------------------------------------------
// ワイヤーフレームの非表示
//---------------------------------------------------
void Renderer::offWireFrame()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//---------------------------------------------------
// ライトを有効にする
//---------------------------------------------------
void Renderer::onLight()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);	// ライトを有効にする
}

//---------------------------------------------------
// ライトを無効にする
//---------------------------------------------------
void Renderer::offLight()
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);	// ライトを無効にする
}

//---------------------------------------------------
// シャドウマップの初期設定
//---------------------------------------------------
HRESULT Renderer::initShadowMap()
{
	// 深度値格納用のテクスチャオブジェクトの作成
	HRESULT hr = D3DXCreateTexture(m_d3dDevice.Get(), SCREEN_WIDTH, SCREEN_HEIGHT, 0U,
		D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, m_depthTexture.GetAddressOf());
	if (FAILED(hr))
	{
		hr = D3DXCreateTexture(m_d3dDevice.Get(), SCREEN_WIDTH, SCREEN_HEIGHT, 0U,
			D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, m_depthTexture.GetAddressOf());
		if (FAILED(hr))
		{
			assert(false && "テクスチャの作成に失敗");
			return E_FAIL;
		}
	}

	// テクスチャをレンダリングターゲットにする際の Z バッファを作成
	D3DSURFACE_DESC desc;
	m_depthTexture->GetLevelDesc(0, &desc);
	hr = m_d3dDevice.Get()->CreateDepthStencilSurface(desc.Width, desc.Height,
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, true, m_texZBuf.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		assert(false && "Z バッファの作成に失敗");
		return E_FAIL;
	}

	// シェーダーを読み込む
	hr = D3DXCreateEffectFromFile(m_d3dDevice.Get(), FILENAME_SHADOW_MAP, nullptr, nullptr,
		D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION, nullptr, m_effect.GetAddressOf(), nullptr);
	if (FAILED(hr))
	{
		assert(false && "シェーダーファイル読み込み失敗");
		return E_FAIL;
	}

	return S_OK;
}

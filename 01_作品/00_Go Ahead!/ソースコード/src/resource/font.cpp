//--------------------------------------------------------------------------------
// 
// フォント [font.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// system
#include "resource/font.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/enum.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
FontManager::FontManager()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT FontManager::init()
{
	HRESULT hr = m_fonts[FONT_TYPE::CURRENT_TIME].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	hr = m_fonts[FONT_TYPE::CURRENT_TIME_MS].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	hr = m_fonts[FONT_TYPE::BEST_TIME].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	hr = m_fonts[FONT_TYPE::BEST_TIME_MS].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	hr = m_fonts[FONT_TYPE::CURRENT_COIN].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	hr = m_fonts[FONT_TYPE::BEST_COIN].init("res\\font\\ZenMaruGothic-Black.ttf", "Zen Maru Gothic Bold");
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void FontManager::uninit()
{
	for (auto& font : m_fonts)
	{
		font.second.uninit();
	}
	m_fonts.clear();
}

//---------------------------------------------------
// 描画
//---------------------------------------------------
void FontManager::draw()
{
	for (auto& font : m_fonts)
	{
		font.second.draw();
	}
}

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Font::Font()
	: m_font(nullptr)
	, m_fmtFlag(0U)
	, m_rect()
	, m_color(DEF_COL)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Font::init(const std::string& inFilename, const std::string& inFontName)
{
	m_filename = inFilename;
	m_fontName = inFontName;

	int result = AddFontResourceEx(m_filename.c_str(), FR_PRIVATE, nullptr);
	if (result == GDI_ERROR)
	{
		assert(false && "フォントの読み込みに失敗");
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Font::uninit()
{
	RemoveFontResourceEx(m_filename.c_str(), FR_PRIVATE, nullptr);
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Font::draw()
{
	if (!m_font)
	{
		return;
	}

	// テキストの描画
	m_font->DrawText(nullptr, m_text.c_str(), -1, &m_rect, m_fmtFlag, m_color);
}

//---------------------------------------------------
// テキストを描画する処理
//---------------------------------------------------
void Font::setText(
	const Vec2& inDrawPos,
	const uint32_t& inFontSize,
	const Color& inRGBA,
	const uint32_t& inFormatFlag,
	const std::string& inText)
{
	// 初期化
	m_font.Reset();
	m_fmtFlag = DT_LEFT;

	// パラメータの設定
	m_color = inRGBA;
	m_text = inText;

	// デバイスの取得
	Device device = GM.getDevice();

	// デフォルトで原点は左上
	m_rect.left = static_cast<LONG>(inDrawPos.x);
	m_rect.top = static_cast<LONG>(inDrawPos.y);
	m_rect.right = static_cast<LONG>(SCREEN_WIDTH);
	m_rect.bottom = static_cast<LONG>(SCREEN_HEIGHT);

	// フォントの生成
	D3DXCreateFont(
		device, inFontSize, 0U, FW_HEAVY, 1U,
		FALSE,
		DEFAULT_CHARSET,
		OUT_TT_ONLY_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE,
		TEXT(m_fontName.c_str()),
		m_font.GetAddressOf());
	if (!m_font)
	{
		assert(false);
	}

	// 指定なし
	{
		if (inFormatFlag == Enum::from(FMT_FLAG::NONE))
		{
			m_rect = { (LONG)inDrawPos.x, (LONG)inDrawPos.y, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
			m_fmtFlag = DT_LEFT;
		}
	}

	// 原点の一括指定
	{
		if (inFormatFlag & Enum::from(FMT_FLAG::LEFT))
		{
			m_rect = { (LONG)inDrawPos.x, (LONG)inDrawPos.y, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::RIGHT))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect = { (LONG)inDrawPos.x + textRect.right, (LONG)(inDrawPos.y), (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::CENTER))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			// テキストのサイズの半分
			D3DXVECTOR2 textHalfSize = { (float)(textRect.right), (float)(textRect.bottom) };
			textHalfSize *= 0.5f;

			m_rect = { (LONG)(inDrawPos.x - textHalfSize.x), (LONG)(inDrawPos.y - textHalfSize.y), (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		}
	}

	// 原点の個別指定
	{
		if (inFormatFlag & Enum::from(FMT_FLAG::LEFT_WIDTH))
		{
			m_rect.left = (LONG)inDrawPos.x;
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::RIGHT_WIDTH))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect.left = (LONG)inDrawPos.x + textRect.right;
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::TOP))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect.top = (LONG)(inDrawPos.y);
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::BOTTOM))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect.top = (LONG)inDrawPos.y - textRect.bottom;
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::CENTER_WIDTH))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect.left = (LONG)(inDrawPos.x + textRect.right * 0.5f);
		}

		if (inFormatFlag & Enum::from(FMT_FLAG::CENTER_HEIGHT))
		{
			// テキストの幅と高さを取得
			RECT textRect = getTextRect(inText);

			m_rect.top = (LONG)(inDrawPos.y - textRect.bottom * 0.5f);
		}
	}

	// 追加機能
	{
		if (inFormatFlag & Enum::from(FMT_FLAG::WORDBREAK))
		{
			m_fmtFlag |= DT_WORDBREAK;
		}
	}
}

//---------------------------------------------------
// テキストの矩形サイズを取得
//---------------------------------------------------
RECT Font::getTextRect(const std::string& inText)
{
	RECT rect{};
	m_font->DrawText(nullptr, inText.c_str(), -1, &rect, DT_CALCRECT, Color(1.0f, 1.0f, 1.0f, 1.0f));
	return rect;
}

//--------------------------------------------------------------------------------
// 
// フォント [font.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// 前方宣言
//---------------------------------------------------
class Font;

//===================================================
// フォントの管理者
//===================================================
class FontManager
{
public:
	//---------------------------------------------------
	// フォントの種類
	//---------------------------------------------------
	enum class FONT_TYPE : uint32_t
	{
		CURRENT_TIME=0U,	// 今回のタイム min sec
		CURRENT_TIME_MS,	// 今回のタイム millisecond
		BEST_TIME,			// ベストタイム min sec
		BEST_TIME_MS,		// ベストタイム millisecond
		CURRENT_COIN,		// 今回のコインの数
		BEST_COIN			// ベストコインの数
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	HRESULT init();
	void uninit();
	void draw();

	Font& getFont(const FONT_TYPE& inType) { return m_fonts[inType]; }

	static FontManager& getInstance() { static FontManager fm; return fm; }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	FontManager();
	~FontManager() = default;
	FontManager(const FontManager&) = delete;
	void operator=(const FontManager&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	std::unordered_map<FONT_TYPE, Font> m_fonts;
};

//===================================================
// フォントのクラス
//===================================================
class Font
{
public:
	// 描画する形式
	enum class FMT_FLAG : DWORD
	{
		NONE = 0x00000000,
		LEFT = 1 << 0,			// テキストの "原点" の位置 : 左上
		RIGHT = 1 << 1,			// テキストの "原点" の位置 : 右上
		CENTER = 1 << 2, 		// テキストの "原点" の位置 : 中心
		LEFT_WIDTH = 1 << 3,	// テキストの "横の原点" の位置 : 左
		RIGHT_WIDTH = 1 << 4,	// テキストの "横の原点" の位置 : 左
		TOP = 1 << 5,			// テキストの "縦の原点" の位置 : 上
		BOTTOM = 1 << 6,		// テキストの "縦の原点" の位置 : 下
		CENTER_WIDTH = 1 << 7,	// テキストの "横の原点" の位置 : 中心
		CENTER_HEIGHT = 1 << 8,	// テキストの "縦の原点" の位置 : 中心
		WORDBREAK = 1 << 9,		// 単語を区切る。行の終端が描画領域の幅を越えそうな場合、その単語の前で改行を挿入する。
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Font();
	~Font() = default;

	HRESULT init(const std::string& inFilename, const std::string& inFontName);	// 初期設定
	void uninit();	// 終了処理
	void draw();	// 描画処理

	/**
　　　* @brief テキストを描画する処理
　　　*
　　　* 引数に渡された文字列をスクリーン上に描画する処理
　　　*
　　　* @param [in] inDrawPos 描画する位置 (左上が原点)
　　　* @param [in] inFontSize フォントのサイズ
　　　* @param [in] inRGBA 色 (rgba の形式で 0.0f ~ 1.0f)
　　　* @param [in] inFormatFlag 形式 (FMT_FLAG) ※ デフォルトで左上に設定される。設定しなかった場合。
　　　* @param [in] inText 描画したいテキスト
　　　*/
	void setText(
		const Vec2& inDrawPos,
		const uint32_t& inFontSize,
		const Color& inRGBA,
		const uint32_t& inFormatFlag,
		const std::string& inText);

	/**
	 * @brief テキストの矩形サイズを取得
	 *
	 * 引数に渡された文字列の幅と高さを取得
	 *
	 * @param[in] inText 描画したいテキスト

	 * @return 矩形のサイズ
	 */
	RECT getTextRect(const std::string& inText);

	const std::string& getText() const { return m_text; }
	ID3DXFont* getFont() { return m_font.Get(); }

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Font(const Font&) = delete;
	void operator=(const Font&) = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	ComPtr<ID3DXFont> m_font;	// フォント
	std::string m_filename;		// ファイル名 (ファイルのパス)
	std::string m_fontName;		// フォント名 (MEMO: フォントファイル右クリック => プロパティ => 詳細 => タイトル)
	std::string m_text;			// 描画するテキスト
	DWORD m_fmtFlag;			// フォーマットフラグ
	RECT m_rect;				// 矩形
	Color m_color;				// 色
};

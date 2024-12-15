//--------------------------------------------------------------------------------
// 
// デバッグのシーン [scene_debug.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
#include "scene/scene_base.h"

//===================================================
// デバッグシーンのクラス
//===================================================
class SceneDebug : public SceneBase
{
public:
	//---------------------------------------------------
	// 優先度
	//---------------------------------------------------
	enum class PRIORITY : uint32_t
	{
		BULLET = static_cast<uint32_t>(SceneBase::BASE_PRIORITY::MAX),	// 弾
		ENEMY,			// 敵
		ITEM,			// アイテム
		EXPLOSION,		// 爆発
		EFFECT,			// エフェクト

		POLYGON_3D,		// ポリゴン
		MESHFIELD_3D,	// メッシュフィールド
		BILLBOARD,		// ビルボード
		MODEL,			// モデル
		MOTION,			// モーション

		NUMBER,			// 数字
		SCORE,			// スコア
		UI,				// UI
		MAX
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	SceneDebug();
	~SceneDebug() override = default;

	HRESULT init() override;	// 初期設定
	void uninit() override;		// 終了処理
	void update() override;		// 更新処理
	void draw() const override;	// 描画処理

	const bool& getIsEdit() const { return m_isEdit; }	// 編集状態の取得

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	SceneDebug(const SceneDebug&) = delete;
	void operator=(const SceneDebug&) = delete;

	void objectList();								// 配置しているオブジェクトの一覧
	void save(const std::string& inFilename);		// 保存
	void loadCreatbleObjectList();					// 生成可能なオブジェクトの一覧の読み込み
	void tool(bool& outResult);						// ツール
	void inspector();								// インスペクター
	void guizmo();									// ギズモ
	void operation();								// 操作
	void panel();									// パネル
	void setPanelObject(const std::string& inName);	// パネルに表示するオブジェクの設定
	void addMat();									// マテリアルの追加

	void loadIni(const char* inFilename);	// ini ファイルの読み込み

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_isSelected;						// 選択しているかどうか
	weak_ptr<GameObject> m_selected;		// 選択しているオブジェクト
	bool m_isEdit;							// 編集するか否か
	char m_saveFilename[MAX_WORD];			// 保存ファイル名
	char m_loadFilename[MAX_WORD];			// 読み込みファイル名
	std::unordered_map<std::string, Texture> m_creatbleObjectList;	// 生成可能なオブジェクトの一覧
	bool m_isListImageHovered;				// 一覧の画像の上にカーソルがあるかどうか
	Texture* m_selectedImage;				// 選択している画像
	std::string m_selectedCreateName;		// 選択している生成したいオブジェクトの名前
	float m_mouseOperationSpeed;			// マウス操作でのオブジェクの移動速度
	std::string m_iniFilename;				// INI ファイル名
	ini::IniFile m_ini;						// INI ファイルの情報
};

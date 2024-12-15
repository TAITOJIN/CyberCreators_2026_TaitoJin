//--------------------------------------------------------------------------------
// 
// 拡張機能 [add-ons.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// マクロ定義
//---------------------------------------------------
// IAO (ImGui Add-Ons)
#define IAO_IS_WND_HOVERED (ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))	// カーソルがウィンドウに被さっているか

//===================================================
// ImGui の拡張機能の名前空間
//===================================================
namespace ImGuiAddOns
{
	//---------------------------------------------------
	// プロトタイプ宣言
	//---------------------------------------------------
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);	// BufferingBar
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color);	// Spinner
	void ToggleButton(const char* str_id, bool* v);	// ToggleButton

	// notify
	void UpdateNotify();	// 通知の更新処理 (MEMO: これを呼び出さないと描画されない)
	void NotifySuccess(const std::string& inMsg);	// 成功通知
	void NotifyWarning(const std::string& inMsg);	// 警告通知
	void NotifyError(const std::string& inMsg);		// エラー通知
	void NotifyInfo(const std::string& inMsg);		// 情報通知
	void NotifyCustom(const ImGuiToastType_& inType, const int& inDuration, const std::string& inTitle, const std::string& inMsg);	// カスタム通知

	// others
	bool Transform(const std::string& inTitle, D3DXVECTOR3* outResult);		// Transform (MEMO: 返り値 => 入力を決定したかどうか)
	bool TransformVec2(const std::string& inTitle, D3DXVECTOR2* outResult);	// Transform Vector2 (MEMO: 返り値 => 入力を決定したかどうか)
	bool Float(const std::string& inTitle, float* outResult);				// float (MEMO: 返り値 => 入力を決定したかどうか)
	bool Int(const std::string& inTitle, int* outResult);					// int (MEMO: 返り値 => 入力を決定したかどうか)
	void Material(const std::string& inTitle, D3DCOLORVALUE& inOutResult);	// Material
	void Color(const std::string& inTitle, D3DXCOLOR& inOutResult);			// 色
}

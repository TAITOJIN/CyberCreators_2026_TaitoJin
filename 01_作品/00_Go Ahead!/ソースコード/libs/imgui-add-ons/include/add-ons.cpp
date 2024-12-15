//--------------------------------------------------------------------------------
// 
// 拡張機能 [add-ons.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// imgui
#include "add-ons.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr int DURATION = 3000;	// 通知の間隔

} // namespace /* anonymous */

//===================================================
// ImGui の拡張機能の名前空間
//===================================================
namespace ImGuiAddOns
{
	//---------------------------------------------------
	// BufferingBar
	//---------------------------------------------------
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{ return false; }

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2.f;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{ return false; }

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = static_cast<float>(g.Time);
		const float r = size.y / 2.f;
		const float speed = 1.5f;

		const float a = speed * 0.f;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);

		return true;
	}

	//---------------------------------------------------
	// Spinner
	//---------------------------------------------------
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{ return false; }

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
		{ return false; }

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = static_cast<int>(abs(ImSin(static_cast<float>(g.Time) * 1.8f) * (num_segments - 5)));

		const float a_min = IM_PI * 2.f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; ++i)
		{
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + static_cast<float>(g.Time) * 8) * radius,
				centre.y + ImSin(a + static_cast<float>(g.Time) * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, static_cast<float>(thickness));

		return true;
	}

	//---------------------------------------------------
	// ToggleButton
	//---------------------------------------------------
	void ToggleButton(const char* str_id, bool* v)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height * 1.55f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
		{ (*v) = !(*v); }

		float t = (*v) ? 1.f : 0.f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))	// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
		{ col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.f), ImVec4(0.64f, 0.83f, 0.34f, 1.f), t)); }
		else
		{ col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.f), ImVec4(0.56f, 0.83f, 0.26f, 1.f), t)); }

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	}

	//---------------------------------------------------
	// 通知の更新処理
	//---------------------------------------------------
	void UpdateNotify()
	{
		// 色
		static constexpr float BLACK = 43.f / 255.f;
		static constexpr float WHIET = 1.f;	// 255 / 255

		// スタイルの設定
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(BLACK, BLACK, BLACK, 200.f / 255.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(WHIET, WHIET, WHIET, 200.f / 255.f));

		// notify の描画処理
		ImGui::RenderNotifications();

		// 破棄
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
	}

	//---------------------------------------------------
	// 成功通知
	//---------------------------------------------------
	void NotifySuccess(const std::string& inMsg)
	{
		ImGui::InsertNotification({ ImGuiToastType_Success, DURATION, inMsg.c_str() });
	}

	//---------------------------------------------------
	// 警告通知
	//---------------------------------------------------
	void NotifyWarning(const std::string& inMsg)
	{
		ImGui::InsertNotification({ ImGuiToastType_Warning, DURATION, inMsg.c_str() });
	}

	//---------------------------------------------------
	// エラー通知
	//---------------------------------------------------
	void NotifyError(const std::string& inMsg)
	{
		ImGui::InsertNotification({ ImGuiToastType_Error, DURATION, inMsg.c_str() });
	}

	//---------------------------------------------------
	// 情報通知
	//---------------------------------------------------
	void NotifyInfo(const std::string& inMsg)
	{
		ImGui::InsertNotification({ ImGuiToastType_Info, DURATION, inMsg.c_str() });
	}

	//---------------------------------------------------
	// カスタム通知
	//---------------------------------------------------
	void NotifyCustom(const ImGuiToastType_& inType, const int& inDuration, const std::string& inTitle, const std::string& inMsg)
	{
		IM_ASSERT(inType < ImGuiToastType_COUNT);

		ImGuiToast toast(inType, inDuration);

		toast.set_title(inTitle.c_str());
		toast.set_content(inMsg.c_str());

		ImGui::InsertNotification(toast);
	}

	//---------------------------------------------------
	// Transform
	//---------------------------------------------------
	bool Transform(const std::string& inTitle, D3DXVECTOR3* outResult)
	{
		// 定数
		static const std::string LOCAL_ID_TRANSFORM = "##TRANS_";	// ウィンドウの ID (MEMO: 他と被らないように)
		static constexpr float LOCAL_POS_WIDTH = 80.f;				// アイテムの位置 (MEMO: x, y, z の入力部の横の位置)
		static constexpr int LOCAL_ITEM_WIDTH = 50;					// アイテムのサイズ (MEMO: InputFloat のサイズ)
		static constexpr int LOCAL_ADJUST_Y = 3;					// 文字の位置の高さの調整 (MEMO: item の後に SameLine すると下にずれるから)

		// 決定したかどうか
		bool isReturn = false;

		// タイトル
		ImGui::Text(inTitle.c_str());

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(-ImGui::GetCursorPosX() + LOCAL_POS_WIDTH, 0));
		ImGui::SameLine();

		// X
		{
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

			std::string name = LOCAL_ID_TRANSFORM + inTitle + "X";
			if (ImGui::InputFloat(name.c_str(), &outResult->x, 0.f, 0.f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
			{ isReturn = true; }
		}

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - LOCAL_ADJUST_Y);

		// Y
		{
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

			std::string name = LOCAL_ID_TRANSFORM + inTitle + "Y";
			if (ImGui::InputFloat(name.c_str(), &outResult->y, 0.f, 0.f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
			{ isReturn = true; }
		}

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - LOCAL_ADJUST_Y);

		// Z
		{
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

			std::string name = LOCAL_ID_TRANSFORM + inTitle + "Z";
			if (ImGui::InputFloat(name.c_str(), &outResult->z, 0.f, 0.f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
			{ isReturn = true; }
		}

		return isReturn;
	}

	//---------------------------------------------------
	// Transform Vector2
	//---------------------------------------------------
	bool TransformVec2(const std::string& inTitle, D3DXVECTOR2* outResult)
	{
		// 定数
		static const std::string LOCAL_ID_TRANSFORM = "##TRANSVEC2_";	// ウィンドウの ID (MEMO: 他と被らないように)
		static constexpr float LOCAL_POS_WIDTH = 80.f;				// アイテムの位置 (MEMO: x, y, z の入力部の横の位置)
		static constexpr int LOCAL_ITEM_WIDTH = 50;					// アイテムのサイズ (MEMO: InputFloat のサイズ)
		static constexpr int LOCAL_ADJUST_Y = 3;					// 文字の位置の高さの調整 (MEMO: item の後に SameLine すると下にずれるから)

		// 決定したかどうか
		bool isReturn = false;

		// タイトル
		ImGui::Text(inTitle.c_str());

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(-ImGui::GetCursorPosX() + LOCAL_POS_WIDTH, 0));
		ImGui::SameLine();

		// X
		{
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

			std::string name = LOCAL_ID_TRANSFORM + inTitle + "X";
			if (ImGui::InputFloat(name.c_str(), &outResult->x, 0.f, 0.f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				isReturn = true;
			}
		}

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - LOCAL_ADJUST_Y);

		// Y
		{
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

			std::string name = LOCAL_ID_TRANSFORM + inTitle + "Y";
			if (ImGui::InputFloat(name.c_str(), &outResult->y, 0.f, 0.f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				isReturn = true;
			}
		}

		return isReturn;
	}

	//---------------------------------------------------
	// float
	//---------------------------------------------------
	bool Float(const std::string& inTitle, float* outResult)
	{
		// 定数
		static const std::string LOCAL_ID_TRANSFORM = "##FLOAT_";	// ウィンドウの ID (MEMO: 他と被らないように)
		static constexpr float LOCAL_POS_WIDTH = 96.f;				// アイテムの位置
		static constexpr int LOCAL_ITEM_WIDTH = 50;					// アイテムのサイズ (MEMO: InputFloat のサイズ)

		// 決定したかどうか
		bool isReturn = false;

		// タイトル
		ImGui::Text(inTitle.c_str());

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(-ImGui::GetCursorPosX() + LOCAL_POS_WIDTH, 0));
		ImGui::SameLine();

		ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

		std::string name = LOCAL_ID_TRANSFORM + inTitle;
		if (ImGui::InputFloat(name.c_str(), outResult, 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			isReturn = true;
		}

		return isReturn;
	}


	//---------------------------------------------------
	// unsigned int
	//---------------------------------------------------
	bool Int(const std::string& inTitle, int* outResult)
	{
		// 定数
		static const std::string LOCAL_ID_TRANSFORM = "##INT_";	// ウィンドウの ID (MEMO: 他と被らないように)
		static constexpr float LOCAL_POS_WIDTH = 96.f;			// アイテムの位置
		static constexpr int LOCAL_ITEM_WIDTH = 50;				// アイテムのサイズ (MEMO: InputInt のサイズ)

		// 決定したかどうか
		bool isReturn = false;

		// タイトル
		ImGui::Text(inTitle.c_str());

		// 横の位置の調整
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(-ImGui::GetCursorPosX() + LOCAL_POS_WIDTH, 0));
		ImGui::SameLine();

		ImGui::SetNextItemWidth(LOCAL_ITEM_WIDTH);

		std::string name = LOCAL_ID_TRANSFORM + inTitle;
		if (ImGui::InputInt(name.c_str(), outResult, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			isReturn = true;
		}

		return isReturn;
	}

	//---------------------------------------------------
	// Material
	//---------------------------------------------------
	void Material(const std::string& inTitle, D3DCOLORVALUE& inOutResult)
	{
		ImVec4 d = ImVec4(inOutResult.r, inOutResult.g, inOutResult.b, inOutResult.a);
		ImGui::ColorEdit4(inTitle.c_str(),
			reinterpret_cast<float*>(&d),
			ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		inOutResult = { d.x, d.y, d.z, d.w };
	}

	//---------------------------------------------------
	// Color
	//---------------------------------------------------
	void Color(const std::string& inTitle, D3DXCOLOR& inOutResult)
	{
		ImVec4 d = ImVec4(inOutResult.r, inOutResult.g, inOutResult.b, inOutResult.a);
		ImGui::ColorEdit4(inTitle.c_str(),
			reinterpret_cast<float*>(&d),
			ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		inOutResult = { d.x, d.y, d.z, d.w };
	}

}

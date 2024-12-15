//--------------------------------------------------------------------------------
// 
// 文字列を扱う [string.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 文字列の名前空間
//===================================================
namespace String
{
	//---------------------------------------------------
	// 可変長引数を受け取り連結する処理 (再帰)
	//---------------------------------------------------
#pragma region print
	// パラメータパックが空になったら終了
	inline std::string print()
	{
		return "";
	}

	template<typename T, typename... Args>
	inline std::string print(const T& head, const Args&... args)
	{
		std::ostringstream oss;
		oss << head;
		return oss.str() + print(args...);
	}
#pragma endregion // print
}

//--------------------------------------------------------------------------------
// 
// 外部ファイルの操作 [file.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// utility
#include "utility/custom_json.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr int INDENTATION = 4;	// インデント
	constexpr const char* KEY_SCHEMA = "$schema";	// キー: スキーマ

} // namespace /* anonymous */

//===================================================
// 外部ファイルの操作の名前空間 (json)
//===================================================
namespace File::Json
{
	//---------------------------------------------------
	// 読み込み
	//---------------------------------------------------
	json load(const std::string& inFilename)
	{
		std::ifstream ifs(inFilename);
		if (!ifs)
		{
			assert(false);
			return json{};
		}

		json j;
		ifs >> j;
		return j;
	}

	//---------------------------------------------------
	// 書き出し
	//---------------------------------------------------
	bool save(const std::string& inFilename, const json& inJson)
	{
		std::ofstream ofs(inFilename);
		if (!ofs)
		{
			assert(false);
			return false;
		}

		ofs << inJson.dump(INDENTATION) << std::endl;
		return true;
	}

	//---------------------------------------------------
	// 読み込んで検証する
	//---------------------------------------------------
	json loadAndPerformValidator(const std::string& inFilename)
	{
		// データを読み込む
		json j = load(inFilename);

		// スキーマのファイルパスを取得
		// TODO: パス変わるので、引数でスキーマのパス指定させる？もしくは、相対パスをプロパティに用意させる
		auto schemaPath = j.find(KEY_SCHEMA);
		if (schemaPath == j.end())
		{
			assert(false);
			return json{};
		}
		std::string path = schemaPath.value();

		// スキーマを読み込む
		json_validator validator;
		json schema = load(path.c_str());

		try
		{
			validator.set_root_schema(schema);
		}
		catch (const std::exception& e)
		{
#if 0
			std::cout << e.what() << std::endl;
#else
			assert(false && e.what());
#endif
			return json{};
		}

		try
		{
			validator.validate(j);
		}
		catch (const std::exception& e)
		{
#if 0
			std::cout << e.what() << std::endl;
#else
			assert(false && e.what());
#endif
			return json{};
		}

		return j;
	}
}

//===================================================
// 外部ファイルの操作の名前空間 (ini)
//===================================================
namespace File::Ini
{
	//---------------------------------------------------
	// 読み込み
	//---------------------------------------------------
	ini::IniFile load(const std::string& inFilename)
	{
		ini::IniFile iniFile;
		iniFile.load(inFilename);
		return iniFile;
	}

	//---------------------------------------------------
	// 書き出し
	//---------------------------------------------------
	bool save(const std::string& inFilename, const ini::IniFile& inIni)
	{
		inIni.save(inFilename);
		return true;
	}
}


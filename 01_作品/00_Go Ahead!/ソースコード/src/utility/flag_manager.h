//--------------------------------------------------------------------------------
// 
// フラグマネージャー [flag_manager.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

// USAGE
// ・フラグの定義 1 << 0 もしくは 0x01 など 16 進数で定義すること！
// ・名前空間の関係で EnumClass っていう名前にしたけど、通常の enum でも大丈夫です！ (名前空間で Enum という名前を既に使用しているから)

//===================================================
// フラグのマネージャーのクラス
//===================================================
template<typename EnumClass, typename Underlying = std::underlying_type_t<EnumClass>>
class FlagManager
{
public:
	//---------------------------------------------------
	// public メンバ変数宣言
	//---------------------------------------------------
	FlagManager();
	~FlagManager() = default;

	void setFlag(const EnumClass& inFlag) { m_flags |= static_cast<Underlying>(inFlag); }						// フラグのセット
	void clearFlag(const EnumClass& inFlag) { m_flags &= ~static_cast<Underlying>(inFlag); }					// フラグのクリア
	void changeFlag(const EnumClass& inFlag) { m_flags ^= static_cast<Underlying>(inFlag); }					// フラグの切り替え
	bool isFlagSet(const EnumClass& inFlag) const { return (m_flags & static_cast<Underlying>(inFlag)) != 0; }	// フラグがセットされているか確認
	void clearAllFlags() { m_flags = 0; }																		// 全フラグのクリア

	void setFlags(const Underlying& inFlags) { m_flags = inFlags; }	// フラグの設定
	const Underlying& getFlags() const { return m_flags; }			// フラグの取得

	std::bitset<sizeof(Underlying) * NUM_OF_BITS_IN_ONE_BYTE> getFlagsAsBitset() const { return std::bitset<sizeof(Underlying) * NUM_OF_BITS_IN_ONE_BYTE>(m_flags); };	// 現在のフラグの状態をビットセットとして取得

	std::string toBinString();									// フラグからバイナリ文字列に
	Underlying fromBinString(const std::string& inBinStr);		// バイナリ文字列からフラグに
	void setFlagsFromBinString(const std::string& inBinStr);	// バイナリ文字列からフラグを設定する

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Underlying m_flags;	// フラグの実際の値
};

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
template<typename EnumClass, typename Underlying>
inline FlagManager<EnumClass, Underlying>::FlagManager() : m_flags(0)
{
	// DO_NOTHING
}

//---------------------------------------------------
// フラグからバイナリ文字列に
//---------------------------------------------------
template<typename EnumClass, typename Underlying>
inline std::string FlagManager<EnumClass, Underlying>::toBinString()
{
	// ビットの幅に基づいて bitset を作成
	std::string binStr = getFlagsAsBitset().to_string();

	// 先頭の 0 を削除 (000101 => 101)
	binStr.erase(0, binStr.find_first_not_of('0'));

	// 分かりやすく 0b を付ける (101 => 0b101)
	return "0b" + (binStr.empty() ? "0" : binStr);
}

//---------------------------------------------------
// バイナリ文字列からフラグに
//---------------------------------------------------
template<typename EnumClass, typename Underlying>
inline Underlying FlagManager<EnumClass, Underlying>::fromBinString(const std::string& inBinStr)
{
	std::string binary = inBinStr.substr(2); // 0b を消す (0b101 => 101)
	std::bitset<sizeof(Underlying) * NUM_OF_BITS_IN_ONE_BYTE> bs(binary);
	return static_cast<Underlying>(bs.to_ulong());
}

//---------------------------------------------------
// バイナリ文字列からフラグを設定する
//---------------------------------------------------
template<typename EnumClass, typename Underlying>
inline void FlagManager<EnumClass, Underlying>::setFlagsFromBinString(const std::string& inBinStr)
{
	setFlags(fromBinString(inBinStr));
}

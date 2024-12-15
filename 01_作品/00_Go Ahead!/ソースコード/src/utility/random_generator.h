//--------------------------------------------------------------------------------
// 
// ランダムな値を生成する [random_generator.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// ランダムな値を生成するクラス
//===================================================
class RandomGenerator
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------

	// 範囲を指定してランダムな値を取得
	template<typename T>
	static T get(
		const T& inMin = std::numeric_limits<T>::min(),
		const T& inMax = std::numeric_limits<T>::max());

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	RandomGenerator() = delete;
	~RandomGenerator() = delete;

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	static std::random_device m_randomDevice;	// 乱数生成エンジン
	static std::mt19937 m_generator;			// メルセンヌ・ツイスタを使用した生成器
};

//---------------------------------------------------
// 範囲を指定してランダムな値を取得
//---------------------------------------------------
template<typename T>
inline T RandomGenerator::get(const T& inMin, const T& inMax)
{
	static_assert(std::is_arithmetic<T>::value, "算術型である必要があります！！！");

	if (inMin > inMax)
	{
		throw std::invalid_argument("inMin が inMax よりも大きいです...");
	}

	if constexpr (std::is_integral<T>::value)
	{
		std::uniform_int_distribution<T> dis(inMin, inMax);
		return dis(m_generator);
	}
	else
	{
		std::uniform_real_distribution<T> dis(inMin, inMax);
		return dis(m_generator);
	}
}

//--------------------------------------------------------------------------------
// 
// 計算 [calculate.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 計算の名前空間
//===================================================
namespace Calculate
{
	//---------------------------------------------------
	// プロトタイプ宣言
	//---------------------------------------------------
	template<typename T> inline T sign(T inValue);	// 値の符号 [return 正: 0 以上 / 負: -1]

	// 向きの補正 (3.14)
	// [pOut: 補正された値 (結果) の格納先 / inRot: 参照する値] [return: 補正された値 (結果)]
	float revisionRotForPI(float* pOut, const float& inRot);

	// 回転の更新処理 (最大半周する場合)
	// [outRotNow: 現在の角度 / inRotDest: 目的の角度 / inDeltaTime: デルタタイム]
	void updateRotObject(float* outRotNow, float inRotDest, const float& inDeltaTime);
	
	void calculateRot(Quaternion* outRot, const Vec3& inAxis, const float& inRadian);		// 向きの計算
	void calculateQuaternionByEularYawPitchRoll(Quaternion* outRot, const Vec3& inRadian);	// オイラー角からクォータニオンを計算する
	void mulRot(Quaternion* inOutRot, const Vec3& inAxis, const float& inRadian);			// 向きを掛ける

	// クォータニオンからオイラー角を計算する 
	Vec3 quaternionToEulerXYZ(const Quaternion& inRot);	// 回転順 XYZ
	Vec3 quaternionToEulerYXZ(const Quaternion& inRot);	// 回転順 YXZ	☆これ！
	Vec3 quaternionToEulerXZY(const Quaternion& inRot);	// 回転順 XZY
	Vec3 quaternionToEulerYZX(const Quaternion& inRot);	// 回転順 YZX
	Vec3 quaternionToEulerZXY(const Quaternion& inRot);	// 回転順 ZXY
	Vec3 quaternionToEulerZYX(const Quaternion& inRot);	// 回転順 ZYX

	//---------------------------------------------------
	// 値の符号
	//---------------------------------------------------
	template<typename T>
	inline T sign(T inValue)
	{
	#if 0
		// [return 正: 1 / 負: -1 / 0: 0]
		return (inValue > 0) ? static_cast<T>(1) : (inValue < 0) ? static_cast<T>(-1) : static_cast<T>(0);
	#else
		// [return 正: 0 以上 / 負: -1]
		return (inValue >= 0) ? static_cast<T>(1) : static_cast<T>(-1);
	#endif
	}

} // namespace Calculate

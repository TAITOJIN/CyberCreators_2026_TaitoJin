//--------------------------------------------------------------------------------
// 
// 計算 [calculate.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// utility
#include "utility/calculate.h"
// game_manager
#include "game_manager/game_manager.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr float ATTENUATION_ROT = 12.f;	// オブジェクトの回転の減衰係数

} // namespace /* anonymous */

//===================================================
// 計算の名前空間
//===================================================
namespace Calculate
{
	//---------------------------------------------------
	// 向きの補正 (3.14)
	//---------------------------------------------------
	float revisionRotForPI(float* pOut, const float& inRot)
	{
		*pOut = inRot;

		if (inRot > D3DX_PI)
		{
			(*pOut) -= (D3DX_PI * 2.f);
		}
		else if (inRot < -D3DX_PI)
		{
			(*pOut) += (D3DX_PI * 2.f);
		}

		return (*pOut);
	}

	//---------------------------------------------------
	// 回転の更新処理 (最大半周する場合)
	//---------------------------------------------------
	void updateRotObject(float* outRotNow, float inRotDest, const float& inDeltaTime)
	{
		// 目的の向きの補正
		revisionRotForPI(&inRotDest, inRotDest);

		float rotDiff = inRotDest - (*outRotNow);	// 目的の角度と現在の角度の差

		if (rotDiff > D3DX_PI)
		{ // 3.14 を超えた
			rotDiff = (inRotDest - D3DX_PI * 2.f) - (*outRotNow);	// 目的の向きから 1 周分引き，改めて差を求める
		}
		else if (rotDiff < -D3DX_PI)
		{ // -3.14 を超えた
			rotDiff = (inRotDest + D3DX_PI * 2.f) - (*outRotNow);	// 目的の向きから 1 周分足し，改めて差を求める
		}

		(*outRotNow) += (rotDiff * ATTENUATION_ROT * inDeltaTime);	// 向きの更新

		// 向きの補正
		revisionRotForPI(outRotNow, (*outRotNow));
	}

	//---------------------------------------------------
	// 向きの計算
	//---------------------------------------------------
	void calculateRot(Quaternion* outRot, const Vec3& inAxis, const float& inRadian)
	{
		D3DXQuaternionRotationAxis(outRot, &inAxis, inRadian);
	}

	//---------------------------------------------------
	// オイラー角からクォータニオンを計算する
	//---------------------------------------------------
	void calculateQuaternionByEularYawPitchRoll(Quaternion* outRot, const Vec3& inRadian)
	{
#if 1
		D3DXQuaternionRotationYawPitchRoll(outRot, inRadian.y, inRadian.x, inRadian.z);
#else
		D3DXQuaternionRotationYawPitchRoll(outRot, inRadian.z, inRadian.y, inRadian.x);
#endif
	}

	//---------------------------------------------------
	// 向きを掛ける
	//---------------------------------------------------
	void mulRot(Quaternion* inOutRot, const Vec3& inAxis, const float& inRadian)
	{
		Quaternion tmp = DEF_ROT;
		D3DXQuaternionRotationAxis(&tmp, &inAxis, inRadian);
		D3DXQuaternionMultiply(inOutRot, inOutRot, &tmp);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 XYZ)
	//---------------------------------------------------
	Vec3 quaternionToEulerXYZ(const Quaternion& inRot)
	{
		float sy = 2.0f * inRot.x * inRot.z + 2.0f * inRot.y * inRot.w;
		float unlocked = std::abs(sy) < 0.99999f;
		return Vec3(
			unlocked ? std::atan2(-(2.0f * inRot.y * inRot.z - 2.0f * inRot.x * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1)
			: std::atan2(2.0f * inRot.y * inRot.z + 2.0f * inRot.x * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1),
			std::asin(sy),
			unlocked ? std::atan2(-(2.0f * inRot.x * inRot.y - 2.0f * inRot.z * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1) : 0
		);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 YXZ)
	//---------------------------------------------------
	Vec3 quaternionToEulerYXZ(const Quaternion& inRot)
	{
		float sx = -(2.0f * inRot.y * inRot.z - 2.0f * inRot.x * inRot.w);
		float unlocked = std::fabsf(sx) < 0.99999f;
		return Vec3(
			std::asinf(sx),
			unlocked ? std::atan2(2.0f * inRot.x * inRot.z + 2.0f * inRot.y * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1.0f)
			: std::atan2(-(2.0f * inRot.x * inRot.z - 2.0f * inRot.y * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1.0f),
			unlocked ? std::atan2(2.0f * inRot.x * inRot.y + 2.0f * inRot.z * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1.0f) : 0.0f);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 XZY)
	//---------------------------------------------------
	Vec3 quaternionToEulerXZY(const Quaternion& inRot)
	{
		float sz = -(2.0f * inRot.x * inRot.y - 2.0f * inRot.z * inRot.w);
		float unlocked = std::abs(sz) < 0.99999f;
		return Vec3(
			unlocked ? std::atan2(2.0f * inRot.y * inRot.z + 2.0f * inRot.x * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1)
			: std::atan2(-(2.0f * inRot.y * inRot.z - 2.0f * inRot.x * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1),
			unlocked ? std::atan2(2.0f * inRot.x * inRot.z + 2.0f * inRot.y * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1) : 0,
			std::asin(sz)
		);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 YZX)
	//---------------------------------------------------
	Vec3 quaternionToEulerYZX(const Quaternion& inRot)
	{
		float sz = 2.0f * inRot.x * inRot.y + 2.0f * inRot.z * inRot.w;
		float unlocked = std::abs(sz) < 0.99999f;
		return Vec3(
			unlocked ? atan2(-(2.0f * inRot.y * inRot.z - 2.0f * inRot.x * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1) : 0,
			unlocked ? atan2(-(2.0f * inRot.x * inRot.z - 2.0f * inRot.y * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1)
			: atan2(2.0f * inRot.x * inRot.z + 2.0f * inRot.y * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1),
			std::asin(sz)
		);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 ZXY)
	//---------------------------------------------------
	Vec3 quaternionToEulerZXY(const Quaternion& inRot)
	{
		float sx = 2.0f * inRot.y * inRot.z + 2.0f * inRot.x * inRot.w;
		float unlocked = std::abs(sx) < 0.99999f;
		return Vec3(
			std::asin(sx),
			unlocked ? std::atan2(-(2.0f * inRot.x * inRot.z - 2.0f * inRot.y * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1) : 0,
			unlocked ? std::atan2(-(2.0f * inRot.x * inRot.y - 2.0f * inRot.z * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1)
			: std::atan2(2.0f * inRot.x * inRot.y + 2.0f * inRot.z * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1)
		);
	}

	//---------------------------------------------------
	// クォータニオンからオイラー角を計算する (回転順 ZYX)
	//---------------------------------------------------
	Vec3 quaternionToEulerZYX(const Quaternion& inRot)
	{
		float sy = -(2.0f * inRot.x * inRot.z - 2.0f * inRot.y * inRot.w);
		float unlocked = std::abs(sy) < 0.99999f;
		return Vec3(
			unlocked ? std::atan2(2.0f * inRot.y * inRot.z + 2.0f * inRot.x * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.z * inRot.z - 1) : 0,
			std::asin(sy),
			unlocked ? std::atan2(2.0f * inRot.x * inRot.y + 2.0f * inRot.z * inRot.w, 2.0f * inRot.w * inRot.w + 2.0f * inRot.x * inRot.x - 1)
			: std::atan2(-(2.0f * inRot.x * inRot.y - 2.0f * inRot.z * inRot.w), 2.0f * inRot.w * inRot.w + 2.0f * inRot.y * inRot.y - 1)
		);
	}

} // namespace Calculate

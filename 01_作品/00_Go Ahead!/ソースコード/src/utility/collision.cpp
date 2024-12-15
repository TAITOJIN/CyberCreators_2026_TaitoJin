//--------------------------------------------------------------------------------
// 
// 当たり判定 [collision.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// utility
#include "utility/collision.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	const Vec3 MAX_COORD_BB = Vec3(-10000.0f, -10000.0f, -10000.0f);	// 最大座標
	const Vec3 MIN_COORD_BB = Vec3(10000.0f, 10000.0f, 10000.0f);		// 最小座標
	constexpr int NUM_SEPARATIONAXIS = 15;								// 分離軸の数
	constexpr float PI = 3.141592654f;									// pi
	constexpr float EPSILON = 1.175494015e-37f;							// エプシロン

} // namespace /* anonymous */

//===================================================
// 当たり判定の名前空間
//===================================================
namespace Collision
{
	//---------------------------------------------------
	// 円同士の当たり判定
	//---------------------------------------------------
	bool isCollisionCircleVsCircle(
		const Circle& inCircle1,
		const Circle& inCircle2)
	{
		float x = inCircle2.pos.x - inCircle1.pos.x;			// 横の距離
		float y = inCircle2.pos.y - inCircle1.pos.y;			// 縦の距離
		float r = (x * x) + (y * y);							// 距離
		float sumRadius = inCircle1.radius + inCircle2.radius;	// 半径の和

		return (r < (sumRadius* sumRadius));
	}

	//---------------------------------------------------
	// 球同士の当たり判定
	//---------------------------------------------------
	bool isCollisionSphereVsSphere(const Sphere& inSphere1, const Sphere& inSphere2)
	{
		Vec3 distance = inSphere2.pos - inSphere1.pos;
		float r = (distance.x * distance.x) + (distance.y * distance.y) + (distance.z * distance.z);
		float sumRadius = inSphere1.radius + inSphere2.radius;

		return (r < (sumRadius* sumRadius));
	}

	//---------------------------------------------------
	// 回転する矩形同士の当たり判定
	//---------------------------------------------------
	bool isCollisionRectVsRect(
		const Rectangle& inRect1,
		const Rectangle& inRect2)
	{
		LineSegment2D edge = GetRectEdge(inRect1, 0);
		if (isSeparatingAxisForRect(edge, inRect2))
		{
			return false;
		}

		edge = GetRectEdge(inRect1, 1);
		if (isSeparatingAxisForRect(edge, inRect2))
		{
			return false;
		}

		edge = GetRectEdge(inRect2, 0);
		if (isSeparatingAxisForRect(edge, inRect1))
		{
			return false;
		}

		edge = GetRectEdge(inRect2, 1);
		return !isSeparatingAxisForRect(edge, inRect1);
	}

	//---------------------------------------------------
	// 2D の矩形同士の当たり判定
	//---------------------------------------------------
	bool isCollisionRectVsRect2D(const Vec2& inPos1, const Vec2& inSize1, const Vec2& inPos2, const Vec2& inSize2)
	{
		return (inPos1.x < inPos2.x + inSize2.x &&
			inPos1.x + inSize1.x > inPos2.x &&
			inPos1.y < inPos2.y + inSize2.y &&
			inPos1.y + inSize1.y > inPos2.y);
	}

	bool isCollisionRectVsRect2D(const ImVec2& inPos1, const ImVec2& inSize1, const ImVec2& inPos2, const ImVec2& inSize2)
	{
		return (inPos1.x < inPos2.x + inSize2.x &&
			inPos1.x + inSize1.x > inPos2.x &&
			inPos1.y < inPos2.y + inSize2.y &&
			inPos1.y + inSize1.y > inPos2.y);
	}

	//---------------------------------------------------
	// ブロック同士の当たり判定
	//---------------------------------------------------
	bool isCollisionBlockVsBlock(const Rectangle& inRect1, const Rectangle& inRect2, const float& centerY1, const float& centerY2, const float& inHalfHeight1, const float& inHalfHeight2)
	{
		// y
		if (fabsf(centerY1 - centerY2) > (inHalfHeight1 + inHalfHeight2))
		{
			return false;
		}

		// x, z
		return isCollisionRectVsRect(inRect1, inRect2);
	}

	//---------------------------------------------------
	// 箱形状の半径の取得
	//---------------------------------------------------
	Vec3 getHalfExtents(const AABB& inAABB, const Vec3& inScale)
	{
		// 箱形状の半径
		Vec3 halfExtents = (inAABB.maxPos - inAABB.minPos) * HALF;

		// スケールの反映
		halfExtents.x *= inScale.x;
		halfExtents.y *= inScale.y;
		halfExtents.z *= inScale.z;

		return halfExtents;
	}

	//---------------------------------------------------
	// カプセル形状の取得
	//---------------------------------------------------
	Vec2 getCapsule(const AABB& inAABB, const Vec3& inScale)
	{
		// 箱形状の半径
		Vec3 halfExtents = getHalfExtents(inAABB, inScale);

		return Vec2((halfExtents.x + halfExtents.z) * HALF, halfExtents.y);
	}

	//---------------------------------------------------
	// 有向境界ボックスの設定
	//---------------------------------------------------
	void setOBB(const AABB& inAABB, const Matrix& inMtxWorld, OBB* outOBB)
	{
		// マトリックスから座標を取得する
		Vec3 worldPos = { inMtxWorld._41, inMtxWorld._42, inMtxWorld._43 };

		// 中心座標を計算する
		outOBB->center = (inAABB.minPos + inAABB.maxPos) * HALF + worldPos;

		// 方向ベクトルの計算
		outOBB->direction[static_cast<UINT>(AXIS::X)] = { inMtxWorld._11, inMtxWorld._12, inMtxWorld._13 };
		outOBB->direction[static_cast<UINT>(AXIS::Y)] = { inMtxWorld._21, inMtxWorld._22, inMtxWorld._23 };
		outOBB->direction[static_cast<UINT>(AXIS::Z)] = { inMtxWorld._31, inMtxWorld._32, inMtxWorld._33 };

		// 長さの計算
		outOBB->halfExtent.x = std::fabsf(inAABB.maxPos.x - inAABB.minPos.x) * HALF;
		outOBB->halfExtent.y = std::fabsf(inAABB.maxPos.y - inAABB.minPos.y) * HALF;
		outOBB->halfExtent.z = std::fabsf(inAABB.maxPos.z - inAABB.minPos.z) * HALF;
	}

	//---------------------------------------------------
	// 有向境界ボックス同士の当たり判定
	//---------------------------------------------------
	bool isCollisionOBBvsOBB(const OBB& inBox1, const OBB& inBox2)
	{
		float R[3][3];		// 2 つの OBB の各軸における内積を格納
		float AbsR[3][3];	// R の絶対値 ( EPSILON を加えて調整したもの ) を格納

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				R[i][j] = D3DXVec3Dot(&inBox1.direction[i], &inBox2.direction[j]);
				AbsR[i][j] = fabsf(R[i][j]) + EPSILON;
			}
		}

		// OBB 間の相対位置の計算
		D3DXVECTOR3 t = {};
		{
			D3DXVECTOR3 _t = inBox2.center - inBox1.center;
			t =
			{
				D3DXVec3Dot(&_t, &inBox1.direction[0]),
				D3DXVec3Dot(&_t, &inBox1.direction[1]),
				D3DXVec3Dot(&_t, &inBox1.direction[2])
			};
		}

		// 軸 L = A0, L = A1, L = A2 判定
		float ra, rb;
		for (int i = 0; i < 3; ++i)
		{
			ra = inBox1.halfExtent[i];
			rb = inBox2.halfExtent[0] * AbsR[i][0] + inBox2.halfExtent[1] * AbsR[i][1] + inBox2.halfExtent[2] * AbsR[i][2];
			if (fabsf(t[i]) > ra + rb)
			{
				return false;
			}
		}

		// 軸 L = B0, L = B1, L = B2 判定
		for (int i = 0; i < 3; ++i)
		{
			ra = inBox1.halfExtent[0] * AbsR[0][i] + inBox1.halfExtent[1] * AbsR[1][i] + inBox1.halfExtent[2] * AbsR[2][i];
			rb = inBox2.halfExtent[i];
			if (fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)
			{
				return false;
			}
		}

		// 軸 L = A0 X B0 判定
		ra = inBox1.halfExtent[1] * AbsR[2][0] + inBox1.halfExtent[2] * AbsR[1][0];
		rb = inBox2.halfExtent[1] * AbsR[0][2] + inBox2.halfExtent[2] * AbsR[0][1];
		if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A0 X B1 判定
		ra = inBox1.halfExtent[1] * AbsR[2][1] + inBox1.halfExtent[2] * AbsR[1][1];
		rb = inBox2.halfExtent[0] * AbsR[0][2] + inBox2.halfExtent[2] * AbsR[0][0];
		if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A0 X B2 判定
		ra = inBox1.halfExtent[1] * AbsR[2][2] + inBox1.halfExtent[2] * AbsR[1][2];
		rb = inBox2.halfExtent[0] * AbsR[0][1] + inBox2.halfExtent[1] * AbsR[0][0];
		if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A1 X B0 判定
		ra = inBox1.halfExtent[0] * AbsR[2][0] + inBox1.halfExtent[2] * AbsR[0][0];
		rb = inBox2.halfExtent[1] * AbsR[1][2] + inBox2.halfExtent[2] * AbsR[1][1];
		if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A1 X B1 判定
		ra = inBox1.halfExtent[0] * AbsR[2][1] + inBox1.halfExtent[2] * AbsR[0][1];
		rb = inBox2.halfExtent[0] * AbsR[1][2] + inBox2.halfExtent[2] * AbsR[1][0];
		if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A1 X B2 判定
		ra = inBox1.halfExtent[0] * AbsR[2][2] + inBox1.halfExtent[2] * AbsR[0][2];
		rb = inBox2.halfExtent[0] * AbsR[1][1] + inBox2.halfExtent[1] * AbsR[1][0];
		if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A2 X B0 判定
		ra = inBox1.halfExtent[0] * AbsR[1][0] + inBox1.halfExtent[1] * AbsR[0][0];
		rb = inBox2.halfExtent[1] * AbsR[2][2] + inBox2.halfExtent[2] * AbsR[2][1];
		if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A2 X B1 判定
		ra = inBox1.halfExtent[0] * AbsR[1][1] + inBox1.halfExtent[1] * AbsR[0][1];
		rb = inBox2.halfExtent[0] * AbsR[2][2] + inBox2.halfExtent[2] * AbsR[2][0];
		if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
		{
			return false;
		}

		// 軸 L = A2 X B2 判定
		ra = inBox1.halfExtent[0] * AbsR[1][2] + inBox1.halfExtent[1] * AbsR[0][2];
		rb = inBox2.halfExtent[0] * AbsR[2][1] + inBox2.halfExtent[1] * AbsR[2][0];
		if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
		{
			return false;
		}

		// OBB 同士が重なっている
		return true;
	}

	//---------------------------------------------------
	// 矩形の 1 辺を取得
	//---------------------------------------------------
	LineSegment2D GetRectEdge(const Rectangle& inRect, const int inPoint)
	{
		LineSegment2D edge;
		Vec2 v1 = inRect.halfExtend;
		Vec2 v2 = inRect.halfExtend;

		// 右回りで取得する
		// 0, 2
		// 1, 3 は対辺を表す
		switch (inPoint % 4)
		{
		case 0:
			v1.x *= -1.0f;
			break;
		case 1:
			v2.y *= -1.0f;
			break;
		case 2:
			v1.y *= -1.0f;
			v2 = getNegateVec(v2);
			break;
		case 3:
			v1 = getNegateVec(v1);
			v2.x *= -1.0f;
			break;
		}

		v1 = getRotateVec(v1, inRect.rot);
		v1 += inRect.center;

		v2 = getRotateVec(v2, inRect.rot);
		v2 += inRect.center;

		edge.point1 = v1;
		edge.point2 = v2;

		return edge;
	}

	//---------------------------------------------------
	// 辺の 1 辺と、四角形の 1 辺とが重なっているか判定
	//---------------------------------------------------
	bool isSeparatingAxisForRect(LineSegment2D inAxis, Rectangle inRect)
	{
		Range axisRange, rect0Range, rect2Range, rectProjection;

		// Segment
		LineSegment2D rectEdge0 = GetRectEdge(inRect, 0);
		LineSegment2D rectEdge2 = GetRectEdge(inRect, 2);

		Vec2 n = inAxis.point1 - inAxis.point2;

		axisRange = getProjectSegment(inAxis, n);
		rect0Range = getProjectSegment(rectEdge0, n);
		rect2Range = getProjectSegment(rectEdge2, n);
		rectProjection = getMaxMinRange(rect0Range, rect2Range);

		return !isoverLappingRanges(axisRange, rectProjection);
	}

	/////////////////////////////////////////////////////
	// Vec2
	/////////////////////////////////////////////////////

	Vec2 rotVec90(Vec2 inVec)
	{
		Vec2 r;
		r.x = -inVec.y;
		r.y = inVec.x;
		return r;
	}

	float getDegreeToRadian(float inDegree)
	{
		return inDegree * (PI / 180.0f);
	}

	float getRadianToDegree(float inRadian)
	{
		return inRadian * (180.0f / PI);
	}

	Vec2 getRotateVec(Vec2 inVec, float inRot)
	{
		float radian = inRot;
#ifdef USE_DEGREE
		radian = getDegreeToRadian(inRot);
#endif
		float sin = sinf(radian);
		float cos = cosf(radian);

		Vec2 r;
		r.x = inVec.x * cos - inVec.y * sin;
		r.y = inVec.x * sin + inVec.y * cos;

		return r;
	}

	bool isEqualVectors(Vec2 inVec1, Vec2 inVec2)
	{
		return equalFloats(inVec1.x - inVec1.x, 0) && equalFloats(inVec2.y - inVec2.y, 0);
	}

	Vec2 getUnitVec(Vec2 inVec)
	{
		float length = getVecLength2D(inVec);
		if (length > 0) {
			return getDivideVec(inVec, length);
		}
		return inVec;
	}

	float getVecLength2D(Vec2 inVec)
	{
		return sqrtf((inVec.x * inVec.x) + (inVec.y * inVec.y));
	}

	Vec2 getDivideVec(Vec2 inVec, float inLength)
	{
		inVec.x = inVec.x / inLength;
		inVec.y = inVec.y / inLength;
		return inVec;
	}

	bool equalFloats(float a, float b)
	{
		float threshold = 1.0f / 8192.0f;
		return fabsf(a - b) < threshold;
	}

	bool isParallelVec(Vec2 inVec1, Vec2 inVec2)
	{
		Vec2 na = rotVec90(inVec1);
		return equalFloats(0, dotProduct2D(na, inVec2));
	}

	bool isVerticalVec(Vec2 inVec1, Vec2 inVec2)
	{
		return equalFloats(0, dotProduct2D(inVec1, inVec2));
	}

	float dotProduct2D(Vec2 inVec1, Vec2 inVec2)
	{
		return inVec1.x * inVec2.x + inVec1.y * inVec2.y;
	}

	float getCross(Vec2 inVec1, Vec2 inVec2)
	{
		return inVec1.x * inVec2.y - inVec1.y * inVec2.x;
	}

	/////////////////////////////////////////////////////
	// line inSegment
	/////////////////////////////////////////////////////

	bool onOneSide(Line inAxis, LineSegment2D inSegment)
	{
		Vec2 d1 = inSegment.point1 - inAxis.base;
		Vec2 d2 = inSegment.point2 - inAxis.base;
		Vec2 n = rotVec90(inAxis.dir);

		// 同じ方向だということを返す
		return dotProduct2D(n, d1) * dotProduct2D(n, d2) > 0;
	}

	bool overLapping(float inMin1, float inMax1, float inMin2, float inMax2)
	{
		return inMin2 <= inMax1 && inMin1 <= inMax2;
	}

	bool isSegmentsCollide(LineSegment2D inSegment1, LineSegment2D inSegment2)
	{
		// 線分の方向をチェックするためのベクトル
		Line axis1, axis2;

		// 線分 1 のベクトルに対して、線分 2 が片側にあるかチェック
		axis1.base = inSegment1.point1;
		axis1.dir = inSegment1.point2 - inSegment1.point1;

		if (onOneSide(axis1, inSegment2)) {
			return false;
		}

		// 同じく
		axis2.base = inSegment2.point1;
		axis2.dir = inSegment2.point2 - inSegment2.point1;

		if (onOneSide(axis2, inSegment1)) {
			return false;
		}

		// 同じベクトルをもつケース
		if (isParallelVec(axis1.dir, axis2.dir)) {
			Range rangeA = getProjectSegment(inSegment1, axis1.dir);
			Range rangeB = getProjectSegment(inSegment2, axis1.dir);

			// 重なっているか
			return isoverLappingRanges(rangeA, rangeB);
		}
		else {
			return true;
		}
	}

	/////////////////////////////////////////////////////
	// Range
	/////////////////////////////////////////////////////

	Range getMaxMinRange(Range inRange1, Range inRange2)
	{
		Range range;
		range.min = inRange1.min < inRange2.min ? inRange1.min : inRange2.min;
		range.max = inRange1.max < inRange2.max ? inRange2.max : inRange1.max;
		return range;
	}

	Range getSortRange(Range inRange)
	{
		Range sorted = inRange;
		if (inRange.min > inRange.max)
		{
			sorted.min = inRange.max;
			sorted.max = inRange.min;
		}
		return sorted;
	}

	Vec2 getNegateVec(Vec2 inVec)
	{
		inVec.x = -inVec.x;
		inVec.y = -inVec.y;
		return inVec;
	}

	Range getProjectSegment(LineSegment2D inSegment, Vec2 inOnto)
	{
		Vec2 ontoUnitVec = getUnitVec(inOnto);

		Range range;
		range.min = dotProduct2D(ontoUnitVec, inSegment.point1);
		range.max = dotProduct2D(ontoUnitVec, inSegment.point2);

		range = getSortRange(range);
		return range;
	}

	bool isoverLappingRanges(Range inRange1, Range inRange2)
	{
		return overLapping(inRange1.min, inRange1.max, inRange2.min, inRange2.max);
	}

	Vec2 projectVec(Vec2 inProject, Vec2 inOnto)
	{
		float d = dotProduct2D(inOnto, inOnto);
		if (0 < d)
		{
			float dp = dotProduct2D(inProject, inOnto);
			return inOnto * (dp / d);
		}
		return inOnto;
	}

	float getNorm(Vec2 inVec)
	{
		return inVec.x * inVec.x + inVec.y * inVec.y;
	}

	float getVec2DLength(Vec2 inVec)
	{
		return sqrtf(getNorm(inVec));
	}

	/////////////////////////////////////////////////////
	// Point
	/////////////////////////////////////////////////////

	Point2D getProjectPoint(LineSegment2D inSegment, Point2D inPoint)
	{
		Vec2 vec1 = Vec2(inSegment.point2.x - inSegment.point1.x, inSegment.point2.y - inSegment.point1.y);
		Vec2 vec2 = Vec2(inPoint.x - inSegment.point1.x, inPoint.y - inSegment.point1.y);
		float rate = dotProduct2D(vec2, vec1) / getNorm(vec1);
		Vec2 vec3 = vec1 * rate;
		Point2D project = Point2D(inSegment.point1.x + vec3.x, inSegment.point1.y + vec3.y);
		return project;
	}

	Vec2 getReflectionPoint(LineSegment2D inSegment, Point2D inPoint)
	{
		Point2D projection = getProjectPoint(inSegment, inPoint);
		// p から射影点までのベクトル
		Vec2 vec = projection - inPoint;
		// 2 倍したものが射影点になる
		Vec2 refrectionPoint = vec * 2;;
		// 始点を足す
		return refrectionPoint + inPoint;
	}

	Point2D getCrossPoint(LineSegment2D inSegment1, LineSegment2D inSegment2)
	{
		// 基準となる線を決める
		Vec2 baseVector = Vec2(inSegment2.point2.x - inSegment2.point1.x, inSegment2.point2.y - inSegment2.point1.y);
		Vec2 d1Vec = Vec2(inSegment1.point1.x - inSegment2.point1.x, inSegment1.point1.y - inSegment2.point1.y);
		Vec2 d2Vec = Vec2(inSegment1.point2.x - inSegment2.point1.x, inSegment1.point2.y - inSegment2.point1.y);
		float d1 = fabsf(getCross(baseVector, d1Vec));
		float d2 = fabsf(getCross(baseVector, d2Vec));
		float t = d1 / (d1 + d2);
		float x = inSegment1.point1.x + (inSegment1.point2.x - inSegment1.point1.x) * t;
		float y = inSegment1.point1.y + (inSegment1.point2.y - inSegment1.point1.y) * t;
		return Point2D(x, y);
	}

	float clampOnRange(float inX, float inMin, float inMax)
	{
		if (inX < inMin)
		{
			return inMin;
		}
		else if (inX > inMax)
		{
			return inMax;
		}
		else {
			return inX;
		}
	}

	//---------------------------------------------------
	// 直線と点との距離
	//---------------------------------------------------
	float getDistanceLinePoint(LineSegment2D inLine, Point2D inPoint)
	{
		Vec2 vec1 = Vec2(inLine.point2.x - inLine.point1.x, inLine.point2.y - inLine.point1.y);
		Vec2 vec2 = Vec2(inPoint.x - inLine.point1.x, inPoint.y - inLine.point1.y);
		return fabsf(getCross(vec1, vec2) / getVec2DLength(vec1));
	}

	//---------------------------------------------------
	// 線分と点との距離を求める
	//---------------------------------------------------
	float getDistanceSegmentPoint(LineSegment2D inSegment, Point2D inPoint)
	{
		// ベクトル p2 - p1 とベクトル p - p1 がなす角 θ が 90 どより大きい場合 (-90 より小さい場合)
		// d は点 p と点 p1 の距離になる
		if (dotProduct2D(inSegment.point2 - inSegment.point1, inPoint - inSegment.point1) < 0)
		{
			return getVec2DLength(inPoint - inSegment.point1);
		}
		// ベクトル p1 - p2 とベクトル p - p2 がなす角 θ が 90 どより大きい場合 (-90 より小さい場合)
		// d は点 p と点 p2 の距離になる
		if (dotProduct2D(inSegment.point1 - inSegment.point2, inPoint - inSegment.point2) < 0)
		{
			return getVec2DLength(inPoint - inSegment.point2);
		}
		// それ以外は d は点 p と直線 p1 p2 の距離になる
		return getDistanceLinePoint(inSegment, inPoint);
	}

	//---------------------------------------------------
	// 線分と線分との距離を求める
	//---------------------------------------------------
	float getSegmentDistance(LineSegment2D inSegment1, LineSegment2D inSegment2)
	{
		// 交差していた場合距離は 0 になる
		if (isSegmentsCollide(inSegment1, inSegment2))
		{
			return 0;
		}
		return std::min(std::min(getDistanceSegmentPoint(inSegment1, inSegment2.point1), getDistanceSegmentPoint(inSegment1, inSegment2.point2)),
			std::min(getDistanceSegmentPoint(inSegment2, inSegment1.point1), getDistanceSegmentPoint(inSegment2, inSegment1.point2)));
	}

	//---------------------------------------------------
	// 回転する矩形と点の当たり判定
	//---------------------------------------------------
	bool isCollisionRectVsPoint(Rectangle rect, Vec2 point)
	{
#ifdef USE_DEGREE
		rect.rot = getDegreeToRadian(rect.rot);
#endif
#ifdef USE_ISCOLLISIONRECTVSPOINT_CROSS
		// 原点中心の矩形
		Vec2 base_positions[4] =
		{
			Vec2(-rect.halfExtend.x, -rect.halfExtend.y),
			Vec2(rect.halfExtend.x, -rect.halfExtend.y),
			Vec2(rect.halfExtend.x, rect.halfExtend.y),
			Vec2(-rect.halfExtend.x, rect.halfExtend.y),
		};

		// 回転矩形座標用
		Vec2 vertices[4] = {};

		// 回転計算
		for (int i = 0; i < 4; ++i)
		{
			vertices[i].x = base_positions[i].x * cosf(rect.rot) + base_positions[i].y * -sinf(rect.rot) + rect.center.x;
			vertices[i].y = base_positions[i].x * sinf(rect.rot) + base_positions[i].y * cosf(rect.rot) + rect.center.y;
		}

		// 矩形の 4 頂点から辺を作る
		LineSegment2D edges[4] =
		{
			{ vertices[0], vertices[1] },	// 上辺
			{ vertices[1], vertices[2] },	// 右辺
			{ vertices[2], vertices[3] },	// 下辺
			{ vertices[3], vertices[0] }	// 左辺
		};

		for (LineSegment2D& start_to_end : edges)
		{
			// 辺の始点と点で辺を作成
			LineSegment2D start_to_point = { start_to_end.point1, point };
			// 二つの辺のベクトルを取得
			Vec2 v1 = start_to_end.point2 - start_to_end.point1;
			Vec2 v2 = start_to_point.point2 - start_to_point.point1;

			// 外積計算
			float cross = v1.x * v2.y - v2.x * v1.y;

			// 結果が負の場合点が上にあるので判定終了
			if (cross < 0)
			{
				return false;
			}
		}

		// 全ての外積結果が下になったので点は矩形の中にある
		return true;
#else
		// 矩形の中心を原点とした相対座標を作成する
		Vec2 relative_position = Vec2(point.x - rect.center.x, point.y - rect.center.y);

		// 相対座標に対して矩形の回転を打ち消す逆行列を掛ける
		Vec2 transform_pos = Vec2(
			cosf(rect.rot) * relative_position.x + sinf(rect.rot) * relative_position.y,
			-sinf(rect.rot) * relative_position.x + cosf(rect.rot) * relative_position.y
		);

		// 矩形と点の当たり判定を行う
		if (-rect.halfExtend.x <= transform_pos.x && rect.halfExtend.x >= transform_pos.x &&
			-rect.halfExtend.y <= transform_pos.y && rect.halfExtend.y >= transform_pos.y)
		{
			return true;
		}

		return false;
#endif
	}

	//---------------------------------------------------
	// 軸平行境界ボックスの設定
	//---------------------------------------------------
	void setAABB(const Mesh& inMesh, AABB* outAABB)
	{
		// 頂点フォーマットのサイズを取得
		DWORD sizeFVF = D3DXGetFVFVertexSize(inMesh->GetFVF());

		// 頂点バッファへのポインタを取得し，ロック
		BYTE* pVtxBuf;
		inMesh->LockVertexBuffer(D3DLOCK_READONLY, reinterpret_cast<void**>(&pVtxBuf));

		// 頂点数の取得
		int nNumVtx = inMesh->GetNumVertices();

		// 最小，最大の頂点の初期化
		outAABB->minPos = MIN_COORD_BB;
		outAABB->maxPos = MAX_COORD_BB;

		for (int i = 0; i < nNumVtx; ++i)
		{
			// 頂点座標の代入
			Vec3 vtx = *reinterpret_cast<Vec3*>(pVtxBuf);

			// 最小の頂点
			// x 座標
			if (vtx.x < outAABB->minPos.x)
			{
				outAABB->minPos.x = vtx.x;
			}

			// y 座標
			if (vtx.y < outAABB->minPos.y)
			{
				outAABB->minPos.y = vtx.y;
			}

			// z 座標
			if (vtx.z < outAABB->minPos.z)
			{
				outAABB->minPos.z = vtx.z;
			}

			// 最大の頂点
			// x 座標
			if (vtx.x > outAABB->maxPos.x)
			{
				outAABB->maxPos.x = vtx.x;
			}

			// y 座標
			if (vtx.y > outAABB->maxPos.y)
			{
				outAABB->maxPos.y = vtx.y;
			}

			// z 座標
			if (vtx.z > outAABB->maxPos.z)
			{
				outAABB->maxPos.z = vtx.z;
			}

			// 頂点フォーマットのサイズ分ポインタを進める
			pVtxBuf += sizeFVF;
		}

		// 頂点バッファのアンロック
		inMesh->UnlockVertexBuffer();
	}

	//---------------------------------------------------
	// 点と AABB の当たり判定
	//---------------------------------------------------
	bool isCollisionPointVsAABB(const LineSegment3D& inSegment, const AABB& inAABB, const Matrix& inMtxParent)
	{
		Matrix mtxWorld, mtxScale, mtxRot, mtxTrans;	// 計算用マトリックス

		// 原点中心の矩形 (右回り)
		Vec2 BasePos[4] = {
			Vec2(inAABB.minPos.x, inAABB.maxPos.z),
			Vec2(inAABB.maxPos.x, inAABB.maxPos.z),
			Vec2(inAABB.maxPos.x, inAABB.minPos.z),
			Vec2(inAABB.minPos.x, inAABB.minPos.z),
		};

		for (int i = 0; i < 4; ++i)
		{
			mtxWorld = DEF_MTX;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, BasePos[i].x, 0.0f, BasePos[i].y);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

			// 親のマトリックスの反映
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &inMtxParent);

			BasePos[i] = Vec2(mtxWorld._41, mtxWorld._43);
		}

		// 矩形の４頂点から辺を作る
		LineSegment2D edges[4] = {
			{ BasePos[0], BasePos[1] },	// 上辺
			{ BasePos[1], BasePos[2] },	// 右辺
			{ BasePos[2], BasePos[3] },	// 下辺
			{ BasePos[3], BasePos[0] }	// 左辺
		};

		Vec2 point = Vec2(inSegment.point2.x, inSegment.point2.z);
		for (LineSegment2D& start_to_end : edges)
		{
			// 辺の始点と点で辺を作成
			LineSegment2D start_to_point = { start_to_end.point1, point };
			// 二つの辺のベクトルを取得
			Vec2 v1 = start_to_end.point2 - start_to_end.point1;
			Vec2 v2 = start_to_point.point2 - start_to_point.point1;

			// 外積計算
			float cross = v1.x * v2.y - v2.x * v1.y;

			// 結果が負の場合点が上にあるので判定終了
			if (cross > 0) {
				return false;
			}
		}

		// 全ての外積結果が下になったので点は矩形の中にある
		return true;
	}

	// 点と矩形の当たり判定
	bool isPointInsideRectangle(const Vec3& point, const Rectangle& rect)
	{
		// 回転する矩形の中心座標からの相対位置
		float dx = point.x - rect.center.x;
		float dy = point.y - rect.center.y;

		// 回転行列を使って点を回転
		float rotatedX = dx * cosf(rect.rot) - dy * sinf(rect.rot);
		float rotatedY = dx * sinf(rect.rot) + dy * cosf(rect.rot);

		// 回転後の点が矩形内にあるかチェック
		if (rotatedX >= -rect.halfExtend.x * 0.5f && rotatedX <= rect.halfExtend.x * 0.5f &&
			rotatedY >= -rect.halfExtend.y * 0.5f && rotatedY <= rect.halfExtend.y * 0.5f) {
			return true;
		}
		return false;
	}

	// 矩形の外に点を押し出す
	void pushPointOutOfRectangle(Vec3& point, const Rectangle& rect)
	{
		// 矩形の中心から点へのベクトル
		float dx = point.x - rect.center.x;
		float dy = point.z - rect.center.y;

		// ベクトルの長さ
		float distance = sqrt(dx * dx + dy * dy);

		// 矩形の外側に押し出す
		float pushDistanceX = (rect.halfExtend.x * 0.5f + 1) * (dx / distance);
		float pushDistanceY = (rect.halfExtend.y * 0.5f + 1) * (dy / distance);

		// 押し出し後の座標を設定
		point.x = rect.center.x + pushDistanceX;
		point.z = rect.center.y + pushDistanceY;
	}

	// AABB 同士の当たり判定
	bool isCollisionAABBVsAABB(
		const LineSegment3D& inSegment,
		const AABB& inAABB1,
		const AABB& inAABB2,
		const Matrix& inMtxParent1,
		const Matrix& inMtxParent2,
		Vec3* outResult)
	{
		bool bIsCollision = false;	// 衝突したかどうか

		// 親マトリックスの設定
		Vec2 size[2] = {};
		Matrix mtxParent[2] = { inMtxParent1, inMtxParent2 };
		AABB aabb[2] = { inAABB1, inAABB2 };
		for (int i = 0; i < 2; ++i)
		{
			// 回転を取り除く
			{
				// 行列を回転，移動，スケーリングの成分に分解
				Vec3 scale;
				Quaternion rot;
				Vec3 trans;
				D3DXMatrixDecompose(&scale, &rot, &trans, &mtxParent[i]);

				// 計算用マトリックス
				Matrix mtxScale, mtxTrans;
				Matrix mtxWorld = DEF_MTX;

				// スケールを反映
				D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

				// 位置を反映
				D3DXMatrixTranslation(&mtxTrans, trans.x, trans.y, trans.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

				// マトリックスを更新
				mtxParent[i] = mtxWorld;
			}

			// 各頂点 (右回り)
			Vec2 vtx[4] =
			{
				Vec2(aabb[i].minPos.x, aabb[i].maxPos.z),
				Vec2(aabb[i].maxPos.x, aabb[i].maxPos.z),
				Vec2(aabb[i].maxPos.x, aabb[i].minPos.z),
				Vec2(aabb[i].minPos.x, aabb[i].minPos.z),
			};

			// 各頂点の設定
			{
				Matrix mtxWorld, mtxTrans;	// 計算用マトリックス

				for (int j = 0; j < 4; j++)
				{
					mtxWorld = DEF_MTX;

					// 位置を反映
					D3DXMatrixTranslation(&mtxTrans, vtx[j].x, 0.0f, vtx[j].y);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

					// 親のマトリックスの反映
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxParent[i]);

					vtx[j] = Vec2(mtxWorld._41, mtxWorld._43);
				}
			}

			size[i] = vtx[2] - vtx[0];
			size[i] = Vec2(fabsf(size[i].x), fabsf(size[i].y)) * HALF;
		}

		Vec2 curtPoint1 = Vec2(inSegment.point2.x, inSegment.point2.z);
		Vec2 prevPoint1 = Vec2(inSegment.point1.x, inSegment.point1.z);

		Vec2 center2 = Vec2(inMtxParent2._41, inMtxParent2._43);
		Vec2 result1 = curtPoint1;

		if (curtPoint1.y < center2.y + size[0].y + size[1].y && curtPoint1.y > center2.y - size[0].y - size[1].y)
		{
			if (prevPoint1.x <= center2.x - size[0].x - size[1].x && curtPoint1.x >= center2.x - size[0].x - size[1].x)
			{ // 左からめり込み
				result1.x = center2.x - size[0].x - size[1].x;
				bIsCollision = true;
			}
			else if (prevPoint1.x >= center2.x + size[0].x + size[1].x && curtPoint1.x <= center2.x + size[0].x + size[1].x)
			{ // 右からめり込み
				result1.x = center2.x + size[0].x + size[1].x;
				bIsCollision = true;
			}
		}

		if (curtPoint1.x < center2.x + size[0].x + size[1].x && curtPoint1.x > center2.x - size[0].x - size[1].x)
		{
			if (prevPoint1.y <= center2.y - size[0].y - size[1].y && curtPoint1.y >= center2.y - size[0].y - size[1].y)
			{ // 下からめり込み
				result1.y = center2.y - size[0].y - size[1].y;
				bIsCollision = true;
			}
			else if (prevPoint1.y >= center2.y + size[0].y + size[1].y && curtPoint1.y <= center2.y + size[0].y + size[1].y)
			{ // 上からめり込み
				result1.y = center2.y + size[0].y + size[1].y;
				bIsCollision = true;
			}
		}

		(*outResult) = Vec3(result1.x, inSegment.point2.y, result1.y);

		return bIsCollision;
	}

} // namespace Collision

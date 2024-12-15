//--------------------------------------------------------------------------------
// 
// 当たり判定 [collision.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// マクロ定義
//---------------------------------------------------
//#define USE_DEGREE	// ifdef this => degree
#define USE_ISCOLLISIONRECTVSPOINT_CROSS	// ifdef this => 外積, ifndef this => 座標変換

//---------------------------------------------------
// エイリアス宣言
//---------------------------------------------------
using Point2D = D3DXVECTOR2;

//===================================================
// 当たり判定の名前空間
//===================================================
namespace Collision
{
	//---------------------------------------------------
	// 構造体
	//---------------------------------------------------
	// 円の情報
	struct Circle
	{
		Vec2 pos;		// 座標
		float radius;	// 半径

		Circle(Vec2 pos = DEF_VEC2, float radius = 0.0f) : pos(pos), radius(radius) {}
	};

	// 球の情報
	struct Sphere
	{
		Vec3 pos;		// 座標
		float radius;	// 半径
	};

	// 矩形の情報
	struct Rectangle
	{
		Vec2 center;		// 中心座標
		Vec2 halfExtend;	// 中心から四隅へのベクトル
		float rot;			// 向き (radian)
	};

	// 線分の情報
	struct LineSegment2D
	{
		Vec2 point1;
		Vec2 point2;
	};

	// 線分の情報
	struct Line
	{
		Vec2 base;
		Vec2 dir;
	};

	// 範囲の情報
	struct Range
	{
		float min;	// 最小
		float max;	// 最大
	};

	// 軸平行境界ボックス ( Axis-Aligned Bounding Box )
	struct AABB
	{
		Vec3 minPos;	// 頂点の最小値
		Vec3 maxPos;	// 頂点の最大値
	};

	// 線分の情報
	struct LineSegment3D
	{
		Vec3 point1;
		Vec3 point2;
	};

	// 軸の種類
	enum class AXIS : UINT { X, Y, Z, MAX };

	// 有向境界ボックス ( Oriented Bounding Box )
	struct OBB
	{
		Vec3 center;									// 中心座標
		Vec3 direction[static_cast<UINT>(AXIS::MAX)];	// 各座標軸（ x, y, z ）の傾きを表すベクトル
		Vec3 halfExtent;								// OBB の各座標軸（ x, y, z ）に沿った長さの半分（中心点から面までの長さ）
	};

	//---------------------------------------------------
	// プロトタイプ宣言
	//---------------------------------------------------
	bool isCollisionCircleVsCircle(const Circle& inCircle1, const Circle& inCircle2);	// 円同士の当たり判定
	bool isCollisionSphereVsSphere(const Sphere& inSphere1, const Sphere& inSphere2);	// 球同士の当たり判定
	bool isCollisionRectVsRect(const Rectangle& inRect1, const Rectangle& inRect2);		// 回転する矩形同士の当たり判定
	bool isCollisionRectVsRect2D(const Vec2& inPos1, const Vec2& inSize1, const Vec2& inPos2, const Vec2& inSize2);	// 2D の矩形同士の当たり判定
	bool isCollisionRectVsRect2D(const ImVec2& inPos1, const ImVec2& inSize1, const ImVec2& inPos2, const ImVec2& inSize2);
	bool isCollisionBlockVsBlock(const Rectangle& inRect1, const Rectangle& inRect2, const float& centerY1, const float& centerY2, const float& inHalfHeight1, const float& inHalfHeight2);	// ブロック同士の当たり判定 (y 軸は回転反映される、x, z 軸は回転反映されない)

	Vec3 getHalfExtents(const AABB& inAABB, const Vec3& inScale);	// 箱形状の半径の取得
	Vec2 getCapsule(const AABB& inAABB, const Vec3& inScale);		// カプセル形状の取得 (MEMO: 返り値 => Vec2(radius, halfHeight))

	void setOBB(const AABB& inAABB, const Matrix& inMtxWorld, OBB* outOBB);	// 有向境界ボックスの設定
	bool isCollisionOBBvsOBB(const OBB& inBox1, const OBB& inBox2);			// 有向境界ボックス同士の当たり判定

	LineSegment2D GetRectEdge(const Rectangle& inRect, const int inPoint);	// 矩形の 1 辺を取得
	bool isSeparatingAxisForRect(LineSegment2D inAxis, Rectangle inRect);	// 辺の 1 辺と、四角形の 1 辺とが重なっているか判定

	//---------------------------------------------------
	// Vector2D
	//---------------------------------------------------
	Vec2 rotVec90(Vec2 inVec);
	float getDegreeToRadian(float inDegree);
	float getRadianToDegree(float inRadian);
	Vec2 getRotateVec(Vec2 inVec, float inRot);	// (ifdef USE_DEGREE) => (inRot = degree)
	bool isEqualVectors(Vec2 inVec1, Vec2 inVec2);
	Vec2 getUnitVec(Vec2 inVec);
	float getVecLength2D(Vec2 inVec);
	Vec2 getDivideVec(Vec2 inVec, float inLength);
	bool equalFloats(float a, float b);
	bool isParallelVec(Vec2 inVec1, Vec2 inVec2);
	bool isVerticalVec(Vec2 inVec1, Vec2 inVec2);
	float dotProduct2D(Vec2 inVec1, Vec2 inVec2);
	float getCross(Vec2 inVec1, Vec2 inVec2);

	//---------------------------------------------------
	// line inSegment
	//---------------------------------------------------
	bool onOneSide(Line inAxis, LineSegment2D inSegment);
	bool overLapping(float inMin1, float inMax1, float inMin2, float inMax2);
	bool isSegmentsCollide(LineSegment2D inSegment1, LineSegment2D inSegment2);

	//---------------------------------------------------
	// Range
	//---------------------------------------------------
	Range getMaxMinRange(Range inRange1, Range inRange2);
	Range getSortRange(Range inRange);
	Vec2 getNegateVec(Vec2 inVec);
	Range getProjectSegment(LineSegment2D inSegment, Vec2 inOnto);
	bool isoverLappingRanges(Range inRange1, Range inRange2);
	Vec2 projectVec(Vec2 inProject, Vec2 inOnto);
	float getNorm(Vec2 inVec);
	float getVec2DLength(Vec2 inVec);

	//---------------------------------------------------
	// Point
	//---------------------------------------------------
	Point2D getProjectPoint(LineSegment2D inSegment, Point2D inPoint);
	Vec2 getReflectionPoint(LineSegment2D inSegment, Point2D inPoint);
	Point2D getCrossPoint(LineSegment2D inSegment1, LineSegment2D inSegment2);
	float clampOnRange(float inX, float inMin, float inMax);

	float getDistanceLinePoint(LineSegment2D inLine, Point2D inPoint);				// 直線と点との距離
	float getDistanceSegmentPoint(LineSegment2D inSegment, Point2D inPoint);		// 線分と点との距離を求める
	float getSegmentDistance(LineSegment2D inSegment1, LineSegment2D inSegment2);	// 線分と線分との距離を求める

	bool isCollisionRectVsPoint(Rectangle rect, Vec2 point);	// 回転する矩形と点の当たり判定

	//---------------------------------------------------
	// AABB
	//---------------------------------------------------
	void setAABB(const Mesh& inMesh, AABB* outAABB);	// 軸平行境界ボックスの設定
	bool isCollisionPointVsAABB(const LineSegment3D& inSegment, const AABB& inAABB, const Matrix& inMtxParent);	// 線分 と AABB の当たり判定

	bool isPointInsideRectangle(const Vec3& point, const Rectangle& rect);
	void pushPointOutOfRectangle(Vec3& point, const Rectangle& rect);

	// AABB 同士の当たり判定
	bool isCollisionAABBVsAABB(
		const LineSegment3D& inSegment,
		const AABB& inAABB1,
		const AABB& inAABB2,
		const Matrix& inMtxParent1,
		const Matrix& inMtxParent2,
		Vec3* outResult);

} // namespace Collision

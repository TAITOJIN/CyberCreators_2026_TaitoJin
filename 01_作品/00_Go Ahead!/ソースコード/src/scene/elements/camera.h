//--------------------------------------------------------------------------------
// 
// カメラ [camera.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
#include "game_object/game_object.h"

//===================================================
// カメラのクラス
//===================================================
class Camera : public GameObject
{
public:
	//---------------------------------------------------
	// public 定数
	//---------------------------------------------------
	static constexpr float REVISION_ANGLE_Y = D3DX_PI * HALF;	// 最初のカメラの向き（D3DX_PI * 0.5f で正面）

	//---------------------------------------------------
	// public 列挙型
	//---------------------------------------------------
	// カメラの状態
	enum class CAMERA_STATE : uint32_t
	{
		NORMAL = 0u,	// 通常
		SHAKE,			// 振動
		MAX
	};

	// カメラの追尾対象
	enum class CAMERA_TARGET : uint32_t
	{
		NONE = 0u,	// 追尾なし
		PLAYER,		// プレイヤー
		MAX
	};

	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	Camera();
	~Camera() override = default;

	HRESULT init(const json& inParam) override;	// 初期設定
	void uninit() override;						// 終了処理
	void update() override;						// 更新処理
	void draw() const override;					// 描画処理

	void inspector() override;					// インスペクター
	void save(json& outObject) const override;	// 保存

	void updateDebug();		// デバッグ時の更新処理
	void setCamera() const;	// カメラの設定処理

	void setPos(const Vec3& inPosV, const Vec3& inPosR);							// カメラの位置の設定
	void setTarget(const CAMERA_TARGET& inTarget);									// ターゲットの変更処理
	void setShake(const float& inShakeFrame, const float& inShakeStrength);			// 振動の設定
	void setDestDistance(const float& inDistance) { m_destDistance = inDistance; }	// 目的の距離の設定

	const Vec3& getPosV() const { return m_posV; }								// 視点の取得
	const Vec3& getPosR() const { return m_posR; }								// 注視点の取得
	Vec2 getRot() const { return Vec2(m_rot.x, m_rot.y - REVISION_ANGLE_Y); }	// 向きの取得
	const float& getDestDistanceKeep() const { return m_destDistanceKeep; }		// 保持している目的の距離の取得

	static Vec3 convertPosWorldToScreen(Vec3* outScreenPos, const Matrix& inMtxWorld);	// ワールド座標からスクリーン座標に変換する関数
	static void raycastMouse(Vec3* outRayOrigin, Vec3* outRayDir);						// マウスを使用したレイキャスト

private:
	//---------------------------------------------------
	// private メンバ関数宣言
	//---------------------------------------------------
	Camera(const Camera&) = delete;
	void operator=(const Camera&) = delete;

	void updateRot(const Vec2& inMoveMouse);	// カメラの向きの更新
	void updatePosByWheel();					// ホイールによるカメラの位置，向きの更新
	void updateState();							// カメラの状態の更新
	void updateRotRestictions();				// カメラの回転制限
	void updatePosV();							// カメラの視点の更新
	void updatePosR();							// カメラの注視点の更新
	Vec2 findRot(const Vec3& inDistance);		// カメラの向きを求める

	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	Vec3 m_posV;							// 視点
	Vec3 m_posR;							// 注視点
	Vec3 m_adjustPos;						// 調整する位置 (揺れなどで)
	Vec3 m_vecU;							// 上方向ベクトル
	Vec3 m_move;							// 移動量
	Vec2 m_rot;								// 向き
	mutable Matrix m_mtxProjection;			// プロジェクションマトリックス
	mutable Matrix m_mtxView;				// ビューマトリックス
	CAMERA_STATE m_state;					// カメラの状態
	CAMERA_TARGET m_target;					// 追尾対象
	float m_distance;						// 注視点と視点の距離
	float m_destDistance;					// 目的の距離
	float m_destDistanceKeep;				// 目的の距離
	float m_moveByWheelRotVol;				// ホイールによる移動量
	float m_rotMoveByStick;					// スティックによる回転量
	float m_shakeFrame;						// 揺らすフレーム
	float m_shakeStrength;					// 揺れの量
	bool m_isTracking;						// 追尾するかどうか [true: する / false: しない]
	float m_firstStartTargetPlayerTime;		// 最初の、プレイヤーを追尾開始までの時間
	float m_firstStartTargetPlayerTimeKeep;	// 最初の、プレイヤーを追尾開始までの時間の保存

	// ツール用 (初期値を保持しておく)
	mutable Vec3 m_firstPosV;
	mutable Vec3 m_firstPosR;
};

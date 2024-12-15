//--------------------------------------------------------------------------------
// 
// カメラ [camera.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// scene
// // elements
#include "scene/elements/camera.h"
// scene
#include "scene/scene_debug.h"
#include "scene/scene_game.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/calculate.h"
#include "utility/file.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
#if 0
	constexpr float RESET_CAMERAPOSV_Y = 300.0f;			// 視点（y）
	constexpr float RESET_CAMERAPOSV_Z = 300.0f;			// 視点（z）
#else
	constexpr float RESET_CAMERAPOSV_Y = 180.0f;			// 視点（y）
	constexpr float RESET_CAMERAPOSV_Z = 180.0f;			// 視点（z）
#endif
	const Vec3 RESET_CAMERAPOSV = Vec3(0.0f, RESET_CAMERAPOSV_Y, -RESET_CAMERAPOSV_Z);	// 視点
	const Vec3 RESET_CAMERAPOSR = Vec3(0.0f, 0.0f, 0.0f);	// 注視点
	constexpr float DAMPING_SPEED_CAMERA = 1.0f;			// 減衰率
	constexpr float VIEWINGANGLE_CAMERA = 45.0f;			// 視野角 (degree)
	constexpr float CORRECTION_CAMERROT = 0.003f;			// 補正
	constexpr float MINOR_ADJUST_ROT = 0.0001f;				// 回転が上限を突破しないための調整
	constexpr float ROTMOVE_RIGHTSTICK = 15.0f;				// 右スティックでのカメラの回転速度
	constexpr float MOVE_CAMERADISTANCE = 10.0f;			// カメラディスタンスの変化量
	constexpr float MAX_MOVE_BY_WHEELROTVOL = 150.0f;		// ホイールによる移動量の制限
	constexpr float MIN_POSDISTANSE_CAMERA = 50.0f;			// 最短距離
	constexpr float MAX_POSDISTANSE_CAMERA = 1000.0f;		// 最長距離
	constexpr float NEAR_RANGE_CAMERA = 10.0f;				// Z 値の最小値
	constexpr float FAR_RANGE_CAMERA = 15000.0f;			// Z 値の最大値
	constexpr float POS_FOLLOW_V = 15.0f;					// 縦
	constexpr float POS_FOLLOW_R = 50.0f;					// 奥行
	constexpr float ADJUST_MOVE = 30.0f;					// 移動量の調整
	constexpr float DEBUG_SPEED_CAMERA = 20.0f;				// デバッグ時のカメラの移動速度
	constexpr float DEBUG_CAMERA_ATN_SPEED = 0.1f;			// デバッグ時のカメラの移動速度調整
	constexpr float SPEED_OPERATION_DISTANCE = 500.0f;		// デバッグ時のカメラの距離の変動速度
	constexpr float ATN_DEST_DISTANCE = 0.01f;				// 目的の距離の減衰率

} // namespace /* anonymous */

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
Camera::Camera()
	: GameObject()
	, m_posV(DEF_VEC3)
	, m_posR(DEF_VEC3)
	, m_adjustPos(DEF_VEC3)
	, m_vecU(DEF_VEC3)
	, m_move(DEF_VEC3)
	, m_rot(DEF_VEC2)
	, m_mtxProjection(DEF_MTX)
	, m_mtxView(DEF_MTX)
	, m_state(Camera::CAMERA_STATE::NORMAL)
	, m_target(Camera::CAMERA_TARGET::NONE)
	, m_distance(0.0f)
	, m_destDistance(0.0f)
	, m_moveByWheelRotVol(0.0f)
	, m_shakeFrame(0.0f)
	, m_shakeStrength(0.0f)
	, m_isTracking(false)
	, m_firstPosV(DEF_VEC3)
	, m_firstPosR(DEF_VEC3)
	, m_firstStartTargetPlayerTime(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT Camera::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject::init(inParam);
	if (FAILED(hr))
	{
		return hr;
	}

	auto& gm = GM;	// ゲームの管理者の取得
	gm.lockCS();

	// ビューポートの設定
	{
		Viewport viewport;
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = SCREEN_WIDTH;
		viewport.Height = SCREEN_HEIGHT;
		viewport.MinZ = 0.0f;
		viewport.MaxZ = 1.0f;
		gm.getDevice()->SetViewport(&viewport);
	}

	// 初期パラメータの設定
	{
		Vec3 posV = inParam[CAMERA_PARAM_POSV];
		Vec3 posR = inParam[CAMERA_PARAM_POSR];
		float rightStickRotMove = inParam[CAMERA_PARAM_RIGHT_STICK_ROT_MOVE];
		bool isTargetPlayer = inParam[CAMERA_PARAM_FIRST_TARGET_IS_PLAYER];
		m_firstStartTargetPlayerTime = inParam[CAMERA_PARAM_FIRSTSTARTTARGETPLAYERTIME];

		m_firstStartTargetPlayerTimeKeep = m_firstStartTargetPlayerTime;

		m_firstPosV = posV;
		m_firstPosR = posR;

		// 位置設定
		setPos(posV, posR);

		// 目的の距離の設定
		auto destDistance = inParam.find(CAMERA_PARAM_DEST_DISTANCE);
		if (destDistance != inParam.end())
		{
			m_destDistance = destDistance.value();
			m_destDistanceKeep = m_destDistance;
		}

		// 上方向ベクトル
		m_vecU = Vec3(0.0f, 1.0f, 0.0f);

		// 回転量
		m_rotMoveByStick = rightStickRotMove;

		// 最初のターゲットの設定
		if (isTargetPlayer)
		{ // プレイヤーをターゲットにする
			setTarget(CAMERA_TARGET::PLAYER);
		}
	}

	shared_ptr<SceneBase> scene = nullptr;
	if(scene = getScene<SceneDebug>())
	{ // デバッグ時
		json j = File::Json::load("res\\debug_config.json");
		bool isNoTarget = j["IsNoTarget"];
		if (isNoTarget)
		{
			setTarget(CAMERA_TARGET::NONE);
		}

		Vec3 posV = j["POSV"];
		Vec3 posR = j["POSR"];

		//m_firstPosV = posV;
		//m_firstPosR = posR;

		// 位置設定
		setPos(posV, posR);
	}
	else if(scene = getScene<SceneGame>())
	{ // ゲーム
		scene->getTime()->addScheduleEvent(m_firstStartTargetPlayerTime, [this]() { setTarget(CAMERA_TARGET::PLAYER); });
	}

	// カメラの設定
	setCamera();

	gm.unlockCS();

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void Camera::uninit()
{
	// DO_NOTHING
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void Camera::update()
{
	if (getScene<SceneDebug>())
	{ // デバッグ時
		updateDebug();
		return;
	}

	float deltaTime = getDeltaTime();	// デルタタイムの取得

	if (m_isTracking)
	{ // 追尾中かつ，(ゲームシーンじゃないまたは，ゲームシーンかつゴールエフェクト中じゃない
		bool can = true;
		auto game = getScene<SceneGame>();
		if (game)
		{
			auto effect = game->getGoalEffect();
			if (effect)
			{
				if (effect->getIsEffecting())
				{
					can = false;
				}
			}
		}

		if (can)
		{
			// ターゲットの位置を取得
			Vec3 targetPos = DEF_VEC3;
			switch (m_target)
			{
			case Camera::CAMERA_TARGET::NONE:
				targetPos = m_posR;
				break;
			case Camera::CAMERA_TARGET::PLAYER:
			{
				const Player& player = *getScene<SceneBase>()->getPlayer();
				btVector3 pos = player.getCharacter().getGhostObject()->getWorldTransform().getOrigin();
				targetPos = Vec3(pos.getX(), pos.getY(), pos.getZ());
				//targetPos = Vec3(pos.getX(), m_posR.y, pos.getZ());
			} break;
			default:
				assert(false);
				break;
			}

			// 距離を算出
			Vec3 distance = targetPos - m_posR;

			// 移動量の更新
			m_move = distance * DAMPING_SPEED_CAMERA * deltaTime;

			// 注視点の更新
			m_posR += m_move;
		}
	}

	// カメラの回転制限
	updateRotRestictions();

	m_distance += (m_destDistance - m_distance) * ATN_DEST_DISTANCE;

	// 視点の更新
	updatePosV();

	// カメラの状態の更新
	updateState();
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void Camera::draw() const
{
	// カメラの設定処理
	setCamera();
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void Camera::inspector()
{
	// 親クラスの処理
	GameObject::inspector();

	// Config の設定
	if (ImGui::CollapsingHeader("Config", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 視点の設定
		{
			Vec3 posV = m_posV;
			bool isReturn = ImGuiAddOns::Transform("PosV", &posV);

			if (isReturn)
			{
				// 反映
				m_posV = posV;
			}
		}

		// 注視点の設定
		{
			Vec3 posR = m_posR;
			bool isReturn = ImGuiAddOns::Transform("PosR", &posR);

			if (isReturn)
			{
				// 反映
				m_posR = posR;
			}
		}

		// 注視点と視点の距離の設定
		{
			float distance = m_distance;
			bool isReturn = ImGuiAddOns::Float("distance", &distance);

			if (isReturn)
			{
				m_distance = distance;
			}
		}
	}
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void Camera::save(json& outObject) const
{
	// 親クラスの処理
	GameObject::save(outObject);

#if 1
	m_firstPosV.y = 693.0f;
	m_firstPosR.y = 420.0f;
#endif
	outObject[CAMERA_PARAM_POSV] = m_firstPosV;
	outObject[CAMERA_PARAM_POSR] = m_firstPosR;
	outObject[CAMERA_PARAM_RIGHT_STICK_ROT_MOVE] = m_rotMoveByStick;
	outObject[CAMERA_PARAM_FIRST_TARGET_IS_PLAYER] = false;
	outObject[CAMERA_PARAM_FIRSTSTARTTARGETPLAYERTIME] = m_firstStartTargetPlayerTimeKeep;

	if (m_destDistanceKeep != 0.0f)
	{
		outObject[CAMERA_PARAM_DEST_DISTANCE] = m_destDistanceKeep;
	}
}

//---------------------------------------------------
// デバッグ時の更新処理
//---------------------------------------------------
void Camera::updateDebug()
{
	auto& inputManager = GM.getInputManager();			// 入力処理の管理者
	auto& keyboard = inputManager.getKeyboard();		// キーボードの取得
	auto& mouse = inputManager.getMouse();				// マウスの取得
	float deltaTime = getDeltaTime();					// デルタタイムの取得
	bool isForcusOnOtherWindow = IAO_IS_WND_HOVERED;	// 別のウィンドウにフォーカスが合っているか

	// マウスの各ボタンのプレス情報
	bool isPressLeft = mouse.getPress(InputMouse::BUTTON::LEFT);
	bool isPressRight = mouse.getPress(InputMouse::BUTTON::RIGHT);
	bool isPressMiddle = mouse.getPress(InputMouse::BUTTON::MIDDLE);

	// マウスの移動量を取得
	Vec2 moveMouse = mouse.getMove() * ADJUST_MOVE;

	// 追尾の切り替え
	if (keyboard.getTrigger(DIK_F10))
	{ // F10 キーが押された
		// 追尾切り替え
		m_isTracking = !m_isTracking;

		// 追尾対象の変更
		m_target = CAMERA_TARGET::NONE;
		if (m_isTracking)
		{ // 追尾する
			m_target = CAMERA_TARGET::PLAYER;
		}
	}

	if (m_isTracking)
	{ // 追尾中
		// ターゲットの位置を取得
		Vec3 targetPos = DEF_VEC3;
		switch (m_target)
		{
		case Camera::CAMERA_TARGET::NONE:
			targetPos = m_posR;
			break;
		case Camera::CAMERA_TARGET::PLAYER:
		{
			const Player& player = *getScene<SceneBase>()->getPlayer();
			btVector3 pos = player.getCharacter().getGhostObject()->getWorldTransform().getOrigin();
			targetPos = Vec3(pos.getX(), pos.getY(), pos.getZ());
			//targetPos = Vec3(pos.getX(), m_posR.y, pos.getZ());
		} break;
		default:
			assert(false);
			break;
		}

		// 距離を算出
		Vec3 distance = targetPos - m_posR;

		// 移動量の更新
		m_move = distance * DAMPING_SPEED_CAMERA * deltaTime;

		// 注視点の更新
		m_posR += m_move;
	}

	// 入力処理
	if (!isForcusOnOtherWindow)
	{ // 別のウィンドウにフォーカスが合っていない
		if (isPressLeft && !isPressRight)
		{ // 左クリック
			// 回転の更新処理
			updateRot(moveMouse);
		}
		if (!isPressLeft && isPressRight && !m_isTracking)
		{ // 右クリック，かつ追尾中でない
			// 回転の更新処理
			updateRot(moveMouse);

			// 注視点の更新
			updatePosR();
		}
		else if (isPressLeft && isPressRight && !m_isTracking)
		{ // 左右クリック，かつ追尾中でない

			// 視点の更新
			{
				float sin = sinf(m_rot.y);
				float cos = cosf(m_rot.y);

				m_posV.x += (-(sin * moveMouse.x - cos * moveMouse.y) * deltaTime);
				m_posV.z += ((sin * moveMouse.y + cos * moveMouse.x) * deltaTime);
			}

			// 注視点の更新
			updatePosR();
		}
		else if (isPressMiddle && !m_isTracking)
		{ // ホイールクリック，かつ追尾中でない
			m_posV.y += (moveMouse.y * deltaTime);

			// 注視点の更新
			updatePosR();
		}
	}

	bool isEdit = false;
	if (auto debug = getScene<SceneDebug>())
	{
		isEdit = debug->getIsEdit();
	}

	if (isEdit)
	{
		// 移動
		bool isMove = false;
		auto targetRot = DEF_ROT;
		{
			Vec2 move(0.0f, 0.0f);

			if (keyboard.getPress(DIK_A))
			{
				move.x += -1.0f;
			}
			if (keyboard.getPress(DIK_D))
			{
				move.x += 1.0f;
			}
			if (keyboard.getPress(DIK_W))
			{
				move.y += -1.0f;
			}
			if (keyboard.getPress(DIK_S))
			{
				move.y += 1.0f;
			}

			if (((move.x != 0.0f) || (move.y != 0.0f)) && keyboard.getPress(DIK_LSHIFT))
			{ // 移動している
				isMove = true;

				// 目的の向きの設定
				float y = atan2f(move.y, move.x) - getScene<SceneBase>()->getCamera()->getRot().y - Camera::REVISION_ANGLE_Y;
				D3DXQuaternionRotationAxis(&targetRot, &AXIS_Y, y);
			}
		}

		// 移動量の更新
		if (isMove)
		{ // 移動する
			Quaternion rot = targetRot;
			Matrix mtxRot;
			D3DXMatrixRotationQuaternion(&mtxRot, &rot);

			// 方向ベクトル
			Vec3 forwardVector(mtxRot._31, mtxRot._32, mtxRot._33);
			Vec3 move = forwardVector * DEBUG_SPEED_CAMERA * -1.0f;

			// 移動量の更新 (加算)
			m_move.x += move.x;
			m_move.z += move.z;
		}

		// 移動量の更新 (減衰)
		m_move.x += (0.0f - m_move.x) * DEBUG_CAMERA_ATN_SPEED;
		m_move.z += (0.0f - m_move.z) * DEBUG_CAMERA_ATN_SPEED;

		// 視点の更新
		m_posV.x += m_move.x * deltaTime;
		m_posV.z += m_move.z * deltaTime;

		// 注視点の更新
		updatePosR();
	}

	// カメラの回転制限
	updateRotRestictions();

	if (!isForcusOnOtherWindow)
	{ // 別のウィンドウにフォーカスが合っていない
		// ホイールによるカメラの位置，向きの更新
		updatePosByWheel();
	}

	// 視点の更新
	updatePosV();

	// カメラの状態の更新
	updateState();
}

//---------------------------------------------------
// カメラの設定処理
//---------------------------------------------------
void Camera::setCamera() const
{
	// デバイスの取得
	const Device& device = GM.getDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(
		&m_mtxProjection,
		D3DXToRadian(VIEWINGANGLE_CAMERA),										// 視野角
		static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT),	// 画面のアスペクト比
		NEAR_RANGE_CAMERA,														// Z 値の最小値
		FAR_RANGE_CAMERA);														// Z 値の最大値

	// プロジェクションマトリックスの設定
	device->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	m_mtxView = DEF_MTX;

	// 位置の調整
	Vec3 posV = m_posV;
	Vec3 posR = m_posR;
#if 0
	posV.y += POS_FOLLOW_V;
	posR.y += POS_FOLLOW_R;
#endif	

	posV += m_adjustPos;
	posR += m_adjustPos;

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView, &posV, &posR, &m_vecU);

	// ビューマトリックスの設定
	device->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//---------------------------------------------------
// カメラの位置の設定
//---------------------------------------------------
void Camera::setPos(const Vec3& inPosV, const Vec3& inPosR)
{
	m_posV = inPosV;
	m_posR = inPosR;

	Vec3 distance = m_posR - m_posV;
	m_distance = D3DXVec3Length(&distance);
	m_destDistance = m_distance;

	m_rot = findRot(distance);
}

//---------------------------------------------------
// ターゲットの変更処理
//---------------------------------------------------
void Camera::setTarget(const CAMERA_TARGET& inTarget)
{
	m_target = inTarget;
	switch (m_target)
	{
	case Camera::CAMERA_TARGET::NONE:
		m_isTracking = false;
		break;
	case Camera::CAMERA_TARGET::PLAYER:
		m_isTracking = true;
		break;
	default:
		assert(false);
		break;
	}
}

//---------------------------------------------------
// 振動の設定
 //---------------------------------------------------
void Camera::setShake(const float& inShakeFrame, const float& inShakeStrength)
{
	m_state = CAMERA_STATE::SHAKE;		// 振動状態にする
	m_shakeFrame = inShakeFrame;		// 揺らすフレームを設定
	m_shakeStrength = inShakeStrength;	// 振動の強さを設定
}

//---------------------------------------------------
// ワールド座標からスクリーン座標に変換する関数
 //---------------------------------------------------
Vec3 Camera::convertPosWorldToScreen(Vec3* outScreenPos, const Matrix& inMtxWorld)
{
	auto& gm = GM;
	gm.lockCS();

	// デバイスの取得
	Device device = gm.getDevice();

	Matrix mtxView, mtxProjecttion;
	Viewport viewport;

	// ビューマトリックスの情報を取得
	device->GetTransform(D3DTS_VIEW, &mtxView);

	// プロジェクションマトリックスの情報を取得
	device->GetTransform(D3DTS_PROJECTION, &mtxProjecttion);

	// ビューポートの情報を取得
	device->GetViewport(&viewport);

	gm.unlockCS();

	Vec3 screenPos, worldPos;
	screenPos = worldPos = DEF_VEC3;

	// ワールド座標をスクリーン座標に変換
	D3DXVec3Project(
		&screenPos,
		&worldPos,
		&viewport,
		&mtxProjecttion,
		&mtxView,
		&inMtxWorld);

	(*outScreenPos) = screenPos;

	return screenPos;
}

//---------------------------------------------------
// マウスを使用したレイキャスト
//---------------------------------------------------
void Camera::raycastMouse(Vec3* outRayOrigin, Vec3* outRayDir)
{
	auto& gm = GM;	// ゲームの管理者
	gm.lockCS();

	Device device = gm.getDevice();								// デバイス
	Vec2 cursorPos = gm.getInputManager().getMouse().getPos();	// カーソルの位置

	// ビューポート
	Viewport viewport;
	device->GetViewport(&viewport);

	// マトリックス
	Matrix projectionMatrix, viewMatrix, worldMatrix;
	projectionMatrix = viewMatrix = worldMatrix = DEF_MTX;

	device->GetTransform(D3DTS_PROJECTION, &projectionMatrix);
	device->GetTransform(D3DTS_VIEW, &viewMatrix);

	gm.unlockCS();

	// マウスの位置 (手前)
	Vec3 nearMouseWorldPos = DEF_VEC3;
	Vec3 nearMouseScreenPos(cursorPos.x, cursorPos.y, 0.0f);
	D3DXVec3Unproject(&nearMouseWorldPos, &nearMouseScreenPos, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);

	// マウスの位置 (奥)
	Vec3 farMouseWorldPos = DEF_VEC3;
	Vec3 farMouseScreenPos(cursorPos.x, cursorPos.y, 1.0f);
	D3DXVec3Unproject(&farMouseWorldPos, &farMouseScreenPos, &viewport, &projectionMatrix, &viewMatrix, &worldMatrix);

	// 結果を反映
	(*outRayOrigin) = nearMouseWorldPos;
	(*outRayDir) = farMouseWorldPos;
}

//---------------------------------------------------
// カメラの向きの更新
//---------------------------------------------------
void Camera::updateRot(const Vec2& inMoveMouse)
{
	float deltaTime = getDeltaTime();

	m_rot.x -= (inMoveMouse.y * CORRECTION_CAMERROT * deltaTime);
	m_rot.y -= (inMoveMouse.x * CORRECTION_CAMERROT * deltaTime);
}

//---------------------------------------------------
// ホイールによるカメラの位置，向きの更新
//---------------------------------------------------
void Camera::updatePosByWheel()
{
	float deltaTime = getDeltaTime();											// デルタタイム
	float wheelRotVol = GM.getInputManager().getMouse().getWheelRotVolume();	// ホイールの回転量

#if 1
	m_distance += (wheelRotVol * deltaTime);	// 拡大・縮小の更新
#else
	// TODO: ホイール直す (240902 追記: 修正済み)
	auto& keyboard = GM.getInputManager().getKeyboard();

	if (keyboard.getPress(DIK_1))
	{
		m_distance += (SPEED_OPERATION_DISTANCE * deltaTime);
	}

	if (keyboard.getPress(DIK_2))
	{
		m_distance -= (SPEED_OPERATION_DISTANCE * deltaTime);
	}
#endif

	// 注視点と視点の距離の制限
	if (m_distance <= MIN_POSDISTANSE_CAMERA)
	{ // 下限突破
		m_distance = MIN_POSDISTANSE_CAMERA;
	}
	else if (m_distance >= MAX_POSDISTANSE_CAMERA)
	{ // 上限突破
		m_distance = MAX_POSDISTANSE_CAMERA;
	}
}

//---------------------------------------------------
// カメラの状態の更新
//---------------------------------------------------
void Camera::updateState()
{
	switch (m_state)
	{
	case Camera::CAMERA_STATE::NORMAL:
	{
		// 調整する位置をリセット
		m_adjustPos = DEF_VEC3;
		break;
	}
	case Camera::CAMERA_STATE::SHAKE:
	{
		// x 軸の振動
		m_adjustPos.x += static_cast<float>(rand() % 10) * m_shakeStrength;
		if (int sign = rand() % 1)
		{
			m_adjustPos.x *= -1;
		}

		// y 軸の振動
		m_adjustPos.y += static_cast<float>(rand() % 10) * m_shakeStrength;
		if (int sign = rand() % 1)
		{
			m_adjustPos.y *= -1;
		}

		// z 軸の振動
		m_adjustPos.z += static_cast<float>(rand() % 10) * m_shakeStrength;
		if (int sign = rand() % 1)
		{
			m_adjustPos.z *= -1;
		}

		// 振動フレーム数をカウントダウン
		m_shakeFrame -= getDeltaTime();
		if (m_shakeFrame <= 0.0f)
		{ // 振動しきった
			m_shakeFrame = 0.0f;			// 振動フレームをリセット
			m_state = CAMERA_STATE::NORMAL;	// カメラの状態を通常にする
		}

		break;
	}
	default:
		assert(false);
		break;
	}
}

//---------------------------------------------------
// カメラの回転制限
//---------------------------------------------------
void Camera::updateRotRestictions()
{
	// カメラの向きの補正
	Calculate::revisionRotForPI(&m_rot.x, m_rot.x);
	Calculate::revisionRotForPI(&m_rot.y, m_rot.y);

	// 回転の制限
	float halfAngle = D3DX_PI * HALF;	// 90°
	if (m_rot.x >= halfAngle - MINOR_ADJUST_ROT)
	{
		m_rot.x = halfAngle - MINOR_ADJUST_ROT;
	}
	else if (m_rot.x <= -halfAngle + MINOR_ADJUST_ROT)
	{
		m_rot.x = -halfAngle + MINOR_ADJUST_ROT;
	}
}

//---------------------------------------------------
// カメラの視点の更新
//---------------------------------------------------
void Camera::updatePosV()
{
	m_posV.x = cosf(m_rot.x) * cosf(m_rot.y);
	m_posV.y = sinf(m_rot.x);
	m_posV.z = cosf(m_rot.x) * sinf(m_rot.y);

	m_posV *= -m_distance;
	m_posV += m_posR;
}

//---------------------------------------------------
// カメラの注視点の更新
//---------------------------------------------------
void Camera::updatePosR()
{
	m_posR.x = cosf(m_rot.x) * cosf(m_rot.y);
	m_posR.y = sinf(m_rot.x);
	m_posR.z = cosf(m_rot.x) * sinf(m_rot.y);

	m_posR *= m_distance;
	m_posR += m_posV;
}

//---------------------------------------------------
// カメラの向きを求める
//---------------------------------------------------
Vec2 Camera::findRot(const Vec3& inDistance)
{
	Vec2 rot = DEF_VEC2;

	rot.y = D3DX_PI * HALF - atan2f(inDistance.x, inDistance.z);

	// horizon
	float hx = -(sinf(rot.y) * D3DX_PI * HALF + atan2f(inDistance.z, inDistance.y)) * Calculate::sign(-inDistance.z);
	float hz = -(cosf(rot.y) * D3DX_PI * HALF + atan2f(inDistance.x, inDistance.y)) * Calculate::sign(-inDistance.x);
	rot.x = -(hx + hz);

	return rot;
}

//--------------------------------------------------------------------------------
// 
// ゲームオブジェクト [game_object.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// 前方宣言
//===================================================
class SceneBase;

//===================================================
// 影のクラス
//===================================================
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	//---------------------------------------------------
	// public メンバ関数宣言
	//---------------------------------------------------
	GameObject();
	GameObject(const GameObject& right);
	GameObject(GameObject&& right) noexcept;
	virtual ~GameObject() = default;
	GameObject& operator=(const GameObject& right);
	GameObject& operator=(GameObject&& right) noexcept;

	virtual HRESULT init(const json& inParam);	// 初期設定 (inParam: 初期パラメータ)
	virtual void uninit() = 0;					// 終了処理
	virtual void update() = 0;					// 更新処理
	virtual void draw() const = 0;				// 描画処理

	virtual void inspector();					// インスペクター
	virtual void save(json& outObject) const;	// 保存 (outObject: 保存パラメータ) TODO: outParam に変更する。継承先でも。

	void setIsDestroy(const bool& inIsDestroy) { m_isDestroy = inIsDestroy; }	// 破棄の設定
	void setPriority(const uint32_t& inPriority) { m_priority = inPriority; }	// 優先度の設定
	void setScene(shared_ptr<SceneBase> inScene) { m_scene = inScene; }			// シーンの設定
	void setTag(const std::string& inTag) { m_tag = inTag; }					// タグの設定

	const bool& getIsDestroy() const { return m_isDestroy; }			// 破棄の取得
	const uint32_t& getPriority() const { return m_priority; }			// 優先度の取得
	template <typename T = SceneBase> shared_ptr<T> getScene() const;	// シーンの取得
	const std::string& getTag() const { return m_tag; }					// タグの取得
	const float& getDeltaTime();										// デルタタイムの取得

	template <typename T> shared_ptr<T> downcast();	// ダウンキャスト
	template <typename T> shared_ptr<T> upcast();	// アップキャスト

private:
	//---------------------------------------------------
	// private メンバ変数宣言
	//---------------------------------------------------
	bool m_isDestroy;						// 破棄するかどうか
	uint32_t m_priority;					// 優先度
	mutable weak_ptr<SceneBase> m_scene;	// シーン
	std::string m_tag;						// タグ
};

//---------------------------------------------------
// シーンの取得
//---------------------------------------------------
template<typename T>
inline shared_ptr<T> GameObject::getScene() const
{
	static_assert(std::is_base_of<SceneBase, T>::value, "T は SceneBase の派生クラスであること！");

	return std::dynamic_pointer_cast<T>(m_scene.lock());
}

//---------------------------------------------------
// ダウンキャスト
//---------------------------------------------------
template<typename T>
inline shared_ptr<T> GameObject::downcast()
{
	static_assert(std::is_base_of<GameObject, T>::value, "T は GameObject の派生クラスであること！");

	return std::dynamic_pointer_cast<T>(shared_from_this());
}

//---------------------------------------------------
// アップキャスト
//---------------------------------------------------
template<typename T>
inline shared_ptr<T> GameObject::upcast()
{
	static_assert(std::is_base_of<GameObject, T>::value, "T は GameObject の基底クラスであること！");

	return std::static_pointer_cast<T>(shared_from_this());
}

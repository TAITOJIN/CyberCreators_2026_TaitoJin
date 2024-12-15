//--------------------------------------------------------------------------------
// 
// 複数のブロックを組み合わせたオブジェクト [blocks_base.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// game_object
// // 3d
// // // block
#include "game_object/3d/block/blocks_object_base.h"
#include "game_object/3d/block/block_blocks.h"
// game_manager
#include "game_manager/game_manager.h"
// utility
#include "utility/file.h"

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
BlocksObjectBase::BlocksObjectBase()
	: GameObject3D()
{
	m_blocks.clear();
}

//---------------------------------------------------
// ムーブコンストラクタ
//---------------------------------------------------
BlocksObjectBase::BlocksObjectBase(BlocksObjectBase&& right) noexcept
	: GameObject3D(std::move(right))
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT BlocksObjectBase::init(const json& inParam)
{
	// 親クラスの処理
	HRESULT hr = GameObject3D::init(inParam);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 読み込む
	loadBlocks(getKey());

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void BlocksObjectBase::uninit()
{
	// 親クラスの処理
	GameObject3D::uninit();
}

//---------------------------------------------------
// 更新処理
//---------------------------------------------------
void BlocksObjectBase::update()
{
#if 0
	// 親クラスの処理
	GameObject3D::update();
#endif

	const Vec3& scale = getScale();
	const Quaternion& rot = getRot();
	const Vec3& pos = getPos();
	const Vec3& offset = getOffset();

	// デバイスの取得
	Device device = GM.getDevice();

	// 計算用マトリックス
	Matrix mtxScale, mtxRot, mtxTrans;
	auto mtxWorld = getMatrix();

	// ワールドマトリックスの初期化
	*mtxWorld = DEF_MTX;

	// スケールの反映
	D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxScale);

	// 向きを反映
	D3DXMatrixRotationQuaternion(&mtxRot, &rot);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxTrans);

	// オフセットを反映
	{
		Matrix mtxOffset = DEF_MTX;

		D3DXMatrixTranslation(&mtxTrans, offset.x, offset.y, offset.z);
		D3DXMatrixMultiply(&mtxOffset, &mtxOffset, &mtxTrans);

		D3DXMatrixMultiply(mtxWorld.get(), mtxWorld.get(), &mtxOffset);
	}

	// ワールドマトリックスの設定
	GM.getDevice()->SetTransform(
		D3DTS_WORLD,
		mtxWorld.get());

	// ワールドマトリックスの設定
	device->SetTransform(D3DTS_WORLD, mtxWorld.get());
}

//---------------------------------------------------
// 描画処理
//---------------------------------------------------
void BlocksObjectBase::draw() const
{
	// DO_NOTHING
}

//---------------------------------------------------
// インスペクター
//---------------------------------------------------
void BlocksObjectBase::inspector()
{
	// 親クラスの処理
	GameObject3D::inspector();
}

//---------------------------------------------------
// 保存
//---------------------------------------------------
void BlocksObjectBase::save(json& outObject) const
{
	// 親クラスの処理
	GameObject3D::save(outObject);
}

//---------------------------------------------------
// ブロックの読み込み処理
//---------------------------------------------------
void BlocksObjectBase::loadBlocks(const std::string& inFilename)
{
	// 読み込む
	json j = File::Json::load(inFilename.c_str());

	// オブジェクトの追加
	auto addObjects = [this, &j](const std::string& inFactoryName)
	{
		auto it = j.find(inFactoryName);
		if (it == j.end())
		{
#ifdef _DEBUG
			GM.cs([&]() { std::cout << "BlocksObjectBase Set => " << inFactoryName << ": Nothing" << std::endl; });
#endif // _DEBUG
			return;
		}

		const json& objects = it.value();
		for (const auto& object : objects)
		{
			auto block = this->getScene<SceneBase>()->addObject(inFactoryName, object);
			if (auto blocksBlock = block->downcast<BlockBlocks>())
			{
				blocksBlock->setMtxParent(this->getMatrix());
			}

			if (auto blockobject = block->downcast<BlockBase>())
			{
				this->m_blocks.push_back(blockobject);
			}
		}
	};

	// MEMO: 透明度のあるオブジェクトは、最後に配置すること。描画順の関係で。

	// 配置
	addObjects(FACTORY_NORMAL_BLOCK);		// 通常ブロック
	addObjects(FACTORY_DUMMY_BLOCK);		// ダミーブロック
	addObjects(FACTORY_FALL_BLOCK);			// 落ちるブロック
	addObjects(FACTORY_GOAL_BLOCK);			// ゴールブロック
	addObjects(FACTORY_MOVE_BLOCK);			// 動くブロック
	addObjects(FACTORY_ROTATE_BLOCK);		// 回転するブロック
	addObjects(FACTORY_BLOCKSBLOCK);		// 複数のブロックの一部
	addObjects(FACTORY_PHYSICS_BLOCK);		// 物理挙動が特にあるブロック
	addObjects(FACTORY_DISAPPEAR_BLOCK);	// 消えるブロック
}

//---------------------------------------------------
// ムーブ代入演算子
//---------------------------------------------------
BlocksObjectBase& BlocksObjectBase::operator=(BlocksObjectBase&& right) noexcept
{
	if (this != &right)
	{
		// 親クラスの処理
		GameObject3D::operator=(std::move(right));
	}

	return *this;
}

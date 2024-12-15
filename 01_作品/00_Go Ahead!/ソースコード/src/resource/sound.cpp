//--------------------------------------------------------------------------------
// 
// サウンド処理 [sound.cpp]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------

//---------------------------------------------------
// インクルード
//---------------------------------------------------
#include "sound.h"
// utility
#include "utility/file.h"

//===================================================
// 無名名前空間
//===================================================
namespace /* anonymous */
{
	//---------------------------------------------------
	// 定数
	//---------------------------------------------------
	constexpr const char* OBJECT_SOUND = "Sound";		// オブジェクト名
	constexpr const char* KEY_LABEL = "LABEL";			// ラベル
	constexpr const char* KEY_NUM = "NUM";				// 用意する数
	constexpr const char* KEY_FILENAME = "FILENAME";	// ファイル名 (パス)
	constexpr const char* KEY_LOOP_CNT = "LOOP_CNT";	// ループカウント
	constexpr const char* KEY_VOLUME = "VOLUME";		// ボリューム
	constexpr const char* SOUND_LIST_FILENAME = "res\\list\\sound_list.json";	// サウンドリストのファイル名

} // namespace /* anonymous */

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT CSoundManager::Init()
{
	// COM ライブラリの初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	HRESULT hr = XAudio2Create(m_XAudio2.GetAddressOf(), 0);
	if (FAILED(hr))
	{
		assert(false && "XAudio2オブジェクトの作成に失敗！");
		return E_FAIL;
	}

	// マスターボイスの生成
	hr = m_XAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr))
	{
		assert(false && "マスターボイスの生成に失敗！");
		return E_FAIL;
	}

	// サウンドの読み込み
	hr = LoadSounds(SOUND_LIST_FILENAME);
	if (FAILED(hr))
	{
		assert(false && "サウンドの読み込みに失敗！");
		return E_FAIL;
	}

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void CSoundManager::Uninit()
{
	// 各サウンドリストの解放
	for (auto& sound : m_sounds)
	{
		sound.second.info.Uninit();
		for (auto& voice : sound.second.data)
		{
			voice.Uninit();
		}
		sound.second.data.clear();
	}
	m_sounds.clear();

	// マスターボイスの破棄
	if (m_pMasteringVoice)
	{
		m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = nullptr;
	}

	// インターフェースの解放
	m_XAudio2.Reset();

	// COM ライブラリの終了処理
	CoUninitialize();
}

//---------------------------------------------------
// セグメント再生
//---------------------------------------------------
HRESULT CSoundManager::Play(const LABEL& label, bool isBegin)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return E_FAIL;
	}

	if (m_sounds[label].data.empty())
	{
		assert(false && "サウンドデータが存在しません！");
		return E_FAIL;
	}

	BYTE playFlag = 0U;
	XAUDIO2_BUFFER buffer = m_sounds[label].info.CreateBuffer();
	if (isBegin)
	{ // 最初から再生
		playFlag |= CSoundManager::CSoundData::PLAY_FLAG_IS_BEGIN;
	}

	for (auto& data : m_sounds[label].data)
	{
		HRESULT hr = data.Play(buffer, playFlag);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	return S_OK;
}
//---------------------------------------------------
// インデックス指定してセグメント再生 (再生中なら停止)
//---------------------------------------------------
HRESULT CSoundManager::Play(const LABEL& label, const UINT32& idx, bool isBegin)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return E_FAIL;
	}

	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return E_FAIL;
	}

	BYTE playFlag = CSoundManager::CSoundData::PLAY_FLAG_IS_STOP;
	XAUDIO2_BUFFER buffer = m_sounds[label].info.CreateBuffer();
	if (isBegin)
	{ // 最初から再生
		playFlag |= CSoundManager::CSoundData::PLAY_FLAG_IS_BEGIN;
	}

	m_sounds[label].data[idx].Play(buffer, playFlag);

	return S_OK;
}

//---------------------------------------------------
// セグメント停止(全て)
//---------------------------------------------------
void CSoundManager::Stop()
{
	for (auto& sounds : m_sounds)
	{
		for (auto& data : sounds.second.data)
		{
			data.Stop();
		}
	}
}

//---------------------------------------------------
// セグメント停止 (ラベル指定)
//---------------------------------------------------
void CSoundManager::Stop(const LABEL& label)
{
	for (auto& data : m_sounds[label].data)
	{
		data.Stop();
	}
}

//---------------------------------------------------
// セグメント停止 (ラベル，インデックス指定)
//---------------------------------------------------
void CSoundManager::Stop(const LABEL& label, const UINT32& idx)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return;
	}

	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return;
	}

	m_sounds[label].data[idx].Stop();
}

//---------------------------------------------------
// 音量設定 (一括)
//---------------------------------------------------
void CSoundManager::SetVolume(const LABEL& label, const float& volume)
{
	for (auto& data : m_sounds[label].data)
	{
		// 音量の設定
		data.SetVolume(volume);
	}
}

//---------------------------------------------------
// 音量設定 (指定)
//---------------------------------------------------
void CSoundManager::SetVolume(const LABEL& label, const UINT32& idx, const float& volume)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return;
	}

	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return;
	}

	m_sounds[label].data[idx].SetVolume(volume);
}

//---------------------------------------------------
// 音量リセット (一括)
//---------------------------------------------------
void CSoundManager::ResetVolume(const LABEL& label)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return;
	}

	float volume = m_sounds[label].info.GetVolume();
	for (auto& data : m_sounds[label].data)
	{
		// 音量の設定
		data.SetVolume(volume);
	}
}

//---------------------------------------------------
// 音量リセット (指定)
//---------------------------------------------------
void CSoundManager::ResetVolume(const LABEL& label, const UINT32& idx)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return;
	}

	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return;
	}

	m_sounds[label].data[idx].SetVolume(m_sounds[label].info.GetVolume());
}

//---------------------------------------------------
// 音量取得 (デフォルト値)
//---------------------------------------------------
float CSoundManager::GetVolume(const LABEL& label)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return 0.0f;
	}

	return m_sounds[label].info.GetVolume();
}

//---------------------------------------------------
// 音量取得 (指定)
//---------------------------------------------------
float CSoundManager::GetVolume(const LABEL& label, const UINT32& idx)
{
	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return m_sounds[label].info.GetVolume();	// デフォルト値
	}

	return m_sounds[label].data[idx].GetVolume();
}

//---------------------------------------------------
//速度の設定 (ラベル，インデックス指定)
//---------------------------------------------------
void CSoundManager::SetSpeed(const LABEL& label, const UINT32& idx, const float& volume)
{
	if (m_sounds.count(label) == 0)
	{
		assert(false && "このラベルには何も登録されていません！");
		return;
	}

	if (idx >= m_sounds[label].data.size())
	{
		assert(false && "インデックスが有効範囲外です！");
		return;
	}

	m_sounds[label].data[idx].SetSpeed(volume);
}

//---------------------------------------------------
// サウンドを取得
//---------------------------------------------------
CSoundManager& CSoundManager::GetInstance()
{
	static CSoundManager instance;
	return instance;
}

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
CSoundManager::CSoundManager() : m_pMasteringVoice(nullptr)
{
	// DO_NOTHING
}

//---------------------------------------------------
// オーディオの読み込み
//---------------------------------------------------
HRESULT CSoundManager::LoadSounds(const char* pFilename)
{
#if 0
	// 読み込んでバリデーションする
	json j = File::Json::loadAndPerformValidator(pFilename);
#else
	json j = File::Json::load(pFilename);
#endif
	// サウンドの読み込み
	for (auto& k : j[OBJECT_SOUND])
	{
		LABEL label = k[KEY_LABEL];
		int num = k[KEY_NUM];
		std::string filename = k[KEY_FILENAME];
		int loopCnt = k[KEY_LOOP_CNT];
		float volume = k[KEY_VOLUME];

		// サウンドの初期設定
		Sound sound;
		sound.info.Init(filename.c_str(), loopCnt, volume);

		for (int i = 0; i < num; i++)
		{
			// データの作成
			CSoundManager::CSoundData data;
			data.SetSourceVoice(sound.info.CreateSourceVoice());	// ソースボイスの生成
			sound.data.push_back(data);								// データを追加
		}

		// サウンドの追加
		m_sounds.emplace(static_cast<LABEL>(label), std::move(sound));

		SetVolume(label, volume);
	}

	return S_OK;
}

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
CSoundManager::CSoundInfo::CSoundInfo()
	: m_audioSize(0U)
	, m_nCntLoop(0)
	, m_fVolume(0.0f)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT CSoundManager::CSoundInfo::Init(const char* pFilename, int nCntLoop, float fVolume)
{
	// パラメータの設定
	m_filename = pFilename;
	m_nCntLoop = nCntLoop;
	m_fVolume = fVolume;

	HANDLE hFile = nullptr;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype = 0;

	// バッファのクリア
	memset(&m_wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));

	// サウンドデータファイルの生成
	hFile = CreateFile(m_filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		assert(false && "サウンドデータファイルの生成に失敗！(1)");
		return E_FAIL;
	}

	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ // ファイルポインタを先頭に移動
		assert(false && "サウンドデータファイルの生成に失敗！(2)");
		return E_FAIL;
	}

	// WAVE ファイルのチェック
	HRESULT hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "WAVEファイルのチェックに失敗！(1)");
		return E_FAIL;
	}

	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "WAVEファイルのチェックに失敗！(2)");
		return E_FAIL;
	}

	if (dwFiletype != 'EVAW')
	{
		assert(false && "WAVEファイルのチェックに失敗！(3)");
		return E_FAIL;
	}

	// フォーマットチェック
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "フォーマットチェックに失敗！(1)");
		return E_FAIL;
	}

	hr = ReadChunkData(hFile, &m_wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "フォーマットチェックに失敗！(2)");
		return E_FAIL;
	}

	// オーディオデータ読み込み
	hr = CheckChunk(hFile, 'atad', &m_audioSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "オーディオデータ読み込みに失敗！(1)");
		return E_FAIL;
	}

	m_audioData.resize(m_audioSize, 0U);
	hr = ReadChunkData(hFile, m_audioData.data(), m_audioSize, dwChunkPosition);
	if (FAILED(hr))
	{
		assert(false && "オーディオデータ読み込みに失敗！(2)");
		return E_FAIL;
	}

	// ファイルをクローズ
	CloseHandle(hFile);

	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void CSoundManager::CSoundInfo::Uninit()
{
	m_audioData.clear();
	m_filename.clear();
}

//---------------------------------------------------
// ソースボイスの生成
//---------------------------------------------------
IXAudio2SourceVoice* CSoundManager::CSoundInfo::CreateSourceVoice()
{
	// ソースボイスの生成
	ComPtr<IXAudio2> XAudio2 = CSoundManager::GetInstance().GetXAudio2();

	IXAudio2SourceVoice* pSourceVoise = nullptr;
	HRESULT hr = XAudio2->CreateSourceVoice(&pSourceVoise, &(m_wfx.Format));
	if (FAILED(hr))
	{
		assert(false && "ソースボイスの生成に失敗！");
		return nullptr;
	}

	// バッファの削除
	pSourceVoise->FlushSourceBuffers();
	return pSourceVoise;
}

//---------------------------------------------------
// バッファの生成
//---------------------------------------------------
XAUDIO2_BUFFER CSoundManager::CSoundInfo::CreateBuffer()
{
	XAUDIO2_BUFFER buffer;
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_audioSize;
	buffer.pAudioData = m_audioData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_nCntLoop;

	return buffer;
}

//---------------------------------------------------
// チャンクのチェック
//---------------------------------------------------
HRESULT CSoundManager::CSoundInfo::CheckChunk(HANDLE hFile, const DWORD& format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead = 0U;
	DWORD dwChunkType = 0U;
	DWORD dwChunkDataSize = 0U;
	DWORD dwRIFFDataSize = 0U;
	DWORD dwFileType = 0U;
	DWORD dwBytesRead = 0U;
	DWORD dwOffset = 0U;

	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ // ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, nullptr) == 0)
		{ // チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr) == 0)
		{ // チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, nullptr) == 0)
			{ // ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{ // ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//---------------------------------------------------
// チャンクデータの読み込み
//---------------------------------------------------
HRESULT CSoundManager::CSoundInfo::ReadChunkData(HANDLE hFile, void* pBuffer, const DWORD& dwBuffersize, const DWORD& dwBufferoffset)
{
	DWORD dwRead = 0U;

	if (SetFilePointer(hFile, dwBufferoffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ // ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	assert(pBuffer);
	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, nullptr) == 0)
	{ // データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

//---------------------------------------------------
// コンストラクタ
//---------------------------------------------------
CSoundManager::CSoundData::CSoundData()
	: m_pSourceVoice(nullptr)
	, m_playbackPos(0U)
{
	// DO_NOTHING
}

//---------------------------------------------------
// 初期設定
//---------------------------------------------------
HRESULT CSoundManager::CSoundData::Init()
{
	return S_OK;
}

//---------------------------------------------------
// 終了処理
//---------------------------------------------------
void CSoundManager::CSoundData::Uninit()
{
	// ソースボイスの破棄
	if (m_pSourceVoice)
	{
		m_pSourceVoice->Stop(0U);
		m_pSourceVoice->DestroyVoice();
		m_pSourceVoice = nullptr;
	}
}

//---------------------------------------------------
// セグメント再生
//---------------------------------------------------
HRESULT CSoundManager::CSoundData::Play(XAUDIO2_BUFFER buffer, BYTE playFlag)
{
	if (!m_pSourceVoice)
	{
		assert(false && "ソースボイスが空です！");
		return E_FAIL;
	}

	// 状態取得
	XAUDIO2_VOICE_STATE xa2state;
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{ // 再生中，かつ停止する

		if ((playFlag & PLAY_FLAG_IS_STOP) != 0)
		{
			// 再生位置を保持
			m_playbackPos = xa2state.SamplesPlayed;

			// 一時停止
			m_pSourceVoice->Stop(0);

			// オーディオバッファの削除
			m_pSourceVoice->FlushSourceBuffers();
		}

		return E_FAIL;
	}

	if ((playFlag & PLAY_FLAG_IS_BEGIN) == 0)
	{ // 途中から再生
		buffer.PlayBegin = static_cast<UINT32>(m_playbackPos);
	}

	// オーディオバッファの登録
	m_pSourceVoice->SubmitSourceBuffer(&buffer);

	// 再生
	m_pSourceVoice->Start(0U);

	return S_OK;
}

//---------------------------------------------------
// セグメント停止
//---------------------------------------------------
void CSoundManager::CSoundData::Stop()
{
	// 状態取得
	XAUDIO2_VOICE_STATE xa2state;
	m_pSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued == 0)
	{ // 停止中
		return;
	}

	// 再生位置を保持
	m_playbackPos = xa2state.SamplesPlayed;

	// 一時停止
	m_pSourceVoice->Stop(0U);

	// オーディオバッファの削除
	m_pSourceVoice->FlushSourceBuffers();
}

//---------------------------------------------------
// 音量の設定
//---------------------------------------------------
void CSoundManager::CSoundData::SetVolume(const float& volume)
{
	m_pSourceVoice->SetVolume(volume);
}

//---------------------------------------------------
// 音量の取得
//---------------------------------------------------
float CSoundManager::CSoundData::GetVolume()
{
	float fVolume = 0.0f;
	m_pSourceVoice->GetVolume(&fVolume);
	return fVolume;
}

//---------------------------------------------------
// 速度の設定
//---------------------------------------------------
void CSoundManager::CSoundData::SetSpeed(const float& speed)
{
	m_pSourceVoice->SetFrequencyRatio(speed);
}

//---------------------------------------------------
// 速度の取得
//---------------------------------------------------
float CSoundManager::CSoundData::GetSpeed()
{
	float fSpeed = 0.0f;
	m_pSourceVoice->GetFrequencyRatio(&fSpeed);
	return fSpeed;
}

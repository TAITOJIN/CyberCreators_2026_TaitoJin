//--------------------------------------------------------------------------------
// 
// サウンド処理 [sound.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//===================================================
// サウンドの管理者クラス
//===================================================
class CSoundManager final
{
public:
	// サウンド一覧
	enum class LABEL : UINT32
	{
		BGM_GAME_000 = 0U,	// 小鳥のさえずり
		BGM_GAME_001,		// 吹雪
		BGM_GAME_002,		// 春
		BGM_GAME_000_ADD,	// のどかな bgm
		BGM_GAME_001_ADD,	// クリスマス
		BGM_GAME_002_ADD,	// のどかな bgm その 2
		SE_GAME_COIN,		// コイン取得音
		BGM_BONFIRE,		// 焚火
		BGM_WATERFALL,		// 滝
		SE_EXPLOSION,		// 樽の爆発
		BGM_HALLOWEEN,		// ハロウィン
		MAX
	};

	HRESULT Init();	// 初期設定
	void Uninit();	// 終了処理

	// 再生
	HRESULT Play(const LABEL& label, bool isBegin = true);						// 既に再生中なら同じラベルで新しく別のインデックスの音声を再生
	HRESULT Play(const LABEL& label, const UINT32& idx, bool isBegin = true);	// インデックス指定 (再生中なら停止)

	// 停止
	void Stop();										// 全て
	void Stop(const LABEL& label);						// ラベル指定
	void Stop(const LABEL& label, const UINT32& idx);	// ラベル，インデックス指定

	// 音量
	void SetVolume(const LABEL& label, const float& volume);					// 設定 (ラベル一括)
	void SetVolume(const LABEL& label, const UINT32& idx, const float& volume);	// 設定 (ラベル，インデックス指定)
	void ResetVolume(const LABEL& label);										// リセット (ラベル一括)
	void ResetVolume(const LABEL& label, const UINT32& idx);					// リセット (ラベル，インデックス指定)
	float GetVolume(const LABEL& label);										// 取得 (デフォルト値)
	float GetVolume(const LABEL& label, const UINT32& idx);						// 取得 (ラベル，インデックス指定)

	// 速度
	void SetSpeed(const LABEL& label, const UINT32& idx, const float& volume);	// 設定 (ラベル，インデックス指定) BGM 向け

	// ゲッター
	ComPtr<IXAudio2> GetXAudio2() { return m_XAudio2; }
	static CSoundManager& GetInstance();

private:
	//===================================================
	// サウンドの情報クラス
	//===================================================
	class CSoundInfo final
	{
	public:
		CSoundInfo();
		~CSoundInfo() = default;

		HRESULT Init(const char* pFilename, int nCntLoop = 0, float fVolume = 1.0f);	// 初期設定
		void Uninit();																	// 終了処理
		IXAudio2SourceVoice* CreateSourceVoice();										// ソースボイスの生成
		XAUDIO2_BUFFER CreateBuffer();													// バッファの生成

		// ゲッター
		std::vector<BYTE>& GetAudioData() { return m_audioData; }
		const DWORD& GetAudioSize() const { return m_audioSize; }
		const std::string& GetFilename() const { return m_filename; }
		const int& GetCntLoop() const { return m_nCntLoop; }
		const float& GetVolume() const { return m_fVolume; }
		const WAVEFORMATEXTENSIBLE& GetWFX() const { return m_wfx; }

	private:
		HRESULT CheckChunk(HANDLE hFile, const DWORD& format, DWORD* pChunkSize, DWORD* pChunkDataPosition);		// チャンクのチェック
		HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, const DWORD& dwBuffersize, const DWORD& dwBufferoffset);	// チャンクデータの読み込み

		std::vector<BYTE> m_audioData;	// オーディオデータ
		DWORD m_audioSize;				// オーディオデータサイズ
		std::string m_filename;			// ファイル名
		int m_nCntLoop;					// ループカウント
		float m_fVolume;				// 音量
		WAVEFORMATEXTENSIBLE m_wfx;		// オーディオウェーブストリームの形式
	};

	//===================================================
	// サウンドのデータクラス
	//===================================================
	class CSoundData final
	{
	public:
		// 再生フラグ
		enum PLAY_FLAG : BYTE
		{
			PLAY_FLAG_IS_STOP = 1 << 0,	// 再生中に再度再生しようとしたら停止する
			PLAY_FLAG_IS_BEGIN = 1 << 1	// 一時停止後に最初から再生する
		};

		CSoundData();
		~CSoundData() = default;

		HRESULT Init();	// 初期設定
		void Uninit();	// 終了処理

		HRESULT Play(XAUDIO2_BUFFER buffer, BYTE playFlag = 0);	// 再生
		void Stop();											// 停止
		void SetVolume(const float& volume);					// 音量の設定
		float GetVolume();										// 音量の取得
		void SetSpeed(const float& speed);						// 速度の設定
		float GetSpeed();										// 速度の取得

		// セッター
		void SetSourceVoice(IXAudio2SourceVoice* ppSourceVoice) { m_pSourceVoice = ppSourceVoice; }
		void SetPlaybackPos(const UINT64& pos) { m_playbackPos = pos; }

		// ゲッター
		IXAudio2SourceVoice* GetSourceVoice() { return m_pSourceVoice; }
		const UINT64& GetPlaybackPos() const { return m_playbackPos; }

	private:
		IXAudio2SourceVoice* m_pSourceVoice;	// ソースボイス
		UINT64 m_playbackPos;					// 再生位置
	};

	// サウンド
	struct Sound
	{
		CSoundInfo info;				// サウンド情報
		std::vector<CSoundData> data;	// サウンドデータ
	};

	CSoundManager();
	~CSoundManager() = default;

	HRESULT LoadSounds(const char* pFilename);	// サウンドの読み込み

	ComPtr<IXAudio2> m_XAudio2;					// XAudio2 オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* m_pMasteringVoice;	// マスターボイス
	std::unordered_map<LABEL, Sound> m_sounds;	// サウンドのハッシュマップ
};

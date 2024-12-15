//--------------------------------------------------------------------------------
// 
// プリコンパイル済みヘッダー [pch.h]
// Author: Taito Jin
// 
//--------------------------------------------------------------------------------
#pragma once

//---------------------------------------------------
// インクルード
//---------------------------------------------------
// Windows
#define _WIN32_DCOM	// CoInitializeEx 関数の呼び出しに必要
#define NOMINMAX
#include <Windows.h>

// DirectX SDK (Software Development Kit)
#include "d3dx9.h"
#define DIRECTINPUT_VERSION	(0x0800)
#include "dinput.h"
#include "xaudio2.h"
#include "XInput.h"
#include <d3dcompiler.h>
#include <wrl/client.h>

// STL (Standard Template Library)
#include <algorithm>
#include <array>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// 入出力
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

// 並行処理
#include <atomic>
#include <future>
#include <mutex>
#include <thread>
#include <condition_variable>

// メモリ管理
#include <memory>

// メモリリークの検出
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>

#ifdef _DEBUG
#define FIND_MEM_LEAKS
#endif // _DEBUG

#ifdef FIND_MEM_LEAKS
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif // FIND_MEM_LEAKS

// その他便利
#include <cassert>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include <comutil.h>
#include <cstdarg>
#include <functional>
#include <random>
#include <string>
#include <utility>
#include <cstdint>
#include <type_traits>
#include <bitset>

// json
#include "nlohmann/json.hpp"
#include "nlohmann/json-schema.hpp"

// imgui-docking
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

// imgui-notify
#include "src/imgui_notify.h"
#include "tahoma.h"

// imgui-add-ons
#include "add-ons.h"

// bullet-physics
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

// inifile-cpp
#include "inicpp.h"

// DXUT
//#include "DXUT.h"

// その他
#include "constants.h"
#include "type_alias.h"
#include "structures.h"
// // utility
#include "utility/custom_json.h"

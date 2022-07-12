#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

//#define WIN_WIDTH 1850
//#define WIN_HEIGHT 900

#define CENTER_X WIN_WIDTH * 0.5f
#define CENTER_Y WIN_HEIGHT * 0.5f

#define WIN_START_X 100
#define WIN_START_Y 100

#define MAX_BONE 500 // 인스턴스오브젝트 각각의 최대 노드개수.
#define MAX_FRAME_KEY 600
#define MAX_INSTANCE 600

#define LERP(s, e, t) s + (e - s) * t
//#define LERP(s, e, t) s * (1 - t) + e * t

#define EPSILON 1

#ifdef NDEBUG
#define V(hr) hr
#else
#define V(hr) assert(SUCCEEDED(hr))
#endif

#define DEVICE Device::Get()->GetDevice()
#define DEVICECONTEXT Device::Get()->GetDeviceContext()

#define KEY_DOWN(k) Control::Get()->Down(k)
#define KEY_UP(k) Control::Get()->Up(k)
#define KEY_PRESS(k) Control::Get()->Press(k)

#define MOUSEPOS Control::Get()->GetMouse()

#define DELTA Timer::Get()->GetElapsedTime()
#define TIME Timer::Get()->GetRunTime()

#define GM GameManager::Get()
#define DM DebugManager::Get()

#define ASSETSWINDOW 0

#include <io.h>
#include <filesystem>
#include <windows.h>
#include <assert.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <functional>
#include <fstream>
#include <float.h>
#include <typeinfo>
#include <iostream>
#include <commdlg.h>
#include "resource.h"
#include <thread>
#include <algorithm>


#include <windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <CommCtrl.h>
#pragma comment(lib, "comctl32")
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <shellapi.h>
#pragma comment(lib, "shlwapi")

#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Indices of file types
#define INDEX_WORDDOC 1
#define INDEX_WEBPAGE 2
#define INDEX_TEXTDOC 3

// Controls
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST,
                                        // because it is a child control under CONTROL_RADIOBUTTONLIST

// IDs for the Task Dialog Buttons
#define IDC_BASICFILEOPEN                       100
#define IDC_ADDITEMSTOCUSTOMPLACES              101
#define IDC_ADDCUSTOMCONTROLS                   102
#define IDC_SETDEFAULTVALUESFORPROPERTIES       103
#define IDC_WRITEPROPERTIESUSINGHANDLERS        104
#define IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS 105


#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


//DirectWrite
#include <d2d1_2.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//DirectXTex
#include "../DirectXTex/DirectXTex.h"

//ImGui

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#define IMAPP_IMPL
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <ImApp.h>

#include <ImGuizmo.h>
#include <ImSequencer.h>
#include <ImZoomSlider.h>
#include <ImCurveEdit.h>
#include <GraphEditor.h>

//Dialog
#include <dirent.h>
#include <ImGuiFileDialog.h>
#include <ImGuiFileDialogConfig.h>

//Assimp
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>



#pragma comment(lib, "Assimp/assimp-vc142-mtd.lib")

using namespace DirectX;
using namespace std;
using namespace DirectX::TriangleTests;


typedef XMFLOAT4 Float4;
typedef XMFLOAT3 Float3;
typedef XMFLOAT2 Float2;
typedef XMVECTOR Vector4;
typedef XMMATRIX Matrix;
typedef XMFLOAT4X4 Float4x4;
typedef XMINT2 Int2;
typedef XMINT3 Int3;
typedef XMINT4 Int4;

typedef function<void()> CallBack;
typedef function<void(int)> CallBackParam;

const XMVECTORF32 kRight = { 1, 0, 0 };
const XMVECTORF32 kUp = { 0, 1, 0 };
const XMVECTORF32 kForward = { 0, 0, 1 };

//Framework Header
#include "GameObject/CDialogEventHandler.h"

#include "Framework/Utility/utility.h"
#include "Framework/Utility/Singleton.h"
#include "Framework/Utility/BinaryReader.h"
#include "Framework/Utility/BinaryWriter.h"
#include "Framework/Utility/Control.h"
#include "Framework/Utility/Timer.h"
#include "Framework/Utility/Xml.h"
#include "Framework/Utility/DirectWrite.h"

using namespace utility;

#include "Framework/Device/Device.h"
#include "Framework/Device/DepthStencil.h"
#include "Framework/Device/RenderTarget.h"

#include "Framework/State/RasterizerState.h"
#include "Framework/State/SamplerState.h"
#include "Framework/State/DepthStencilState.h"
#include "Framework/State/BlendState.h"

#include "Framework/Shader/Shader.h"
#include "Framework/Shader/VertexShader.h"
#include "Framework/Shader/HullShader.h"
#include "Framework/Shader/DomainShader.h"
#include "Framework/Shader/GeometryShader.h"
#include "Framework/Shader/PixelShader.h"
#include "Framework/Shader/ComputeShader.h"

#include "Framework/Buffer/VertexBuffer.h"
#include "Framework/Buffer/IndexBuffer.h"
#include "Framework/Buffer/ConstBuffer.h"
#include "Framework/Buffer/ComputeStructuredBuffer.h"
#include "Framework/Buffer/Texture2DBuffer.h"
#include "Framework/Buffer/VertexLayouts.h"
#include "Framework/Buffer/GlobalBuffers.h"
#include "Framework/Buffer/LightBuffer.h"

#include "Framework/Render/Mesh.h"
#include "Framework/Render/Texture.h"
#include "Framework/Render/Material.h"
#include "Framework/Render/TextureClass.h"

#include "Framework/Math/Vector3.h"
#include "Framework/Math/Transform.h"
#include "Framework/Math/Math.h"

#include "GameObject/Utility/E_States.h"

using namespace GameMath;
using namespace states;

#include "Framework/Collider/Collider.h"
#include "Framework/Collider/BoxCollider.h"
#include "Framework/Collider/SphereCollider.h"
#include "Framework/Collider/CapsuleCollider.h"
#include "Framework/Collider/TetrahedronCollider.h"
#include "Framework/Collider/FrustumCollider.h"

#include "Framework/ModelExport/ModelType.h"
#include "Framework/ModelExport/ModelExporter.h"

#include "Framework/Model/ModelMesh.h"
#include "Framework/Model/ModelReader.h"
#include "Framework/Model/Model.h"
#include "Framework/Model/Models.h"
#include "Framework/Model/ModelClip.h"
#include "Framework/Model/ModelAnimator.h"
#include "Framework/Model/ModelAnimators.h"

#include "Environment/Light.h"
#include "Environment/Camera.h"
#include "Environment/Environment.h"
#include "Environment/Frustum.h"
#include "Environment/Shadow.h"
#include "Environment/Reflection.h"
#include "Environment/Refraction.h"

#include "Algorithm/Node.h"
#include "Algorithm/Heap.h"
#include "Algorithm/AStar.h"

#include "GameObject/UtilityStructures.h"

//Object Header

#include "Objects/Basic/Cube.h"
#include "Objects/Basic/Sphere.h"
#include "Objects/Basic/UIImage.h"
#include "Objects/Basic/EmptyObject.h"

#include "Objects/Landscape/Terrain.h"
#include "Objects/Landscape/TerrainEditor.h"
#include "Objects/Landscape/SkyBox.h"
#include "Objects/Landscape/Sky.h"
#include "Objects/Landscape/TerrainData.h"
#include "Objects/Landscape/QuadTreeTerrain.h"
#include "Objects/Landscape/TerrainLOD.h"
#include "Objects/Landscape/Billboard.h"
#include "Objects/Landscape/Water.h"
#include "Objects/Landscape/Scattering.h"

#include "Objects/Model/ModelObject.h"
#include "Objects/Model/ModelAnimObject.h"

#include "Objects/PostEffect/Outline.h"

#include "Objects/Particle/Particle.h"
#include "Objects/Particle/Spark.h"
#include "Objects/Particle/SpriteEffect.h"
#include "Objects/Particle/Rain.h"
#include "Objects/Particle/Snow.h"
#include "Objects/Particle/ParticleManager.h"
#include "Objects/Particle/EffectData.h"
#include "Objects/Particle/Effect.h"
#include "Objects/Deferred/GBuffer.h"

#include "GameObject/Tool/ToolModel.h"
#include "GameObject/Monster/Monster.h"
#include "GameObject/Monster/Warrok.h"
#include "GameObject/Player/Player.h"
#include "GameObject/Monster/InstanceMutant.h"
#include "GameObject/Monster/InstancingMutants.h"

#include "GameObject/Utility/State.h"

#include "GameObject\/Monster\/MutantStates\MutantPatrolState.h"
#include "GameObject\/Monster\/MutantStates\MutantStalkingState.h"
#include "GameObject\/Monster\/MutantStates\MutantAttackState.h"
#include "GameObject\/Monster\/MutantStates\MutantOnDamageState.h"
#include "GameObject\/Monster\/MutantStates\MutantDieState.h"

//Managers
#include "GameObject/Manager/GameManager.h"
#include "GameObject/Manager/DebugManager.h"

//Scene Header
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Program/Program.h"

extern HWND hWnd;

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

//#define WIN_WIDTH 1920
//#define WIN_HEIGHT 1080


#define CENTER_X WIN_WIDTH * 0.5f
#define CENTER_Y WIN_HEIGHT * 0.5f

#define WIN_START_X 100
#define WIN_START_Y 100

#define MAX_BONE 500
#define MAX_FRAME_KEY 600
#define MAX_INSTANCE 400

#define LERP(s, e, t) s + (e - s) * t
//#define LERP(s, e, t) s * (1 - t) + e * t

#define EPSILON 1

#ifdef NDEBUG
#define V(hr) hr
#else
#define V(hr) assert(SUCCEEDED(hr))
#endif

#define DEVICE Device::Get()->GetDevice()
#define DC Device::Get()->GetDeviceContext()

#define KEY_DOWN(k) Control::Get()->Down(k)
#define KEY_UP(k) Control::Get()->Up(k)
#define KEY_PRESS(k) Control::Get()->Press(k)

#define MOUSEPOS Control::Get()->GetMouse()

#define DELTA Timer::Get()->GetElapsedTime()

#define CAMERA Environment::Get()->GetTargetCamera()
#define LIGHT Environment::Get()->GetLight()

#define GM GameManager::Get()

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
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

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

typedef function<void()> CallBack;
typedef function<void(int)> CallBackParam;

const XMVECTORF32 kRight = { 1, 0, 0 };
const XMVECTORF32 kUp = { 0, 1, 0 };
const XMVECTORF32 kForward = { 0, 0, 1 };

//Framework Header
#include "Framework/Utility/Utility.h"
#include "Framework/Utility/Singleton.h"
#include "Framework/Utility/BinaryReader.h"
#include "Framework/Utility/BinaryWriter.h"
#include "Framework/Utility/Control.h"
#include "Framework/Utility/Timer.h"
#include "Framework/Utility/Xml.h"
#include "Framework/Utility/DirectWrite.h"

using namespace Utility;

#include "Framework/Device/Device.h"
#include "Framework/Device/DepthStencil.h"
#include "Framework/Device/RenderTarget.h"

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
#include "Framework/Buffer/StructuredBuffer.h"
#include "Framework/Buffer/VertexLayouts.h"
#include "Framework/Buffer/GlobalBuffers.h"

#include "Framework/Render/Mesh.h"
#include "Framework/Render/Texture.h"
#include "Framework/Render/Material.h"

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

#include "Framework/State/RasterizerState.h"
#include "Framework/State/SamplerState.h"
#include "Framework/State/DepthStencilState.h"
#include "Framework/State/BlendState.h"

#include "Framework/ModelExport/ModelType.h"
#include "Framework/ModelExport/ModelExporter.h"

#include "Framework/Model/ModelMesh.h"
#include "Framework/Model/ModelReader.h"
#include "Framework/Model/Model.h"
#include "Framework/Model/Models.h"
#include "Framework/Model/ModelClip.h"
#include "Framework/Model/ModelAnimator.h"
#include "Framework/Model/ModelAnimators.h"

#include "Environment/Camera.h"
#include "Environment/Environment.h"
#include "Environment/Frustum.h"
#include "Environment/Shadow.h"
#include "Environment/Reflection.h"
#include "Environment/Refraction.h"

#include "Algorithm/Node.h"
#include "Algorithm/Heap.h"
#include "Algorithm/AStar.h"
//Object Header
#include "Objects/Basic/Cube.h"
#include "Objects/Basic/Sphere.h"
#include "Objects/Basic/UIImage.h"

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

#include "Objects/Model/Krillin.h"
#include "Objects/Model/ModelObject.h"
#include "Objects/Model/Ken.h"
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
#include "GameObject/Monster/Mutant.h"
#include "GameObject/Player/Player.h"

#include "GameObject/Utility/State.h"
#include "GameObject/Monster/MonsterStates/PatrolState.h"
#include "GameObject/Monster/MonsterStates/StalkingState.h"
#include "GameObject/Monster/MonsterStates/AttackState.h"


//Managers
#include "GameObject/Manager/GameManager.h"


//Scene Header
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Program/Program.h"

extern HWND hWnd;

// ShootingGame.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <memory>
#include <time.h>

#include "CSprite.h"
#include "CTexture.h"
#include "CEntity.h"
#include "GameObject.h"
#include "CInput.h"
#include "ShootingGame.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pD3DDevice;
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);
DWORD g_dwPrevTime = 0L;

// g_Player;
shared_ptr<int> a;
//CInput* g_Input;

D3DXVECTOR3 g_InvaderDir = { 1.f,0.f,0.f };

typedef shared_ptr<GameObject> GoPtr;
typedef vector<GoPtr> arrayGoPtr;
typedef vector<GameObject*> arrayLpGo;

bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
void Render(GoPtr player,arrayGoPtr& invader,arrayGoPtr& shoot, arrayGoPtr& invaderShoot);
void SetInvaderDir(arrayGoPtr& Invader,const D3DXVECTOR3& dir);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SHOOTINGGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	HWND hWnd = NULL;
    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow,hWnd))
    {
        return FALSE;
    }

	InitDirect3D(hWnd);

	auto g_Input = shared_ptr<CInput>(new CInput(hWnd, hInstance));
	


	

	shared_ptr<GameObject>g_Player=shared_ptr<GameObject>(new GameObject(g_pD3DDevice, _T("Airplane.bmp")
		, { 320.f,420.f,0.f }));
	g_Player->SetSpeed(200.f);
	g_Player->SetAlive(true);

	vector<shared_ptr<GameObject>> g_shoot;
	vector<shared_ptr<GameObject>> g_Invader;
	stack<shared_ptr<GameObject>> g_ShootPool; //shoot의 죽어있는 것들만 모아놓는 용도

	arrayGoPtr g_InvaderShoot;

	for (int i = 0; i < 20; i++) {
			auto _invader = new GameObject(g_pD3DDevice, _T("monster.bmp")
				, { 32.f + (float(i%5) * 64),32.f+(float(i / 5) * 64),0.f }
				, { 32.f,32.f,0.f }
				, { 0.f,0.f,0.f }
				, 100.f);
			_invader->SetDir(g_InvaderDir);
			_invader->SetAlive(true);

			g_Invader.push_back(shared_ptr<GameObject>(_invader));
	}
	for (int i = 0; i < 20; i++) {
		auto _shoot = new GameObject(g_pD3DDevice, _T("shoot.bmp"), { 0,0,0 }, { 8,16,0 }, { 0,-1,0 }, 200.f);

		g_shoot.push_back(shared_ptr<GameObject>(_shoot));
		g_ShootPool.push(g_shoot[i]);
	}

	for (int i = 0; i < 5; i++) {
		auto _shoot= shared_ptr<GameObject>(new GameObject(g_pD3DDevice, _T("InvaderShoot.bmp"), { 0,0,0 }, { 8,16,0 }, { 0,1,0 }, 150.f));

		g_InvaderShoot.push_back(_shoot);
	}
	srand((unsigned int)time(NULL));

	g_dwPrevTime = GetTickCount();
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHOOTINGGAME));

    MSG msg;
	ZeroMemory(&msg, sizeof(msg));
    // 기본 메시지 루프입니다.
    while (msg.message!=WM_QUIT)
    {
        if (PeekMessage(&msg, NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else {
			g_Input->ReadKeyboard();
			if (g_Input->IsEscapePressed())
				msg.message = WM_QUIT;

			D3DXVECTOR3 keyDir = { 0.f,0.f,0.f };

			g_Input->GetInputDir(keyDir);

			//keyDir.y += g_Input->IsKeyPressed(DIK_UPARROW) ? -1.f : 0.f;;
			//keyDir.y += g_Input->IsKeyPressed(DIK_DOWNARROW) ? 1.f : 0.f;;

			//static DWORD dwPrevTime = GetTickCount();
			DWORD dwCurTime = GetTickCount();
			DWORD dwDt = dwCurTime - g_dwPrevTime;
			float fDeltaTime = (float)(dwDt)*0.001f;
			g_dwPrevTime = dwCurTime;

			g_Player->SetDir(keyDir);
			
			g_Player->Update(fDeltaTime);


			for (size_t i = 0; i < g_Invader.size(); i++) {
				g_Invader[i]->Update(fDeltaTime);
			}
			//for_each(g_invader.begin(), g_invader.end(), [](auto go)) {
//
//			}
			for (size_t i = 0; i < g_shoot.size(); i++) {
				if(g_shoot[i]->GetAlive())
					g_shoot[i]->Update(fDeltaTime);
			}
			for (size_t i = 0; i < g_InvaderShoot.size(); i++) {
				if (g_InvaderShoot[i]->GetAlive())
					g_InvaderShoot[i]->Update(fDeltaTime);
			}

			//invader가 화면 가장자리에 도달했는지 체크
			for (size_t i = 0; i< g_Invader.size(); ++i) { 
				if (g_Invader[i]->GetAlive() == true) {
					auto pos = g_Invader[i]->getPos();
					if (pos.x > 640.f - 32.f) {
						g_InvaderDir = { -1.f,0.f,0.f };
						SetInvaderDir(g_Invader,g_InvaderDir);
						//break;
					}
					else if (pos.x < 32.f) {
						g_InvaderDir = { 1.f,0.f,0.f };
						SetInvaderDir(g_Invader,g_InvaderDir);
						//break;
					}
				}
			}
			//적 총알 화면 바운드 처리
			for (size_t i = 0; i < g_InvaderShoot.size(); i++) {
				if (g_InvaderShoot[i]->GetAlive() == true) {
					auto pos = g_InvaderShoot[i]->getPos();
					if (pos.y > 480.f) {
						g_InvaderShoot[i]->SetAlive(false);
					}
					//플레이어와 충돌 처리
					auto pcPos = g_Player->getPos();
					if (pos.x > pcPos.x - (32.f + 8.f)&&
						pos.x < pcPos.x + (32.f + 8.f)&&
						pos.y > pcPos.y - (32.f + 16.f)&&
						pos.y < pcPos.y + (32.f + 16.f)) 
					{
						g_InvaderShoot[i]->SetAlive(false);
					}

				}
			}
			
			
			for (size_t j = 0; j < g_shoot.size(); j++) {
				if (g_shoot[j]->GetAlive() == true) {
					auto shoot_pos = g_shoot[j]->getPos();
					if (shoot_pos.y < 0.f) {
						g_shoot[j]->SetAlive(false);
						g_ShootPool.push(g_shoot[j]);
					}
					for (size_t i = 0; i < g_Invader.size(); ++i) {
						if (g_Invader[i]->GetAlive()) {
							auto pos = g_Invader[i]->getPos();

							if (shoot_pos.x + 8 > pos.x - 32 && shoot_pos.y - 16 < pos.y + 32 && shoot_pos.x - 8 < pos.x + 32 && shoot_pos.y + 16 > pos.y - 32) {
								g_shoot[j]->SetAlive(false);
								g_Invader[i]->SetAlive(false);
								g_ShootPool.push(g_shoot[j]);
								break;
							}
						}
					}
				}
			}
			//발사 처리
			if (g_Input->IsSpaceKeyPressed())
			{
				if (!g_ShootPool.empty())
				{
					auto pShoot_1 = g_ShootPool.top();
					auto pos = g_Player->getPos();
					pShoot_1->SetPos(pos + D3DXVECTOR3(-16, -32, 0));
					pShoot_1->SetAlive(true);
					g_ShootPool.pop();
				}
				if (!g_ShootPool.empty())
				{
					auto pos = g_Player->getPos();
					auto pShoot_2 = g_ShootPool.top();
					pShoot_2->SetPos(pos + D3DXVECTOR3(16, -32, 0));
					pShoot_2->SetAlive(true);
					g_ShootPool.pop();
				}
			}
			//인베이터 총알 발사 처리
			static float s_ShootTimer = 0.f;
			s_ShootTimer += fDeltaTime;
			if(s_ShootTimer>1.f){
				s_ShootTimer = 0.f;
				for (size_t i = 0; i < g_InvaderShoot.size(); i++) {
					if (g_InvaderShoot[i]->GetAlive() == false) {
						//생성할 몬스터 얻기
						int idx=-1;
						int loop = 10;
						do {
							idx = rand() % (int)g_Invader.size();
							loop--;
						} while (g_Invader[idx]->GetAlive() == false && loop > 0);

						if (g_Invader[idx]->GetAlive() == true) {
							auto pos = g_Invader[idx]->getPos();
							g_InvaderShoot[i]->SetPos(pos + D3DXVECTOR3{ 16, 32, 0 });
							g_InvaderShoot[i]->SetAlive(true);
						}

						break;
					}
				}
			}
				
			Render(g_Player,g_Invader,g_shoot,g_InvaderShoot);
		}
    }

	g_shoot.clear();
	g_Invader.clear();

	CTexture::ReleaseAll();
	ReleaseDirect3D();

    return (int) msg.wParam;
}
void SetInvaderDir(arrayGoPtr& Invader, const D3DXVECTOR3& dir) {
	for (int n = 0; n < 20; ++n) {
		Invader[n]->SetPos(Invader[n]->getPos() + D3DXVECTOR3(0, 10, 0));
		Invader[n]->SetDir(dir);
	}
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHOOTINGGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL; //MAKEINTRESOURCEW(IDC_SHOOTINGGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow,HWND& hWnd)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void Render(GoPtr player, arrayGoPtr& invader, arrayGoPtr& shoot, arrayGoPtr& invaderShoot)
{
	if (g_pD3DDevice == NULL)
		return;
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene())) {
		player->Draw();
		
		for (int i = 0; i<20; i++)
			invader[i]->Draw();
		for (int i = 0; i<20; i++)
			shoot[i]->Draw();
		for (int i = 0; i < 5; i++) {
			if (invaderShoot[i]->GetAlive()) {
				invaderShoot[i]->Draw();
			}
		}
		g_pD3DDevice->EndScene();
	}
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
bool InitDirect3D(HWND hwnd)
{
	//DX 오브젝트 생성
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = 640;
	d3dpp.BackBufferHeight = 480;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, hwnd
		, D3DCREATE_HARDWARE_VERTEXPROCESSING
		, &d3dpp
		, &g_pD3DDevice) == E_FAIL)

		return false;
}

void ReleaseDirect3D()
{
	if (g_pD3DDevice != NULL)
		g_pD3DDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();

	g_pD3DDevice = NULL;
	g_pD3D = NULL;
}
// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

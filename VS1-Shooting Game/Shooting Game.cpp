// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <time.h>

// define the screen resolution and keyboard macros
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
float DEGTORAD = 0.017453f;

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface

LPDIRECT3DTEXTURE9 spaceship;    
LPDIRECT3DTEXTURE9 background;   
LPDIRECT3DTEXTURE9 explosions_c; 
LPDIRECT3DTEXTURE9 rock;   
LPDIRECT3DTEXTURE9 fire_blue;
LPDIRECT3DTEXTURE9 rock_small;
LPDIRECT3DTEXTURE9 explosions_b;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

class Animation {
public:
	RECT rect;
	vector<RECT> frames;
	float Frame, Speed;		
	
	Animation(){}

	Animation(int x, int y, int w, int h, int count, float speed) :Frame(0), Speed(speed) {
		for (int i = 0; i < count; i++) {
			SetRect(&rect, x+i*w, y, x+(i*w)+w, y+h);
			frames.push_back(rect);
		}
	}

	void update(){
		Frame += Speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;

	}
		
	bool isEnd()
	{
		return Frame + Speed >= frames.size();
	}	
};

class Entity {
public:
	float x, y, dx, dy, R, angle;
	bool life;
	string name;
	Animation anim;
	LPDIRECT3DTEXTURE9 Sprite;

	Entity():life(1){}
	
	void settings(Animation &a, LPDIRECT3DTEXTURE9 &sprite, int X, int Y, float Angle = 0, int radius = 1) {
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
		Sprite = sprite;
	}

	virtual void update() {};

	void draw(LPD3DXSPRITE c_d3dspt) {
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    
		D3DXVECTOR3 position(x, y, 0.0f);
		c_d3dspt->Draw(Sprite, &anim.frames[(int(anim.Frame))], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	virtual ~Entity() {  };
};

class asteroid : public Entity {
public:
	asteroid() {
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}

	void update() {
		x += dx;
		y += dy;

		if (x > SCREEN_WIDTH) x = 0; if (x < 0) x = SCREEN_WIDTH;
		if (y > SCREEN_HEIGHT) y = 0; if (y < 0) y = SCREEN_HEIGHT;
	}
};

class bullet : public Entity {
public:
	bullet(){
		name = "bullet";
	}

	void update() {
		dx = cos(angle*DEGTORAD) * 6;
		dy = sin(angle*DEGTORAD) * 6;

		x += dx;
		y += dy;

		if (x > SCREEN_WIDTH || x<0 || y>SCREEN_HEIGHT || y < 0) life = 0;
	}

};

class player : public Entity {
public :
	bool thrust;

	player() { name = "player"; }

	void update() {
		if (thrust) {
			dx += cos(angle*DEGTORAD)*0.2;
			dy += sin(angle*DEGTORAD)*0.2;
		}
		else {
			dx *= 0.99;
			dx *= 0.99;
		}

		int maxSpeed = 15;
		float speed = sqrt(dx*dx + dy*dy);
		if (speed > maxSpeed) {
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x > SCREEN_WIDTH) x = 0; if (x < 0) x = SCREEN_WIDTH;
		if (y > SCREEN_HEIGHT) y = 0; if (y < 0) y = SCREEN_HEIGHT;
	}
};

bool isCollide(Entity *a, Entity *b) {
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y)<
		(a->R + b->R)*(a->R + b->R);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"background.jpg",    // the file name
		SCREEN_WIDTH,    // default width
		SCREEN_HEIGHT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&background);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"spaceship.png",    // the file name
		116,    // default width
		260,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&spaceship);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"type_C.png",    // the file name
		12288,    // default width
		256,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&explosions_c);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"rock.png",    // the file name
		1024,    // default width
		64,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&rock);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"fire_blue.png",    // the file name
		512,    // default width
		64,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&fire_blue);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"rock_small.png",    // the file name
		1024,    // default width
		64,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&rock_small);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"type_B.png",    // the file name
		12288,    // default width
		192,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&explosions_b);    // load to sprite

	return;
}

void do_game_logic(player *p, list<Entity*> &entities, Animation &E, Animation &R, Animation &E_S, Animation &P, Animation &P_G )
{

	//주인공 처리 
	if (KEY_DOWN(VK_UP))
		p->thrust = true;

	if (KEY_DOWN(VK_LEFT))
		p->angle -= 3;

	if (KEY_DOWN(VK_RIGHT))
		p->angle += 3;

	else p->thrust = false;

	for (auto a:entities)
		for (auto b : entities) {
			if(a->name=="asteroid" && b->name == "bullet")
				if (isCollide(a, b))
				{
					a->life = false;
					b->life = false;

					Entity *e = new Entity();
					e->settings(E, explosions_c, a->x-64, a->y-64);
					e->name = "explosion";
					entities.push_back(e);

					for (int i = 0; i < 2; i++) {
						if (a->R == 15) continue;
						Entity *e = new asteroid();
						e->settings(R, rock_small, a->x, a->y, rand() % 360, 15);
						entities.push_back(e);
					}
				}

			if (a->name == "player" && b->name == "asteroid")
				if (isCollide(a, b))
				{
					b->life = false;

					Entity *e = new Entity();
					e->settings(E_S, explosions_b, a->x-39, a->y-36);
					e->name = "explosion";
					entities.push_back(e);

					p->settings(P, spaceship, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 20);
					p->dx = 0; p->dy = 0;
				}
		}

	if (p->thrust) p->anim = P_G;
	else p->anim = P;

	for (auto e : entities)
		if (e->name == "explosion")
			if (e->anim.isEnd()) e->life = 0;

	if (rand() % 100 == 0)
	{
		asteroid *a = new asteroid();
		a->settings(R, rock, 0, rand() % SCREEN_HEIGHT, rand() % 360, 25);
		entities.push_back(a);
	}

	for (auto i = entities.begin(); i != entities.end();)
	{
		Entity *e = *i;

		e->update();
		e->anim.update();

		if (e->life == false) { i = entities.erase(i); delete e; }
		else i++;
	}

}

// this is the function used to render a single frame
void render_frame(list<Entity*> &entities)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 /*
											 static int frame = 21;    // start the program on the final frame
											 if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 // calculate the x-position
											 int xpos = frame * 182 + 1;

											 RECT part;
											 SetRect(&part, xpos, 0, xpos + 181, 128);
											 D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));
											 */
											// 배경화면
	RECT Background;
	SetRect(&Background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position0(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(background, &Background, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));
	
	for (auto i : entities) i->draw(d3dspt);

	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	d3dspt->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	spaceship->Release();
	background->Release();
	explosions_c->Release();
	rock->Release();
	fire_blue->Release();
	rock_small->Release();
	explosions_b->Release();

	return;
}


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_EX_TOPMOST | WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	srand(time(0));

	initD3D(hWnd);
	
	Animation sExplosion(0, 0, 256, 256, 48, 0.7);
	Animation sRock(0, 0, 64, 64, 16, 0.7);
	Animation sRock_small(0, 0, 64, 64, 16, 0.7);
	Animation sBullet(0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(39, 0, 39, 36, 1, 0);
	Animation sPlayer_go(39, 40, 39, 40, 1, 0);
	Animation sExplosion_ship(0, 0, 192, 192, 64, 0.7);

	list<Entity*> entities;

	for (int i = 0; i < 15; i++) {
		asteroid *a = new asteroid();
		a->settings(sRock, rock, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, rand() % 360, 25);
		entities.push_back(a);
	}

	player *p = new player();
	p->settings(sPlayer, spaceship, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0, 20);
	entities.push_back(p);

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (KEY_DOWN(VK_SPACE)) {
				bullet *b = new bullet();
				b->settings(sBullet, fire_blue, p->x, p->y, p->angle, 10);
				entities.push_back(b);
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);

		do_game_logic(p, entities, sExplosion, sRock_small, sExplosion_ship, sPlayer, sPlayer_go);

		render_frame(entities);		

		while ((GetTickCount() - starting_point) < 25);
	}

	cleanD3D();

	return msg.wParam;
}
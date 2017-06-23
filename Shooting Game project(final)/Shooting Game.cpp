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
int STAGE = 0;
float DEGTORAD = 0.017453f;


// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface

LPDIRECT3DTEXTURE9 intro;
LPDIRECT3DTEXTURE9 outro;
LPDIRECT3DTEXTURE9 spaceship;
LPDIRECT3DTEXTURE9 background;
LPDIRECT3DTEXTURE9 explosions_c;
LPDIRECT3DTEXTURE9 rock;
LPDIRECT3DTEXTURE9 fire_blue;
LPDIRECT3DTEXTURE9 fire_red;
LPDIRECT3DTEXTURE9 rock_small;
LPDIRECT3DTEXTURE9 explosions_b;
LPDIRECT3DTEXTURE9 fire_spectial;
LPDIRECT3DTEXTURE9 item_1;
LPDIRECT3DTEXTURE9 item_2;
LPDIRECT3DTEXTURE9 item_3;
LPDIRECT3DTEXTURE9 item_4;
ID3DXFont* Font;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

class Animation {
public:
	RECT rect;
	vector<RECT> frames;
	float Frame, Speed;

	Animation() {}

	Animation(int x, int y, int w, int h, int count, float speed) :Frame(0), Speed(speed) {
		for (int i = 0; i < count; i++) {
			SetRect(&rect, x + i*w, y, x + (i*w) + w, y + h);
			frames.push_back(rect);
		}
	}

	void update() {
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
	float x, y, dx, dy, R, angle, random;
	bool life;
	string name;
	Animation anim;
	D3DXMATRIX matTranslate, b_matTranslate, matRotateZ, matProj;
	LPDIRECT3DTEXTURE9 Sprite;

	Entity() :life(1) { random = (rand() /float(RAND_MAX)+0.3); }

	virtual void settings(Animation &a, LPDIRECT3DTEXTURE9 &sprite, int X, int Y, float Angle = 0, int radius = 1) {
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
		Sprite = sprite;
	}

	virtual void update() {};

	virtual void draw() {
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 position(x, y, 0.0f);
		if (name == "player" || name == "bullet" || name == "s_bullet") {
			D3DXMatrixIdentity(&matProj);
			D3DXMatrixRotationZ(&matRotateZ, (angle + 90)*DEGTORAD);
			D3DXMatrixTranslation(&matTranslate, -x - 19, -y - 18, 0.0f);
			D3DXMatrixTranslation(&b_matTranslate, x + 19, y + 18, 0.0f);
			matProj = matTranslate * matRotateZ * b_matTranslate;
			d3dspt->SetTransform(&matProj);
		}
		else if (name == "asteroid" || name == "asteroid_s") {
			D3DXMatrixIdentity(&matProj); D3DXMatrixIdentity(&matTranslate);
			D3DXMatrixScaling(&matProj, random, random, 0.0f);
			D3DXMatrixTranslation(&matTranslate, x*(1-random), y*(1-random), 0.0f);
			matProj = matProj * matTranslate;
			d3dspt->SetTransform(&matProj);
		}
		else { D3DXMatrixIdentity(&matProj); d3dspt->SetTransform(&matProj); }
		d3dspt->Draw(Sprite, &anim.frames[(int(anim.Frame))], &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	virtual ~Entity() {  };
};


class player : public Entity {
public:
	bool thrust;
	float p_speed;
	player() :p_speed(0.1) { name = "player"; }

	void update() {
		if (thrust) {
			dx += cos(angle*DEGTORAD)*p_speed;
			dy += sin(angle*DEGTORAD)*p_speed;
		}
		else {
			dx *= 0.95;
			dy *= 0.95;
		}

		int maxSpeed = 15;
		float speed = sqrt(dx*dx + dy*dy);
		if (speed > maxSpeed) {
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;
		if (life == true) {
			if (x > SCREEN_WIDTH) x = 0; if (x < 0) x = SCREEN_WIDTH;
			if (y > SCREEN_HEIGHT) y = 0; if (y < 0) y = SCREEN_HEIGHT;
		}
	}
};

class manager : public Entity {
public:
	int score_i, bullet_i, bullet_m, bullet_sb, end;
	float dwTotalTime, dwStartTime, dwEndTime, reRoadTime, fireTime, totalscore;
	RECT rect_s, rect_t, rect_r, rect_b, rect_sb, rect_totalscore;
	char score[50], time[20], reroad[10], bullet[10], bullet_s[10], totalscore_a[50];
	bool reRoad, fireon;
	player *p;
	D3DXMATRIX matProj;

	manager(player *p) :score_i(0), dwStartTime(0), dwEndTime(0), dwTotalTime(0), reRoadTime(0), reRoad(0), p(p), bullet_m(40), bullet_i(40), bullet_sb(10), fireon(0), fireTime(0),
		totalscore(0), end(2) {	SetRect(&rect_s, 0, 0, 100, 100); SetRect(&rect_t, 0, 20, 200, 200);
	}

	virtual void draw() {
		D3DXMatrixIdentity(&matProj); d3dspt->SetTransform(&matProj);
		if (reRoad == 1) Font->DrawTextA(d3dspt, reroad, -1, &rect_r, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		Font->DrawTextA(d3dspt, score, -1, &rect_s, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		Font->DrawTextA(d3dspt, time, -1, &rect_t, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		Font->DrawTextA(d3dspt, bullet, -1, &rect_b, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		Font->DrawTextA(d3dspt, bullet_s, -1, &rect_sb, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
		if (end == 1) {
			Font->DrawTextA(d3dspt, totalscore_a, -1, &rect_totalscore, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
			end == 0;
		}
	}

	virtual void update() {
		if(p->life == true) totalscore = score_i * dwTotalTime/1000;
		SetRect(&rect_r, p->x - 15, p->y - 15, p->x + 10, p->y);
		SetRect(&rect_b, p->x - 15, p->y + 35, p->x + 10, p->y + 45);
		SetRect(&rect_sb, p->x - 15, p->y + 50, p->x + 10, p->y + 60);
		SetRect(&rect_totalscore, SCREEN_WIDTH/2-200, SCREEN_HEIGHT/2, SCREEN_WIDTH/2 , SCREEN_HEIGHT/2);
		sprintf(score, "Score : %d", score_i); sprintf(time, "Time : %3.3f", float(dwTotalTime / 1000)); sprintf(reroad, "Reroad!");
		sprintf(bullet, "%d / %d", bullet_i, bullet_m); sprintf(bullet_s, "BOOM : %d", bullet_sb); sprintf(totalscore_a, "Total Score : %3.1f ::: Exit : ESC Key",totalscore);
		if (reRoadTime / 1000 > 1) { reRoad = 1; reRoadTime = 0; }
		if (fireTime / 1000 > 20) { fireon = 0; }
	}
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
	manager *m;
	bullet(manager *m) : m(m) {
		name = "bullet";
	}

	void update() {

		dx = cos((angle)*DEGTORAD) * 8;
		dy = sin((angle)*DEGTORAD) * 8;

		x += dx;
		y += dy;

		if (x > SCREEN_WIDTH || x<0 || y>SCREEN_HEIGHT || y < 0) { life = 0; }
	}

};

class s_bullet : public Entity {
public:
	manager *m;
	s_bullet(manager *m) : m(m) {
		name = "s_bullet";
	}

	void update() {

		dx = cos((angle)*DEGTORAD) * 4;
		dy = sin((angle)*DEGTORAD) * 4;

		x += dx;
		y += dy;

		if (x > SCREEN_WIDTH || x<0 || y>SCREEN_HEIGHT || y < 0) { life = 0; }
	}

};

class item : public Entity {
public:
	item() { name = "item"; }

	void update() {

		dx = cos((angle)*DEGTORAD) * 2;
		dy = sin((angle)*DEGTORAD) * 2;

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
	d3dpp.Windowed = true;
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
		L"intro.png",    // the file name
		1600,    // default width
		900,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&intro);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"intro.jpg",    // the file name
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
		&outro);    // load to sprite

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
		L"fire_red.png",    // the file name
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
		&fire_red);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Spectialboom.png",    // the file name
		1400,    // default width
		122,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&fire_spectial);    // load to sprite


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

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"item_s.png",    // the file name
		128,    // default width
		31,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&item_1);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"item_b.png",    // the file name
		128,    // default width
		31,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&item_2);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"item_f.png",    // the file name
		128,    // default width
		31,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&item_3);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"item_m.png",    // the file name
		128,    // default width
		31,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&item_4);    // load to sprite


	AddFontResourceEx(L"SDSwaggerTTF.ttf", FR_PRIVATE, 0);
	D3DXCreateFont(d3ddev,
		11,
		11,
		FW_NORMAL,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"SDSwaggerTTF.ttf",
		&Font);

	return;
}

void do_game_logic(player *p, manager *m, list<Entity*> &entities, Animation &E, Animation &R, Animation &E_S, Animation &P, Animation &P_G, Animation &it)
{

	if (KEY_DOWN(VK_UP))
		p->thrust = true;

	if (KEY_UP(VK_UP))
		p->thrust = false;

	if (KEY_DOWN(VK_LEFT))
		p->angle -= 5;

	if (KEY_DOWN(VK_RIGHT))
		p->angle += 5;

	if (KEY_DOWN(VK_SHIFT))
		if (m->reRoad == 1) { m->reRoad = 0;  m->bullet_i = m->bullet_m; }


	for (auto a : entities)
		for (auto b : entities) {
			if ((a->name == "asteroid" || a->name == "asteroid_s") && (b->name == "bullet" || b->name == "s_bullet"))
				if (isCollide(a, b))
				{
					if (a->name == "asteroid") m->score_i += 100;
					if (a->name == "asteroid_s") m->score_i += 50;

					a->life = false;
					b->life = false;

					Entity *e = new Entity();
					e->settings(E, explosions_c, a->x - 64, a->y - 64);
					e->name = "explosion";
					entities.push_back(e);

					int x = rand() % 40;
					if (x == 0) {
						Entity *t = new item();
						t->settings(it, item_1, a->x, a->y, a->angle, 15);
						t->name = "item_1";
						entities.push_back(t);
					}
					else if (x == 1) {
						Entity *t = new item();
						t->settings(it, item_2, a->x, a->y, a->angle, 15);
						t->name = "item_2";
						entities.push_back(t);
					}

					else if (x == 2) {
						Entity *t = new item();
						t->settings(it, item_3, a->x, a->y, a->angle, 15);
						t->name = "item_3";
						entities.push_back(t);
					}

					else if (x == 3) {
						Entity *t = new item();
						t->settings(it, item_4, a->x, a->y, a->angle, 15);
						t->name = "item_4";
						entities.push_back(t);
					}

					for (int i = 0; i < 2; i++) {
						if (a->R == 15) continue;
						Entity *e = new asteroid();
						e->settings(R, rock_small, a->x, a->y, rand() % 360, 15);
						e->name = "asteroid_s";
						entities.push_back(e);
					}
				}

			if (a->name == "player" && (b->name == "asteroid" || b->name == "asteroid_s"))
				if (isCollide(a, b)) {
					a->life = false;
					b->life = false;
					m->bullet_i = 15;

					Entity *e = new Entity();
					e->settings(E_S, explosions_b, a->x - 39, a->y - 36);
					e->name = "explosion";
					entities.push_back(e);

					p->settings(P, spaceship, -SCREEN_WIDTH-1000, -SCREEN_HEIGHT-1000);
					p->p_speed = 0;
										
					m->end = 1;
				}

			if (a->name == "player" && (b->name == "item_1" || b->name == "item_2" || b->name == "item_3" || b->name == "item_4"))
				if (isCollide(a, b))
				{
					if (b->name == "item_1") { p->p_speed += 0.2; b->life = false; }
					else if (b->name == "item_2") { m->bullet_sb += 3; b->life = false; }
					else if (b->name == "item_3") { m->fireon = 1; b->life = false; }
					else if (b->name == "item_4") { m->bullet_m += 15; b->life = false; }
				}
		}
	
	if (p->life != false) {
		if (p->thrust) p->anim = P_G;
		else p->anim = P;
	}


	for (auto e : entities) {
		if (e->name == "explosion")
			if (e->anim.isEnd()) e->life = 0;
	}

	static float time = 0;
	if (time < 150) time += 0.02;
	if (rand() % int(200 - time) == 0)
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

		if (e->name == "player" && m->end ==0) { i = entities.erase(i); delete e; }
		else if (e->life == false && e->name != "player") { i = entities.erase(i); delete e; }
		else i++;
	}

}

// this is the function used to render a single frame
void render_frame(list<Entity*> &entities, manager *m)
{
	D3DXMATRIX matRotateInit;
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
	D3DXMatrixIdentity(&matRotateInit);
	d3dspt->SetTransform(&matRotateInit);
	RECT Background;
	SetRect(&Background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position0(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(background, &Background, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));


	for (auto i : entities) i->draw();

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

	srand((unsigned int)time(NULL));

	initD3D(hWnd);

	MSG msg;

	Animation sintro(0, 0, 1600, 900, 1, 0.7);
	Animation sExplosion(0, 0, 256, 256, 48, 0.7);
	Animation sRock(0, 0, 64, 64, 16, 0.7);
	Animation sRock_small(0, 0, 64, 64, 16, 0.7);
	Animation sBullet(0, 0, 32, 64, 16, 0.8);
	Animation sSpectialboom(0, 0, 100, 108, 14, 0.2);
	Animation sPlayer(39, 0, 39, 36, 1, 0);
	Animation sPlayer_go(39, 40, 39, 40, 1, 0);
	Animation sExplosion_ship(0, 0, 192, 192, 64, 0.7);
	Animation item(0, 0, 32, 31, 4, 0.3);

	Entity *in = new Entity();
	in->settings(sintro, intro, 0, 0);
	RECT rect_in;
	SetRect(&rect_in, SCREEN_WIDTH * 1 / 2-70, SCREEN_HEIGHT * 3 / 4, SCREEN_WIDTH * 1 / 2+15, SCREEN_HEIGHT * 3 / 4);
	while (TRUE)
	{
		if (STAGE == 0) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				if (KEY_DOWN(VK_SPACE)) { STAGE++; }

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (KEY_DOWN(VK_ESCAPE))
				PostMessage(hWnd, WM_DESTROY, 0, 0);

			d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

			d3ddev->BeginScene();    // begins the 3D scene

			d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

			in->anim.update();
			in->draw();
			Font->DrawTextA(d3dspt, "Press Space Key!", -1, &rect_in, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));

			d3dspt->End();    // end sprite drawing

			d3ddev->EndScene();    // ends the 3D scene

			d3ddev->Present(NULL, NULL, NULL, NULL);
		}

		else if (STAGE == 1) {
			list<Entity*> entities;

			player *p = new player();
			p->settings(sPlayer, spaceship, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 20);
			entities.push_back(p);

			manager *m = new manager(p);
			entities.push_back(m);

			for (int i = 0; i < 10; i++) {
				asteroid *a = new asteroid();
				a->settings(sRock, rock, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, rand() % 360, 25);
				entities.push_back(a);
			}
			// enter the main loop:	

			while (TRUE)
			{
				m->dwStartTime = timeGetTime();
				DWORD starting_point = GetTickCount();

				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT) { STAGE = 2; break; }
						

					if (KEY_DOWN(VK_SPACE)) {
						if (m->bullet_i > 0) {
							bullet *b = new bullet(m);
							b->settings(sBullet, fire_blue, p->x, p->y, p->angle, 20);
							entities.push_back(b);
							m->bullet_i--;
						}

						if (m->fireon && m->bullet_i > 0) {
							bullet *c = new bullet(m);
							c->settings(sBullet, fire_red, p->x, p->y, p->angle + 45, 20);
							entities.push_back(c);
						}

					}
					if (KEY_DOWN(VK_CONTROL)) {
						if (m->bullet_sb > 0) {
							for (int i = 0; i < 12; i++) {
								s_bullet *b = new s_bullet(m);
								b->settings(sSpectialboom, fire_spectial, p->x, p->y, (p->angle + (180 * i))*DEGTORAD * 10, 30);
								entities.push_back(b);
							}
							m->bullet_sb--;
						}
					}

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if (KEY_DOWN(VK_ESCAPE))
					PostMessage(hWnd, WM_DESTROY, 0, 0);

				do_game_logic(p, m, entities, sExplosion, sRock_small, sExplosion_ship, sPlayer, sPlayer_go, item);
				
				render_frame(entities, m);

				if (STAGE == 2) break;

				while ((GetTickCount() - starting_point) < 15);
				m->dwEndTime = timeGetTime();
				if (p->life == true) m->dwTotalTime += m->dwEndTime - m->dwStartTime;
				if (m->bullet_i <= 0) { m->reRoadTime += m->dwEndTime - m->dwStartTime; }
				if (m->fireon) { m->fireTime += m->dwEndTime - m->dwStartTime; }
			}			
		}
		else if (STAGE == 2) {
			RECT rect_out;
			SetRect(&rect_out, SCREEN_WIDTH * 1 / 3+15, SCREEN_HEIGHT * 3 / 4, SCREEN_WIDTH * 1 / 3+30 , SCREEN_HEIGHT * 3 / 4);
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
						break;

					if (KEY_DOWN(VK_SPACE)) { STAGE--; }

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			if (KEY_DOWN(VK_ESCAPE)) {
				PostMessage(hWnd, WM_DESTROY, 0, 0); break;
			}

				d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

				d3ddev->BeginScene();    // begins the 3D scene

				d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

				in->anim.update();
				in->draw();
				Font->DrawTextA(d3dspt, "Continue : Press Space Key, Exit : Press ESC!", -1, &rect_out, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));

				d3dspt->End();    // end sprite drawing

				d3ddev->EndScene();    // ends the 3D scene

				d3ddev->Present(NULL, NULL, NULL, NULL);			
		}
	}
	cleanD3D();

	return msg.wParam;
}
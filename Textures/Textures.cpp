
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices
LPDIRECT3DTEXTURE9      g_pTexture = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture1 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture2 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture3 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture4 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture5 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture6 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture7 = NULL; // Our texture
LPDIRECT3DTEXTURE9      g_pTexture8 = NULL; // Our texture


// A structure for our custom vertex type. We added texture coordinates
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR color;    // The color
    FLOAT tu, tv;   // The texture coordinates	
};

// Our custom FVF, which describes our custom vertex structure

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Create the Textures and vertex buffers
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Use D3DX to create a texture from a file based image
    D3DXCreateTextureFromFile( g_pd3dDevice, L"banana.bmp", &g_pTexture );
	D3DXCreateTextureFromFile(g_pd3dDevice, L"banana1.bmp", &g_pTexture1);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"banana2.bmp", &g_pTexture2);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"banana3.bmp", &g_pTexture3);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"banana4.bmp", &g_pTexture4);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"sprite animation background.png", &g_pTexture5);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"sprite animation background1.png", &g_pTexture6);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"sprite animation background2.png", &g_pTexture7);
	D3DXCreateTextureFromFile(g_pd3dDevice, L"sprite animation background3.png", &g_pTexture8);
	
    
    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6 * sizeof( CUSTOMVERTEX ),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
    if( FAILED( g_pVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
        return E_FAIL;
    for( DWORD i = 0; i < 50; i++ )
    {
        FLOAT theta = ( 2 * D3DX_PI * i ) / ( 50 - 1 );

        pVertices[0].position = {-1, -1, 0};
        pVertices[0].color = 0xffffffff;
        pVertices[0].tu = 0.f;
        pVertices[0].tv = 1.0f;


        pVertices[1].position = { -1, 1, 0 };
        pVertices[1].color = 0xffffffff;
        pVertices[1].tu = 0.f;
        pVertices[1].tv = 0.f;


		pVertices[2].position = {1, -1, 0 };
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = 1.0f;
		pVertices[2].tv = 1.0f;


		pVertices[3].position = { 1, -1, 0 };
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = 1.0f;
		pVertices[3].tv = 1.0f;


		pVertices[4].position = { -1, 1, 0 };
		pVertices[4].color = 0xffffffff;
		pVertices[4].tu = 0.f;
		pVertices[4].tv = 0.f;


		pVertices[5].position = { 1, 1, 0 };
		pVertices[5].color = 0xffffffff;
		pVertices[5].tu = 1.0f;
		pVertices[5].tv = 0.f;

    }
    g_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pTexture8 != NULL)
		g_pTexture8->Release();

	if (g_pTexture7 != NULL)
		g_pTexture7->Release();

	if (g_pTexture6 != NULL)
		g_pTexture6->Release();

	if (g_pTexture5 != NULL)
		g_pTexture5->Release();

	if (g_pTexture4 != NULL)
		g_pTexture4->Release();

    if( g_pTexture3 != NULL )
        g_pTexture3->Release();
	
	if (g_pTexture2 != NULL)
		g_pTexture2->Release();

	if (g_pTexture1 != NULL)
		g_pTexture1->Release();

	if (g_pTexture != NULL)
		g_pTexture->Release();

    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // Set up world matrix
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );
    //D3DXMatrixRotationX( &matWorld, timeGetTime() / 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f, -7.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 11, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	static int counter = 0;
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );
    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Setup the world, view, and projection matrices
        SetupMatrices();
		counter +=2;

		switch (counter % 150)
		{
			case 0:
				g_pd3dDevice->SetTexture(0, g_pTexture);
				g_pd3dDevice->SetTexture(1, g_pTexture5);
				break;

			case 30:
				g_pd3dDevice->SetTexture(0, g_pTexture1);
				g_pd3dDevice->SetTexture(1, g_pTexture6);
				break;

			case 60:
				g_pd3dDevice->SetTexture(0, g_pTexture2);
				g_pd3dDevice->SetTexture(1, g_pTexture7);
				break;
			
			case 90:
				g_pd3dDevice->SetTexture(0, g_pTexture3);
				g_pd3dDevice->SetTexture(1, g_pTexture8);
				break;

			case 120:
				g_pd3dDevice->SetTexture(0, g_pTexture4);
				
				break;
		
		}
        // Setup our texture. Using Textures introduces the texture stage states,
        // which govern how Textures get blended together (in the case of multiple
        // Textures) and lighting information. In this case, we are modulating
        // (blending) our texture with the diffuse color of the vertices.
        
		// 생성한 텍스처에 스테이지를 할당한다.
		

		// 0번 텍스처에 0번 텍스처 인덱스를 사용한다.(FVF에 정점마다 8개의 텍스처 인덱스가 있다)
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		// 1번 텍스처에 0번 텍스처 인덱스를 사용한다.
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

		// 텍스처 맵핑을 할때 확대 필터를 사용하고, Linear 방식으로 보간한다.
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		// D3DTSS_COLOROP, D3DTSS_ALPHAOP과 함께
		// D3DTOP_SELECTARG1를 사용하면 이번 stage에 input를 바로 output으로 전달한다.
		// 0번 스테이지의 컬러 값과 알파 값이 그대로 output으로 전달된다.
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		// color operation을 modulate로 설정
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,  8);

		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, 15);

		// 컬러의 첫 번째 인자는 현재 스테이지의 텍스처의 컬러 값
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		// 컬러의 두 번째 인자는 이전 스테이지에서 전달된 컬러 값
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

		// 알파의 첫 번째 인자는 현재 스테이지의 텍스처의 알파 값
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, D3DTA_TEXTURE);

		// 알파의 두 번째 인자는 이전 스테이지에서 전달된 알파 값
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, D3DTA_CURRENT);

		// 결국 SetTextureStageState() 함수를 사용하여 텍스처1과 텍스처2의 알파 블랜딩하였다.


		// 3번째 스테이지에서부터는 컬러와 알파값에 대한 연산을 하지 않는다.
		g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);
        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 05: Textures",
                              WS_OVERLAPPEDWINDOW, 600, 100, 500, 500,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the scene geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}


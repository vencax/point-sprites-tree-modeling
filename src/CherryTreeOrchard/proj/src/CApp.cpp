
#include "./stdafx.h"
#include "./main.h"
#include "./Graphics.h"
#include "./natur/Scene.h"
#include "./Camera.h"
#include "./CApp.h"


#define CLASSNAME "LoD"
#define TITLE	CLASSNAME
#define WIDTH		800
#define HEIGHT	600

// --------------------------------------------------------------
//	Ctor
// --------------------------------------------------------------
CApp::CApp(void)
{
	// init member variables
	m_wWidth = WIDTH;
	m_wHeight = HEIGHT;

	//create window
	InitWindow();

	// create 3D
	Graphics::Create( m_window, m_wWidth, m_wHeight, true);

	// construct input
	m_pInput = new CInput( );

	// contruct camera
	m_camera = new TCam( Graphics::GetDevice( ), m_pInput);

	// construct scene
	m_scene = new CScene( Graphics::GetDevice( ), m_camera );

	// resize scene
	Graphics::Resize( m_wWidth, m_wHeight);

	// initialize scene
	m_scene->InitScene( );

}


/************************************************************************/
/* Name:		~CApplication											*/
/* Description:	unreg win class and call dests			*/
/************************************************************************/
CApp::~CApp(void)
{
	//	cleenup everithing
	if( m_scene)
	{
		delete m_scene;
	}

	if( m_pInput)
	{
		delete m_pInput;
	}

	if( m_camera)
	{
		delete m_camera;
	}

	// unregister class
	UnregisterClass(CLASSNAME,GetModuleHandle(NULL));
}

/************************************************************************/
/* Name:		InitWindow												*/
/* Description:	create	window class, window and child elements			*/
/************************************************************************/

void CApp::InitWindow(void)
{
	WNDCLASSEX WindowClass;
	RECT DesktopSize;

	//InitCommonControls();

	//Get desktop resolution
	GetClientRect(GetDesktopWindow(),&DesktopSize);

	//fill window class and register class
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = winProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = GetModuleHandle(NULL);
	WindowClass.hIcon = NULL;
	WindowClass.hCursor = NULL;
	WindowClass.hbrBackground = CreateSolidBrush( RGB( 77, 77, 77));
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = CLASSNAME;
	WindowClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&WindowClass);

	//create window
	m_window = CreateWindowEx(WS_EX_CONTROLPARENT, CLASSNAME, TITLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,(DesktopSize.right - WIDTH) / 2,(DesktopSize.bottom - HEIGHT) / 2,WIDTH,HEIGHT,NULL,NULL,GetModuleHandle(NULL),NULL);

}

void
CApp::PrintFPS( void)
{
	char newText[ 64];

	// prepare new
	sprintf( newText, "%s, FPS: %.2f", TITLE, this->GetScene( )->GetFPS( ) );
	SetWindowText( this->GetWindowHandle( ), newText);
}




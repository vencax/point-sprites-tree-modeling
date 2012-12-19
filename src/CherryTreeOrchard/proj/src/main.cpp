
//	Bakalarska prace
//	Vaclav Klecanda
//	Tema:	point sprites ve vykreslovani prirodnich objektu a LoD

#include "./stdafx.h"
#include "./main.h"

//	globals
CApp cApp;				//	applycation object

unsigned int tickCount = 0;

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{	

    // Enter the message loop
    MSG msg; 
    ZeroMemory( &msg, sizeof(msg) );

	// main loop
    while( msg.message != WM_QUIT )
    {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// start measuring FPS
			cApp.GetScene( )->Starttiming( );

			// update input and camera
			cApp.GetInput( )->update( );
			cApp.GetCamera( )->Update( );

			// once 5 frames do update the scene
			// if trees dont move between circles, etc.
			if( tickCount > 2)
			{
				cApp.GetScene( )->Update( );
				tickCount = 0;
			}
			else
			{
				tickCount++;
			}

			// each frame do tickUpdate
			cApp.GetScene( )->Animate( );

			// and draw scene

			cApp.GetScene( )->Draw( );

			cApp.GetScene( )->Endtiming( );

			// and print FPS in window label
			cApp.PrintFPS( );
		}
	}
	
    // Clean up everything and exit the app
    //	destructor of the CApp class will do it

    return 0;
}
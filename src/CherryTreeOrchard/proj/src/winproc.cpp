
#include "./stdafx.h"
#include "./main.h"


// ----------------------------------------------------------
/*The window's message handler */

LRESULT WINAPI winProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
		
	//case WM_COMMAND:
	//	hMenu = GetMenu (hWnd);
	//	switch(LOWORD(wParam))
	//	{
	//	case ID_FILE_LSYSDEFINE:
	//		DialogBox (cApp.GetInstance( ), ( LPCSTR)IDD_LSYSTEMDEFINITION, hWnd, DefineDlgProc) ;
	//		return 0;
	//	case ID_FILE_OPEN40003:
	//		// open dialog for selecting file with LSys
	//		HRESULT openDlg;
	//		//openDlg = openfiledlg( NULL, NULL, NULL, "Select file with LSystem to be viewed", BSTR *pathName);
	//		return 0;
	//	case ID_FILE_EXIT:
	//		SendMessage (hWnd, WM_CLOSE, 0, 0L);
	//		return 0;
	//	}
	//	return 0;

	case WM_SIZE:
		// nWidth = LOWORD(lParam); nHeight = HIWORD(lParam);
		cApp.Resize( LOWORD(lParam), HIWORD(lParam));

		break;

	case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
				
    }
	
    return DefWindowProc( hWnd, msg, wParam, lParam );
}
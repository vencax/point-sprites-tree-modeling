#pragma once

// aplication class
class CApp : public Graphics
{


public:
	CApp(void);
	~CApp(void);

	void						InitWindow(void);
	
	inline HWND							GetWindowHandle( void)			{ return m_window; }
	inline HINSTANCE					GetInstance( void)					{ return m_hInstance; }
	inline DWORD						GetWidth( void)						{ return m_wWidth; }
	inline DWORD						GetHeight( void)						{ return m_wHeight; }
	inline CScene*						GetScene( void)						{ return m_scene; }
	inline CInput*							GetInput( void)						{ return m_pInput; }
	inline TCam*							GetCamera( void)					{ return m_camera; }

	void	PrintFPS( void);

	// private members
private:

	HWND					m_window;
	HINSTANCE			m_hInstance;					

	DWORD					m_wWidth,
									m_wHeight;

	CScene		*m_scene;
	CInput		*m_pInput;
	TCam		*m_camera;

};

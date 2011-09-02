#include "pllogo.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

#ifdef WIN32
HWND		g_hwnd = NULL;
HDC			g_hDC = NULL;			// Private GDI Device Context
HGLRC		g_hRC = NULL;			// Permanent Rendering Context
#else
#error "unknown HWND implementation"
#endif

#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "glaux.h"			// Header File For The Glaux Library

GLuint	g_texture = 0;			// Storage For One Texture ( NEW )
STRING g_logo;
PluginSys::Plugin * g_Plugin = NULL;
u32 g_fps = 50;

namespace OpenGl
{
	bool LoadGLTextures()		// Load Bitmaps And Convert To Textures
	{
		bool ret = false;
		AUX_RGBImageRec * TextureImage = 0;					// Create Storage Space For The Texture

		// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
		if (TextureImage = auxDIBImageLoad(g_logo.c_str()))
		{
			glGenTextures(1, &g_texture);					// Create The Texture

			// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, g_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 
				0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			ret = true;
		}

		SAFE_DELETE(TextureImage->data);				// Free The Texture Image Memory
		SAFE_DELETE(TextureImage);						// Free The Image Structure

		return ret;
	}
	bool CreateOpenGLWindow()
	{
		static PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			16,											// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(g_hDC = GetDC(g_hwnd)))					// Did We Get A Device Context?
		{
			return false;								// Return FALSE
		}

		GLuint		PixelFormat;						// Holds The Results After Searching For A Match

		if (!(PixelFormat = ChoosePixelFormat(g_hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			return false;								// Return FALSE
		}

		if(!SetPixelFormat(g_hDC, PixelFormat, &pfd))	// Are We Able To Set The Pixel Format?
		{
			return false;								// Return FALSE
		}

		if (!(g_hRC = wglCreateContext(g_hDC)))			// Are We Able To Get A Rendering Context?
		{
			return false;								// Return FALSE
		}

		if(!wglMakeCurrent(g_hDC, g_hRC))				// Try To Activate The Rendering Context
		{
			return false;								// Return FALSE
		}
		return true;
	}
	bool InitGL()										// All Setup For OpenGL Goes Here
	{
		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		return true;										// Initialization Went OK
	}
	void ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
	{
		if (height==0)										// Prevent A Divide By Zero By
		{
			height=1;										// Making Height Equal One
		}

		glViewport(0,0,width,height);						// Reset The Current Viewport

		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix

		// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
	}
	void DrawGLScene()									// Here's Where We Do All The Drawing
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glLoadIdentity();									// Reset The View
		glTranslatef(0.0f, 0.0f, -10.0f);

		glBindTexture(GL_TEXTURE_2D, g_texture);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

		glEnd();

		SwapBuffers(g_hDC);
	}
}
bool IniLogo()
{
	while (!g_hwnd)
	{
#ifdef WIN32
		g_Plugin->GetOther((void*)PI_GS_UI_WIN_HANDLE, (void*)&g_hwnd);
		::Sleep(1);
#else
#error "unknown Sleep implementation"
#endif
	}

	if (!OpenGl::CreateOpenGLWindow())
	{
		return false;
	}

	OpenGl::ReSizeGLScene(500, 400);					// Set Up Our Perspective GL Screen

	if (!OpenGl::LoadGLTextures())						// Jump To Texture Loading Routine ( NEW )
	{
		return false;									// If Texture Didn't Load Return FALSE
	}

	if (!OpenGl::InitGL())								// Initialize Our Newly Created GL Window
	{
		return false;									// Return FALSE
	}

	return true;
}
void DrawLogo()
{
	OpenGl::DrawGLScene();
}
extern "C" LOGO_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);

	g_logo = config.Get(PLUGIN_LOG_FILE_CONFIG_NAME);
	// load fps
	STRING fps = config.Get(PLUGIN_LOG_FPS_CONFIG_NAME);
	g_fps = boost::lexical_cast<u32>(fps.c_str());

	g_Plugin = p;
	
	return true;
}
extern "C" LOGO_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	return true;
}

extern "C" LOGO_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return g_hwnd != 0;
	}
	return false;
}
extern "C" LOGO_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		g_hwnd = *((HWND*)param);
		return true;
	}
	return false;
}
extern "C" LOGO_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	if (!IniLogo())
	{
		return false;
	}

	boost::timer tm;
	double tps = 1.f / g_fps;
	while (1)
	{
		double elapsed = tm.elapsed();
		if (elapsed < tps)
		{
#ifdef WIN32
			::Sleep(1);
#else
#error "unknown Sleep implementation"
#endif
		}
		else
		{
			tm.restart();
			DrawLogo();
		}
	}
	
	return true;
}
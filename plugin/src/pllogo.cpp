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
// Minimal BMP loader replaces deprecated glaux (auxDIBImageLoad) for modern MSVC CI.
#include <stdlib.h>
#include <string.h>

GLuint	g_texture = 0;			// Storage For One Texture ( NEW )
STRING g_logo;
PluginSys::Plugin * g_Plugin = NULL;
u32 g_fps = 50;
f32 g_alpha = 1.f;
f32 g_alphaspeed = 0.1f;

namespace OpenGl
{
	// Load a 24-bit Windows BMP into top-down RGB bytes.
	unsigned char * LoadBmpRgb(const char * path, int * outW, int * outH)
	{
		FILE * f = fopen(path, "rb");
		if (!f)
		{
			return 0;
		}
		unsigned char header[54];
		if (fread(header, 1, 54, f) != 54 || header[0] != 'B' || header[1] != 'M')
		{
			fclose(f);
			return 0;
		}
		int dataPos = header[10] + (header[11] << 8) + (header[12] << 16) + (header[13] << 24);
		int width = header[18] + (header[19] << 8) + (header[20] << 16) + (header[21] << 24);
		int height = header[22] + (header[23] << 8) + (header[24] << 16) + (header[25] << 24);
		int bpp = header[28] + (header[29] << 8);
		if (width <= 0 || height == 0 || bpp != 24)
		{
			fclose(f);
			return 0;
		}
		int absH = height < 0 ? -height : height;
		bool topDown = height < 0;
		int rowRaw = width * 3;
		int rowStride = (rowRaw + 3) & ~3;
		size_t imgSize = (size_t)rowStride * (size_t)absH;
		unsigned char * bgr = (unsigned char *)malloc(imgSize);
		if (!bgr)
		{
			fclose(f);
			return 0;
		}
		fseek(f, dataPos, SEEK_SET);
		if (fread(bgr, 1, imgSize, f) != imgSize)
		{
			free(bgr);
			fclose(f);
			return 0;
		}
		fclose(f);
		unsigned char * rgb = (unsigned char *)malloc((size_t)width * (size_t)absH * 3);
		if (!rgb)
		{
			free(bgr);
			return 0;
		}
		for (int y = 0; y < absH; y++)
		{
			int srcY = topDown ? y : (absH - 1 - y);
			unsigned char * src = bgr + (size_t)srcY * rowStride;
			unsigned char * dst = rgb + (size_t)y * (size_t)width * 3;
			for (int x = 0; x < width; x++)
			{
				dst[x * 3 + 0] = src[x * 3 + 2];
				dst[x * 3 + 1] = src[x * 3 + 1];
				dst[x * 3 + 2] = src[x * 3 + 0];
			}
		}
		free(bgr);
		*outW = width;
		*outH = absH;
		return rgb;
	}
	bool LoadGLTextures()		// Load Bitmaps And Convert To Textures
	{
		bool ret = false;
		int w = 0, h = 0;
#ifdef UNICODE
		std::string path;
		{
			int n = WideCharToMultiByte(CP_ACP, 0, g_logo.c_str(), -1, NULL, 0, NULL, NULL);
			if (n > 0)
			{
				path.resize((size_t)(n - 1));
				WideCharToMultiByte(CP_ACP, 0, g_logo.c_str(), -1, &path[0], n, NULL, NULL);
			}
		}
		const char * cpath = path.c_str();
#else
		const char * cpath = g_logo.c_str();
#endif
		unsigned char * data = LoadBmpRgb(cpath, &w, &h);
		if (data)
		{
			glGenTextures(1, &g_texture);
			glBindTexture(GL_TEXTURE_2D, g_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			ret = true;
			free(data);
		}
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
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
	void DrawGLScene(double elapsed)									// Here's Where We Do All The Drawing
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glLoadIdentity();									// Reset The View
		glTranslatef(0.0f, 0.0f, -10.0f);

		glBindTexture(GL_TEXTURE_2D, g_texture);

		glColor4f(1.0f, 1.0f, 1.0f, g_alpha);
		g_alpha -= (f32)(elapsed * g_alphaspeed);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

		glEnd();

		SwapBuffers(g_hDC);
	}
	void QuitGL()
	{
		glDeleteTextures(1, &g_texture);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(g_hRC);
		ReleaseDC(g_hwnd, g_hDC);
	}
}
bool IniLogo()
{
	while (!g_hwnd)
	{
		g_Plugin->GetOther((void*)PI_GS_UI_WIN_HANDLE, (void*)&g_hwnd);
		SLEEP(1);
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
void DrawLogo(double elapsed)
{
	OpenGl::DrawGLScene(elapsed);
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

	// load alpha
	STRING alpha = config.Get(PLUGIN_LOG_ALPHA_CONFIG_NAME);
	g_alpha = boost::lexical_cast<f32>(alpha.c_str());

	// load alphaspeed
	STRING alphaspeed = config.Get(PLUGIN_LOG_ALPHA_SPEED_CONFIG_NAME);
	g_alphaspeed = boost::lexical_cast<f32>(alphaspeed.c_str());

	g_Plugin = p;
	
	return true;
}
extern "C" LOGO_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	OpenGl::QuitGL();
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
	while (g_alpha >= 0.f)
	{
		double elapsed = tm.elapsed();
		if (elapsed < tps)
		{
			SLEEP(1);
		}
		else
		{
			tm.restart();
			DrawLogo(elapsed);
		}
	}
	
	return true;
}
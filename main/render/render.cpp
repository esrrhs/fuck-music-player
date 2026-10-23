#include "Logger.h"
#include "render.h"
#include "../localheader.h"
#include <boost/pool/singleton_pool.hpp>
#include <d3d9.h>
#include <stdio.h>
#include "../god/god.h"

Render::Render() : m_pD3DDevice(NULL)
{
}

bool Render::Ini()
{
	LOG_ENTER
	if (!create_device())
	{
		LOG_ERROR("create_device failed");
		LOG_LEAVE
		return false;
	}
	LOG_LEAVE
	return true;
}
bool Render::Begin()
{
	if (!m_pD3DDevice)
	{
		return false;
	}
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->BeginScene();
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	return true;
}
bool Render::End()
{
	if (!m_pD3DDevice)
	{
		return false;
	}
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->EndScene();
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->Present(0, 0, 0, 0);
	return true;
}
bool Render::create_device()
{
	LOG_ENTER
	// 写死d3d
	bool ok = create_d3d9_device();
	LOG_LEAVE
	return ok;
}
bool Render::create_d3d9_device()
{
	LOG_ENTER

	// 创建d3d
	LPDIRECT3D9 d_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d_D3D)
	{
		LOG_ERROR("Direct3DCreate9");
		return false;
	}
	LOG_TRACE("Direct3DCreate9 ok");

	D3DDISPLAYMODE d3ddm;
	if (FAILED(d_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		LOG_ERROR("GetAdapterDisplayMode");
		d_D3D->Release();
		return false;
	}

	void * hwnd = SINGLETON(God).GetHwnd();

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 
	d3dpp.Windowed = TRUE; // 指明是窗口模式，而非全局独占模式 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 无需交换链 
	d3dpp.BackBufferFormat = d3ddm.Format; // 后备缓冲区颜色格式 
	d3dpp.EnableAutoDepthStencil = TRUE; // 使深度缓冲区有效 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // 深度缓冲区颜色格式 
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//使用屏幕刷新率
	d3dpp.hDeviceWindow = (HWND)hwnd;

	// Explicit backbuffer size from the client rect (0x0 params fail on some drivers).
	RECT crc;
	if (GetClientRect((HWND)hwnd, &crc))
	{
		LONG cw = crc.right - crc.left;
		LONG ch = crc.bottom - crc.top;
		if (cw > 0 && ch > 0)
		{
			d3dpp.BackBufferWidth = (UINT)cw;
			d3dpp.BackBufferHeight = (UINT)ch;
		}
	}

	// main.dll runs on a boost worker thread (plugin\\thread), not the wx UI thread.
	// D3DCREATE_MULTITHREADED is required for CreateDevice/Present off the UI thread.
	const DWORD createFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;

	LPDIRECT3DDEVICE9 d_3DDevice = NULL;
	// Prefer HAL; fall back to REF (useful on headless / limited-GPU CI runners).
	HRESULT hr = d_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)hwnd,
		createFlags, &d3dpp, &d_3DDevice);
	{
		FILE * df = fopen("d3d-init.txt", "a");
		if (df)
		{
			fprintf(df, "HAL hr=0x%08lX hwnd=%p bb=%ux%u\n", (unsigned long)hr, hwnd,
				(unsigned)d3dpp.BackBufferWidth, (unsigned)d3dpp.BackBufferHeight);
			fclose(df);
		}
	}
	if (FAILED(hr))
	{
		LOG_ERROR("CreateDevice HAL failed; trying REF");
		hr = d_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, (HWND)hwnd,
			createFlags, &d3dpp, &d_3DDevice);
		FILE * df = fopen("d3d-init.txt", "a");
		if (df)
		{
			fprintf(df, "REF hr=0x%08lX\n", (unsigned long)hr);
			fclose(df);
		}
	}
	if (FAILED(hr) || !d_3DDevice)
	{
		LOG_ERROR("CreateDevice");
		FILE * df = fopen("d3d-init.txt", "a");
		if (df)
		{
			fprintf(df, "CreateDevice FAILED\n");
			fclose(df);
		}
		d_D3D->Release();
		return false;
	}
	{
		FILE * df = fopen("d3d-init.txt", "a");
		if (df)
		{
			fprintf(df, "CreateDevice OK device=%p\n", (void*)d_3DDevice);
			fclose(df);
		}
	}
	m_pD3DDevice = d_3DDevice;
	// Keep d_D3D alive for the process lifetime (device holds ref); release our create ref.
	d_D3D->Release();

	LOG_TRACE("CreateDevice ok");

	LOG_LEAVE
	return true;
}

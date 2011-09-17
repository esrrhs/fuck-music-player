#include "Logger.h"
#include "render.h"
#include "../localheader.h"
#include <boost/pool/singleton_pool.hpp>
#include <d3d9.h>
#include "../god/god.h"

bool Render::Ini()
{
	LOG_ENTER
	create_device();
	LOG_LEAVE
	return true;
}
bool Render::Begin()
{
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->BeginScene();
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	return true;
}
bool Render::End()
{
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->EndScene();
	((LPDIRECT3DDEVICE9)m_pD3DDevice)->Present(0, 0, 0, 0);
	return true;
}
bool Render::create_device()
{
	LOG_ENTER
	// д��d3d
	create_d3d9_device();
	LOG_LEAVE
	return true;
}
bool Render::create_d3d9_device()
{
	LOG_ENTER

	// ����d3d
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
		return false;
	}

	void * hwnd = SINGLETON(God).GetHwnd();

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 
	d3dpp.Windowed = TRUE; // ָ���Ǵ���ģʽ������ȫ�ֶ�ռģʽ 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ���轻���� 
	d3dpp.BackBufferFormat = d3ddm.Format; // �󱸻�������ɫ��ʽ 
	d3dpp.EnableAutoDepthStencil = TRUE; // ʹ��Ȼ�������Ч 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // ��Ȼ�������ɫ��ʽ 
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//ʹ����Ļˢ����
	d3dpp.hDeviceWindow = (HWND)hwnd;

	LPDIRECT3DDEVICE9 d_3DDevice;
	if (FAILED(d_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d_3DDevice)))
	{
		LOG_ERROR("CreateDevice");
		return false;
	}
	m_pD3DDevice = d_3DDevice;

	LOG_TRACE("CreateDevice ok");

	LOG_LEAVE
	return true;
}
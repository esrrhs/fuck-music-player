#include "uimng.h"
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>
#include <d3d9.h>
#include "globle.h"

UIMng::UIMng()
{
}
UIMng::~UIMng()
{
}
void UIMng::ini()
{
	LOG_TRACE("UIMng", "ini...");
	LPDIRECT3D9 d_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d_D3D)
	{
		LOG_ERROR("UIMng", "Direct3DCreate9");
		return;
	}
	LOG_TRACE("UIMng", "Direct3DCreate9 ok");

	D3DDISPLAYMODE d3ddm;
	if (FAILED(d_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		LOG_ERROR("UIMng", "GetAdapterDisplayMode");
		return;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 
	d3dpp.Windowed = TRUE; // ָ���Ǵ���ģʽ������ȫ�ֶ�ռģʽ 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ���轻���� 
	d3dpp.BackBufferFormat = d3ddm.Format; // �󱸻�������ɫ��ʽ 
	d3dpp.EnableAutoDepthStencil = TRUE; // ʹ��Ȼ�������Ч 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // ��Ȼ�������ɫ��ʽ 
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//ʹ����Ļˢ����
	d3dpp.hDeviceWindow = (HWND)SINGLETON(Globle).m_hwnd;
	
	LPDIRECT3DDEVICE9 d_3DDevice;
	if (FAILED(d_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)SINGLETON(Globle).m_hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d_3DDevice)))
	{
		LOG_ERROR("UIMng", "CreateDevice");
		return;
	}
	LOG_TRACE("UIMng", "CreateDevice ok");

	LOG_TRACE("UIMng", "ini ok");
}
void UIMng::run()
{
	
}
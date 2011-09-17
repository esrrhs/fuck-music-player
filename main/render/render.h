#ifndef __RENDER__
#define __RENDER__

class Render
{
public:
	bool Ini();
	bool Begin();
	bool End();
	void * GetDevice()
	{
		return m_pD3DDevice;
	}
private:
	bool create_device();
	bool create_d3d9_device();
private:
	void * m_pD3DDevice;
};

#endif	
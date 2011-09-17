#ifndef __GOD__
#define __GOD__

class God
{
public:
	bool Ini();
	bool Loop();
	bool Quit();
	void * GetHwnd()
	{
		return m_hwnd;
	}
	void SetHwnd(void * hwnd)
	{
		m_hwnd = hwnd;
	}
	u32 GetFps()
	{
		return m_fps;
	}
	void SetFps(u32 fps)
	{
		m_fps = fps;
	}
private:
	bool Hearbeat(double elapsed);
private:
	void * m_hwnd;
	u32 m_fps;
};

#endif	
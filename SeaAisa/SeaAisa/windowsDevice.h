#pragma once
#include<Windows.h>
class WindowsDevice {
public:

	HDC hdc;
	HWND hwnd;
	PAINTSTRUCT ps;
	HBITMAP hBitmap;
	BYTE* pBits;
	BITMAPINFOHEADER bmih;
	BITMAP bitmap;
	HDC hdcmem;
	WNDCLASSEX wc;
	int w, h;
	int offw, offh;
	wchar_t* ClassName;

	WindowsDevice(){}

	bool Init(HINSTANCE hInstance, int nShowCmd);
};





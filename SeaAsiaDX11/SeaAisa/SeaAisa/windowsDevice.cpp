#include "windowsDevice.h"

bool WindowsDevice::Init(HINSTANCE hInstance, int nShowCmd)
{
	ClassName = L"Visual Frame";

	w = GetSystemMetrics(SM_CXSCREEN);
	h = GetSystemMetrics(SM_CYSCREEN);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, (LPCTSTR)107);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(wc.hInstance, (LPCTSTR)107);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"window can't be register", ClassName, MB_ICONERROR);
		return 0;
	}

	RECT rc = { 0,0,w,h };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd = CreateWindow(ClassName, L"Visual window", WS_OVERLAPPEDWINDOW, 0, 0, w, h,
		NULL, NULL, hInstance, NULL);

	RECT rc1;
	int offw, offh;
	GetClientRect(hwnd, &rc1);
	offw = w - (rc1.right - rc1.left);
	offh = h - (rc1.bottom - rc1.top);


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);


}

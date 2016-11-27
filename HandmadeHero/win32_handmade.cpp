#include <Windows.h>
//redefine the static keyword to describe the 3 different things it can do

//variables that are scoped to a function can still be static in that they will be initialized only once and will persist between function calls
//but won't be accessible outside the function (not global)
#define local_persist static ;

//most typical use of creating global variables
#define global_variable static ;

//functions that can only be used inside the file they are defined
#define internal static;

global_variable bool Running; //initialized to 0 by default if static

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
//this is information about the monitor
//because you can have different monitors of course, 
//it might be necesssary to know specifics about which one is plugged in
global_variable HDC BitmapDeviceContext;

//DIB
internal void
Win32ResizeDeviceIndependentBitmapSection(int Width, int Height)
{
	if (BitmapHandle)
	{
		DeleteObject(BitmapHandle);
	}
	if (!BitmapDeviceContext)
	{
		BitmapDeviceContext = CreateCompatibleDC(0);
	}
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
		

	BitmapHandle = CreateDIBSection(
		BitmapDeviceContext, &BitmapInfo
		, DIB_RGB_COLORS,
		&BitmapMemory,
		0, 0
	);

}
internal void
Wind32UpdateWindow(HDC DeviceContext,int X,int Y,int Width,int Height)
{
	/*
	StretchDIBits(DeviceContext,
		X, Y, Width, Height,
		X, Y, Width, Height,

		DIB_RGB_COLORS, SRCCOPY);
		*/
}

void HandleWindowMessages(void)
{
	MSG Message;

	//windows doesn't send any  message to our code so we have to just loop and keep checking for messages in the queue
	while (Running)
	{
		BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
		//returns -1 if its a quit message
		if (MessageResult)
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else {
			break;
		}
	}
}


LRESULT CALLBACK 
MainWindowCallback(
	HWND   Window,
	UINT   Message,
	WPARAM WParam,
	LPARAM LParam
)
{

	LRESULT Result = 0;
	switch (Message)
	{
		//the window was resized
		case WM_SIZE:
		{
			//this is the rectangle of the window that does not include the windows parts like the border and menu and such
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;
			Win32ResizeDeviceIndependentBitmapSection(Width, Height);

		}break;
		case WM_DESTROY:
		{
			Running = false;
		}break;
		//the window x button clicked
		case WM_CLOSE:
		{
			Running = false;
		}break;
		case WM_ACTIVATE:
		{
		}break;
		case WM_PAINT:
		{
			//i guess that this just creates a rectangle that is the entire window
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			//how did left get set? its just whatever the entire window is?
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

			PatBlt(DeviceContext, X, Y, Width, Height, BLACKNESS);

			EndPaint(Window, &Paint);
		}
		default:
		{
			Result = DefWindowProc(Window,Message,WParam,LParam);
		}break;

	}
	return Result;

}




int CALLBACK WinMain(
	 HINSTANCE Instance,
	 HINSTANCE PrevInstance,
	 LPSTR     lpCmdLine,
	 int       nCmdShow
) {

	//MessageBox(0, L"test", L"test", MB_OK | MB_ICONINFORMATION);

	WNDCLASS WindowClass = {};
	WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback ;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = L"HandmadeHeroWindowClass";

	//no idea why you have to call Register class
	//but it takes a pointer to the WNDClass you made
	//no sure why this would fail
	if (RegisterClass(&WindowClass))
	{
		//this is what makes the window show up on the screen
		HWND WindowHandle = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			L"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0);
		//not sure why this call would fail
		if (WindowHandle)
		{
			Running = true;
			HandleWindowMessages();
		}
	}
}

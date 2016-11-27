#include <Windows.h>

void HandleWindowMessages(void)
{
	MSG Message;

	//windows doesn't send any  message to our code so we have to just loop and keep checking for messages in the queue
	for (;;)
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
		case WM_SIZE:
		{
		}break;
		case WM_DESTROY:
		{
		}break;
		case WM_CLOSE:
		{
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
			HandleWindowMessages();
		}
	}
}

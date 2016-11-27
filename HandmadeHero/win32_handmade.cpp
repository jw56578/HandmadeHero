#include <Windows.h>
#include <stdint.h>
//redefine the static keyword to describe the 3 different things it can do

//variables that are scoped to a function can still be static in that they will be initialized only once and will persist between function calls
//but won't be accessible outside the function (not global)
#define local_persist static ;

//most typical use of creating global variables
#define global_variable static ;

//functions that can only be used inside the file they are defined
#define internal static;

//redefine things into more simpler types
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;




global_variable bool Running; //initialized to 0 by default if static

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

//another random example of how to write bits to the memory to create colors on the screen
internal void
RenderWeirdGradient(int BlueOffset, int GreenOffset)
{
	int Width = BitmapWidth;
	int Height = BitmapHeight;

	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for (int Y = 0;
		Y < BitmapHeight;
		++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0;
			X < BitmapWidth;
			++X)
		{
			uint8 Blue = (X + BlueOffset);
			uint8 Green = (Y + GreenOffset);
			//<< bit wise shit to put the bits in the correct location
			//or all the individual 8 bits together into 32 bits
			//because Pixel is a uint32, ++ will move the pointer this far
			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Pitch;
	}
}



//DIB
internal void
Win32ResizeDeviceIndependentBitmapSection(int Width, int Height)
{

	if (BitmapMemory)
	{
		//tell the operating system that it can use this memory now
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	//a negative height is used for pixels starting at top left which we are used to
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;


	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
	//how to get a block of memory from windows
	//0 = dont' care where the memory is
	BitmapMemory = VirtualAlloc(0, 
		//how many bytes you want
		BitmapMemorySize, 
		//tell windows to track this memory immediatelyi
		MEM_COMMIT,
		//only read and write, no executing
		PAGE_READWRITE);

	return;
	//this is the size of one row
	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8 *)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint8 *Pixel = (uint8 *)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{
			//we are dealng with one byte at a time 
			//as indicated by uint8 pointer

			//put something in the location the pointer is pointing at
			//this is the blue channel
			*Pixel = 0;
			//because the pointer is defined as int8, adding one to it will move the pointer 8 bits
			++Pixel;
			//this is green channel
			*Pixel = 0;
			Pixel++;
			//this is the red channel
			*Pixel = 255;
			Pixel++;
			*Pixel = 0;
			Pixel++;

		}
		//move to the next row
		Row += Pitch;
	}

}
internal void
Win32UpdateWindow(HDC DeviceContext,RECT *WindowRect, int X,int Y,int Width,int Height)
{
	//remember the arrow syntax is when you are accessing properties from 
	//  a pointer to a struct
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	
	StretchDIBits(DeviceContext,
		/*
		X, Y, Width, Height,
		X, Y, Width, Height,
		*/
		0,0,BitmapWidth,BitmapHeight,
		0,0,WindowWidth,WindowHeight,
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
		
}

void HandleWindowMessages(HWND Window)
{
	MSG Message;
	int XOffset = 0;
	int YOffset = 0;

	//windows doesn't send any  message to our code so we have to just loop and keep checking for messages in the queue
	while (Running)
	{
		//GetMessage blocks the thread waiting for a message
		//we don't want to block, we want to be able to do other things while waiting for a message
		while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT)
			{
				Running = false;
			}

			TranslateMessage(&Message);
			DispatchMessageA(&Message);
		}

		//just doing random whatever to show animation
		//if we didn't use peekmessage, the animation would be stopped
		RenderWeirdGradient(XOffset, YOffset);

		HDC DeviceContext = GetDC(Window);
		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		int WindowWidth = ClientRect.right - ClientRect.left;
		int WindowHeight = ClientRect.bottom - ClientRect.top;
		Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
		ReleaseDC(Window, DeviceContext);

		++XOffset;
		YOffset += 2;

	}
}


LRESULT CALLBACK
MainWindowCallback(HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
	case WM_SIZE:
	{
		RECT ClientRect;
		GetClientRect(Window, &ClientRect);
		int Width = ClientRect.right - ClientRect.left;
		int Height = ClientRect.bottom - ClientRect.top;
		Win32ResizeDeviceIndependentBitmapSection(Width, Height);
	} break;

	case WM_CLOSE:
	{
		// TODO(casey): Handle this with a message to the user?
		Running = false;
	} break;

	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	} break;

	case WM_DESTROY:
	{
		// TODO(casey): Handle this as an error - recreate window?
		Running = false;
	} break;

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		int X = Paint.rcPaint.left;
		int Y = Paint.rcPaint.top;
		int Width = Paint.rcPaint.right - Paint.rcPaint.left;
		int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

		RECT ClientRect;
		GetClientRect(Window, &ClientRect);

		Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
		EndPaint(Window, &Paint);
	} break;

	default:
	{
		//            OutputDebugStringA("default\n");
		Result = DefWindowProc(Window, Message, WParam, LParam);
	} break;
	}

	return(Result);
}



int CALLBACK WinMain(
	 HINSTANCE Instance,
	 HINSTANCE PrevInstance,
	 LPSTR     lpCmdLine,
	 int       nCmdShow
) {

	//MessageBox(0, L"test", L"test", MB_OK | MB_ICONINFORMATION);

	WNDCLASS WindowClass = {};
	//we are handling messages but you still have to assign a callback function because windows will call it without a message
	WindowClass.lpfnWndProc = MainWindowCallback ;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = L"HandmadeHeroWindowClass";

	//no idea why you have to call Register class
	//but it takes a pointer to the WNDClass you made
	//no sure why this would fail
	if (RegisterClass(&WindowClass))
	{
		//this is what makes the window show up on the screen
		HWND Window = CreateWindowEx(
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
		if (Window)
		{
			Running = true;
			HandleWindowMessages(Window);
		}
	}
}

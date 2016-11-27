#include <Windows.h>;

void foo(void)
{
	//debug in watch window Foo, 32 to see the entire array of number per character
	//if you did Foo, 100 in watch you could see that it is showing you array entries beyond the scope of the sentence
	//  you can see the values in other parts of memory, cool! hacking
	char *Foo = "this si the first thing we have printed \n";
	OutputDebugStringA("this si the first thing we have printed \n");
}

struct projectile
{
	char unsigned IsThisOnFile;
	int unsigned Damage;
	int unsigned HitPoints;
	short unsigned Stuff;
};

void x_WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {
	char Smalls;
	char unsigned SmallU;
	//go to disassembly to see the op codes
	char unsigned Test;
	/*
	//0x006ffe5b, does this get the same address every time? NOPE. 
	this is a security feature if you put the memory in the same place everytime it could get hacked
	so a different random address in memory is choosen to start the stack every time
	*/
	char unsigned *TestPointer;

	TestPointer = &Test;

	Test = 255;
	Test = Test + 1;

	INT64 TestInteger = MAXINT64;
	INT64 *TestIntegerPointer = &TestInteger;
	unsigned short TestShort;

	TestShort = 500;
	/* 
	if you look at the location of the variable in the memory window you will see something similiar to
	0x007AF724  244   1 204 204 204 204 204 204 204 204 204 204  60 247 122   0 204 204 204 204 204 204 204 204 255 255 255 255 255 255 255 127 204 204 204 204  
	0x007AF748  204 204 204 204  91 247 122   0 204 204 204 204 204 204 204 204 204 204 204   0 204 204 204 204 204 204 204 204 204 204 204 204 204 204 204 204
	
	if you subtract those 2 hex values then it will equal the number of bytes in a row which in this case is 36
	the number of bytes columns is just dependent on the size of the memory window, its not important

	this will create values in the byte blocks at whatever memory address =  244 1 ( in decimal display)
	this is little endian format
	normally you would read it as 00000001  ‭11110100‬
	but little endian reverses that visually
	so its
	11110100 00000001
	11110100 = 244
	000000001 = 256
	244 + 256  =500

	there are 2 bytes used and 34 bytes not used for this memory address
	the point of using a typed variable like short is to know how many of the bytes to read from the starting memory address
	if you kept reading then it would not be the correct value that was put in there, 500 in this example

	*/


	/*
	this is demonstrating how structures are saved in memory
	usually the compiler will decide to store everything as 32 bit
	so it will just use however many bits are needed for the fields in order then jump to the next 32 bit space
                 | this is char | this is int   | this is int   | this is short|  
	0x012FF9CC    1 204 204 204   1   0   0   0   1   0   0   0   1   0 204 204 204 204 204 204 204 204 204 204 244   1 204 204 204 204 204 204 204 204 204 204
	              
	*/
	projectile TestStruct;
	TestStruct.IsThisOnFile = 1;
	TestStruct.Damage = 1;
	TestStruct.HitPoints = 1;
	TestStruct.Stuff = 1;

	foo();
}

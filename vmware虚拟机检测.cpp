#include <iostream>
#include<string>
#include<windows.h>
using namespace std;
bool IsInsideVMW()
{
	bool rc = true;
	__try
	{
		__asm
		{
			push edx
			push ecx
				push ebx
				mov eax, 'VMXh'
				mov ebx, 0
				mov ecx, 10
				mov edx, 'VX'
				in eax, dx
				cmp ebx, 'VMXh'
				setz[rc]
				pop ebx
				pop ecx
				pop edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}
	return rc;
}
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {
	bool a;
	a = IsInsideVMW();
	if (a) {
		MessageBox(NULL, TEXT("I am in vmware"), TEXT("虚拟机检测"), MB_OK);
	}
	else {
		MessageBox(NULL, TEXT("I am not in vmware"), TEXT("虚拟机检测"), MB_OK);
		
	}
	return 0;
}
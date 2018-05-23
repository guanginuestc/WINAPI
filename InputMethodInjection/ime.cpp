#include<Windows.h>
#include<imm.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	char buf[200];
	
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		
		MessageBox(NULL, TEXT("attached!"), TEXT("start"),MB_OK);
		GetModuleFileNameA(GetModuleHandleA(0), buf, sizeof(buf));
		MessageBoxA(NULL, buf, "start", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		/*
		break;*/
		MessageBox(NULL, TEXT("detached!"), TEXT("start"), MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}
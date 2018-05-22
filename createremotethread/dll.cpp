#include<Windows.h>

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDll,
	_In_ DWORD fdwReason,
	_In_ LPVOID lpvReserved) {
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(NULL, "I am attached!", "MyDll", MB_OKCANCEL);
		break;
	case DLL_PROCESS_DETACH:
		MessageBoxA(NULL, "I am detached!", "MyDll", MB_OKCANCEL);
		break;
	default:
		break;
	}
}
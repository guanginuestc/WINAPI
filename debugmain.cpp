#include<Windows.h>
bool Beingdebug = true;
bool debugmethod1() {//使用IsDebuggerPresent进行判断，该方法的破解方法为hook掉对应的函数
	return IsDebuggerPresent();
}
bool debugmethod2() {//使用CheckRemoteDebuggerPresent进行判断，该方法的破解方法为hook掉对应的函数
	BOOL result=false;
	if (CheckRemoteDebuggerPresent((HANDLE)0xffffffff, &result)) {//此处应为GetCurrentProcess(),但是-1同样代表当前进程的伪句柄，至少当前是这样的
		return result;
	}
	else {
		MessageBox(NULL, TEXT("Can not use CheckRemoteDebuggerPresent"), TEXT("METHOD2"), MB_OKCANCEL);
		return result;
	}
}
bool debugmethod3() {//查询BeingDebugged标志位，可以通过循环查询进行判断
	bool result = 0;
	__asm {
		mov eax, fs:[30h];//进程的PEB地址存放在fs这个寄存器位置上
		mov al, BYTE PTR[eax + 2];//查询BeingDebugged标志位
		mov result, al;
	}
	return result;
}

//NTSYSAPI NTSTATUS NTAPI NtQueryInformationProcess(
//	IN HANDLE 　　　　　　　　ProcessHandle, 　　　　　　// 进程句柄
//	IN PROCESSINFOCLASS 　　InformationClass, 　　　　 // 信息类型
//	OUT PVOID 　　　　　　　　ProcessInformation, 　　　 // 缓冲指针
//	IN ULONG 　　　　　　 　　ProcessInformationLength, // 以字节为单位的缓冲大小
//	OUT PULONG 　　　　　 　　ReturnLength OPTIONAL     // 写入缓冲的字节数
//	);
bool debugmethod4() {//ntglobal
	int result = 0;
	__asm {
		mov eax, fs:[0x30];
		mov eax, [eax + 0x68];
		mov result, eax;
	}
	return result == 0x70;
}

bool debugmethod5()
{
	return false;
}
long __stdcall callback(_EXCEPTION_POINTERS * excp) {
	excp->ContextRecord->Eip += 1;//让进程恢复执行
	Beingdebug = false;//如果触发异常，则修改标志位，来判定是否在被调试
	return EXCEPTION_CONTINUE_EXECUTION;
}

bool debugmethod6() {
	SetUnhandledExceptionFilter(callback);
	_asm int 3
	return false;
}
bool debugmethod7()
{
	char fib[1024] = { 0 };
	// 会抛出一个异常并被调试器捕获
	DeleteFiber(fib);
	// 0x57的意思是ERROR_INVALID_PARAMETER
	return (GetLastError() == 0x57);
}
int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow
	) {
	
	

	
	if (debugmethod4()) {
		MessageBox(NULL, TEXT("Being Debug!"), TEXT("M3"), MB_OKCANCEL);
	}
	else {
		MessageBox(NULL, TEXT("Not Debug!"), TEXT("M3"), MB_OKCANCEL);
	}
}
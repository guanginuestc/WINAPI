#include<Windows.h>
#include<iostream>
#include<TlHelp32.h>
#include<vector>
using namespace std;
vector<DWORD> GetProcessIDByName(LPCTSTR szProcessName)
{
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	vector<DWORD> dwPID;
	ZeroMemory(&st, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	st.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ps, sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//拍摄进程快照
	if (hSnapshot == INVALID_HANDLE_VALUE)//快照拍摄失败
	{
		return dwPID;
	}


	if (!Process32First(hSnapshot, &ps))
	{
		return dwPID;
	}
	do
	{

		if (lstrcmpi(ps.szExeFile, szProcessName) == 0)//遍历进程快照，比较进程名
		{


			dwPID.push_back(ps.th32ProcessID);//将进程id放到容器中
		}
	} while (Process32Next(hSnapshot, &ps));
	// 没有找到  
	CloseHandle(hSnapshot);
	return dwPID;//返回容器
}

void DebugLoop() {
	DEBUG_EVENT de;
	DWORD dwContinueStatus;
	DWORD arg[] = { 0x01,0x05 };
	while (WaitForDebugEvent(&de, INFINITE)) {//breakpoint
		dwContinueStatus = DBG_CONTINUE;
		if (EXCEPTION_BREAKPOINT == de.u.Exception.ExceptionRecord.ExceptionCode) {
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_ALL;
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, NULL, de.dwThreadId);
			GetThreadContext(hThread, &ctx);
			if (ctx.Eip == 0x004010B0) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, de.dwProcessId);
				DWORD numofwrite;
				cout << arg[0] << "+" << arg[1] << ":" << ctx.Eax << "\tesp"<<ctx.Esp<<endl;
				ctx.Esp -= 2*sizeof(DWORD);
				WriteProcessMemory(hProcess, (LPVOID)ctx.Esp, arg, sizeof(DWORD)*sizeof(arg), &numofwrite);
				ctx.Eip = 0x004010A7;
				SetThreadContext(hThread, &ctx);
				CloseHandle(hProcess);
			}
			CloseHandle(hThread);

		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);
	}
}
int main() {
	WCHAR processname[] = TEXT("testfoo.exe");
	vector<DWORD> processid = GetProcessIDByName(processname);
	if (processid.size() == 0) {
		wprintf(L"process %s not found\n", processname);
		return 0;
	}
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processid[0]);
	if (hProcess == NULL) {
		cout << "open failed!" << endl;
		return 0;
	}

	DWORD sp []= { 0xcc };
	DWORD esp = 0x004010AF;
	DWORD numofwriten;

	if (!WriteProcessMemory(hProcess, (LPVOID)esp, (LPCVOID)sp, sizeof(DWORD) * sizeof(sp), &numofwriten)) {
		cout << "write memory failed!" << endl;
	}
	CloseHandle(hProcess);
	if (!DebugActiveProcess(processid[0])) {
		cout << "debug failed!" << endl;
	}
	else {
		wprintf(L"begin debug %s\n", processname);
	}

	DebugLoop();
	if (!DebugActiveProcessStop(processid[0])) {
		cout << "stop debug failed!" << endl;
	}
	else {
		cout << "stop debug!" << endl;
	}
	return 1;
}
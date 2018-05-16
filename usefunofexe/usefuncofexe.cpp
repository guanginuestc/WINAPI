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
bool RemoveBP(vector<DWORD> processid, BYTE &torecovery) {
	return TRUE;
}
bool SetByte(vector<DWORD> processid, BYTE &org,BYTE &BP,DWORD addr) {
	/*
	修改对应进程的某位字符，将addr地址的字节拷贝到org中，并修改为BP中的字节
	*/

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processid[0]);
	if (hProcess == NULL) {
		cout << "open failed!" << endl;
		return FALSE;
	}

	
	DWORD numofwriten;

	if (!ReadProcessMemory(hProcess, (LPVOID)addr, (LPVOID)&org, sizeof(org), &numofwriten)) {
		cout << "read failed!" << endl;
		CloseHandle(hProcess);
		return FALSE;

	}
	if (!WriteProcessMemory(hProcess, (LPVOID)addr, (LPCVOID)&BP, 1, &numofwriten)) {//将断点写入到程序中。
		cout << "write memory failed!" << endl;
		CloseHandle(hProcess);
		return FALSE;
	}
	CloseHandle(hProcess);
	return TRUE;
}
void DebugLoop(vector<DWORD> processid) {
	DEBUG_EVENT de;
	DWORD dwContinueStatus;
	DWORD arg[] = { 0x01,0x05 };
	BYTE torecovery = 0x00, BP = 0xCC;
	DWORD addr = 0x004010AF;
	if (!SetByte(processid, torecovery, BP, addr)) {
		cout << "设置断点失败" << endl;
		return;
	}
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
				cout << arg[0] << "+" << arg[1] << ":" << ctx.Eax << "\tesp" << ctx.Esp << endl;
				printf("input new number:");
				scanf("%d %d", &arg[0], &arg[1]);
				if (!(arg[0]&& arg[1])) {
					ctx.Eip--;
					SetThreadContext(hThread, &ctx);
					SetByte(processid, BP, torecovery, addr);
					CloseHandle(hThread);
					CloseHandle(hProcess);
					ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);
					return;
				}
				ctx.Esp -= 2 * sizeof(DWORD);
				WriteProcessMemory(hProcess, (LPVOID)ctx.Esp, arg, sizeof(DWORD)*sizeof(arg), &numofwrite);
				ctx.Eip = 0x004010A7;
				SetThreadContext(hThread, &ctx);
				
				CloseHandle(hThread);
				CloseHandle(hProcess);
				
			}
			else {
				CloseHandle(hThread);
			}

		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);
	}
}
int main() {
	WCHAR processname[] = TEXT("testfoo.exe");//调用的进程名称
	vector<DWORD> processid = GetProcessIDByName(processname);
	if (processid.size() == 0) {
		wprintf(L"process %s not found\n", processname);
		return FALSE;
	}
	
	
	
	if (!DebugActiveProcess(processid[0])) {//调试打开进程
		cout << "debug failed!" << endl;
	}
	else {
		wprintf(L"begin debug %s\n", processname);
	}

	DebugLoop(processid);
	
	if (!DebugActiveProcessStop(processid[0])) {
		cout << "stop debug failed!" << endl;
	}
	else {
		cout << "stop debug!" << endl;
	}
	return 1;
}
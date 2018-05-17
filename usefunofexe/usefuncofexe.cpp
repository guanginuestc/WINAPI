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
void DebugLoop(vector<DWORD> processid,DWORD BP_position,DWORD LoopStart) {
	/*
	循环调用函数主体
	processid：对应关心进程的id
	BP_position：函数执行结束平衡堆栈后的第一条语句地址
	LoopStart：函数执行的地址
	*/
	DEBUG_EVENT de;
	DWORD dwContinueStatus;
	DWORD arg[] = { 0x01,0x05 };//参数列表
	BYTE torecovery = 0x00, BP = 0xCC;
	DWORD addr = BP_position;
	if (!SetByte(processid, torecovery, BP, addr)) {//设置断点
		cout << "设置断点失败" << endl;
		return;
	}
	while (WaitForDebugEvent(&de, INFINITE)) {//breakpoint
		
		dwContinueStatus = DBG_CONTINUE;
		if (EXCEPTION_BREAKPOINT == de.u.Exception.ExceptionRecord.ExceptionCode) {//检测程序当前是否为Int3异常
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_ALL;
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, NULL, de.dwThreadId);
			GetThreadContext(hThread, &ctx);
			if (ctx.Eip == BP_position+1) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, de.dwProcessId);
				DWORD numofwrite;
				cout << arg[0] << "+" << arg[1] << ":"<< ctx.Eax <<std::oct<< "\tesp" << std::hex << ctx.Esp << endl;
				printf("input new number:");
				scanf("%d %d", &arg[0], &arg[1]);
				if (!(arg[0]&& arg[1])) {//如果接收到参数均为0，则调试器进行退出和复位程序
					ctx.Eip--;//将eip指向正确的位置
					SetThreadContext(hThread, &ctx);//修改线程上下文，让程序恢复正常
					SetByte(processid, BP, torecovery, addr);//删除下的断点
					CloseHandle(hThread);
					CloseHandle(hProcess);
					ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus);
					return;
				}
				ctx.Esp -= sizeof(arg);
				WriteProcessMemory(hProcess, (LPVOID)ctx.Esp, arg, sizeof(DWORD)*sizeof(arg), &numofwrite);//将参数写入到进行堆栈中
				ctx.Eip = LoopStart;
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
	DWORD BP_position = 0x004010AF;
	DWORD LoopStart = 0x004010A7;
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

	DebugLoop(processid, BP_position,LoopStart);//循环调用对应的函数
	
	if (!DebugActiveProcessStop(processid[0])) {//停止调试进程
		cout << "stop debug failed!" << endl;
	}
	else {
		cout << "stop debug!" << endl;
	}
	return 1;
}
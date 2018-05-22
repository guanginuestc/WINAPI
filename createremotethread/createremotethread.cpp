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
void main() {
	cout << "start!" << endl;
	WCHAR den[] = L"testfoo.exe";//目标进程的名称
	vector<DWORD> hProcessId = GetProcessIDByName(den);//获取进程id
	if (hProcessId.size() == 0) {
		cout << "process not found!" << endl;
		return;
	}
	char dll[] = "D:\\Program Files (x86)\\workspace\\createdll\\Debug\\createdll.dll";//目标dll的名称

	HMODULE ker = GetModuleHandleA("kernel32.dll");//得到kernel32的模块句柄，因为loadlibrary在该dll中
	FARPROC loadlibrary = GetProcAddress(ker, "LoadLibraryA");//此处应该使用LoadLibraryA或LoadLibraryW，不能直接使用LoadLibrary因为LoadLibrary是一个宏，在代码运行时不能直接使用
	
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, hProcessId[0]);//打开进程，获得进程句柄
	if (hProcess == NULL) {
		cout << "open failed!" << endl;
		return;
	}
	LPVOID lpparameter = VirtualAllocEx(hProcess, NULL, sizeof(dll), MEM_COMMIT, PAGE_EXECUTE_READWRITE);//在目标进程中分配空间，用于存储要加载的dll的名称
	WriteProcessMemory(hProcess, lpparameter, dll, sizeof(dll), NULL);//将目标dll的名字写入到目标进程
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)loadlibrary, lpparameter, 0, NULL);//创建远程线程
	WaitForSingleObject(hThread, INFINITE);//等待远程线程运行结束
	
	CloseHandle(hThread);
	CloseHandle(hProcess);
	cout << "注入成功" << endl;
	getchar();
}
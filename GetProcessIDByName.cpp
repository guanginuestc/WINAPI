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
			 
			
			dwPID.push_back(ps.th32ParentProcessID);//将进程id放到容器中
		}
	} while (Process32Next(hSnapshot, &ps));
	// 没有找到  
	CloseHandle(hSnapshot);
	return dwPID;//返回容器
}
void main() {
	TCHAR name[] = TEXT("notepad.exe");
	vector<DWORD> idlist;
	idlist = GetProcessIDByName(name);
	if (idlist.size()) {//若是找到
		cout << "编号\t进程对应id(十六进制)\t进程对应id(十进制)" << endl;
		for (vector<DWORD>::size_type it =0; it<idlist.size(); it++) {
			cout << it << "\t" << hex<<idlist[it] <<"\t\t\t"<<dec<<idlist[it]<<endl;
		}
	}
	else {//没找到
		cout << "未找到对应进程" << endl;
	}
}
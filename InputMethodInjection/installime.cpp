#include<Windows.h>
#include<imm.h>
#include<iostream>
HKL m_hImeFile32;
DWORD m_retV;

void UnistallIME() {
	::SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &m_retV, SPIF_SENDWININICHANGE);//还原默认输入法
																					   //卸载输入法
	if (UnloadKeyboardLayout(m_hImeFile32)) {
		MessageBox(NULL, L"UnInstallIME", L"tit", MB_OK);
	}
}
void InstallIME(int a) {
	//获取当前输入法

	::SystemParametersInfo(
		SPI_GETDEFAULTINPUTLANG, 0, &m_retV, 0
		);
	if(a==1)
		m_hImeFile32 = ImmInstallIMEW(TEXT("Ime_A.ime"), TEXT("我的输入法"));//输入法安装
	else if(a==2)
		m_hImeFile32 = ImmInstallIMEW(TEXT("Ime_B.ime"), TEXT("我的输入法"));//输入法安装
	if (ImmIsIME(m_hImeFile32)) {
		SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &m_hImeFile32, SPIF_SENDWININICHANGE);//设置默认输入法
		MessageBox(NULL, L"InstallIME", L"my", MB_OK);
	}
}
using namespace std;
void main() {
	int a;
	while (true) {
		cout << "输入以下选项\n1.安装输入法1\n2.卸载输入法1\n3.安装输入法2\n4.卸载输入法2\n5.退出\n请输入:" << endl;
		cin >> a;
		switch (a)
		{
		case 1:
			InstallIME(1);

			break;
		case 2:
			UnistallIME();
			break;
		case 3:
			InstallIME(2);

			break;
		case 4:
			UnistallIME();
			break;
		case 5:
			return;
		default:
			break;
		}
		getchar();
	}
}
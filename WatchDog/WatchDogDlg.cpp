
// WatchDogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WatchDog.h"
#include "WatchDogDlg.h"
#include "afxdialogex.h"
#include <string>
#include <shellapi.h>
#include "tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIME_CHECK    1
#define  WM_SHOWTASK  WM_USER+10

//��stringת����wstring  
std::wstring string2wstring(std::string str)
{
	std::wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
									//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

//��wstringת����string  
std::string wstring2string(std::wstring wstr)
{
	std::string result;
	//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//���ֽڱ���ת���ɶ��ֽڱ���  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWatchDogDlg �Ի���



CWatchDogDlg::CWatchDogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WATCHDOG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWatchDogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPINFO_LIST, m_ListInfo);
}

BEGIN_MESSAGE_MAP(CWatchDogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_AUTOBOOT_CHK, &CWatchDogDlg::OnBnClickedAutobootChk)
	ON_BN_CLICKED(IDC_ISRESTARTOS_CHK, &CWatchDogDlg::OnBnClickedIsrestartosChk)
END_MESSAGE_MAP()


// CWatchDogDlg ��Ϣ�������

BOOL CWatchDogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CRect rectClient;
	m_ListInfo.GetClientRect(&rectClient);
	m_ListInfo.SetExtendedStyle(m_ListInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE);
	int nShare = int(rectClient.Width() / 9);
	//mStr.Format("���%d",(i+1));
	
	m_ListInfo.InsertColumn(0, _T("����س���"), LVCFMT_LEFT, nShare * 8);
	m_ListInfo.InsertColumn(1, _T("�ӳ�ʱ��"), LVCFMT_LEFT, nShare * 1);
	//���õ�ǰ·��

	char* currentPathBuffer=new char[MAX_PATH];
	m_ini.GetCurrentPath(currentPathBuffer);
	CString currentPath(currentPathBuffer);
	currentPath += "/WatchDog.ini";
	m_ini.SetFilePath(currentPath);
	delete[] currentPathBuffer;
	currentPathBuffer = nullptr;
	StartSoftNum= m_ini.GetInt(_T("APPS"), _T("AppNum"), NOPARITY);

	int delayTime = 0;
	for (int i = 0; i < StartSoftNum; i++)
	{
		CString app;
		std::wstring appName = _T("App") + std::to_wstring(i + 1);
		m_ini.GetString(_T("APPS"), appName.c_str(), _T(""), app.GetBuffer(MAX_PATH), MAX_PATH);
		app.ReleaseBuffer();

		std::wstring delayTimeName = _T("StartDelayTime") + std::to_wstring(i + 1);
		delayTime = m_ini.GetInt(_T("APPS"), delayTimeName.c_str(), NOPARITY);
		m_ListInfo.InsertItem(i, app);
		m_ListInfo.SetItemText(i, 1, std::to_wstring(delayTime).c_str());
	}
		
	IsRestartOS= m_ini.GetInt(_T("APPS"), _T("IsRestartOS"), NOPARITY);
	((CButton*)GetDlgItem(IDC_ISRESTARTOS_CHK))->SetCheck(IsRestartOS);
	
	m_ini.GetString(_T("APPS"), _T("RestarOSTime"), _T(""), RestartOSTime.GetBuffer(MAX_PATH), MAX_PATH);
	RestartOSTime.ReleaseBuffer();

	SetDlgItemTextW(IDC_RESTARTOS_EDIT, RestartOSTime);
	/*User32HDll = LoadLibrary(_T("user32.dll"));
	IsHungAppWindow = (PROCISHUNGAPPWINDOW)GetProcAddress(User32HDll, "IsHungAppWindow");*/
	HKEY hKey;
	//�ҵ�ϵͳ��������
	CString lpRun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//��������
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	
	//�ж��Ƿ��Ѿ����ÿ�������
	TCHAR PowerBoot[MAX_PATH] = {};
	DWORD nLongth = MAX_PATH;
	long result = RegGetValue(hKey, NULL, _T("WatchDog"), RRF_RT_REG_SZ, 0, PowerBoot, &nLongth);
	if (result == ERROR_SUCCESS)
	{
		((CButton*)GetDlgItem(IDC_AUTOBOOT_CHK))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_AUTOBOOT_CHK))->SetCheck(0);
	}
	RegCloseKey(hKey);
	SetTimer(TIME_CHECK, 5000, NULL);
	ToTray();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CWatchDogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWatchDogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWatchDogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWatchDogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (TIME_CHECK == nIDEvent)
	{
		if (!CheckExeStatus())
		{
			ReStartSoft();
		}
		/*for (auto iter = this->handls.begin(); iter != this->handls.end(); ++iter)
		{
			if (IsHungAppWindow(HWND(*iter)))
			{
				ReStartOS();
			}
		}*/
		if (IsRestartOS)
		{
			CTime currentTime = CTime::GetCurrentTime();
			int hour, min, sec;
			sscanf_s(wstring2string(RestartOSTime.GetBuffer()).c_str(), "%d:%d:%d", &hour, &min, &sec);
			RestartOSTime.ReleaseBuffer();

			if (hour == currentTime.GetHour() && min == currentTime.GetMinute()
				&& abs(sec - currentTime.GetSecond()) < 10)
			{
				ReStartOS();
			}
		}	
	}
	CDialogEx::OnTimer(nIDEvent);
}


// ������״̬
bool CWatchDogDlg::CheckExeStatus()
{
	for (int i = 0; i < StartSoftNum; i++)
	{
		CString app;
		std::wstring appName = _T("App") + std::to_wstring(i + 1);
		m_ini.GetString(_T("APPS"), appName.c_str(), _T(""), app.GetBuffer(MAX_PATH), MAX_PATH);
		app.ReleaseBuffer();
		CString szExeName =app.Right(app.GetLength()-1-app.ReverseFind('\\'));

		BOOL BRunning = FALSE;
		PROCESSENTRY32 processEntry32;
		HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (((int)toolHelp32Snapshot) != -1)
		{
			processEntry32.dwSize = sizeof(processEntry32);
			if (Process32First(toolHelp32Snapshot, &processEntry32))
			{
				do
				{
					CString str(L"");
					str = processEntry32.szExeFile;
					if (str == szExeName)
					{
						//������������
						BRunning = TRUE;
						break;
					}
				}

				while (Process32Next(toolHelp32Snapshot, &processEntry32));
			}
			CloseHandle(toolHelp32Snapshot);
		}
		if (!BRunning)
		{
			return false;
		}
	}
	return true;
}

bool CWatchDogDlg::ReStartSoft()
{
	bool exRe = true;
	CString app;
	int delayTime = 0;
	for (auto iter = this->handls.begin(); iter != this->handls.end(); ++iter)
	{
		TerminateProcess(*iter, 0);
	}
	this->handls.clear();
	for (int i = 0; i < StartSoftNum; i++)
	{
		std::wstring appName = _T("App") + std::to_wstring(i + 1);
		m_ini.GetString(_T("APPS"), appName.c_str(), _T(""), app.GetBuffer(MAX_PATH), MAX_PATH);
		app.ReleaseBuffer();

		std::wstring delayTimeName=_T("StartDelayTime")+ std::to_wstring(i + 1);
		delayTime=m_ini.GetInt(_T("APPS"), delayTimeName.c_str(), NOPARITY);
		Sleep(delayTime);
		

		STARTUPINFO            StartInfo;
		PROCESS_INFORMATION    procStruct;
		memset(&StartInfo, 0, sizeof(STARTUPINFO));
		StartInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartInfo.wShowWindow = SW_SHOW;
		StartInfo.cb = sizeof(STARTUPINFO);

		CreateProcess(
			(LPCTSTR)app,
			NULL,
			NULL,
			NULL,
			FALSE,
			NORMAL_PRIORITY_CLASS,
			NULL,
			NULL,
			&StartInfo,
			&procStruct);

		this->handls.push_back(procStruct.hProcess);
		/*CloseHandle(procStruct.hProcess);
		CloseHandle(procStruct.hThread);*/

		/*HINSTANCE rs = ShellExecute(NULL, _T("open"), app.GetBuffer(0), NULL, NULL, SW_SHOWNORMAL);
		app.ReleaseBuffer();
		if (int(rs) < 32)
		{
			exRe = false;
			break;
		}*/
	}

	return exRe;
}

void CWatchDogDlg::ReStartOS()
{
	HANDLE   hToken;
	TOKEN_PRIVILEGES   tkp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	//"�Զ��ػ�"   ExitWindowsEx(EWX_POWEROFF, 0);
	ExitWindowsEx(EWX_REBOOT, 0);
	//"�Զ�ע��"   ExitWindowsEx(EWX_LOGOFF, 0);
}

void CWatchDogDlg::ToTray(void)
{
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uID = IDR_MAINFRAME;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_SHOWTASK;
	//�Զ������Ϣ���� WM_SHOWTASK ͷ�����ж���ΪWM_USER+10
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	StrCpyW(m_nid.szTip, L"������м��");//������������ʱ������ʾ������
	Shell_NotifyIcon(NIM_ADD, &m_nid);//�����������ͼ��
}


LRESULT CWatchDogDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message) //�ж���Ϣ����
	{
	case WM_SHOWTASK:
		//������û��������Ϣ 
		if (lParam == WM_LBUTTONDBLCLK)

		{
			//���˫��ʱ�����ڳ��� 
			if (AfxGetApp()->m_pMainWnd->IsWindowVisible()) //�жϴ��ڵ�ǰ״̬
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE); //���ش���
			}
			else
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL); //��ʾ����
			}

		}
		else if (lParam == WM_RBUTTONUP)
		{ //����Ҽ���������ѡ�� 
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);//�õ����λ��
			CMenu menu;
			menu.CreatePopupMenu();//����һ������ʽ�˵�
								   //���Ӳ˵���رա������������ϢWM_DESTROY�������ڣ���
								   //���أ��������������
			menu.AppendMenu(MF_STRING, WM_DESTROY, L"�˳�");
			//ȷ������ʽ�˵���λ��
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
			//��Դ����
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
	case WM_SYSCOMMAND:
		//�����ϵͳ��Ϣ 
		if (wParam == SC_MINIMIZE)
		{
			//���յ���С����Ϣʱ���������� 
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
			return 0;
		}
		if (wParam == SC_CLOSE)
		{
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
			return 0;
			//::Shell_NotifyIcon(NIM_DELETE, &m_notify); //�ر�ʱɾ��ϵͳ����ͼ��
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CWatchDogDlg::OnBnClickedAutobootChk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//д��ע���,���������� 
	HKEY hKey;
	//�ҵ�ϵͳ�������� 
	LPCTSTR lpRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	//long lRet =RegOpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
	//��������Key 
	//long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		MessageBox(_T("����ʧ��!"));
		return;
	}
	CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOBOOT_CHK);
	TCHAR pFileName[MAX_PATH] = { 0 };
	//�õ����������ȫ·�� 
	DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
	if (pBtn->GetCheck())
	{
		lRet = RegSetValueEx(hKey, _T("WatchDog"), 0, REG_SZ, (BYTE *)pFileName, (lstrlen(pFileName) + 1)*sizeof(TCHAR));
		
		//�ر�ע��� 
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			MessageBox(_T("ϵͳ��������,������ϵͳ����!"));
			return;
		}
	}
	else
	{
		lRet = RegDeleteKey(hKey, _T("WatchDog"));
		//lRet=RegDeleteKeyEx(hKey, _T("WatchDog"), KEY_WOW64_64KEY, 0);
		//�ر�ע��� 
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			MessageBox(_T("ϵͳ��������,���ܹر���ϵͳ��������!"));
		}
	}
}


void CWatchDogDlg::OnBnClickedIsrestartosChk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

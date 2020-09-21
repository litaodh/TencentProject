
// WatchDogDlg.cpp : 实现文件
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

//将string转换成wstring  
std::wstring string2wstring(std::string str)
{
	std::wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
									//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

//将wstring转换成string  
std::string wstring2string(std::wstring wstr)
{
	std::string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CWatchDogDlg 对话框



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


// CWatchDogDlg 消息处理程序

BOOL CWatchDogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rectClient;
	m_ListInfo.GetClientRect(&rectClient);
	m_ListInfo.SetExtendedStyle(m_ListInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE);
	int nShare = int(rectClient.Width() / 9);
	//mStr.Format("相机%d",(i+1));
	
	m_ListInfo.InsertColumn(0, _T("被监控程序"), LVCFMT_LEFT, nShare * 8);
	m_ListInfo.InsertColumn(1, _T("延迟时间"), LVCFMT_LEFT, nShare * 1);
	//设置当前路径

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
	//找到系统的启动项
	CString lpRun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//打开启动项
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	
	//判断是否已经设置开机启动
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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWatchDogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWatchDogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWatchDogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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


// 检测软件状态
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
						//程序正在运行
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
	//"自动关机"   ExitWindowsEx(EWX_POWEROFF, 0);
	ExitWindowsEx(EWX_REBOOT, 0);
	//"自动注销"   ExitWindowsEx(EWX_LOGOFF, 0);
}

void CWatchDogDlg::ToTray(void)
{
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uID = IDR_MAINFRAME;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_SHOWTASK;
	//自定义的消息名称 WM_SHOWTASK 头函数中定义为WM_USER+10
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	StrCpyW(m_nid.szTip, L"软件运行监控");//当鼠标放在上面时，所显示的内容
	Shell_NotifyIcon(NIM_ADD, &m_nid);//在托盘区添加图标
}


LRESULT CWatchDogDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message) //判断消息类型
	{
	case WM_SHOWTASK:
		//如果是用户定义的消息 
		if (lParam == WM_LBUTTONDBLCLK)

		{
			//鼠标双击时主窗口出现 
			if (AfxGetApp()->m_pMainWnd->IsWindowVisible()) //判断窗口当前状态
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE); //隐藏窗口
			}
			else
			{
				AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL); //显示窗口
			}

		}
		else if (lParam == WM_RBUTTONUP)
		{ //鼠标右键单击弹出选单 
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);//得到鼠标位置
			CMenu menu;
			menu.CreatePopupMenu();//声明一个弹出式菜单
								   //增加菜单项“关闭”，点击则发送消息WM_DESTROY给主窗口（已
								   //隐藏），将程序结束。
			menu.AppendMenu(MF_STRING, WM_DESTROY, L"退出");
			//确定弹出式菜单的位置
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
			//资源回收
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
	case WM_SYSCOMMAND:
		//如果是系统消息 
		if (wParam == SC_MINIMIZE)
		{
			//接收到最小化消息时主窗口隐藏 
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
			return 0;
		}
		if (wParam == SC_CLOSE)
		{
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
			return 0;
			//::Shell_NotifyIcon(NIM_DELETE, &m_notify); //关闭时删除系统托盘图标
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CWatchDogDlg::OnBnClickedAutobootChk()
{
	// TODO: 在此添加控件通知处理程序代码
	//写入注册表,开机自启动 
	HKEY hKey;
	//找到系统的启动项 
	LPCTSTR lpRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	//long lRet =RegOpenKey(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
	//打开启动项Key 
	//long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
	{
		MessageBox(_T("设置失败!"));
		return;
	}
	CButton* pBtn = (CButton*)GetDlgItem(IDC_AUTOBOOT_CHK);
	TCHAR pFileName[MAX_PATH] = { 0 };
	//得到程序自身的全路径 
	DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
	if (pBtn->GetCheck())
	{
		lRet = RegSetValueEx(hKey, _T("WatchDog"), 0, REG_SZ, (BYTE *)pFileName, (lstrlen(pFileName) + 1)*sizeof(TCHAR));
		
		//关闭注册表 
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			MessageBox(_T("系统参数错误,不能随系统启动!"));
			return;
		}
	}
	else
	{
		lRet = RegDeleteKey(hKey, _T("WatchDog"));
		//lRet=RegDeleteKeyEx(hKey, _T("WatchDog"), KEY_WOW64_64KEY, 0);
		//关闭注册表 
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			MessageBox(_T("系统参数错误,不能关闭随系统启动功能!"));
		}
	}
}


void CWatchDogDlg::OnBnClickedIsrestartosChk()
{
	// TODO: 在此添加控件通知处理程序代码
}

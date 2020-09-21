
// WatchDogDlg.h : 头文件
//

#pragma once
#include "Ini.h"
#include "afxcmn.h"
#include <vector>
//typedef BOOL(WINAPI *PROCISHUNGAPPWINDOW)(HWND);
// CWatchDogDlg 对话框
class CWatchDogDlg : public CDialogEx
{
// 构造
public:
	CWatchDogDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WATCHDOG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	/*HINSTANCE User32HDll;
	PROCISHUNGAPPWINDOW IsHungAppWindow;*/
	NOTIFYICONDATA m_nid;
	void ToTray(void);	
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	std::vector<HANDLE> handls;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// 检测软件状态
	bool CheckExeStatus();
	bool ReStartSoft();
	void ReStartOS();
	CIni m_ini;
	int StartSoftNum;
	CListCtrl m_ListInfo;
	afx_msg void OnBnClickedAutobootChk();
	bool IsRestartOS;
	CString RestartOSTime;
	afx_msg void OnBnClickedIsrestartosChk();
};

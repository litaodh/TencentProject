
// WatchDogDlg.h : ͷ�ļ�
//

#pragma once
#include "Ini.h"
#include "afxcmn.h"
#include <vector>
//typedef BOOL(WINAPI *PROCISHUNGAPPWINDOW)(HWND);
// CWatchDogDlg �Ի���
class CWatchDogDlg : public CDialogEx
{
// ����
public:
	CWatchDogDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WATCHDOG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	/*HINSTANCE User32HDll;
	PROCISHUNGAPPWINDOW IsHungAppWindow;*/
	NOTIFYICONDATA m_nid;
	void ToTray(void);	
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	std::vector<HANDLE> handls;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// ������״̬
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

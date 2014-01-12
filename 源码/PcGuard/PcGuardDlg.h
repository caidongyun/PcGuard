// PcGuardDlg.h : header file
//

#if !defined(AFX_PCGUARDDLG_H__D2E7D285_AF26_49BC_88AF_15F219AC8B1B__INCLUDED_)
#define AFX_PCGUARDDLG_H__D2E7D285_AF26_49BC_88AF_15F219AC8B1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WM_TRAYICON_MSG WM_USER+11
/////////////////////////////////////////////////////////////////////////////
// CPcGuardDlg dialog

class CPcGuardDlg : public CDialog
{
// Construction
public:
	int m_nScrollXPos;
	CString m_sText;
	//���ڱ��泬���Ӿ�̬�ı������Ļ���� 
	RECT m_pRectLink;
	// ��������
	LRESULT OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam);
	NOTIFYICONDATA m_Ntnd;//���������йؽṹ
	// ���� Ĭ������
	bool TrayMyIcon(bool bAdd=true);
	CPcGuardDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPcGuardDlg)
	enum { IDD = IDD_PCGUARD_DIALOG };
	CButton	m_BtKill; // ɱ����ť
	CButton	m_BtReg;// ע����ذ�ť
	CButton	m_BtFile;// �ļ���ذ�ť
	CButton	m_BtPro;// ���̹���ť
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcGuardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPcGuardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	// ����ť�¼�
	afx_msg void OnButtonPro(); 
	afx_msg void OnButtonKill();
	afx_msg void OnButtonReg();
	afx_msg void OnButtonFile();
	// ��ʱ���¼�
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	// �������̲˵���ʾ��Ϣ
	afx_msg void OnMenuShow();
	afx_msg void OnMenuQuit();
	// �����Ϣ
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// ����󵥻���Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCGUARDDLG_H__D2E7D285_AF26_49BC_88AF_15F219AC8B1B__INCLUDED_)

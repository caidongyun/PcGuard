// FileMonitorDlg.h : header file
//

#if !defined(AFX_FILEMONITORDLG_H__0D2E531E_1010_4904_AFF4_B827CA66DD0C__INCLUDED_)
#define AFX_FILEMONITORDLG_H__0D2E531E_1010_4904_AFF4_B827CA66DD0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CFileMonitorDlg dialog
class CFileMonitorDlg : public CDialog
{
// Construction
public:
	CFileMonitorDlg(CWnd* pParent = NULL);	// standard constructor
	~CFileMonitorDlg();
// Dialog Data
	//{{AFX_DATA(CFileMonitorDlg)
	enum { IDD = IDD_FILEMONITOR_DIALOG };
	CButton	m_BtSet;
	CListCtrl	m_CListCtrl;
	CString	m_strPathName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileMonitorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    HICON m_hIconSet;//���ð�ťͼ����
	// Generated message map functions
	//{{AFX_MSG(CFileMonitorDlg)
	virtual BOOL OnInitDialog();//��ʼ��
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();//��ʼ����
	afx_msg void OnButtonStop();//ֹͣ����
	afx_msg void OnButtonClear();//����б��¼
	afx_msg void OnButtonSave();//������Ӽ�¼
	afx_msg void OnButtonExport();//�������Ӽ�¼
	afx_msg void OnBtnBrowse();//�������
	afx_msg void OnButtonSet();//����
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);//�Ҽ���Ϣ����
	afx_msg void OnOpen();//�Ҽ��˵� ����Ϣ����
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
	HMODULE m_Hmodule;//Ƥ��ģ����
		//���³�ԱΪ�ж��Ƿ�������
	BOOL	m_bAddNew;//����
	BOOL	m_bDel;//ɾ��
	BOOL	m_bRename;//������
	BOOL	m_bAttribute;//�����޸�
	BOOL    m_bOccurTime;//����ʱ��
	BOOL	m_bOther;//����

	void InitDlgItem();//��ʼ���ؼ�
	//�̺߳�������������
	static DWORD WINAPI ThreadMonitorProc(LPVOID lParam);
	CFont		m_hFont;
	HANDLE m_hThread; // �߳̾��
	HANDLE m_hDir; // �洢ѡ���Ŀ¼���
	int m_nNumber;  // �仯�ļ����ļ��е����
	char* m_cNumber; // �仯�ļ����ļ��е����

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEMONITORDLG_H__0D2E531E_1010_4904_AFF4_B827CA66DD0C__INCLUDED_)

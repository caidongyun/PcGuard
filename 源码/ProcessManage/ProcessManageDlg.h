// ProcessManageDlg.h : header file
//

#if !defined(AFX_PROCESSMANAGEDLG_H__A3F29421_DE79_4D52_816F_5D080647CB10__INCLUDED_)
#define AFX_PROCESSMANAGEDLG_H__A3F29421_DE79_4D52_816F_5D080647CB10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxcmn.h>
#include <Tlhelp32.h>//ϵͳ����
#include <psapi.h>//PSAPI
#pragma comment (lib, "psapi.lib")
/////////////////////////////////////////////////////////////////////////////
// CProcessManageDlg dialog

class CProcessManageDlg : public CDialog
{
// Construction
public:
	CProcessManageDlg(CWnd* pParent = NULL);	// standard constructor
    ~CProcessManageDlg();
// Dialog Data
	//{{AFX_DATA(CProcessManageDlg)
	enum { IDD = IDD_PROCESSMANAGE_DIALOG };
	CListCtrl	m_ListDll;//ģ����Ϣ�б�ؼ���������
	CListCtrl	m_ListProcess;//�����б�ؼ���������
	CString	m_nDll;//������ģ��������̬ �ı��ؼ� ��������
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessManageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProcessManageDlg)
	virtual BOOL OnInitDialog();//��ʼ��
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//��ʱ�����ڶ�ʱ����ϵͳ�ڴ�ʹ����
	afx_msg void OnTimer(UINT nIDEvent);
	//���������б���ĳһ����Ϣ����(LVN_COLUMNCLICK)
	afx_msg void OnColumnclickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	//����(��ʾ����ģ����Ϣ)��ť��Ϣ����
	afx_msg void OnButtonShow();
	//����ģ���б���ĳһ����Ϣ����
	afx_msg void OnColumnclickListDll(NMHDR* pNMHDR, LRESULT* pResult);
	//���������б�ĳһ����Ϣ����
	afx_msg void OnClickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	//�Ҽ������˵�
	afx_msg void OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	//���� ˢ�²˵�����Ϣ
	afx_msg void OnRefresh();
	//���� ��ֹ���̲˵�����Ϣ
	afx_msg void OnTerminate();
	//���� ��ͣ���̲˵�����Ϣ
	afx_msg void OnSuspend();
	//���� �ָ����̲˵�����Ϣ
	afx_msg void OnResume();
	//���� �ļ����� �˵�����Ϣ
	afx_msg void OnFilePath();
	//���� �ٶ�һ�� �˵�����Ϣ
	afx_msg void OnBaiduCheck();
	//���� ���ƽ���·���˵�����Ϣ
	afx_msg void OnCopy();
	//���� ����ע�����Ӳ˵�����Ϣ
	afx_msg void OnRegMonitor();
	//���� ģ���б��Ҽ���Ϣ
	afx_msg void OnRclickListDll(NMHDR* pNMHDR, LRESULT* pResult);
	//���� �򿪵���Ӧģ��λ�ò˵���Ϣ
	afx_msg void OnOpenModule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	HMODULE m_Hmodule;//Ƥ��ģ����
	CString m_numOfDll;//����ģ����
	static bool m_bAccend;   //�ж����������������ʽ
    static int m_nSortCol;  //ָ��������Ĺؼ���
	CImageList m_ImageList; //����ͼ���б�
	DWORD m_vProcessId[100];//��Ž���ID����
	bool EnablePrivilege();// ����������Ȩ��
	int EnumProcess(void); //ö�ٽ���
	CString GetProcessPath(DWORD idProcess);//��ý���·��
	BOOL GetDllInfo(DWORD dwPID);//���ģ����Ϣ
	//������(���û������б���ĳһ��ʱ��Ҫ����˳����,��Ҫ�б�����������)
	static int CALLBACK MyCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);//����ص�����
};

//{{AFX_INSERT_LOCATION}}()
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMANAGEDLG_H__A3F29421_DE79_4D52_816F_5D080647CB10__INCLUDED_)

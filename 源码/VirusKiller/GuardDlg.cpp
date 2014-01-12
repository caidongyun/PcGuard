// GuardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VirusKiller.h"
#include "GuardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuardDlg dialog


CGuardDlg::CGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGuardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGuardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuardDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGuardDlg, CDialog)
	//{{AFX_MSG_MAP(CGuardDlg)
	ON_BN_CLICKED(IDC_BUTTON_FIREWALL, OnButtonFirewall)
	ON_BN_CLICKED(IDC_BUTTON_COMPUTER, OnButtonComputer)
	ON_BN_CLICKED(IDC_BUTTON_magnify, OnBUTTONmagnify)
	ON_BN_CLICKED(IDC_BUTTON_NSLOOKUP, OnButtonNslookup)
	ON_BN_CLICKED(IDC_BUTTON_PER, OnButtonPer)
	ON_BN_CLICKED(IDC_BUTTON_SERVICE, OnButtonService)
	ON_BN_CLICKED(IDC_BUTTON_SYS_CONFIG, OnButtonSysConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGuardDlg message handlers
// �����߼���ȫWindows����ǽ
void CGuardDlg::OnButtonFirewall() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(0, NULL, "wf.msc", NULL,NULL, SW_NORMAL);
}
// �򿪼��������
void CGuardDlg::OnButtonComputer() 
{
	// TODO: compmgmt.msc
	::ShellExecute(0, NULL, "compmgmt.msc", NULL,NULL, SW_NORMAL);
}

// �� �Ŵ�ʵ�ó���
void CGuardDlg::OnBUTTONmagnify() 
{
	// TODO: magnify
	::ShellExecute(0, NULL, "magnify.exe", NULL,NULL, SW_NORMAL);
}
// IP��ַ�����
void CGuardDlg::OnButtonNslookup() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(0, NULL, "Nslookup.exe", NULL,NULL, SW_NORMAL);
}

// ��������ܼ�����
void CGuardDlg::OnButtonPer() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(0, NULL, "perfmon.msc", NULL,NULL, SW_NORMAL);
}
// ���ط�������
void CGuardDlg::OnButtonService() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(0, NULL, "services.msc", NULL,NULL, SW_NORMAL);
}
// ϵͳ����ʵ�ó���
void CGuardDlg::OnButtonSysConfig() 
{
	// TODO: Add your control notification handler code here
	::ShellExecute(0, NULL, "Msconfig.exe", NULL,NULL, SW_NORMAL);
}

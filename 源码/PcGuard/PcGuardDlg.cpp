// PcGuardDlg.cpp : implementation file
////////////////////////////////////////////////////////////
// PcGuard ������	2011.6.09
// ����08-2 ���� 
////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PcGuard.h"
#include "PcGuardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcGuardDlg dialog

CPcGuardDlg::CPcGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPcGuardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPcGuardDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPcGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPcGuardDlg)
	DDX_Control(pDX, IDC_BUTTON_KILL, m_BtKill);
	DDX_Control(pDX, IDC_BUTTON_REG, m_BtReg);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_BtFile);
	DDX_Control(pDX, IDC_BUTTON_PRO, m_BtPro);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPcGuardDlg, CDialog)
	//{{AFX_MSG_MAP(CPcGuardDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PRO, OnButtonPro)
	ON_BN_CLICKED(IDC_BUTTON_KILL, OnButtonKill)
	ON_BN_CLICKED(IDC_BUTTON_REG, OnButtonReg)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_COMMAND(ID_MENU_SHOW, OnMenuShow)
	ON_COMMAND(ID_MENU_QUIT, OnMenuQuit)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE( WM_TRAYICON_MSG,OnTrayCallBackMsg)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPcGuardDlg message handlers

BOOL CPcGuardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//   ����ť���� icon
    m_BtPro.ModifyStyle(0,BS_ICON);
	m_BtPro.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_PRO));

	m_BtReg.ModifyStyle(0,BS_ICON);
	m_BtReg.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_REG));

	m_BtFile.ModifyStyle(0,BS_ICON);
	m_BtFile.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FILE));

	m_BtKill.ModifyStyle(0,BS_ICON);
	m_BtKill.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_KILL));

     m_sText=_T("Virus-free PC, enjoy the Internet of life!");
	//����̬�ı�����Ļ��������m_pRectLink��    
     GetDlgItem(IDC_LINK) -> GetWindowRect(&m_pRectLink);    
    //����Ļ����ת��Ϊ�ͻ�����    
     ScreenToClient(&m_pRectLink); 
     m_nScrollXPos=0; // ������Ļ��ʼλ��

		//�궨�庯��ָ������ 
	typedef int ( WINAPI *SKINH_ATTACHEX)(LPCTSTR strSkinFile,LPCTSTR strPassword);
	// ȡ��SKINH_ATTACHEX�����ĵ�ַ
		SKINH_ATTACHEX pFunc = (SKINH_ATTACHEX)::GetProcAddress(::LoadLibrary("SkinH.dll"), "SkinH_AttachEx");
		if(pFunc)
		{
			// ����Ƥ���ļ�
			pFunc(_T("skin\\pixos.she"), NULL);
		}
		else
		{
			MessageBox(_T("Ƥ�������ʧ�ܣ�"),_T("ȱ��SkinH.dll"));
		}
    // ���ü�ʱ��
	SetTimer(1,800,0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPcGuardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE )
	{	
		TrayMyIcon(); // ��С��ʱ��������
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPcGuardDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPcGuardDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPcGuardDlg::OnButtonPro() 
{
	// TODO: Add your control notification handler code here
	// ����ProcessManage �����Ƿ����
	HWND hWnd =::FindWindow(NULL,_T("ProcessManage")); 
	if(NULL==hWnd)
	{
		// ���д˳���
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = _T("runas");      
		sei.lpFile = _T("ProcessManage.exe");   
		sei.nShow = SW_SHOWNORMAL;   
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	else // ��ʾ�˴���
	{

		::ShowWindow(hWnd,SW_HIDE);// ����һ��
		::ShowWindow(hWnd,SW_RESTORE);// ��ʾ����
	}
	GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("���̹�����������!"));
}

void CPcGuardDlg::OnButtonKill() 
{
	// TODO: Add your control notification handler code here
	// ���� �����Ƿ����
	HWND hWnd =::FindWindow(NULL,_T("VirusKiller")); 
	if(NULL==hWnd)
	{
		// ���д˳���

		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = _T("runas");      
		sei.lpFile = _T("VirusKiller.exe");   
		sei.nShow = SW_SHOWNORMAL;   
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	else // ��ʾ�˴���
	{

		::ShowWindow(hWnd,SW_HIDE);// ����һ��
		::ShowWindow(hWnd,SW_RESTORE);// ��ʾ����
	}
	GetDlgItem(IDC_STATIC_KILL)->SetWindowText(_T("������ɱ������!"));
}

void CPcGuardDlg::OnButtonReg() 
{
	// TODO: Add your control notification handler code here
		// ���� �����Ƿ����
	HWND hWnd =::FindWindow(NULL,_T("RegMonitor")); 
	if(NULL==hWnd)
	{
		// ���д˳���

		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = _T("runas");      
		sei.lpFile = _T("RegMonitor.exe");   
		sei.nShow = SW_SHOWNORMAL;   
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	else // ��ʾ�˴���
	{

		::ShowWindow(hWnd,SW_HIDE);// ����һ��
		::ShowWindow(hWnd,SW_RESTORE);// ��ʾ����
	}
	GetDlgItem(IDC_STATIC_REG)->SetWindowText(_T("ע������������!"));
}

void CPcGuardDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
		// ���� �����Ƿ����
	HWND hWnd =::FindWindow(NULL,_T("FileMonitor")); 
	if(NULL==hWnd)
	{
		// ���д˳���

		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = _T("runas");      
		sei.lpFile = _T("FileMonitor.exe");   
		sei.nShow = SW_SHOWNORMAL;   
		ShellExecuteEx(&sei);
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	else // ��ʾ�˴���
	{

		::ShowWindow(hWnd,SW_HIDE);// ����һ��
		::ShowWindow(hWnd,SW_RESTORE);// ��ʾ����
	}
	GetDlgItem(IDC_STATIC_FILE)->SetWindowText(_T("�ļ�����������!"));
}

void CPcGuardDlg::OnTimer(UINT nIDEvent) 
{
	// ��̬��ʾ��Ļ
	if(m_nScrollXPos>m_sText.GetLength())// �ж���ʾ�����
	{
		m_nScrollXPos=0; // ��ʼλ����Ϊ0 ���¿�ʼ��ʾ
	}
	else
	{
		GetDlgItem(IDC_STATIC_MARQUE)->SetWindowText(m_sText.Left(m_nScrollXPos));
		m_nScrollXPos+=3;// ÿ�����������ַ�
	}
	// �鿴���̹�����������
	if(::FindWindow(NULL,_T("ProcessManage")))
	{
		GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("���̹�����������!"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_PRO)->SetWindowText(_T("���̹�����û����!"));
	}
	// �鿴ɱ��������
	if(::FindWindow(NULL,_T("VirusKiller")))
	{
		GetDlgItem(IDC_STATIC_KILL)->SetWindowText("������ɱ������!");
	}
	else
	{
		GetDlgItem(IDC_STATIC_KILL)->SetWindowText("������ɱû����!");
	}
	// �鿴ע�����ӿ�����
	if(::FindWindow(NULL,_T("RegMonitor")))
	{
		GetDlgItem(IDC_STATIC_REG)->SetWindowText(_T("ע������������!"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_REG)->SetWindowText(_T("ע������û����!"));
	}
    // �鿴�ļ����ӿ�����?
	if(::FindWindow(NULL,_T("FileMonitor")))
	{
		GetDlgItem(IDC_STATIC_FILE)->SetWindowText(_T("�ļ�����������!"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_FILE)->SetWindowText(_T("�ļ�����û����!"));
	}
	CDialog::OnTimer(nIDEvent);
}

void CPcGuardDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CPcGuardDlg::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CDialog::OnCaptureChanged(pWnd);
}

bool CPcGuardDlg::TrayMyIcon(bool bAdd)
{
	BOOL bRet = false;
	m_Ntnd.cbSize = sizeof(NOTIFYICONDATA); //�������������йؽṹ��С
	m_Ntnd.hWnd = GetSafeHwnd(); // ����������е�ͼ����в�������Ӧ����Ϣ�ʹ���������������Ĵ���
	m_Ntnd.uID = IDR_MAINFRAME; // ��������ͼ��ID
	if ( bAdd == TRUE )
	{
		// �����Ա��־��������Щ��Ա����������Ч�ģ��ֱ�Ϊ
		// NIF_ICON, NIF_MESSAGE, NIF_TIP���ֱ������������Ч�ĳ�Ա��hIcon, uCallbackMessage, szTip
		m_Ntnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		// ���Ǹ���Ϣ��־������������������Ӧͼ����в�����ʱ�򣬾ͻᴫ����Ϣ��Hwnd������Ĵ���
		m_Ntnd.uCallbackMessage = WM_TRAYICON_MSG; ;// �Զ������Ϣ����
		// Ҫ���ӣ�ɾ�����޸ĵ�ͼ����
		m_Ntnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		// ����ƶ�������ͼ����ʱ����ʾ����
		strncpy(m_Ntnd.szTip, _T("PcGuard \n PC��ʿ"), sizeof(m_Ntnd.szTip));
		ShowWindow(SW_MINIMIZE);// ��С��
		ShowWindow(SW_HIDE);// ����������
		bRet = Shell_NotifyIcon(NIM_ADD, &m_Ntnd);// ��ϵͳ������Ϣ���������������ͼ��
	}
	else
	{
		ShowWindow(SW_SHOWNA); // ��ʾ����
		SetForegroundWindow(); // �Ѵ���������ǰ
		bRet = Shell_NotifyIcon(NIM_DELETE, &m_Ntnd);// ��ϵͳ������Ϣ ɾ����������ͼ��
	}
	return bRet;
}

// ���û����������������ͼ���ʱ��(��������������Ҽ�),����hWnd������Ĵ��ڴ�����Ϣ
LRESULT CPcGuardDlg::OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam)
{
	//wParam���յ���ͼ���ID����lParam���յ���������Ϊ
	if(wparam!=IDR_MAINFRAME)
		return 0;
	switch(lparam)
	{
	case WM_RBUTTONUP:
		{
			CMenu mMenu, *pMenu = NULL;
			CPoint pt;
			mMenu.LoadMenu(IDR_MENU1);
			pMenu = mMenu.GetSubMenu(0);
			GetCursorPos(&pt);//�õ����λ��
			SetForegroundWindow();
			//ȷ������ʽ�˵���λ��
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
			//��Դ����
			HMENU hmenu=mMenu.Detach();
			mMenu.DestroyMenu();
			break;
		}
	case WM_LBUTTONDBLCLK: // ������������ͼ��
		ShowWindow(SW_RESTORE);// �����
		SetForegroundWindow(); // ��ʾ��ǰ
		TrayMyIcon(false);  // ȡ������
		break;

	default:
		break;
	}
	return 1;
}

//��ʾ���������
void CPcGuardDlg::OnMenuShow() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_RESTORE); // ��ʾ����
	TrayMyIcon(FALSE);// ȡ������
}

// �˳�

void CPcGuardDlg::OnMenuQuit() 
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE,0,0);
}

void CPcGuardDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	   //������������ھ�̬�ı���ʱ����������С��״    
    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y > m_pRectLink.top && point.y < m_pRectLink.bottom )    
    //�˴�����ж������㷨    
     {    
        HCURSOR hCursor;    
         hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);    
        //�������ΪС��״    
         SetCursor(hCursor);    
     }    

	CDialog::OnMouseMove(nFlags, point);
}

void CPcGuardDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//�˴�����ж������㷨    
    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y > m_pRectLink.top && point.y < m_pRectLink.bottom)    
	{    
        //����������    
        if (nFlags == MK_LBUTTON)       
		{    
            //Ϊ�������Ч�����˴��������ϱ任�����״�Ĵ���    
			ShellExecute(0, NULL, "http://blog.csdn.net/gfsfg8545", NULL,NULL, SW_NORMAL);        
		}    
	}    
	CDialog::OnLButtonDown(nFlags, point);
}

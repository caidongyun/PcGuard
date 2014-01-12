// RegMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegMonitor.h"
#include "RegMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TYPE_PROCESSMANAGE_MSG WM_USER+111 //���̹����� ���͵���Ϣ
/////////////////////////////////////////////////////////////////////////////
// CRegMonitorDlg dialog
CRegMonitorDlg::CRegMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegMonitorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegMonitorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);// ����ͼ��
	m_hHook=NULL;
	m_Psubmenu=NULL;
	m_bRegCreateKeyEx = TRUE;
	m_bRegSetValueEx = TRUE;
	m_bRegDeleteKey = TRUE;
	m_bRegDeleteValue = TRUE;
	m_bRegQueryValueEx = TRUE;
	m_bIgnoreExplorer = TRUE;
	m_bIgnoreRegedit = TRUE;
}

CRegMonitorDlg::~CRegMonitorDlg()//��������
{
	//�ͷ�Skin.dll��Դ
	if(m_HmoduleSkin)
	{
		::FreeLibrary(m_HmoduleSkin);
	}
	//�ͷ�User32.dll��Դ
	if(m_HmoduleUser32)
	{
		::FreeLibrary(m_HmoduleSkin);
	}
}
void CRegMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegMonitorDlg)
	DDX_Control(pDX, IDC_LIST, m_listCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegMonitorDlg, CDialog)
	//{{AFX_MSG_MAP(CRegMonitorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_WM_COPYDATA()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_COMMAND(IDR_MENU11, OnMenuCreateKey)
	ON_COMMAND(IDR_MENU12, OnMenuDelKey)
	ON_COMMAND(IDR_MENU13, OnMenuSetValue)
	ON_COMMAND(IDR_MENU14, OnMenuDelValue)
	ON_COMMAND(IDR_MENU15, OnMenuQueryValue)
	ON_COMMAND(IDR_MENU21, OnMenuOpenRegedit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(IDR_MENU22, OnMenuOpenProcessManage)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegMonitorDlg message handlers

BOOL CRegMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	/*******************************///����Ƥ��/*****************************************/
	srand((unsigned)time(0));
	int i=rand()%35 +1;
	CString s=_T("");
	s.Format("skin//%d.she",i);
	//���������ڼ䶯̬���ӷ�ʽ
	//����Ŀ��DLL
	m_HmoduleSkin=::LoadLibrary("SkinH.dll");
	//�궨�庯��ָ������ 
	typedef int ( WINAPI *SKINH_ATTACHEX)(LPCTSTR strSkinFile,LPCTSTR strPassword);
	
	if(NULL !=m_HmoduleSkin)
	{						// ȡ��SKINH_ATTACHEX�����ĵ�ַ
		SKINH_ATTACHEX pFunc = (SKINH_ATTACHEX)::GetProcAddress(m_HmoduleSkin, "SkinH_AttachEx");
		if(pFunc)
		{
			pFunc(s, NULL);
		}
		else
		{
			MessageBox(_T("Ƥ�������ʧ�ܣ�"),_T("ȱ��SkinH.dll"));
		}
	}

	/***********************************************************************************************************/
	// ����Vista/Win7��,��ͬȨ�޽��̼�(����Ͳ���Ҫ���������Ҫ)������Ϣ��WM_COPYDATA 
	// ��һ�κ���Ҫ��Ȼ�Ļ� ������ĳ������Թ���Ա�������(Ӧ�ó�����Windows 7������ʱ����ΪUIPI���ƣ�
	// ������Ϣ���ݱ������) ChangeWindowMessageFilter
	// ����������������ǿ�����ӻ���ɾ���ܹ�ͨ����Ȩ�ȼ������Windows��Ϣ�������ӵ�нϸ���Ȩ�ȼ��Ľ���,
	// ������һ��������,����ͨ����Windows��Ϣ������ӵ�����������İ�����,ֻ��������������ϵ���Ϣ������
	// �ݽ���

	typedef BOOL (WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT,DWORD); 
	ChangeWindowMessageFilter_PROC pfnChangeWindowMessageFilter; 
	pfnChangeWindowMessageFilter = (ChangeWindowMessageFilter_PROC)::GetProcAddress\
		( (m_HmoduleUser32=::GetModuleHandle(_T("USER32.dll") ) ),"ChangeWindowMessageFilter"); 
	if (pfnChangeWindowMessageFilter) 
	{  
		// ������һ�������� WM_COPYDATA
		pfnChangeWindowMessageFilter(WM_COPYDATA, 1); 
	}
/*************************************************************************************************************/

	InitDlgItem();//��ʼ���Ի���
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegMonitorDlg::OnPaint() 
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
HCURSOR CRegMonitorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ��ʼ
void CRegMonitorDlg::OnButtonStart() 
{
	if(InstallHook(m_hWnd))
	{
		DWORD pid = 0;
		if(m_bIgnoreExplorer)
			pid = GetProcessID("explorer.exe");
		SetExplorerPID(pid);
		UpdateHookFunction();// ���¼���
		CButton *pBt=(CButton *)GetDlgItem(IDC_BUTTON_START);
		pBt->SetCheck(BST_CHECKED);
		pBt->EnableWindow(false);
		pBt=(CButton *)GetDlgItem(IDC_BUTTON_STOP);
		pBt->SetCheck(BST_CHECKED);
		pBt->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_TIP1)->SetWindowText(_T("���ڼ���..."));
		GetDlgItem(IDC_STATIC_TIP2)->SetWindowText(_T("�������н���"));
	}	
}

void CRegMonitorDlg::UpdateHookFunction()
{
	SetHookFunction(m_bRegCreateKeyEx, m_bRegSetValueEx, m_bRegDeleteKey, 
		m_bRegDeleteValue, m_bRegQueryValueEx);// ���ù��Ӽ��ӵ�����
}

// ͨ��ProcessName�õ�����ID
DWORD CRegMonitorDlg::GetProcessID(PCHAR ProcessName)
{
	
	CString str1;
	CString str2;
	PROCESSENTRY32 info = {sizeof(PROCESSENTRY32)};
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);// ��������
    BOOL bReport = Process32First(hProcess,&info);
	
    while(bReport)
    {
		str1 = ProcessName;
		str2 = info.szExeFile;
		str1.MakeLower();//���ı�ΪСд��ĸ����Ƚ�
		str2.MakeLower();
		if(str1 == str2) // �ж��Ƿ�Ϊ�˽���
			return info.th32ProcessID; // ���ؽ��� ID
        bReport = Process32Next(hProcess, &info);   
    }
	return 0;
}

void CRegMonitorDlg::InitDlgItem()
{
	SetWindowText(_T("RegMonitor"));
	GetDlgItem(IDC_STATIC_TIP1)->SetWindowText(_T("ע������,��ѡ��ʼ"));
	//�б��ʼ���б�
	m_listCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_TWOCLICKACTIVATE |LVS_EX_GRIDLINES);

	m_listCtrl.SetBkColor(RGB(247,247,255));
	m_listCtrl.SetTextColor(RGB(0,100,155));
	m_listCtrl.SetTextBkColor(RGB(247,247,255));

	m_listCtrl.InsertColumn(0,_T("������"),LVCFMT_LEFT,120);
	m_listCtrl.InsertColumn(1,_T("ʱ��"),LVCFMT_LEFT,80); 
	m_listCtrl.InsertColumn(2,_T("����"),LVCFMT_LEFT,83);
	m_listCtrl.InsertColumn(3,_T("����"),LVCFMT_LEFT,266);
	m_listCtrl.InsertColumn(4,_T("����"),LVCFMT_LEFT,80);
	m_listCtrl.InsertColumn(5,_T("����"),LVCFMT_LEFT,80);
	m_listCtrl.InsertColumn(6,_T("����"),LVCFMT_LEFT,80);
	
	// �����б�����
	LOGFONT lf;
	::ZeroMemory(&lf,sizeof(lf)); 
	lf.lfHeight = 90;         
	lf.lfWeight = FW_NORMAL;   
	lf.lfItalic = false;	
	::lstrcpy(lf.lfFaceName,_T("Verdana")); 
	m_hFont.CreatePointFontIndirect(&lf);
	HIMAGELIST m_hSmallImageList;
	// ���庯��ָ�� SHGetImageList
	typedef BOOL (WINAPI * SHGIL_PROC)	(HIMAGELIST *phLarge, HIMAGELIST *phSmall);
	// ���� shell32.dll ģ��
	HMODULE hShell32 = LoadLibrary("shell32.dll");
	//  �õ�����SHGetImageList����ڵ�ַ ע��ָ��Ϊ 71 ��Ȼ����
	SHGIL_PROC Shell_GetImageLists = (SHGIL_PROC)GetProcAddress(hShell32, (LPCSTR)71);
	Shell_GetImageLists(NULL, &m_hSmallImageList);// ��ó���Сͼ��
	
	if(m_hSmallImageList != NULL) 
		m_imglst.Attach(m_hSmallImageList);
	
	m_listCtrl.SetImageList(&m_imglst, LVSIL_SMALL);// �����б�Сͼ��

	//ZwQueryKey ö������ 
	m_pZwQueryKey = (ZwQueryKey)::GetProcAddress(LoadLibrary("ntdll.dll"), "ZwQueryKey");


	// Get the popup menu 
	CMenu* mmenu = GetMenu();
	m_Psubmenu = mmenu->GetSubMenu(0);// ����Ӳ˵�
	// Ĭ������sub(0) ȫѡ��
	m_Psubmenu->CheckMenuItem(IDR_MENU11, MF_CHECKED | MF_BYCOMMAND);
	m_Psubmenu->CheckMenuItem(IDR_MENU12, MF_CHECKED | MF_BYCOMMAND);
	m_Psubmenu->CheckMenuItem(IDR_MENU13, MF_CHECKED | MF_BYCOMMAND);
	m_Psubmenu->CheckMenuItem(IDR_MENU14, MF_CHECKED | MF_BYCOMMAND);
	m_Psubmenu->CheckMenuItem(IDR_MENU15, MF_CHECKED | MF_BYCOMMAND);

}
//////////////////////////////////////////////////////////////////////////////////////

// ������Ӧ�ļ�����Ϣ
void   CRegMonitorDlg::CreateKeyExA_Msg(PARAMS* params, int index)
{
	char szTemp[1024]={0};
	CString str=_T("�����¼�(A)");
	m_listCtrl.SetItemText(index, 2, str);
		
	switch (int(params->hKey)) // Reserved Key Handles.
	{
		// �ֱ�Լ��������������
	case HKEY_CLASSES_ROOT:
		str.Format("HKEY_CLASSES_ROOT\\%s", params->buf1);
		break;
	case HKEY_CURRENT_USER:
		str.Format("HKEY_CURRENT_USER\\%s", params->buf1);
		break;
	case HKEY_LOCAL_MACHINE:
		str.Format("HKEY_LOCAL_MACHINE\\%s", params->buf1);
		break;
	case HKEY_USERS:
		str.Format("HKEY_USERS\\%s", params->buf1);
		break;
		// WINNT����ϵͳ
	case HKEY_PERFORMANCE_DATA:
		str.Format("HKEY_PERFORMANCE_DATA\\%s", params->buf1);
		break;
	case HKEY_CURRENT_CONFIG:
		str.Format("HKEY_CURRENT_CONFIG\\%s", params->buf1);
		break;
		// WIN9X����ϵͳ
	case HKEY_DYN_DATA:
		str.Format("HKEY_DYN_DATA\\%s", params->buf1);
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp);
		str.Format("%s\\%s", szTemp, params->buf1);
	}
	m_listCtrl.SetItemText(index, 3, str);
	m_listCtrl.SetItemText(index, 4, "N/A");
	m_listCtrl.SetItemText(index, 5, "N/A");
	m_listCtrl.SetItemText(index, 6, "N/A");
}

// ������Ӧ�ļ�����Ϣ

void  CRegMonitorDlg::CreateKeyExW_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("�����¼�(W)");
	m_listCtrl.SetItemText(index, 2, str);
		
	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	switch (int(params->hKey))// Reserved Key Handles.
	{
	case HKEY_CLASSES_ROOT:
		str.Format("HKEY_CLASSES_ROOT\\%s", szTemp);
		break;
	case HKEY_CURRENT_USER:
		str.Format("HKEY_CURRENT_USER\\%s", szTemp);
		break;
	case HKEY_LOCAL_MACHINE:
		str.Format("HKEY_LOCAL_MACHINE\\%s", szTemp);
		break;
	case HKEY_USERS:
		str.Format("HKEY_USERS\\%s", szTemp);
		break;
	case HKEY_PERFORMANCE_DATA:
		str.Format("HKEY_PERFORMANCE_DATA\\%s", szTemp);
		break;
	case HKEY_CURRENT_CONFIG:
		str.Format("HKEY_CURRENT_CONFIG\\%s", szTemp);
		break;
	case HKEY_DYN_DATA:
		str.Format("HKEY_DYN_DATA\\%s", szTemp);
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp2);
		str.Format("%s\\%s", szTemp2, szTemp);
	}
	m_listCtrl.SetItemText(index, 3, str);
	m_listCtrl.SetItemText(index, 4, "N/A");
	m_listCtrl.SetItemText(index, 5, "N/A");
	m_listCtrl.SetItemText(index, 6, "N/A");
}
// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::SetValueExA_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	CString str=_T("���ü�ֵ(A)");
	m_listCtrl.SetItemText(index, 2, str);
	HKeyToKeyName(params->PID, params->hKey, szTemp);
	m_listCtrl.SetItemText(index, 3, szTemp);
	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index, 4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, params->buf1);

	switch (params->type) // ��ȡ��ֵ��������
	{
	case REG_NONE:  // No value type
		m_listCtrl.SetItemText(index, 6, "REG_NONE");
		break;
	case REG_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_SZ");
		str.Format("%s", params->buf2);
		break;
	case REG_EXPAND_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_EXPAND_SZ");
		str.Format("%s", params->buf2);
		break;
	case REG_BINARY:
		m_listCtrl.SetItemText(index, 6, "REG_BINARY");
		str = "";
		int i;
		for(i=0; i < params->cbbuf2; i++)
		{
			str.Format("%s%02X ", str, *((BYTE*)params->buf2 + i));
		}
		break;
	case REG_DWORD:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2),
			*((DWORD*)params->buf2));
		break;
	case REG_DWORD_BIG_ENDIAN:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD_BIG_ENDIAN");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2),
			*((DWORD*)params->buf2));
		break;
	case REG_LINK:
		m_listCtrl.SetItemText(index, 6, "REG_LINK");
		break;
	case REG_MULTI_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_MULTI_SZ");
		break;
	case REG_RESOURCE_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_LIST");
		break;
	case REG_FULL_RESOURCE_DESCRIPTOR:
		m_listCtrl.SetItemText(index, 6, "REG_FULL_RESOURCE_DESCRIPTOR");
		break;
	case REG_RESOURCE_REQUIREMENTS_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_REQUIREMENTS_LIST");
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp);
		str.Format("%s\\%s", szTemp, params->buf2);
		break;
		}
		
	m_listCtrl.SetItemText(index, 5, str);
}

// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::SetValueExW_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("���ü�ֵ(W)");
	m_listCtrl.SetItemText(index, 2, str);
	HKeyToKeyName(params->PID, params->hKey, szTemp2);
	m_listCtrl.SetItemText(index, 3, szTemp2);

	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index, 4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, szTemp);

	WideToMultiByte((WCHAR*)params->buf2, szTemp);
	switch (params->type) // ��ȡ��ֵ��������
	{
	case REG_NONE:
		m_listCtrl.SetItemText(index, 6, "REG_NONE");
		break;
	case REG_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_SZ");
		str.Format("%s", szTemp);
		break;
	case REG_EXPAND_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_EXPAND_SZ");
		str.Format("%s", szTemp);
		break;
	case REG_BINARY:
		m_listCtrl.SetItemText(index, 6, "REG_BINARY");
		str = "";
		int i;
		for(i=0; i<params->cbbuf2; i++)
		{
			str.Format("%s%02X ", str, *((BYTE*)params->buf2 + i));
		}
		break;
	case REG_DWORD:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		break;
	case REG_DWORD_BIG_ENDIAN:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD_BIG_ENDIAN");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		break;
	case REG_LINK:
		m_listCtrl.SetItemText(index, 6, "REG_LINK");
		break;
	case REG_MULTI_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_MULTI_SZ");
		break;
	case REG_RESOURCE_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_LIST");
		break;
	case REG_FULL_RESOURCE_DESCRIPTOR:
		m_listCtrl.SetItemText(index, 6, "REG_FULL_RESOURCE_DESCRIPTOR");
		break;
	case REG_RESOURCE_REQUIREMENTS_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_REQUIREMENTS_LIST");
		break;
	default:
		break;
	}

	m_listCtrl.SetItemText(index, 5, str);
}

// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::DeleteKeyA_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	CString str=_T("ɾ���Ӽ�(A)");
	m_listCtrl.SetItemText(index, 2, str);

	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str.Format("HKEY_CLASSES_ROOT\\%s", params->buf1);
		break;
	case HKEY_CURRENT_USER:
		str.Format("HKEY_CURRENT_USER\\%s", params->buf1);
		break;
	case HKEY_LOCAL_MACHINE:
		str.Format("HKEY_LOCAL_MACHINE\\%s", params->buf1);
		break;
	case HKEY_USERS:
		str.Format("HKEY_USERS\\%s", params->buf1);
		break;
	case HKEY_PERFORMANCE_DATA:
		str.Format("HKEY_PERFORMANCE_DATA\\%s", params->buf1);
		break;
	case HKEY_CURRENT_CONFIG:
		str.Format("HKEY_CURRENT_CONFIG\\%s", params->buf1);
		break;
	case HKEY_DYN_DATA:
		str.Format("HKEY_DYN_DATA\\%s", params->buf1);
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp);
		str.Format("%s\\%s", szTemp, params->buf1);
	}
	m_listCtrl.SetItemText(index, 3, str);
	m_listCtrl.SetItemText(index, 4, "N/A");
	m_listCtrl.SetItemText(index, 5, "N/A");
	m_listCtrl.SetItemText(index, 6, "N/A");
}

// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::DeleteKeyW_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("ɾ���Ӽ�(W)");
	m_listCtrl.SetItemText(index, 2, str);
		
	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str.Format("HKEY_CLASSES_ROOT\\%s", szTemp);
		break;
	case HKEY_CURRENT_USER:
		str.Format("HKEY_CURRENT_USER\\%s", szTemp);
		break;
	case HKEY_LOCAL_MACHINE:
		str.Format("HKEY_LOCAL_MACHINE\\%s", szTemp);
		break;
	case HKEY_USERS:
		str.Format("HKEY_USERS\\%s", szTemp);
		break;
	case HKEY_PERFORMANCE_DATA:
		str.Format("HKEY_PERFORMANCE_DATA\\%s", szTemp);
		break;
	case HKEY_CURRENT_CONFIG:
		str.Format("HKEY_CURRENT_CONFIG\\%s", szTemp);
		break;
	case HKEY_DYN_DATA:
		str.Format("HKEY_DYN_DATA\\%s", szTemp);
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp2);
		str.Format("%s\\%s", szTemp2, szTemp);
	}
	m_listCtrl.SetItemText(index, 3, str);
	m_listCtrl.SetItemText(index, 4, "N/A");
	m_listCtrl.SetItemText(index, 5, "N/A");
	m_listCtrl.SetItemText(index, 6, "N/A");
}

// ������Ӧ�ļ�����Ϣ

void  CRegMonitorDlg::DeleteValueA_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
		CString str=_T("ɾ����ֵ(A)");
	m_listCtrl.SetItemText(index, 2, str);
		
	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str = "HKEY_CLASSES_ROOT";
		break;
	case HKEY_CURRENT_USER:
		str = "HKEY_CURRENT_USER";
		break;
	case HKEY_LOCAL_MACHINE:
		str = "HKEY_LOCAL_MACHINE";
		break;
	case HKEY_USERS:
		str = "HKEY_USERS";
		break;
	case HKEY_PERFORMANCE_DATA:
		str = "HKEY_PERFORMANCE_DATA";
		break;
	case HKEY_CURRENT_CONFIG:
		str = "HKEY_CURRENT_CONFIG";
		break;
	case HKEY_DYN_DATA:
		str = "HKEY_DYN_DATA";
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp);
		str.Format("%s", szTemp);
	}
	m_listCtrl.SetItemText(index, 3, str);

	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index, 4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, params->buf1);

		m_listCtrl.SetItemText(index, 5, "N/A");
		m_listCtrl.SetItemText(index, 6, "N/A");
}

// ������Ӧ�ļ�����Ϣ

void  CRegMonitorDlg::DeleteValueW_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("ɾ����ֵ(W)");
	m_listCtrl.SetItemText(index, 2, str);
		
	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str = "HKEY_CLASSES_ROOT";
		break;
	case HKEY_CURRENT_USER:
		str = "HKEY_CURRENT_USER";
		break;
	case HKEY_LOCAL_MACHINE:
		str = "HKEY_LOCAL_MACHINE";
		break;
	case HKEY_USERS:
		str = "HKEY_USERS";
		break;
	case HKEY_PERFORMANCE_DATA:
		str = "HKEY_PERFORMANCE_DATA";
		break;
	case HKEY_CURRENT_CONFIG:
		str = "HKEY_CURRENT_CONFIG";
		break;
	case HKEY_DYN_DATA:
		str = "HKEY_DYN_DATA";
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp2);
		str.Format("%s", szTemp2);
	}
	m_listCtrl.SetItemText(index, 3, str);

	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index, 4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, szTemp);

		m_listCtrl.SetItemText(index, 5, "N/A");
		m_listCtrl.SetItemText(index, 6, "N/A");
}


// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::QueryValueExA_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("��ֵ��ѯ(A)");
	m_listCtrl.SetItemText(index, 2, str);
		
	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str = "HKEY_CLASSES_ROOT";
		break;
	case HKEY_CURRENT_USER:
		str = "HKEY_CURRENT_USER";
		break;
	case HKEY_LOCAL_MACHINE:
		str = "HKEY_LOCAL_MACHINE";
		break;
	case HKEY_USERS:
		str = "HKEY_USERS";
		break;
	case HKEY_PERFORMANCE_DATA:
		str = "HKEY_PERFORMANCE_DATA";
		break;
	case HKEY_CURRENT_CONFIG:
		str = "HKEY_CURRENT_CONFIG";
		break;
	case HKEY_DYN_DATA:
		str = "HKEY_DYN_DATA";
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp2);
		str.Format("%s", szTemp2);
	}
	m_listCtrl.SetItemText(index, 3, str);
		
	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index, 4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, params->buf1);
		
	switch (params->type)
	{
	case REG_NONE:
		m_listCtrl.SetItemText(index, 6, "REG_NONE");
		if(params->cbbuf2 == 4)
			str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		else
		{
			if (params->cbbuf2 == (int)strlen(params->buf2)+1)
				str.Format("%s", params->buf2);
			else
			{
				str = "";
				int i;
				for(i=0; i<params->cbbuf2; i++)
				{
					str.Format("%s%02X ", str, *((BYTE*)params->buf2 + i));
				}
			}
		}
		break;
	case REG_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_SZ");
		str.Format("%s", params->buf2);
		break;
	case REG_EXPAND_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_EXPAND_SZ");
		str.Format("%s", params->buf2);
		break;
	case REG_BINARY:
		m_listCtrl.SetItemText(index, 6, "REG_BINARY");
		str = "";
		int i;
		for(i=0; i<params->cbbuf2; i++)	
		{
				str.Format("%s%02X ", str,*((BYTE*)params->buf2 + i));
		}
		break;
	case REG_DWORD:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		break;
	case REG_DWORD_BIG_ENDIAN:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD_BIG_ENDIAN");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		break;
	case REG_LINK:
		m_listCtrl.SetItemText(index, 6, "REG_LINK");
		break;
	case REG_MULTI_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_MULTI_SZ");
		break;
	case REG_RESOURCE_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_LIST");
		break;
	case REG_FULL_RESOURCE_DESCRIPTOR:
		m_listCtrl.SetItemText(index, 6, "REG_FULL_RESOURCE_DESCRIPTOR");
		break;
	case REG_RESOURCE_REQUIREMENTS_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_REQUIREMENTS_LIST");
		break;
	default:
		CString sTemp;
		sTemp.Format("%d", params->type);
		m_listCtrl.SetItemText(index, 6, sTemp);
			
		str = "";
		break;
	}
	m_listCtrl.SetItemText(index, 5, str);
}

// ������Ӧ�ļ�����Ϣ
void  CRegMonitorDlg::QueryValueExW_Msg(PARAMS* params,int index)
{
	char szTemp[1024]={0};
	char szTemp2[1024]={0};
	CString str=_T("��ֵ��ѯ(W)");
	m_listCtrl.SetItemText(index, 2, str);

	switch (int(params->hKey))
	{
	case HKEY_CLASSES_ROOT:
		str = "HKEY_CLASSES_ROOT";
		break;
	case HKEY_CURRENT_USER:
		str = "HKEY_CURRENT_USER";
		break;
	case HKEY_LOCAL_MACHINE:
		str = "HKEY_LOCAL_MACHINE";
		break;
	case HKEY_USERS:
		str = "HKEY_USERS";
		break;
	case HKEY_PERFORMANCE_DATA:
		str = "HKEY_PERFORMANCE_DATA";
		break;
	case HKEY_CURRENT_CONFIG:
		str = "HKEY_CURRENT_CONFIG";
		break;
	case HKEY_DYN_DATA:
		str = "HKEY_DYN_DATA";
		break;
	default:
		HKeyToKeyName(params->PID, params->hKey, szTemp2);
		str.Format("%s", szTemp2);	
	}
	m_listCtrl.SetItemText(index, 3, str);
		
	WideToMultiByte((WCHAR*)params->buf1, szTemp);
	if(params->buf1 == 0 || strlen(params->buf1) == 0)
		m_listCtrl.SetItemText(index,4, "Ĭ��ֵ");
	else
		m_listCtrl.SetItemText(index, 4, szTemp);
		
	WideToMultiByte((WCHAR*)params->buf2, szTemp);
	switch (params->type)
	{
	case REG_NONE:
		m_listCtrl.SetItemText(index, 6, "REG_NONE");
		if(params->cbbuf2 == 4)
			str.Format("0x%08X(%d)", *((DWORD*)params->buf2), 
			*((DWORD*)params->buf2));
		else
		{
			if (params->cbbuf2 == (int)wcslen((WCHAR*)params->buf2)*2+2)
					str.Format("%s", szTemp);
			else
			{
				str = "";
				int i;
				for(i=0; i<params->cbbuf2; i++)
				{
					str.Format("%s%02X ", str, *((BYTE*)params->buf2 + i));
				}
			}
		}
		break;
	case REG_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_SZ");
		str.Format("%s", szTemp);
		break;
	case REG_EXPAND_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_EXPAND_SZ");
		str.Format("%s", szTemp);
		break;
	case REG_BINARY:
		m_listCtrl.SetItemText(index, 6, "REG_BINARY");
		str = "";
		int i;
		for(i=0; i<params->cbbuf2; i++)
		{
			str.Format("%s%02X ", str, *((BYTE*)params->buf2 + i));
		}
		break;
	case REG_DWORD:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), *((DWORD*)params->buf2));
			break;
	case REG_DWORD_BIG_ENDIAN:
		m_listCtrl.SetItemText(index, 6, "REG_DWORD_BIG_ENDIAN");
		str.Format("0x%08X(%d)", *((DWORD*)params->buf2), *((DWORD*)params->buf2));
		break;
	case REG_LINK:
		m_listCtrl.SetItemText(index, 6, "REG_LINK");
		break;
	case REG_MULTI_SZ:
		m_listCtrl.SetItemText(index, 6, "REG_MULTI_SZ");
		break;
	case REG_RESOURCE_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_LIST");
		break;
	case REG_FULL_RESOURCE_DESCRIPTOR:
		m_listCtrl.SetItemText(index, 6, "REG_FULL_RESOURCE_DESCRIPTOR");
		break;
	case REG_RESOURCE_REQUIREMENTS_LIST:
		m_listCtrl.SetItemText(index, 6, "REG_RESOURCE_REQUIREMENTS_LIST");
		break;
	default:
		CString sTemp;
		sTemp.Format("%d", params->type);
		m_listCtrl.SetItemText(index, 6, sTemp);
		str = "";
		break;
	}
	m_listCtrl.SetItemText(index, 5, str);
}

//////////////////////////////////////////////////////////////////////////////////////

// ������յ���  CopyData ��Ϣ
BOOL CRegMonitorDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// �ж��Ƿ�Ϊ ���̹����������Ľ��̼�ͨ����Ϣ
	if(TYPE_PROCESSMANAGE_MSG == pCopyDataStruct->dwData)
	{
		ProcessMsg *pProMsg=(ProcessMsg *)pCopyDataStruct->lpData;
		if(0==pProMsg->PID)
		{
			GetDlgItem(IDC_STATIC_TIP1)->SetWindowText(_T("��������Ч����"));
		}
		else
		{
			//OnButtonStart();
			HWND hBtn = GetDlgItem(IDC_BUTTON_START)->GetSafeHwnd();
			SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_START, BN_CLICKED), (LPARAM)hBtn);
			SetFilter(pProMsg->PID);
			GetDlgItem(IDC_STATIC_TIP2)->SetWindowText("����:"+(CString)pProMsg->ProcessName);
		}
		return CDialog::OnCopyData(pWnd, pCopyDataStruct);
	}

	SHFILEINFO shFileInfo = {0};
	CString str = _T("");
	PARAMS* params= (PARAMS*)pCopyDataStruct->lpData;
	char szPath[MAX_PATH];
	char szName[MAX_PATH];
	ProcessPidToPath(params->PID, szName, szPath);
	::SHGetFileInfo(szPath, 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);
	if (m_bIgnoreRegedit)
	{
		CString strTemp = szName;
		strTemp.MakeLower();
		if (strTemp.Find("regedit.exe") > -1)
			return TRUE;
	}
	int index = m_listCtrl.InsertItem(0xFFFF, szPath, shFileInfo.iIcon);
   //��ȡ��ǰʱ��
	CTime time = CTime::GetCurrentTime();	
	CString strTime;						
	strTime.Format("%d:%d:%d", time.GetHour(), time.GetMinute(), time.GetSecond());
    

  switch (pCopyDataStruct->dwData)
  {
	case TYPE_RegCreateKeyExA:
		m_listCtrl.SetItemText(index, 1, strTime);
        CreateKeyExA_Msg(params,index);
		break;
	case TYPE_RegCreateKeyExW:
	    m_listCtrl.SetItemText(index, 1, strTime);
        CreateKeyExW_Msg(params,index);
		break;
	///////////////////////////////////////////////////////////////////////
	case TYPE_RegSetValueExA:
		m_listCtrl.SetItemText(index, 1, strTime);
		SetValueExA_Msg(params,index);
		break;

	case TYPE_RegSetValueExW:
		m_listCtrl.SetItemText(index, 1, strTime);
		SetValueExW_Msg(params,index);
		break;
		///////////////////////////////////////////////////////////////////////
	case TYPE_RegDeleteKeyA:
		m_listCtrl.SetItemText(index, 1, strTime);
		DeleteKeyA_Msg(params,index);
		break;

	case TYPE_RegDeleteKeyW:
		m_listCtrl.SetItemText(index, 1, strTime);
		DeleteKeyW_Msg(params,index);
		break;
		///////////////////////////////////////////////////////////////////////
	case TYPE_RegDeleteValueA:
		m_listCtrl.SetItemText(index, 1, strTime);
		DeleteValueA_Msg(params,index);
		break;
	case TYPE_RegDeleteValueW:
		m_listCtrl.SetItemText(index, 1, strTime);
		DeleteValueW_Msg(params,index);
		break;
		///////////////////////////////////////////////////////////////////////
	case TYPE_RegQueryValueExA:
		m_listCtrl.SetItemText(index, 1, strTime);
		QueryValueExA_Msg(params,index);
		break;

	case TYPE_RegQueryValueExW:
		m_listCtrl.SetItemText(index, 1, strTime);
		QueryValueExA_Msg(params,index);
			break;
	default:
		break;
	}
	m_listCtrl.Scroll(CSize(0,10));
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CRegMonitorDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

// ͨ������IDת������Ӧ�Ľ��� ·��
void CRegMonitorDlg::ProcessPidToPath(DWORD ProcessId, PCHAR ProcessName, PCHAR ProcessPath)
{
	PROCESSENTRY32 info = {sizeof(PROCESSENTRY32)};
    MODULEENTRY32 minfo = {sizeof(MODULEENTRY32)};
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    BOOL bReport = Process32First(hProcess,&info);
	
    while(bReport)
    {
		if(info.th32ProcessID == ProcessId)
		{
			HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, info.th32ProcessID);
			Module32First(hModule, &minfo);
			strcpy(ProcessName, info.szExeFile);
			strcpy(ProcessPath, minfo.szExePath);
			return ;
		}
        bReport = Process32Next(hProcess, &info);   
    }
	strcpy(ProcessName, "");
	strcpy(ProcessPath, "");
}

// ͨ�� Key �� KeyName ö���Ӽ�ʹ�� ZwQueryKey(�ں�ģʽ��)
void CRegMonitorDlg::HKeyToKeyName(DWORD PID, HKEY hKey, char *pszKeyName)
{
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID); // ���Զ�̽��̾��
	
	HANDLE hKey2;
	// ����Զ�̽��̾�� ͨ�����ַ����ﵽ������̹���һ���ں˶���
	DuplicateHandle(
		hRemoteProcess, 
		HANDLE(hKey),
		GetCurrentProcess(), 
		&hKey2, 
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	WCHAR wszText[1024] = {0};
	ULONG len = 0;
	if(m_pZwQueryKey)
	{
		(*m_pZwQueryKey)(hKey2, KeyNameInformation, wszText, 1024, &len);// ��ѯ�ü��������Ϣ
		WideToMultiByte(wszText+2, pszKeyName); // �ַ�ת��
	}
}



// ���ֽ�ת�� ӳ��һ��unicode�ַ�����һ�����ֽ��ַ���

int CRegMonitorDlg::WideToMultiByte(const WCHAR *wChar, char *mChar)
{
	int aLen = WideCharToMultiByte(CP_ACP,// ANSI����ҳ
		0, 
		wChar,// ָ�򽫱�ת����unicode�ַ���
		wcslen(wChar) + 1,
		NULL, 0, NULL, NULL);// ����ֵ�ǽ��յ���ת���ַ����Ļ�������������ֽ���
	return WideCharToMultiByte(CP_ACP, 0, wChar, wcslen(wChar) + 1, mChar, aLen, NULL, NULL);
}

// ֹͣ
void CRegMonitorDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	if(UninstallHook())
	{
		// ж�غ������Ӧ�Ĳ���
		m_hHook = NULL;
		CButton *pBt=(CButton *)GetDlgItem(IDC_BUTTON_STOP);
		pBt->SetCheck(BST_CHECKED);
		pBt->EnableWindow(false);
		pBt=(CButton *)GetDlgItem(IDC_BUTTON_START);
		pBt->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_TIP1)->SetWindowText(_T("����ֹͣ"));
		GetDlgItem(IDC_STATIC_TIP2)->SetWindowText(_T(""));
	}
	
}

// ����˵���Ϣ

void CRegMonitorDlg::OnMenuCreateKey() 
{
	m_bRegCreateKeyEx = !m_bRegCreateKeyEx;
	// �����ϴ�ѡ������ Check  ״̬
	UINT state = m_Psubmenu->GetMenuState(IDR_MENU11, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
		m_Psubmenu->CheckMenuItem(IDR_MENU11, MF_UNCHECKED | MF_BYCOMMAND);
	else
		m_Psubmenu->CheckMenuItem(IDR_MENU11, MF_CHECKED | MF_BYCOMMAND);
	
	UpdateHookFunction();
}

void CRegMonitorDlg::OnMenuDelKey() 
{
	m_bRegDeleteKey = !m_bRegDeleteKey;
	// �����ϴ�ѡ������ Check  ״̬
	UINT state = m_Psubmenu->GetMenuState(IDR_MENU12, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	
	if (state & MF_CHECKED)
		m_Psubmenu->CheckMenuItem(IDR_MENU12, MF_UNCHECKED | MF_BYCOMMAND);
	else
		m_Psubmenu->CheckMenuItem(IDR_MENU12, MF_CHECKED | MF_BYCOMMAND);
	
	UpdateHookFunction();	
}

void CRegMonitorDlg::OnMenuSetValue() 
{
	m_bRegSetValueEx = !m_bRegSetValueEx;
	// �����ϴ�ѡ������ Check  ״̬
	UINT state = m_Psubmenu->GetMenuState(IDR_MENU13, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	
	if (state & MF_CHECKED)
		m_Psubmenu->CheckMenuItem(IDR_MENU13, MF_UNCHECKED | MF_BYCOMMAND);
	else
		m_Psubmenu->CheckMenuItem(IDR_MENU13, MF_CHECKED | MF_BYCOMMAND);
	
	UpdateHookFunction();
}

void CRegMonitorDlg::OnMenuDelValue() 
{
	m_bRegDeleteValue = !m_bRegDeleteValue;
	// �����ϴ�ѡ������ Check  ״̬
	UINT state = m_Psubmenu->GetMenuState(IDR_MENU14, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	
	if (state & MF_CHECKED)
		m_Psubmenu->CheckMenuItem(IDR_MENU14, MF_UNCHECKED | MF_BYCOMMAND);
	else
		m_Psubmenu->CheckMenuItem(IDR_MENU14, MF_CHECKED | MF_BYCOMMAND);
	
	UpdateHookFunction();
}

void CRegMonitorDlg::OnMenuQueryValue() 
{
	m_bRegQueryValueEx = !m_bRegQueryValueEx;
	// �����ϴ�ѡ������ Check  ״̬
	UINT state = m_Psubmenu->GetMenuState(IDR_MENU15, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	
	if (state & MF_CHECKED)
		m_Psubmenu->CheckMenuItem(IDR_MENU15, MF_UNCHECKED | MF_BYCOMMAND);
	else
		m_Psubmenu->CheckMenuItem(IDR_MENU15, MF_CHECKED | MF_BYCOMMAND);
	
	UpdateHookFunction();
}

// ��ע�����
void CRegMonitorDlg::OnMenuOpenRegedit() 
{
	SHELLEXECUTEINFO ShellInfo; //����SHELLEXECUTEINFO�ṹ
	memset(&ShellInfo, 0, sizeof(ShellInfo)); 
	ShellInfo.cbSize = sizeof(ShellInfo);
	ShellInfo.hwnd = NULL; 
	ShellInfo.lpVerb = _T("open"); //��
	ShellInfo.lpFile = _T("regedit.exe");//��ע���༭��
	ShellInfo.nShow = SW_SHOWNORMAL;//������ʾһ������ͬʱ�������״̬ 
	ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS; 
	ShellExecuteEx(&ShellInfo);
}

 // �����¼
void CRegMonitorDlg::OnButtonSave() 
{
	//�ж��б��Ƿ�Ϊ��
	if (m_listCtrl.GetItemCount() <= 0)
	{
		MessageBox("�б���ϢΪ��!");
		return;
	}

	//���챣��Ի�����
	CFileDialog filedlg(FALSE, NULL, "Log.txt", OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,
		"��־�ļ�(*.log)|*.log||", this); 
	// �򿪶Ի���
	if (IDOK != filedlg.DoModal()) 
	{
		return;
	}
	// ��ȡѡ����ļ�·����
	CString strPathName = filedlg.GetPathName(); 


	//�����ļ�
	CFile file;
	file.Open(strPathName, CFile::modeCreate | CFile::modeWrite); // ���ļ�
	for (int i=0; i<m_listCtrl.GetItemCount(); i++)
	{
		for (int j=0; j<7; j++)
		{
			file.Write(m_listCtrl.GetItemText(i, j), strlen(m_listCtrl.GetItemText(i, j))); // ����д���ļ�
			file.Write("   ", 3); // ����֮�����ÿո�
		}
		file.Write("\r\n\r\n", strlen("\r\n\r\n")); //д����
	}
	file.Close(); // �ر��ļ�
}

void CRegMonitorDlg::OnMenuOpenProcessManage() 
{
	// ����ProcessManage �����Ƿ����
	HWND hWnd =::FindWindow(NULL,_T("ProcessManage")); 
	if(NULL==hWnd)
	{
		// ���д˳���

		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = TEXT("runas");      
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
}

// ����б��¼
void CRegMonitorDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	m_listCtrl.DeleteAllItems();
}

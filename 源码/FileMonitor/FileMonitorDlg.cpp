// FileMonitorDlg.cpp : implementation file
//

#include <ctime>
#include <cstdlib>
#include "stdafx.h"
#include "FileMonitor.h"
#include "FileMonitorDlg.h"
#include "SetDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CFileMonitorDlg dialog

CFileMonitorDlg::CFileMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileMonitorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileMonitorDlg)
	m_strPathName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hThread = NULL; // ��ʼ���߳̾��
	m_nNumber = 0; // ��ʼ���ļ����ļ��е���� 
	m_cNumber = new char[MAX_PATH]; // ��ʼ���ļ����ļ��е���� 
	itoa(m_nNumber, m_cNumber, 10); // ����ת�����ַ���
	m_hDir = INVALID_HANDLE_VALUE; // ��ʼ��Ŀ¼���
    m_Hmodule=NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//�����������ͷ���Դ
CFileMonitorDlg::~CFileMonitorDlg()
{
	//�ͷ���Դ
	if(m_Hmodule)
	{
		::FreeLibrary(m_Hmodule);
	}
	// ��ֹ�߳�
	if(m_hThread != NULL)
	{
		::TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
	// �ر�Ŀ¼���
	if(m_hDir !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDir);
		m_hDir = INVALID_HANDLE_VALUE;
	}
}

void CFileMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileMonitorDlg)
	DDX_Control(pDX, IDC_BUTTON_SET, m_BtSet);
	DDX_Control(pDX, IDC_LIST, m_CListCtrl);
	DDX_Text(pDX, IDC_EDIT_PATHNAME, m_strPathName);
	//}}AFX_DATA_MAP
}
//��Ϣӳ��
BEGIN_MESSAGE_MAP(CFileMonitorDlg, CDialog)
	//{{AFX_MSG_MAP(CFileMonitorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_COMMAND(IDM_OPEN,OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileMonitorDlg message handlers

//��ʼ���Ի���Ԫ��
void CFileMonitorDlg::InitDlgItem()
{
	m_CListCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	m_CListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_TWOCLICKACTIVATE |LVS_EX_GRIDLINES);
	m_CListCtrl.SetBkColor(RGB(247,247,255));
	m_CListCtrl.SetTextColor(RGB(0,100,155));
	m_CListCtrl.SetTextBkColor(RGB(247,247,255));

	m_CListCtrl.InsertColumn(0,_T("���"),LVCFMT_LEFT,60);
	m_CListCtrl.InsertColumn(1,_T("ʱ��"),LVCFMT_LEFT,80);
	m_CListCtrl.InsertColumn(2,_T("��������"),LVCFMT_LEFT,100);
	m_CListCtrl.InsertColumn(3,_T("�ļ�����·��"),LVCFMT_LEFT,400);
	
	//��ȡ�����ļ�
	m_bAddNew = (::GetPrivateProfileInt("Selections","Add",0,".\\Config.ini")==0 ?false:true);
	m_bDel = (::GetPrivateProfileInt("Selections","Delete",0,".\\Config.ini")==0 ?false:true);
	m_bAttribute = (::GetPrivateProfileInt("Selections","Attribute",0,".\\Config.ini")==0 ?false:true);
	m_bOccurTime = (::GetPrivateProfileInt("Selections","OccurTime",0,".\\Config.ini")==0 ?false:true);
	m_bRename=(::GetPrivateProfileInt("Selections","Rename",0,".\\Config.ini")==0 ?false:true);
	m_bOther = (::GetPrivateProfileInt("Selections","Other",0,".\\Config.ini")==0 ?false:true);
	//��ʼ������
	LOGFONT lf;
	::ZeroMemory(&lf,sizeof(lf)); 
	lf.lfHeight = 90;         
	lf.lfWeight = FW_NORMAL;   
	lf.lfItalic = false;	
	::lstrcpy(lf.lfFaceName,_T("Verdana")); 
	m_hFont.CreatePointFontIndirect(&lf);
	GetDlgItem(IDC_EDIT_PATHNAME)->SetFont(&m_hFont);
	/*******************************///����Ƥ��/*****************************************/
	//���������
	srand((unsigned)time(0));
	int i=rand()%35 +1;
	CString s=_T("");
	s.Format("skin//%d.she",i);
	//���������ڼ䶯̬���ӷ�ʽ
	//����Ŀ��DLL
	m_Hmodule=::LoadLibrary("SkinH.dll");
	//�궨�庯��ָ������ 
	typedef int ( WINAPI *SKINH_ATTACHEX)(LPCTSTR strSkinFile,LPCTSTR strPassword);
	
	if(NULL !=m_Hmodule)
	{						// ȡ��SKINH_ATTACHEX�����ĵ�ַ
		SKINH_ATTACHEX pFunc = (SKINH_ATTACHEX)::GetProcAddress(m_Hmodule, "SkinH_AttachEx");
		if(pFunc)
		{
			pFunc(s, NULL);
		}
		else
		{
			MessageBox(_T("Ƥ�������ʧ�ܣ�"),_T("ȱ��SkinH.dll"));
		}
	}

   
	m_BtSet.ModifyStyle(0, BS_ICON);
	//����
	 m_hIconSet=AfxGetApp()->LoadIcon(IDI_ICON1);
	m_BtSet.SetIcon(m_hIconSet);

}
BOOL CFileMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDlgItem();//���ó�ʼ������

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialog::OnSysCommand(nID, lParam);
	
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileMonitorDlg::OnPaint() 
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
HCURSOR CFileMonitorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFileMonitorDlg::OnButtonStart() 
{
	if (m_strPathName != "") // �ж�Ҫ���ӵ�Ŀ¼�Ƿ��
	{
		DWORD ThreadID;  
		m_hThread=::CreateThread(NULL, 0, ThreadMonitorProc, this, 0, &ThreadID); //���������߳�
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_PATH)->SetWindowText("���ڼ��...");
	}
	else
	{
		MessageBox("��ѡ��Ҫ��ص�Ŀ¼!");
	}
}

//ֹͣ����
void CFileMonitorDlg::OnButtonStop() 
{
	
	// ��ֹ�߳�
	if(m_hThread != NULL)
	{
		::TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
	// �ر�Ŀ¼���
	if(m_hDir !=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDir);
		m_hDir = INVALID_HANDLE_VALUE;
	}
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_PATH)->SetWindowText("ֹͣ�˼��...");
}

//����б�
void CFileMonitorDlg::OnButtonClear() 
{
	m_CListCtrl.DeleteAllItems();
    SetDlgItemText(IDC_STATIC_LOG,_T("0����¼"));
}
//�����б��¼
void CFileMonitorDlg::OnButtonSave() 
{
	if (m_CListCtrl.GetItemCount() <= 0)
	{
		MessageBox("�б���ϢΪ��!");
		return;
	}
	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);
	char appPath[256];
	GetCurrentDirectory(256, appPath);//ȡ��Ӧ�ó���ǰ·��
	CString strFilePath=_T(""); //·��
	strFilePath.Format("%s",appPath);
	strFilePath +="\\log.txt";
	//�����ļ�
	CFile file;
	file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite); // ���ļ�
	for (int i=0; i<m_CListCtrl.GetItemCount(); i++)
	{
		for (int j=0; j<4; j++)
		{
			file.Write(m_CListCtrl.GetItemText(i, j), strlen(m_CListCtrl.GetItemText(i, j))); // ����д���ļ�
			file.Write("   ", 3); //����ո����
		}
		file.Write("\r\n\r\n", strlen("\r\n\r\n")); //������Ŀ����
	}
	file.Close(); // �ر��ļ�
}

//�������Ӽ�¼
void CFileMonitorDlg::OnButtonExport() 
{
	// CFileDialog�ڴ��·���Ļ���������Ϊ260,�㷴��ѡ���ļ�֮��,�����˻��������Ȼ���� ����Ҫ new 
	GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);
	//���챣��Ի�����
	CFileDialog *FileDlg= new CFileDialog(FALSE, NULL, ".txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |OFN_ALLOWMULTISELECT|OFN_ENABLESIZING,
		"��־�ļ�(*.log)|*.log||", this); 
	CString strfileNamePath;//���������ļ�����·��
	char appPath[MAX_PATH]={'\0'};
	::GetCurrentDirectory(MAX_PATH, appPath);//ȡ��Ӧ�ó���ǰ·��
	CString strFilePath; //Ҫ�����ļ�·��
	strFilePath.Format("%s",appPath);
	strFilePath +="\\log.txt";
	if(IDOK!=FileDlg->DoModal())
	{
		return ;
	}
	strfileNamePath=FileDlg->GetPathName();//�õ��ļ�����·��
		
	::MoveFile((LPCTSTR)strFilePath,(LPCTSTR)strfileNamePath);//�ƶ���Ҫ������Ŀ¼��
	delete FileDlg;
}

void CFileMonitorDlg::OnBtnBrowse() 
{
	TCHAR bufName[MAX_PATH]; //������ַ����ַ���,�ṹ��ʾ��  
	TCHAR bufPath[MAX_PATH]; //������ַ����ַ���,·��

	BROWSEINFO broInfo; // �������Ŀ¼�ṹ
	broInfo.hwndOwner = m_hWnd; // �����ڵľ��
	broInfo.pidlRoot = NULL; // ָ����Ŀ¼
	broInfo.pszDisplayName = bufName; 
	broInfo.lpszTitle = _T("ѡ��Ŀ¼"); // λ�ڶԻ��򶥶˵�һ������ 
	broInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT; // ��־��������λ��Ч
	broInfo.lpfn = NULL; // �ص�����   
	broInfo.lParam = NULL; // �����ص������Ĳ�����һ��32λֵ  

	//��ȡĿ¼·��
	LPCITEMIDLIST pidl = SHBrowseForFolder(&broInfo); 
	if( pidl == NULL )
		return;

	//�������ַ�����������ʾ
	SHGetPathFromIDList( pidl, bufPath );	
	m_strPathName = bufPath;	
	GetDlgItem(IDC_STATIC_PATH)->SetWindowText("��ѡ��ļ��Ŀ¼Ϊ��" + m_strPathName);
	UpdateData(FALSE);
}
//�����߳�
 DWORD WINAPI CFileMonitorDlg::ThreadMonitorProc(LPVOID lParam)
{
	//����ת��
	CFileMonitorDlg * pDlg = (CFileMonitorDlg *)lParam;
	//�����ļ�����Ŀ¼���õ�Ŀ¼���
	pDlg->m_hDir = CreateFile(pDlg->m_strPathName, GENERIC_READ|GENERIC_WRITE, 
                                               		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
		                                       		NULL, 
		                                       		OPEN_EXISTING,
		                                       		FILE_FLAG_BACKUP_SEMANTICS, NULL);
	//�����ɹ�����ж�
	if (pDlg->m_hDir == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// ��ȡ��صķ���ֵ
	char bufTemp[(sizeof(FILE_NOTIFY_INFORMATION)+MAX_PATH)*2];
	FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)bufTemp;
	DWORD dwReturn;
	
	int Num=0; //��ʾ��¼����
	while (TRUE)
	{
		//���Ŀ¼�����仯
		if (::ReadDirectoryChangesW(pDlg->m_hDir, pNotify, sizeof(bufTemp), true, 
			FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|
			FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|
			FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_LAST_ACCESS|
			FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_SECURITY,
			&dwReturn, NULL, NULL))
		{
			//��ȡ��ǰʱ��
			CTime time = CTime::GetCurrentTime();	
			CString strTime;						
			strTime.Format("%d:%d:%d", time.GetHour(), time.GetMinute(), time.GetSecond());
			//��ȡ�仯���ļ���
			char strName[1024];
			memset(strName, 0, sizeof(strName) );
			WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength/2, strName, 99, NULL, NULL);
			//���Ӷ���
			switch (pNotify->Action)
			{
			case FILE_ACTION_ADDED: // ����
				{
					if(pDlg->m_bAddNew)
					{
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "������ļ�");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);
					}
					break;
				}
			case FILE_ACTION_REMOVED: // ɾ��
				
					if(pDlg->m_bDel)
					{
						//�б���������Ϣ
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "ɾ�����ļ�");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);
					}
				
					break;
				
			case FILE_ACTION_MODIFIED: // ���Ըı�
				
					if(pDlg->m_bAttribute)
					{
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "�޸����ļ�");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);
			
					}

					break;
				
			case FILE_ACTION_RENAMED_OLD_NAME: // ������
				
					if(pDlg->m_bRename)
					{
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "���������ļ�");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);
					}
					break;
				
			case FILE_ACTION_RENAMED_NEW_NAME: // ������
				
					if(pDlg->m_bRename)
					{
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "���������ļ�");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);

					}
				
					break;
				
			default:
					//����
					if(pDlg->m_bOther)
					{
						pDlg->m_CListCtrl.InsertItem(0, pDlg->m_cNumber);
						if(pDlg->m_bOccurTime)
						{
							pDlg->m_CListCtrl.SetItemText(0, 1, strTime);
						}
						pDlg->m_CListCtrl.SetItemText(0, 2, "δ�����仯");
						pDlg->m_CListCtrl.SetItemText(0, 3, strName);
					}
					break;
			}
			//������Ӧ�����
			Num++;
			pDlg->m_nNumber++; // ��ż�һ
			itoa(pDlg->m_nNumber, pDlg->m_cNumber, 10); // ����ת�����ַ���(ʮ����)
			
		}
		else 
		{
			break; 
		}

		CString str;
		str.Format("%d",Num);
		str += "����¼";

		pDlg->SetDlgItemText(IDC_STATIC_LOG,str);//��ʾ�Ѿ����ӵ���Ŀ��
	}

	return 0;
}
void CFileMonitorDlg::OnButtonSet() 
{
	CSetDlg set;
	if(set.DoModal() == IDOK)
	{
		this->m_bAddNew = set.m_bAddNew;
		this->m_bDel = set.m_bDel;
		this->m_bAttribute = set.m_bAttribute;
		this->m_bRename = set.m_bRename;
		this->m_bOccurTime= set.m_bOccurTime;
		this->m_bOther  = set.m_bOther;
		
		//д��.ini �����ļ�
		::WritePrivateProfileString("Selections","Add",set.m_bAddNew? "1":"0",".\\Config.ini");
		::WritePrivateProfileString("Selections","Delete",set.m_bDel? "1":"0",".\\Config.ini");
		::WritePrivateProfileString("Selections","Attribute",set.m_bAttribute? "1":"0",".\\Config.ini");
		::WritePrivateProfileString("Selections","OccurTime",set.m_bOccurTime? "1":"0",".\\Config.ini");
		::WritePrivateProfileString("Selections","Rename",set.m_bRename? "1":"0",".\\Config.ini");
		::WritePrivateProfileString("Selections","Other",set.m_bOther? "1":"0",".\\Config.ini");
	}	
}



void CFileMonitorDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if(pNMListView->iItem != -1)
	{
	  DWORD dwPos = GetMessagePos();
      CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
  
      CMenu menu;
      VERIFY( menu.LoadMenu(IDR_MENU1));
      CMenu* popup = menu.GetSubMenu(0);
      ASSERT( popup != NULL );
      popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
   }
	*pResult = 0;
}

void CFileMonitorDlg::OnOpen() 
{
	
  	CString tem = m_strPathName;
	CString str = _T("Explorer /select,");
	int nItem = m_CListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	tem += m_CListCtrl.GetItemText(nItem, 3);
	str +=tem;
	WinExec(str,SW_SHOWNORMAL);//��ʾ�ļ���λ��
	
}

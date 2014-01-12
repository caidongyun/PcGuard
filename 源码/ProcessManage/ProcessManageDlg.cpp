// ProcessManageDlg.cpp : implementation file

#include <Ctime>
#include <cstdlib>
#include "stdafx.h"
#include "ProcessManage.h"
#include "ProcessManageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma comment(lib,"version.lib")//�汾��Ϣ

#define TYPE_PROCESSMANAGE_MSG WM_USER+111 //���͸�ע�����ӵ���Ϣ

typedef struct PROCESS_MANAGE_MSG
{
	DWORD	PID;//����ID��
	char	ProcessName[MAX_PATH];//��������CString�׳���
}ProcessMsg;

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
__cdecl:   ������   C��   C++���Ա�д�����̣�Ҳ������Ҫ�ɵ�����������������̣�   
__stdcall:   ��safecall��Ҫ���ڵ���Windows   API   ����������safecall������˫�ؽӿ�

��Ϊvc6.0 Ϊ�Ͱ汾������û�з�װOpenThread ��������Ҫ���ö�̬����dll �ķ���  
��ע�� һ��Ҫָ��Ϊstdcall��ʽ��(����Ҳ���԰�װ�°汾��Microsoft Platform SDK for Windows **)
*/

typedef  HANDLE  ( __stdcall *OPENTHREAD) (DWORD dwFlag, BOOL bUnknow, DWORD dwThreadId);
OPENTHREAD lpfnOpenThread = (OPENTHREAD) ::GetProcAddress(::LoadLibrary("kernel32.dll"),"OpenThread");
///////////////////////////////////////////////////////////////////////////////////////////////////////

// CProcessManageDlg 
//��̬��Ա��ʼ��
 bool CProcessManageDlg::m_bAccend=false;
 int CProcessManageDlg::m_nSortCol=0;


CProcessManageDlg::CProcessManageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessManageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessManageDlg)
	m_nDll = _T("");
	m_numOfDll=_T("");
	m_Hmodule=NULL;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CProcessManageDlg::~CProcessManageDlg()
{
	
	//�ͷ���Դ
	if(m_Hmodule)
	{
		::FreeLibrary(m_Hmodule);
	}
}

void CProcessManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessManageDlg)
	DDX_Control(pDX, IDC_LIST_DLL, m_ListDll);
	DDX_Text(pDX, IDC_STATIC_DLL, m_nDll);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ListProcess);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcessManageDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessManageDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROCESS, OnColumnclickListProcess)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, OnButtonShow)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DLL, OnColumnclickListDll)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, OnClickListProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, OnRclickListProcess)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_TERMINATE, OnTerminate)
	ON_COMMAND(ID_SUSPEND, OnSuspend)
	ON_COMMAND(ID_RESUME, OnResume)
	ON_COMMAND(ID_FILE_PATH, OnFilePath)
	ON_COMMAND(ID_BAIDU_CHECK, OnBaiduCheck)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_REG_MONITOR, OnRegMonitor)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DLL, OnRclickListDll)
	ON_COMMAND(ID_OPEN_MODULE, OnOpenModule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessManageDlg message handlers

BOOL CProcessManageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_ListProcess.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);//�����б���ʽ
	//�б����ַ�������
	CString strHeader1[5]={_T("������"),_T("����ID"),_T("������ʱ��"),_T("��ǰռ���ڴ�"),_T("�ļ�·��")};
	int nWidth1[5]={120,55,90,92,300};
	int i=0;
	//�����б���
	for (i=0;i<5;i++)
	m_ListProcess.InsertColumn( i,strHeader1[i], LVCFMT_LEFT, nWidth1[i]);
	m_ListProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//��������

	m_ImageList.Create(16,16,true|ILC_COLOR32,12,30);//����ͼ���б�
	m_ImageList.SetBkColor(RGB(255, 255, 255));//��ͼ�񱳾�ɫ����Ϊ��ɫ
	EnablePrivilege();//����Ȩ��
	EnumProcess();//�оٽ���
	m_ListProcess.SetImageList(&m_ImageList, LVSIL_SMALL);//�趨�б���ͼ�ؼ���ͼ���б�

	GetDlgItem(IDC_LIST_DLL)->ShowWindow(SW_HIDE); //������ʾDLL���б�
	m_ListDll.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	m_ListDll.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP); //����
	CString strHeader2[7]={"ģ��·��","����","��˾","�汾","ԭʼ�ļ���","�Ϸ���Ȩ","�޸�ʱ��"};
	int nWidth2[7]={180,150,110,80,60,80,66};
	//�����б���
	for (int nCol=0;nCol<7;nCol++)
		m_ListDll.InsertColumn(nCol,strHeader2[nCol],LVCFMT_LEFT,nWidth2[nCol]);
    
	/*******************************///����Ƥ��/*****************************************/
	//��������� ���ڶ�̬����һ��Ƥ��
	srand((unsigned)time(0));
	i=rand()%35 +1;
	CString s=_T("");
	s.Format("skin//%d.she",i);
	//���������ڼ䶯̬���ӷ�ʽ
	//����Ŀ��DLL
	m_Hmodule=::LoadLibrary("SkinH.dll");
	//�궨�庯��ָ������ 
	typedef int ( WINAPI *SKINH_ATTACHEX)(LPCTSTR strSkinFile,LPCTSTR strPassword);
	
	if(NULL !=m_Hmodule)
	{	
		// ȡ��SKINH_ATTACHEX�����ĵ�ַ
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

	/********************************************************************************************/
	SetTimer(0,300,NULL);//���ö�ʱ�� 300����
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessManageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialog::OnSysCommand(nID, lParam);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessManageDlg::OnPaint() 
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
HCURSOR CProcessManageDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


//����Ȩ��
bool CProcessManageDlg::EnablePrivilege()
{
	// ����������Ȩ,�Ա����ϵͳ����
	BOOL status = false;
	HANDLE hToken;
	// ��һ�����̵ķ�������
	if(::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		// ȡ����Ȩ����Ϊ"SetPrivilege"��LUID
		LUID uID;
		::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &uID);
		
		// ������Ȩ����
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = uID;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		if (ERROR_SUCCESS == GetLastError())
			status =true;
		else
			status =false;
		// �رշ������ƾ��
		::CloseHandle(hToken);
	}
	return status;
}



//�оٽ���
int CProcessManageDlg::EnumProcess(void)
{
	int i=0;
	int j=0;
	HANDLE hProcessSnap;//���վ��	
	CString tem,title;
    //The SHGetFileInfo API provides an easy way to get attributes for a file given a pathname
	SHFILEINFO info;
	//�����ļ�ϵͳʱ��(����,�˳�,�ں�,�û�)
	FILETIME      ftCreation,
                  ftExit,
                  ftKernel,
                  ftUser;
	//����ʱ��ṹ COleDateTimeʹ�õ�λ����˫���������(����ȷ)
    COleDateTime  timeCreation,
                  timeCurrent;
    COleDateTimeSpan   timeDiff;

	m_ListProcess.DeleteAllItems();// ����б��е�������
	PROCESSENTRY32 pe32={0};// ����PROCESSENTRY32 �ṹ
	pe32.dwSize = sizeof( PROCESSENTRY32 );	// ����PROCESSENTRY32�ṹ�Ĵ�С
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );// �������̿���

	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		return 0;
	}
	// �״ε���
	if(!Process32First( hProcessSnap, &pe32 ) )
	{
		CloseHandle( hProcessSnap );
		return 0;
	}
   // �������̿��գ�������ʾÿ�����̵���Ϣ
	while( Process32Next( hProcessSnap, &pe32 ))
	{	
		// ��ȡ�ļ�ͼ��
		SHGetFileInfo(GetProcessPath(pe32.th32ProcessID),0,&info,sizeof(&info),
			SHGFI_ICON | SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
		j=m_ImageList.Add(info.hIcon);// ��ӳ���ͼ��
		m_ListProcess.InsertItem(i,pe32.szExeFile,j);  // ������  

		tem.Format("%d",pe32.th32ProcessID);
		m_ListProcess.SetItemText(i,1,tem);	// PID

		title=pe32.szExeFile;
		// �ж��Ƿ�Ϊ smss.exe����
		//smss.exe(Session Manager Subsystem)���ý���Ϊ�Ự������ϵͳ���Գ�ʼ��ϵͳ����
		if(!strcmp(title,"smss.exe"))
		{
			m_ListProcess.SetItemText(i,4,"C:\\WINDOWS\\system32\\smss.exe");
		}
		//�Ƿ�Ϊϵͳ����
		else if(!strcmp(pe32.szExeFile,"system")||!strcmp(pe32.szExeFile,"[System Process]"))
		{
			m_ListProcess.SetItemText(i,0,"Idle");
			m_ListProcess.SetItemText(i,4,"NT OS Kernel");// NT ���Ľ���
		}
		else if(!strcmp(pe32.szExeFile,"system")||!strcmp(pe32.szExeFile,"System"))
		{
			m_ListProcess.SetItemText(i,4,"NT OS Kernel");
		}
		else
		{
			m_ListProcess.SetItemText(i,4,GetProcessPath(pe32.th32ProcessID));//����·��
		}	
		HANDLE     hProcess;// ���̾��
		timeCurrent = COleDateTime::GetCurrentTime();// ��õ�ǰʱ��
		hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 
			FALSE,pe32.th32ProcessID);// �򿪴˽���
		if(NULL != hProcess)
		{
			GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser);
			timeCreation = ftCreation;
			timeDiff = timeCurrent - timeCreation;
			tem.Format("%uСʱ:%u��:%u��",timeDiff.GetHours(), timeDiff.GetMinutes(), 
				timeDiff.GetSeconds());// ��ȡ��������ʱ��
			m_ListProcess.SetItemText(i,2,tem);// ���ý�������ʱ��
			//ʹ��API����GetProcessMemoryInfo����ȡ�ڴ��ʹ�����
			//ppsmemCounters�Ƿ����ڴ�ʹ������Ľṹ
			PROCESS_MEMORY_COUNTERS pmc;
			pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
			if(::GetProcessMemoryInfo(hProcess, &pmc,sizeof(pmc)))
			{
				tem.Format(_T("%.2f M"),(float)(pmc.WorkingSetSize/1024)/1024);
				m_ListProcess.SetItemText(i,3,tem);
			}
			CloseHandle(hProcess);// �رմ˽��̾��
		}
		m_vProcessId[i]=pe32.th32ProcessID;  //���ID��Ϣ
		m_ListProcess.SetItemData(i,i);//����,Ϊ������׼��
		i++;
	} 
	CString str1;
	str1.Format("%d",i);// ��������
	SetDlgItemText(IDC_STATIC_PRO_NUM,str1);
	CloseHandle( hProcessSnap );//�رվ��
	UpdateData(FALSE);
	return 0;
}


//�оٽ���ģ����Ϣ
BOOL CProcessManageDlg::GetDllInfo(DWORD dwPID)
{
	// ��ֹ����
	m_ListDll.SetRedraw(FALSE);
	// ģ��������
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbHmodule;// ����ģ����
	unsigned int i;
	m_ListDll.DeleteAllItems();
	char szModName[MAX_PATH];
	// ��Ŀ����̣�ȡ�ý��̾��
	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,FALSE, dwPID);
	// ö�� ����ģ��
	if( hProcess &&  EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbHmodule))
	{
		for ( i = 0; i <=(cbHmodule / sizeof(HMODULE)); i++ )
		{        	
			// �õ�ģ�������·����
			if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
				sizeof(szModName)))
			{
				//ģ�������޸�ʱ��
				CString ModName, FTime;
				// �ļ�������
				CFileStatus status;
				ModName = szModName;
				ModName.Replace("\\??\\","");
				ModName.Replace("\\SystemRoot","C:\\WINDOWS");
				m_ListDll.InsertItem(0xffff,ModName);
				//CString ת char*
				char* szFileName=(LPTSTR)(LPCTSTR)ModName;
				// ����ļ��汾��Ϣ Returns size of version info in bytes 
				DWORD dwSize =::GetFileVersionInfoSize(szFileName,NULL); 
				LPVOID pBlock = malloc(dwSize); 
				// �Ѱ汾��Ϣ д�������� Read version info into buffer 
				if(::GetFileVersionInfo(szFileName,0,dwSize,pBlock))
				{
					char* pVerValue = NULL; 
					UINT nSize = 0; 
					//���ǵõ�����
					VerQueryValue(pBlock,_T("\\VarFileInfo\\Translation"), 
						(LPVOID*)&pVerValue,&nSize); 
					
					CString strSubBlock,strTranslation,strTemp;
					//����ת��  080404b0Ϊ���ģ�040904E4ΪӢ��
					strTemp.Format("000%x",*((unsigned short int *)pVerValue)); 
					strTranslation = strTemp.Right(4); 
					strTemp.Format("000%x",*((unsigned short int *)&pVerValue[2])); 
					strTranslation += strTemp.Right(4); 
					//�ļ�����  FileDescription
					strSubBlock.Format("\\StringFileInfo\\%s\\FileDescription",strTranslation); 
					VerQueryValue(pBlock,(LPTSTR)(LPCTSTR)strSubBlock,(LPVOID*)&pVerValue,&nSize);
					if((NULL!=pVerValue)&&(0!=nSize))// �ж��Ƿ�õ���ѯ���
					{
						strTemp.Format("%s",pVerValue); 
						m_ListDll.SetItemText(i, 1, strTemp);
					}
					else
					{
						m_ListDll.SetItemText(i, 1, "");
					}
					//��˾���� CompanyName
					strSubBlock.Format("\\StringFileInfo\\%s\\CompanyName",strTranslation); 
					VerQueryValue(pBlock,(LPTSTR)(LPCTSTR)strSubBlock,(LPVOID*)&pVerValue,&nSize); 
					if((NULL!=pVerValue)&&(0!=nSize))
					{
						strTemp.Format("%s",pVerValue); 
						m_ListDll.SetItemText(i, 2, strTemp);
					}
					else
					{
						m_ListDll.SetItemText(i, 2, "");
					}
					
					//��Ʒ�汾 ProductVersion
					strSubBlock.Format("\\StringFileInfo\\%s\\ProductVersion",strTranslation); 
					VerQueryValue(pBlock,(LPTSTR)(LPCTSTR)strSubBlock,(LPVOID*)&pVerValue,&nSize); 
					if((NULL!=pVerValue)&&(0!=nSize))
					{
						strTemp.Format("%s",pVerValue); 
						m_ListDll.SetItemText(i, 3, strTemp);
					}
					else
					{
						m_ListDll.SetItemText(i, 3, "");
					}
					//ԭʼ�ļ��� OriginalFileName
					strSubBlock.Format("\\StringFileInfo\\%s\\OriginalFileName",strTranslation);
					VerQueryValue(pBlock,(LPTSTR)(LPCTSTR)strSubBlock,(LPVOID*)&pVerValue,&nSize);
					if((NULL!=pVerValue)&&(0!=nSize))
					{
						strTemp.Format("%s",pVerValue);
						m_ListDll.SetItemText(i, 4, strTemp);
					}
					else
					{
						m_ListDll.SetItemText(i, 4, "");
					}
					//�Ϸ���Ȩ LegalTradeMarks
					strSubBlock.Format("\\StringFileInfo\\%s\\LegalTradeMarks",strTranslation);
					VerQueryValue(pBlock,(LPTSTR)(LPCTSTR)strSubBlock,(LPVOID*)&pVerValue,&nSize);
					if((NULL!=pVerValue)&&(0!=nSize))
					{
						strTemp.Format("%s",pVerValue);
						m_ListDll.SetItemText(i, 5, strTemp);
					}
					else
					{
						m_ListDll.SetItemText(i, 5, "");
					}
				}
				else
				{
					m_ListDll.SetItemText(i, 1, "");
					m_ListDll.SetItemText(i, 2, "");
					m_ListDll.SetItemText(i, 3, "");
					m_ListDll.SetItemText(i, 4, "");
					m_ListDll.SetItemText(i, 5, "");
				}
				delete pBlock; // �ͷ�������ڴ�
				// ���ڲ鿴�ļ���Ϣ�ĺ���
				CFile::GetStatus(szFileName,status);
				FTime = status.m_mtime.Format("%m/%d/%y");
				m_ListDll.SetItemText(i, 6, FTime); 
				
				
				m_ListDll.SetItemData(i,i);
			}
		}
		int j = 0;
		j= m_ListDll.GetItemCount();	
		m_numOfDll.Format("%d",j-1);
		CloseHandle(hProcess);
		//��������
		m_ListDll.SetRedraw(TRUE);
		m_ListDll.Invalidate();
		m_ListDll.UpdateWindow();
		
		return TRUE;
	}
	else  
	{
		CloseHandle( hProcess );
		return FALSE;
	}
}


//��ȡ����·��
CString CProcessManageDlg::GetProcessPath(DWORD idProcess)
{
	// ��ȡ����·��
	CString sPath;
	char lPath[MAX_PATH] = "";

	// ��Ŀ�����(ͨ�� PID)
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, idProcess );

	if( NULL != hProcess )
	{
		HMODULE hMod;
		DWORD cbNeeded;
		// ö�ٸý��̵�ģ��
		if(::EnumProcessModules(hProcess, &hMod, sizeof( hMod ), &cbNeeded ) )
		{
			// ��ó���ǰ��Ŀ¼
			DWORD dw =::GetModuleFileNameEx( hProcess, hMod, (LPSTR)(LPCTSTR)sPath, MAX_PATH );
			// �ҵ���һ�γ���:\\�ַ�����λ��
			int n=sPath.Find(":\\");
			// �Ӵ��̷���ʼ�ľ���·�� ����(c:\\...)
			sPath=sPath.Mid(n-1);
			// ���ļ���ת���ɳ��ļ���
			::GetLongPathName(sPath,lPath,255);
		}
		CloseHandle( hProcess );
	}
	return(lPath);
}
//��ʱ�����ڶ�ʱ����ϵͳ�ڴ�ʹ����
void CProcessManageDlg::OnTimer(UINT nIDEvent) 
{
	CString str;
	MEMORYSTATUS ms;// ����MEMORYSTATUS�ṹ
	GlobalMemoryStatus(&ms);//����ڴ�ʹ��״̬
	str.Format("%d", ms.dwMemoryLoad);
	str = str +"%";
	SetDlgItemText(IDC_STATIC_MEMORY,str);//�ھ�̬�ı�������ʾ
	CDialog::OnTimer(nIDEvent);
}




 //������  ����֮ǰ��Ҫ�������� --->  m_list.SetItemData(i,i);
 int CALLBACK CProcessManageDlg::MyCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{	
	CListCtrl * pListCtrl = (CListCtrl *)lParamSort;
	CString text1; 
	CString text2;

	int index;
	LVFINDINFO info;
	info.flags = LVFI_PARAM;

	info.lParam = lParam1;
	index = pListCtrl->FindItem(&info);
	text1 = pListCtrl->GetItemText(index, m_nSortCol); //��ȡǰһ���ı���Ϣ

	info.lParam = lParam2;
	index = pListCtrl->FindItem(&info);
	text2 = pListCtrl->GetItemText(index, m_nSortCol);  //��ȡ��һ���ı���Ϣ 

	//�ַ�����
	if(m_bAccend)
		return text1.CompareNoCase(text2);
	else
		return text2.CompareNoCase(text1);

}
 //�����б�ͷ��ʱ������LVN_COLUMNCLICK֪ͨ��Ϣ  �������Ϣ����
void CProcessManageDlg::OnColumnclickListProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//
	m_nSortCol = pNMListView->iSubItem; //��ָ��������ؼ��� 
	//�����Զ������������������
	m_ListProcess.SortItems(MyCompare, (DWORD)&m_ListProcess);
	m_bAccend = !m_bAccend;
	*pResult = 0;
}

void CProcessManageDlg::OnButtonShow() 
{
	// TODO: Add your control notification handler code here
	CButton* pBut;	
	pBut=(CButton*)GetDlgItem(IDC_BUTTON_SHOW);
	CWnd *pWnd, *pWnd2;
	CRect   rcWnd, rcWnd2;//������ʾ����
	CString str;
	pBut->GetWindowText(str);
	if(_T("��ʾ���̰�����ģ��")==str)//��ʾ����ģ��
	{
		GetDlgItem(IDC_LIST_DLL)->ShowWindow(SW_SHOW);//��ʾ���ص�ģ���б�
		pBut->SetWindowText(_T("����ʾ���̰�����ģ��"));
		pWnd = GetDlgItem( IDC_LIST_PROCESS );
		pWnd->GetWindowRect(&rcWnd); 
		pWnd->SetWindowPos( NULL,0,0,rcWnd.Width(),rcWnd.Height()-140,SWP_NOZORDER | SWP_NOMOVE );
		pWnd2 = GetDlgItem( IDC_LIST_DLL);
		pWnd2->GetWindowRect(&rcWnd2);
		pWnd2->SetWindowPos( NULL,0,0,rcWnd.Width(),rcWnd2.Height(),SWP_NOZORDER | SWP_NOMOVE);
	}
	else //����ʾģ����Ϣ
	{
		GetDlgItem(IDC_LIST_DLL)->ShowWindow(SW_HIDE);// ����ģ����Ϣ�б�
		pBut->SetWindowText(_T("��ʾ���̰�����ģ��"));
		pWnd = GetDlgItem( IDC_LIST_PROCESS );
		pWnd->GetWindowRect(&rcWnd); 
		pWnd->SetWindowPos( NULL,0,0,rcWnd.Width(),rcWnd.Height()+140,SWP_NOZORDER | SWP_NOMOVE );
	}
	
}

void CProcessManageDlg::OnColumnclickListDll(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nSortCol = pNMListView->iSubItem; //��ָ��������ؼ��� 
	m_ListDll.SortItems(MyCompare, (DWORD)&m_ListProcess);
	m_bAccend = !m_bAccend;
	*pResult = 0;
}

void CProcessManageDlg::OnClickListProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CString szCID,exp;
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (nItem != -1)
	{
		szCID= m_ListProcess.GetItemText(nItem,1);
		DWORD dwPID = atol(szCID);	//���б���ȡ��PID
		GetDllInfo(dwPID);// ͨ�� PID��ô˽��̵�ģ����Ϣ
	}
   //�ھ�̬�ı�����ʾDLL����
	m_nDll = m_ListProcess.GetItemText(nItem,0)+"���� ";
	m_nDll += m_numOfDll +" ��ģ��";
	UpdateData(FALSE);	
	*pResult = 0;
}

//�����б��Ҽ���Ϣ 
void CProcessManageDlg::OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult) 
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

//����˵� ˢ����Ϣ
void CProcessManageDlg::OnRefresh() 
{
	// TODO: Add your command handler code here
	m_ListProcess.SetRedraw(FALSE);  //��������
	EnumProcess();        //�����оٽ���
	m_ListProcess.SetRedraw(TRUE);
	m_ListProcess.Invalidate();
	m_ListProcess.UpdateWindow();
}

void CProcessManageDlg::OnTerminate() 
{
	// TODO: Add your command handler code here
	int i=m_ListProcess.GetSelectionMark(); //��һ���б���ͼ�ؼ�����ѡ����(����ɾ����)
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);

	//�Ȼ�øý��̾����ͨ�����̱�ʶ
	HANDLE ProcessHandle;

	//��������IDָ���Ļ�ô��ڵ�һ�����̵ľ��
	ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,m_vProcessId[nItem]);

	if(ProcessHandle)
	{
		BOOL bRet=TerminateProcess(ProcessHandle,0);
		if (!bRet)
		{
			AfxMessageBox("��������ʧ��!");
		}
		else
		{
			m_ListProcess.DeleteItem(i);     //�ӿؼ���ɾ������,���������ᵼ�������б�����
		}
	}
	else
	{
		AfxMessageBox("�򿪽��̾��ʧ��!");
	}
}

// ��ͣ����
void CProcessManageDlg::OnSuspend() 
{
	// TODO: Add your command handler code here
	CString str;
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	str = m_ListProcess.GetItemText(nItem, 1);

	unsigned int Pid;
	THREADENTRY32 th32;
	th32.dwSize=sizeof(th32);
	Pid = (DWORD)atoi(str); // CString ת��ΪDWORD
	HANDLE hThreadSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0); //�������̿���
	if(hThreadSnap==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("CreateToolhelp32Snapshot����ʧ��");
	}

	BOOL bRet = ::Thread32First(hThreadSnap,&th32);
	while(bRet)
	{
		if(th32.th32OwnerProcessID == Pid)
		{	//HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, __dwMainThreadId);
			// ����vc6.0���Ͱ汾�е�Winbase.h û�д˺��������
			HANDLE handle=lpfnOpenThread(THREAD_ALL_ACCESS,FALSE,th32.th32ThreadID); // ���߳��ں˶���
			if(!(::SuspendThread(handle))) //��ͣ�߳�
			{
			}
			CloseHandle(handle);// �ر��ں˶���
		}
		bRet=::Thread32Next(hThreadSnap,&th32);
	}
	::CloseHandle(hThreadSnap);// �ر��ں˶���
}

// �ָ���������
void CProcessManageDlg::OnResume() 
{
	// TODO: Add your command handler code here
	CString str;
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	str = m_ListProcess.GetItemText(nItem, 1);

	THREADENTRY32 th32;
	th32.dwSize=sizeof(th32);
	unsigned int Pid;
	Pid = (DWORD)atoi(str);
	HANDLE hThreadSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if(hThreadSnap==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("CreateToolhelp32Snapshot����ʧ�ܣ�");
	}

	BOOL bRet = ::Thread32First(hThreadSnap,&th32);
	while(bRet)
	{
		if(th32.th32OwnerProcessID==Pid)
		{	
			HANDLE handle=lpfnOpenThread(THREAD_ALL_ACCESS,FALSE,th32.th32ThreadID);// ���߳��ں˶���
			if(::ResumeThread(handle)) //�ָ���������
			{
			}
			CloseHandle(handle);
		}
		bRet = ::Thread32Next(hThreadSnap,&th32);
	}
	::CloseHandle(hThreadSnap);
}

// �򿪵��ļ�����
void CProcessManageDlg::OnFilePath() 
{
	CString tem = "";
	char szt[255];
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	m_ListProcess.GetItemText(nItem, 0, szt, 512);
	if(!strcmp(szt,"system")||!strcmp(szt,"[System Process]"))
	{
	}
	else
	{
		m_ListProcess.GetItemText(nItem, 4, szt, 512);
		tem.Format("Explorer /select, %s",szt);
		WinExec(tem,SW_SHOWNORMAL);
	}
}
// �ٶ�һ�¸ý���
void CProcessManageDlg::OnBaiduCheck() 
{
	CString strParams,  strDir,strURL;
	SHELLEXECUTEINFO ShellInfo;
    memset(&ShellInfo, 0, sizeof(ShellInfo)); 
    
	char ProcessName[100]=" ";	
	int nItem=m_ListProcess.GetSelectionMark();
	m_ListProcess.GetItemText(nItem,0,ProcessName,100);
    strURL=_T("www.baidu.com/s?wd=");
	strURL+= ProcessName;
    ShellInfo.cbSize = sizeof(ShellInfo);
    ShellInfo.hwnd = NULL; 
    ShellInfo.lpVerb = _T("open"); 
    ShellInfo.lpFile = strURL;
    ShellInfo.nShow = SW_SHOWNORMAL; 
    ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS; 
    BOOL bResult = ShellExecuteEx(&ShellInfo);

}

// ���ƽ���·��
void CProcessManageDlg::OnCopy() 
{
	//��õ�ǰѡ����
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED); 
	if(OpenClipboard())//�򿪼�����
	{
		CString str;
		HANDLE hClip;
		char *pBuf;
		EmptyClipboard();//��ռ�����
		str=m_ListProcess.GetItemText(nItem,4);//����ı�
		hClip=::GlobalAlloc(GMEM_MOVEABLE,str.GetLength()+1);//�Ӷ��з����ڴ�
		pBuf=(char*)GlobalLock(hClip);//���ڴ����
		strcpy(pBuf,str);//������������ڴ���
		GlobalUnlock(hClip);//����
		SetClipboardData(CF_TEXT,hClip);
		CloseClipboard();//�رռ�����
	}
}




void CProcessManageDlg::OnRegMonitor() 
{
	/*
	��WM_COPYDATA��ǰ�᣺
	1��֪��������Ϣ���̵ľ����
	2��������Ϣ����������WM_COPYDATA��Ϣӳ�䣬�ܶ���������Ӧ��
	*/
	//��õ�ǰѡ����
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED); 
	CString strPid=m_ListProcess.GetItemText(nItem,1);//���PID��
	CString StrProName=m_ListProcess.GetItemText(nItem,0);//��ý�����
	HWND hWnd =::FindWindow(NULL,_T("RegMonitor")); // ����RegMonitor
	if(NULL==hWnd)
	{ 
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };   
		// Ask for privileges elevation.   
		sei.lpVerb = TEXT("runas");      
		sei.lpFile = _T("RegMonitor.exe");   
		sei.nShow = SW_SHOWNORMAL;   
		ShellExecuteEx(&sei);
		//CreateProcessAs("RegMonitor.exe");
		Sleep(300);
		WaitForSingleObject(sei.hProcess, INFINITE);
		OnRegMonitor();
    }
	else
	{
		
		
		// ����Vista/Win7��,��ͬȨ�޽��̼�(����Ͳ���Ҫ���������Ҫ)������Ϣ��WM_COPYDATA 
		/*typedef BOOL (WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT,DWORD); 
		ChangeWindowMessageFilter_PROC m_pfnChangeWindowMessageFilter; 
		m_pfnChangeWindowMessageFilter = 
			(ChangeWindowMessageFilter_PROC)::GetProcAddress (::GetModuleHandle(_T("USER32")),"ChangeWindowMessageFilter"); 
		if (m_pfnChangeWindowMessageFilter) 
		{ 
			//m_pfnChangeWindowMessageFilter(WM_COPYDATA, 1); 
		}*/
		
		COPYDATASTRUCT data;//�������ݽṹ
		ProcessMsg ProMsg ;//�˴���������ָ��(�м�)�� ���ܶ�̬�����ڴ�洢����
		ProMsg.PID=(DWORD)atoi(strPid);
		char * pStr = (LPTSTR)(LPCTSTR)StrProName;//ת����char*
		strncpy(ProMsg.ProcessName, pStr, sizeof(ProMsg.ProcessName));//������Ŀ��
		data.cbData = sizeof(ProcessMsg);
		data.lpData = (void*)&ProMsg;
		data.dwData = TYPE_PROCESSMANAGE_MSG;
		::SendMessage(hWnd,WM_COPYDATA,(WPARAM)hWnd,(LPARAM)&data);//���ͣ�
	}


}

void CProcessManageDlg::OnRclickListDll(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if(pNMListView->iItem != -1)
	{
	  DWORD dwPos = GetMessagePos();
      CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
  
      CMenu menu;
      VERIFY( menu.LoadMenu(IDR_MENU2));
      CMenu* popup = menu.GetSubMenu(0);
      ASSERT( popup != NULL );
      popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
   }
	*pResult = 0;
}
//���� �򿪵���Ӧģ��λ�ò˵���Ϣ
void CProcessManageDlg::OnOpenModule() 
{
	CString tem = _T("");
	char sztemp[255];
	// ���ѡ�е���
	int nItem = m_ListProcess.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	// ��� ģ��·��
	m_ListDll.GetItemText(nItem, 0, sztemp, 512);
	tem.Format("Explorer /select, %s",sztemp);
	// �򿪵���ģ��
	WinExec(tem,SW_SHOWNORMAL);
}
